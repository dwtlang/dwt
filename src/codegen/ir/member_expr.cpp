// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/member_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::member_expr::member_expr(std::shared_ptr<expr> prec_expr, token_ref tok) {
  set_name(tok);
  splice(prec_expr);
}

ir::member_expr::member_expr() {
}

ir::member_expr::~member_expr() {
}

void ir::member_expr::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
