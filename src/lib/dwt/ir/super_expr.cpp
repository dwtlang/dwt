// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/super_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::super_expr::super_expr(token_ref tok) {
  set_name(tok);
}

ir::super_expr::super_expr() {
}

ir::super_expr::~super_expr() {
}

void ir::super_expr::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
