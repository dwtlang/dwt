// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ir/mult_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {
namespace ir {

mult_expr::mult_expr(std::unique_ptr<expr> lhs,
                     std::unique_ptr<expr> rhs,
                     token_ref tok)
  : _tok(tok) {
  splice(std::move(lhs));
  splice(std::move(rhs));
}

mult_expr::~mult_expr() {
}

void mult_expr::accept(visitor &visitor) {
  visitor.visit(*this);
}

token_ref mult_expr::gettok() {
  return _tok;
}

} // namespace ir
} // namespace dwt
