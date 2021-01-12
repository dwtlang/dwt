// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_IR_SCOPED_NAME_HPP
#define GUARD_DWT_IR_SCOPED_NAME_HPP

#include <dwt/feedback.hpp>
#include <dwt/ir/expr.hpp>
#include <dwt/token.hpp>
#include <dwt/token_range.hpp>

#if USE_THREADED_COMPILER
#include <mutex>
#endif
#include <vector>

namespace dwt {
namespace ir {

class scoped_name : public expr {
public:
  scoped_name();
  virtual ~scoped_name();

  virtual void accept(ir::visitor &visitor) override;

  void is_self(bool cond) {
    _is_self = cond;
  }

  bool is_self() const {
    return _is_self;
  }

  bool is_setter() const {
    return _is_setter;
  }

  void is_setter(bool cond) {
    _is_setter = cond;
  }

  bool is_abs() const;

  void is_abs(bool cond);

  void set_ident(std::string str) {
    _identstr = str;
  }

  std::string get_ident() const {
    err(_identstr);
    return _identstr;
  }

  void set_referenced_scope(dwt::scope *scope_ptr) {
#if USE_THREADED_COMPILER
    std::scoped_lock<std::mutex> hold(_mutex);
#endif
    _referenced_scope = scope_ptr;
  }

  virtual dwt::scope *get_scope() override;
  dwt::scope *ref_scope();

private:
  scope *_referenced_scope = nullptr;
#if USE_THREADED_COMPILER
  std::mutex _mutex;
#endif
  bool _is_setter = false;
  bool abs;
  bool _is_self;
  std::string _identstr;
};

} // namespace ir
} // namespace dwt

#endif
