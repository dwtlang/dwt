// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/script.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::script::script(token_ref tok) {
  set_name(tok);
}

ir::script::script() {
  set_name(token_ref("::"));
}

ir::script::~script() {
}

void ir::script::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
