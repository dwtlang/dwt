// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/ast.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {
namespace ir {

visitor::visitor() {
}

visitor::~visitor() {
}

void visitor::visit(ir::ast &obj) {
}

} // namespace ir
} // namespace dwt
