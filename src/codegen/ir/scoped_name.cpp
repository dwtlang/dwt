// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

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

std::shared_ptr<dwt::scope> ir::scoped_name::get_scope() {
  std::scoped_lock hold(_namelock);
  std::shared_ptr<dwt::scope> s = _referenced_scope.lock();

  if (!s) {
    s = dwt::scope::resolve(_identstr, ref_scope());
    _referenced_scope = s;
  }

  return s;
}

std::shared_ptr<dwt::scope> ir::scoped_name::ref_scope() {
  return ast::get_scope();
}

} // namespace dwt
