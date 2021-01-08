// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/use_stmt.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::use_stmt::use_stmt(token_ref tok) {
  set_name(tok);
}

ir::use_stmt::use_stmt() {
}

ir::use_stmt::~use_stmt() {
}

void ir::use_stmt::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt