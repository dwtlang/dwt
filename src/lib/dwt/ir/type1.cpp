// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/type1.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::type1::type1(expr *e) {
  splice(e);
  std::string name = "__~" + std::to_string(id());
  set_name(token_ref(name));
}

ir::type1::type1() {
  std::string name = "__~" + std::to_string(id());
  set_name(token_ref(name));
}

ir::type1::~type1() {
}

void ir::type1::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
