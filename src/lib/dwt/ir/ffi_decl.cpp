// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ir/ffi_decl.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::ffi_decl::ffi_decl(token_ref tok) {
  set_name(tok);
}

ir::ffi_decl::ffi_decl() {
}

ir::ffi_decl::~ffi_decl() {
}

void ir::ffi_decl::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
