// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/var_decl.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::var_decl::var_decl(token_ref tok) {
  set_name(tok);
}

ir::var_decl::var_decl() {
}

ir::var_decl::~var_decl() {
}

void ir::var_decl::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
