// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ir/subscript_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::subscript_expr::subscript_expr(std::unique_ptr<expr> e,
                                   std::unique_ptr<expr> i) {
  splice(std::move(e));
  splice(std::move(i));
}

ir::subscript_expr::subscript_expr() {
}

ir::subscript_expr::~subscript_expr() {
}

void ir::subscript_expr::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
