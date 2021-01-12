// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ir/type4.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::type4::type4(expr *v) {
  splice(v);
}

ir::type4::~type4() {
}

void ir::type4::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
