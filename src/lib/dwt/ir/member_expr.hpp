// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_IR_MEMBER_EXPR_HPP
#define GUARD_DWT_IR_MEMBER_EXPR_HPP

#include <dwt/ir/expr.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class member_expr : public expr {
public:
  member_expr(std::unique_ptr<expr>, token_ref);
  member_expr();
  virtual ~member_expr();
  virtual void accept(ir::visitor &visitor);

  void is_setter(bool cond) {
    _is_setter = cond;
  }

  bool is_setter() const {
    return _is_setter;
  }

private:
  bool _is_setter = false;
};

} // namespace ir
} // namespace dwt

#endif
