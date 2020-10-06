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
#include <map>
#include <memory>
#include <mutex>
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
  static std::shared_ptr<scope> current;
  static std::atomic<uint64_t> next_id;
  static std::mutex lock;
  static std::shared_ptr<scope>
  resolve(std::string, std::shared_ptr<scope> from = scope::current);

  static std::vector<std::string> split(std::string);

  scope(std::shared_ptr<scope>, token_ref, int);
  scope();
  virtual ~scope();

  std::shared_ptr<scope> up() const;
  bool is_anonymous() const;
  bool is_global() const;
  int32_t lookup() const;
  std::string name() const;
  std::string qualified_name() const;
  std::shared_ptr<scope> find_scope(std::string) const;
  std::shared_ptr<scope> find_ident(std::string) const;

  static std::shared_ptr<scope> open(token_ref, int = SCOPE_APPEND);
  static std::shared_ptr<scope> open(int = SCOPE_APPEND);
  static std::shared_ptr<scope> add(token_ref, int = SCOPE_EXCLUSIVE);
  static std::shared_ptr<scope> close();
  static std::string to_string();
  static std::string prettify_name(std::string);

private:
  static std::shared_ptr<scope>
    resolve(std::vector<std::string>, std::shared_ptr<scope> = scope::current);

  typedef std::map<std::string, std::shared_ptr<scope>> subscopes;
  std::vector<std::shared_ptr<scope>> _anonymous_subscopes;
  subscopes _visible_subscopes;
  subscopes _token_types;
  std::weak_ptr<scope> _parent_scope;
  token_ref _name_ref;
  int32_t _index;
  uint64_t _id;
};

} // namespace dwt

#endif
