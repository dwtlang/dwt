// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/is_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {
namespace ir {

is_expr::is_expr(std::shared_ptr<expr> lhs,
                 std::shared_ptr<expr> rhs,
                 token_ref tok) {
  splice(lhs);
  splice(rhs);
  set_name(tok);
}

is_expr::~is_expr() {
}

void is_expr::accept(visitor &visitor) {
  visitor.visit(*this);
}

} // namespace ir
} // namespace dwt
