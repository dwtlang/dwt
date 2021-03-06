// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ir/visitor.hpp>
#include <dwt/ir/xor_expr.hpp>

namespace dwt {

ir::xor_expr::xor_expr(std::unique_ptr<ir::expr> lhs,
                       std::unique_ptr<ir::expr> rhs,
                       token_ref tok) {
  splice(std::move(lhs));
  splice(std::move(rhs));
  set_name(tok);
}

ir::xor_expr::~xor_expr() {
}

void ir::xor_expr::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
