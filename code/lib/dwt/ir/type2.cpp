// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/type2.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::type2::type2(token_ref tok) {
  set_name(tok);
}

ir::type2::type2() {
}

ir::type2::~type2() {
}

void ir::type2::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
