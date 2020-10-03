// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/print_stmt.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::print_stmt::print_stmt(token_ref tok) {
  set_name(tok);
}

ir::print_stmt::print_stmt() {
}

ir::print_stmt::~print_stmt() {
}

void ir::print_stmt::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
