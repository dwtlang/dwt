// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_NUMERIC_EXPR_HPP
#define GUARD_DWT_IR_NUMERIC_EXPR_HPP

#include <dwt/ir/primary_expr.hpp>
#include <dwt/token.hpp>

namespace dwt {
namespace ir {

class numeric_expr : public primary_expr {
public:
  numeric_expr(token_ref tok);
  numeric_expr(double num);

  virtual ~numeric_expr();
  virtual void accept(ir::visitor &visitor);

  double val() const;

private:
  double _val;
};

} // namespace ir
} // namespace dwt

#endif
