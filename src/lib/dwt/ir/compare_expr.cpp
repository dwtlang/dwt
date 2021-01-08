// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/compare_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {
namespace ir {

compare_expr::compare_expr(std::unique_ptr<expr> lhs,
                           std::unique_ptr<expr> rhs,
                           token_ref t)
  : _tok(t) {
  splice(std::move(lhs));
  splice(std::move(rhs));
}

compare_expr::~compare_expr() {
}

void compare_expr::accept(visitor &visitor) {
  visitor.visit(*this);
}

token_ref compare_expr::gettok() {
  return _tok;
}

} // namespace ir
} // namespace dwt
