// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/type4.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::type4::type4(std::shared_ptr<expr> v) {
  splice(v);
}

ir::type4::~type4() {
}

void ir::type4::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
