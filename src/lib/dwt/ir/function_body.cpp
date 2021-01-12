// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ir/function_body.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::function_body::function_body(token_ref tok) {
  set_name(tok);
}

ir::function_body::function_body() {
}

ir::function_body::~function_body() {
}

void ir::function_body::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
