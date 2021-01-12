// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_IR_ASSIGN_EXPR_HPP
#define GUARD_DWT_IR_ASSIGN_EXPR_HPP

#include <dwt/ir/expr.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class assign_expr : public expr {
public:
  assign_expr(token_ref tok);
  assign_expr();

  virtual ~assign_expr();

  virtual void accept(ir::visitor &visitor);

  token_ref gettok();

private:
  token_ref _tok;
};

} // namespace ir
} // namespace dwt

#endif
