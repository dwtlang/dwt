// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/declaration.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::declaration::declaration(token_ref tok)
  : _index_of(0)
  , _is_global(false)
  , _is_constant(false) {
  set_name(tok);
}

ir::declaration::declaration() {
}

ir::declaration::~declaration() {
}

void ir::declaration::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

bool ir::declaration::is_constant() const {
  return _is_constant;
}

void ir::declaration::is_global(bool is_global) {
  _is_global = is_global;
}

void ir::declaration::is_constant(bool is_constant) {
  _is_constant = is_constant;
}

void ir::declaration::index_of(size_t index_of) {
  _index_of = index_of;
}

size_t ir::declaration::index_of() const {
  return _index_of;
}

} // namespace dwt
