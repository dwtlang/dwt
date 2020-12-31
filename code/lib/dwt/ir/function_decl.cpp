// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/function_decl.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::function_decl::function_decl(token_ref tok) {
  set_name(tok);
}

ir::function_decl::function_decl() {
}

ir::function_decl::~function_decl() {
}

void ir::function_decl::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

void ir::function_decl::arity(size_t arity) {
  _arity = arity;
}

size_t ir::function_decl::arity() const {
  return _arity;
}

} // namespace dwt
