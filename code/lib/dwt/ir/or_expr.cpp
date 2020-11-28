// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/or_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::or_expr::or_expr(ir::expr *lhs, ir::expr *rhs, token_ref tok) {
  splice(lhs);
  splice(rhs);
  set_name(tok);
}

ir::or_expr::~or_expr() {
}

void ir::or_expr::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
