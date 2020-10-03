// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_IR_SCOPED_NAME_HPP
#define GUARD_DWT_IR_SCOPED_NAME_HPP

#include <dwt/feedback.hpp>
#include <dwt/ir/expr.hpp>
#include <dwt/token.hpp>
#include <dwt/token_range.hpp>

#include <mutex>
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

  void set_referenced_scope(std::shared_ptr<dwt::scope> s) {
    std::scoped_lock<std::mutex> hold(_namelock);
    _referenced_scope = s;
  }

  virtual std::shared_ptr<dwt::scope> get_scope() override;
  std::shared_ptr<dwt::scope> ref_scope();

private:
  std::weak_ptr<dwt::scope> _referenced_scope;
  std::mutex _namelock;
  bool _is_setter = false;
  bool abs;
  bool _is_self;
  std::string _identstr;
};

} // namespace ir
} // namespace dwt

#endif
