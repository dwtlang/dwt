// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_SCOPE_HPP
#define GUARD_DWT_SCOPE_HPP

#include <dwt/token_ref.hpp>

#include <atomic>
#include <memory>
#include <vector>

namespace dwt {

enum scope_flags {
  SCOPE_APPEND = 1 << 0,
  SCOPE_CREATE = 1 << 1,
  SCOPE_EXCLUSIVE = 1 << 2,
  SCOPE_ANONYMOUS = 1 << 3
};

class scope {
public:
  static std::shared_ptr<scope> global;
  static scope *current;

  static scope *resolve(std::string, scope * = scope::current);
  static std::vector<std::string> divide_namespaces(std::string);
  static scope *open(token_ref, int = SCOPE_APPEND);
  static scope *open(int = SCOPE_APPEND);
  static scope *add(token_ref, int = SCOPE_EXCLUSIVE);
  static scope *close();
  static std::string to_string();

  scope(scope *, token_ref, int);
  scope();
  virtual ~scope();

  scope *up() const;
  bool is_anonymous() const;
  bool is_global() const;
  int32_t lookup() const;

  std::string name() const;
  std::string qualified_name() const;
  scope *find_scope(std::string) const;
  scope *find_ident(std::string) const;

private:
  static scope *resolve(std::vector<std::string>, scope * = scope::current);
  static std::atomic<uint64_t> next_id;

  std::vector<std::unique_ptr<scope>> _visible_subscopes;
  std::vector<std::unique_ptr<scope>> _private_subscopes;
  std::vector<std::unique_ptr<scope>> _identifiers;
  scope *_parent_scope;
  token_ref _name_ref;
  int32_t _index;
  uint64_t _id;
};

} // namespace dwt

#endif
