// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ir/loop_stmt.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::loop_stmt::loop_stmt(token_ref tok, loop_type ltype)
  : _type(ltype) {
  set_name(tok);
}

ir::loop_stmt::loop_stmt(loop_type ltype)
  : _type(ltype) {
}

ir::loop_stmt::~loop_stmt() {
}

void ir::loop_stmt::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
