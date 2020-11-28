// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/feedback.hpp>
#include <dwt/ffi.hpp>
#include <dwt/globals.hpp>
#include <dwt/reporting.hpp>
#include <dwt/scope.hpp>

namespace dwt {

std::shared_ptr<scope> scope::global = std::make_shared<scope>();
scope *scope::current = scope::global.get();
std::atomic<uint64_t> scope::next_id = 0;

scope::scope(scope *parent_scope, token_ref name_ref, int flags)
  : _parent_scope(parent_scope)
  , _name_ref(name_ref)
  , _index(-1)
  , _id(next_id++) {

  if (is_global()) {
    _index = globals::table().index_of(qualified_name());
    if (_index < 0) {
      _index = globals::table().add_r(qualified_name());
    }
  }
}

scope::scope()
  : _parent_scope(nullptr)
  , _index(globals::table().add_r("::")) {
}

scope::~scope() {
}

std::string scope::to_string() {
  auto s = scope::current;
  return s->qualified_name();
}

scope *scope::up() const {
  return _parent_scope;
}

bool scope::is_anonymous() const {
  return _name_ref.text() == "";
}

bool scope::is_global() const {
  bool isglob = true;
  auto s = this;

  while (s) {
    if (s->is_anonymous() && s->up()) {
      isglob = false;
      break;
    }
    s = s->up();
  }

  return isglob;
}

int32_t scope::lookup() const {
  return _index;
}

// Get the name associated with the given scope. Anonymous scope names are
// automatically generated based on a unique id. This ensures that calls
// to qualified_name() will always return a unique string. The compiler
// relies on this property because it uses these strings to resolve local
// and global variables.
std::string scope::name() const {
  if (is_anonymous() && up()) {
    return "<anon" + std::to_string(_id) + ">";
  }
  return _name_ref.text();
}

std::string scope::qualified_name() const {
  std::string qualified = name();
  scope *scope_ptr = up();

  if (!scope_ptr) {
    qualified = "::";
  }

  while (scope_ptr) {
    qualified = scope_ptr->name() + "::" + qualified;
    scope_ptr = scope_ptr->up();
  }

  return qualified;
}

scope *scope::add(std::string name, int flags) {
  return add(token_ref(name), flags);
}

scope *scope::add(token_ref name_ref, int flags) {
  scope *scope_ptr = current->find_ident(name_ref.text());

  if (!scope_ptr) {
    scope_ptr = new scope(current, name_ref, flags);
    current->_identifiers.emplace_back(std::unique_ptr<scope>(scope_ptr));
  } else if (flags & SCOPE_EXCLUSIVE) {
    oops("e@1 redefinition of '$1' n@2 first defined here...",
         name_ref,
         scope_ptr->_name_ref);
  }

  return scope_ptr;
}

scope *scope::open(token_ref name_ref, int flags) {
  scope *scope_ptr = nullptr;

  if (flags & SCOPE_ANONYMOUS) {
    scope_ptr = new scope(current, name_ref, flags);
    current->_private_subscopes.emplace_back(std::unique_ptr<scope>(scope_ptr));
  } else {
    scope_ptr = current->find_scope(name_ref.text());

    if (!scope_ptr) {
      if (flags & SCOPE_CREATE) {
        scope_ptr = new scope(current, name_ref, flags);
        current->_visible_subscopes.emplace_back(
          std::unique_ptr<scope>(scope_ptr));
      }
    } else if (flags & SCOPE_EXCLUSIVE) {
      oops(
        "e@1 redefinition of '$1'"
        "n@2 first defined here...",
        name_ref,
        scope_ptr->_name_ref);
    }
  }

  if (scope_ptr) {
    current = scope_ptr;
  }

  return scope_ptr;
}

scope *scope::open(int flags) {
  return open(token_ref(), flags);
}

scope *scope::close() {
  scope *scope_ptr = current->up();

  if (scope_ptr) {
    current = scope_ptr;
  }

  return current;
}

scope *scope::find_scope(std::string name) const {
  for (auto &scope_ptr : _visible_subscopes) {
    if (scope_ptr->name() == name) {
      return scope_ptr.get();
    }
  }
  return nullptr;
}

scope *scope::find_ident(std::string name) const {
  for (auto &scope_ptr : _identifiers) {
    if (scope_ptr->name() == name) {
      return scope_ptr.get();
    }
  }
  return find_scope(name);
}

scope *scope::resolve(std::vector<std::string> namespaces, scope *scope_ptr) {
  size_t nr_namespaces = namespaces.size();

  for (size_t i = 0, j = nr_namespaces; i < nr_namespaces; ++i, --j) {
    auto name = namespaces[i];
    if (j > 1) {
      scope_ptr = scope_ptr->find_scope(name);
    } else {
      scope_ptr = scope_ptr->find_ident(name);
    }

    if (!scope_ptr) {
      break;
    }
  }

  return scope_ptr;
}

std::vector<std::string> scope::divide_namespaces(std::string ident) {
  // break up a scope string containing something like "A::B::C"
  // into a string vector containing "A", "B", and "C".
  std::vector<std::string> namespaces;
  std::string separator = "::";
  size_t strpos = 0;

  while (ident.size() > 0) {
    strpos = ident.find(separator);

    if (strpos != std::string::npos) {
      if (strpos > 0) {
        namespaces.push_back(ident.substr(0, strpos));
      }
      ident = ident.substr(strpos + 2);
    } else {
      namespaces.push_back(ident);
      break;
    }
  }

  return namespaces;
}

scope *scope::resolve(std::string ident, scope *scope_ptr) {
  // if the identifier leads with "::" then we need to search for the
  // identifier from global scope, not the scope where the reference
  // was made so we override scope_ptr before performing the search.
  if (ident.size() > 1) {
    if (ident[0] == ':' && ident[1] == ':') {
      scope_ptr = global.get();
    }
  }

  auto namespaces = divide_namespaces(ident);
  auto scope_itr = scope_ptr;

  while (scope_itr) {
    scope_ptr = resolve(namespaces, scope_itr);

    if (scope_ptr) {
      break;
    }

    scope_itr = scope_itr->up();
  }

  return scope_ptr;
}

} // namespace dwt
