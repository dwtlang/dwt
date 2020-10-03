// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/numeric_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {
namespace ir {

numeric_expr::numeric_expr(token_ref tok)
  : primary_expr(tok) {

  _val = std::stod(tok.text());
}

numeric_expr::numeric_expr(double num)
  : primary_expr() {

  _val = num;
}

numeric_expr::~numeric_expr() {
}

void numeric_expr::accept(visitor &visitor) {
  visitor.visit(*this);
}

double numeric_expr::val() const {
  return _val;
}

} // namespace ir
} // namespace dwt
