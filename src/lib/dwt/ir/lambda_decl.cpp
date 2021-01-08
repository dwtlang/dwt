// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/lambda_decl.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::lambda_decl::lambda_decl(token_ref tok) {
  set_name(tok);
}

ir::lambda_decl::lambda_decl() {
  std::string name = "__" + std::to_string(id());
  set_name(token_ref(name));
}

ir::lambda_decl::~lambda_decl() {
}

void ir::lambda_decl::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
