// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/stmt.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::stmt::stmt() {
}

ir::stmt::~stmt() {
}

void ir::stmt::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
