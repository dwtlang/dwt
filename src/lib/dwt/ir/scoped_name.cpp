// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <cassert>
#include <dwt/ir/scoped_name.hpp>
#include <dwt/ir/visitor.hpp>
#include <dwt/token.hpp>

namespace dwt {

ir::scoped_name::scoped_name()
  : abs(false)
  , _is_self(false) {
}

ir::scoped_name::~scoped_name() {
}

bool ir::scoped_name::is_abs() const {
  return abs;
}

void ir::scoped_name::is_abs(bool cond) {
  abs = cond;
}

void ir::scoped_name::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

dwt::scope *ir::scoped_name::get_scope() {
#if USE_THREADED_COMPILER
  std::scoped_lock hold(_mutex);
#endif

  dwt::scope *scope_ptr = _referenced_scope;

  if (!scope_ptr) {
    scope_ptr = dwt::scope::resolve(_identstr, ref_scope());
    _referenced_scope = scope_ptr;
  }

  return scope_ptr;
}

dwt::scope *ir::scoped_name::ref_scope() {
  return ast::get_scope();
}

} // namespace dwt
