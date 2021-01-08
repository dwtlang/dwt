// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/visitor.hpp>
#include <dwt/ir/yield_stmt.hpp>

namespace dwt {

ir::yield_stmt::yield_stmt(token_ref tok) {
  set_name(tok);
}

ir::yield_stmt::yield_stmt() {
}

ir::yield_stmt::~yield_stmt() {
}

void ir::yield_stmt::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
