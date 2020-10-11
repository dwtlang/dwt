// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/feedback.hpp>
#include <dwt/globals.hpp>
#include <dwt/reporting.hpp>
#include <dwt/scope.hpp>

#include <regex>

namespace dwt {

std::shared_ptr<scope> scope::global = std::make_shared<scope>();
std::shared_ptr<scope> scope::current = scope::global;
std::atomic<uint64_t> scope::next_id = 0;
std::mutex scope::lock;

scope::scope(std::shared_ptr<scope> parent_scope, token_ref name_ref, int flags)
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
  : _index(globals::table().add_r("::")) {
}

scope::~scope() {
}

std::string scope::to_string() {
  auto s = scope::current;
  return s->qualified_name();
}

std::shared_ptr<scope> scope::up() const {
  return _parent_scope.lock();
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
    s = s->up().get();
  }

  return isglob;
}

int32_t scope::lookup() const {
  return _index;
}

/*
 * Get the name associated with the given scope. Anonymous scope names are
 * automatically generated based on a unique id. This ensures that calls
 * to qualified_name() will always return a unique string. The compiler
 * relies on this property because it uses these strings to resolve local
 * and global variables.
 */
std::string scope::name() const {
  if (is_anonymous() && up()) {
    return "<anon" + std::to_string(_id) + ">";
  }
  return _name_ref.text();
}

std::string scope::qualified_name() const {
  std::shared_ptr<scope> s = _parent_scope.lock();
  std::string qualified = name();

  if (!s) {
    qualified = "::";
  }

  while (s) {
    qualified = s->name() + "::" + qualified;
    s = s->up();
  }

  return qualified;
}

std::string scope::prettify_name(std::string name) {
  std::regex re("<anon[[:digit:]]+>::");
  return std::regex_replace(name, re, "");
}

std::shared_ptr<scope> scope::add(token_ref name_ref, int flags) {
  std::shared_ptr<scope> s;
  std::string name = name_ref.text();

  s = std::make_shared<scope>(scope::current, name_ref, flags);
  auto p = std::make_pair(name, s);

  if (!scope::current->_token_types.insert(p).second) {
    if (flags & SCOPE_EXCLUSIVE) {
      auto prev_scope = scope::current->_token_types.find(name);
      if (prev_scope != scope::current->_token_types.end()) {
        oops(
          "e@1 redefinition of '$1'"
          "n@2 first defined here...",
          name_ref,
          prev_scope->second->_name_ref);
      } else {
        BUG();
      }
    }
  }

  return s;
}

std::shared_ptr<scope> scope::open(int flags) {
  return open(token_ref(), flags);
}

std::shared_ptr<scope> scope::open(token_ref name_ref, int flags) {
  std::shared_ptr<scope> s;
  std::string name = name_ref.text();

  if (flags & SCOPE_ANONYMOUS) {
    s = std::make_shared<scope>(scope::current, name_ref, flags);
    scope::current->_anonymous_subscopes.push_back(s);
  } else {
    s = scope::current->find_scope(name);

    if (s) {
      if (flags & SCOPE_EXCLUSIVE) {
        oops(
          "e@1 redefinition of '$1'"
          "n@2 first defined here...",
          name_ref,
          s->_name_ref);
      }
    } else if (flags & SCOPE_CREATE) {
      s = std::make_shared<scope>(scope::current, name_ref, flags);
      auto p = std::make_pair(name, s);

      if (!scope::current->_token_types.insert(p).second) {
        if (flags & SCOPE_EXCLUSIVE) {
          auto prev_scope = scope::current->_token_types.find(name);
          if (prev_scope != scope::current->_token_types.end()) {
            oops(
              "e@1 redefinition of '$1'"
              "n@2 first defined here...",
              name_ref,
              prev_scope->second->_name_ref);
          } else {
            BUG();
          }
        }
      }

      scope::current->_visible_subscopes.insert(p);
    }
  }

  if (s) {
    scope::current = s;
  } else {
    BUG();
  }

  return s;
}

std::shared_ptr<scope> scope::close() {
  std::shared_ptr<scope> s = scope::current->up();

  if (s) {
    scope::current = s;
  }

  return scope::current;
}

std::shared_ptr<scope> scope::find_scope(std::string name) const {
  auto itr = _visible_subscopes.find(name);
  std::shared_ptr<scope> s;

  if (itr != _visible_subscopes.end()) {
    s = itr->second;
  }

  return s;
}

std::shared_ptr<scope> scope::find_ident(std::string name) const {
  auto itr = _token_types.find(name);
  std::shared_ptr<scope> s;

  if (itr != _token_types.end()) {
    s = itr->second;
  }

  return s;
}

std::shared_ptr<scope> scope::resolve(std::vector<std::string> scope_names,
                                      std::shared_ptr<scope> s) {
  size_t nr_scopes = scope_names.size();

  for (size_t i = 0, j = nr_scopes; i < nr_scopes; ++i, --j) {
    auto name = scope_names[i];
    if (j > 1) {
      s = s->find_scope(name);
    } else {
      s = s->find_ident(name);
    }

    if (!s) {
      break;
    }
  }

  return s;
}

std::vector<std::string> scope::split(std::string str) {
  std::vector<std::string> substrs;
  std::string sep = "::";
  size_t pos = 0;

  while (str.size()) {
    if ((pos = str.find(sep)) != std::string::npos) {
      if (pos > 1) {
        substrs.push_back(str.substr(0, pos));
      }
      str = str.substr(pos + sep.size());
      if (!str.size()) {
        substrs.push_back(str);
      }
    } else {
      substrs.push_back(str);
      break;
    }
  }

  return substrs;
}

std::shared_ptr<scope> scope::resolve(std::string ident,
                                      std::shared_ptr<scope> s) {
  if (ident.size() > 1) {
    if (ident[0] == ':' && ident[1] == ':') {
      s = scope::global;
      ident = ident.substr(2, ident.size());
    }
  }

  auto scope_names = split(ident);
  auto current_scope = s;

  while (current_scope) {
    if ((s = resolve(scope_names, current_scope))) {
      break;
    }

    current_scope = current_scope->up();
  }

  return s;
}

} // namespace dwt
