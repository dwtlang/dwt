// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ir/lambda.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::lambda::lambda(token_ref tok) {
  set_name(tok);
}

ir::lambda::lambda() {
  std::string name = "__" + std::to_string(id());
  set_name(token_ref(name));
}

ir::lambda::~lambda() {
}

void ir::lambda::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
