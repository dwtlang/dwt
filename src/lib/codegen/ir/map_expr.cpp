// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/map_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::map_expr::map_expr() {
  std::string name = "__" + std::to_string(id());
  set_name(token_ref(name));
}

ir::map_expr::~map_expr() {
}

void ir::map_expr::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
