// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/unary_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::unary_expr::unary_expr(std::unique_ptr<expr> e, token_ref tok) {
  splice(std::move(e));
  set_name(tok);
}

ir::unary_expr::unary_expr() {
}

ir::unary_expr::~unary_expr() {
}

void ir::unary_expr::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
