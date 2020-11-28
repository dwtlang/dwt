// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/assign_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {
namespace ir {

assign_expr::assign_expr(token_ref tok) {
  set_name(tok);
}

assign_expr::assign_expr() {
}

assign_expr::~assign_expr() {
}

void assign_expr::accept(visitor &visitor) {
  visitor.visit(*this);
}

} // namespace ir
} // namespace dwt
