// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/upvar_obj.hpp>
#include <dwt/var.hpp>

namespace dwt {

upvar_obj::upvar_obj(stack<var> *stack, size_t offset, size_t slot)
  : _stack(stack)
  , _offset(offset)
  , _slot(slot)
  , _next_upvar(nullptr) {
}

upvar_obj::upvar_obj(const upvar_obj &other)
  : _stack(other._stack)
  , _closed(other._closed)
  , _offset(other._offset)
  , _slot(other._slot)
  , _next_upvar(nullptr) {
}

upvar_obj::~upvar_obj() {
}

obj_type upvar_obj::type() {
  return OBJ_UPVALUE;
}

obj *upvar_obj::clone() {
  return new upvar_obj(*this);
}

void upvar_obj::blacken() {
  mark_as(MARK_GREY);

  if (VAR_IS_OBJ(_closed)) {
    obj *o = VAR_AS_OBJ(_closed);
    if (o) {
      o->mark_as(MARK_GREY);
    }
  }
}

std::string upvar_obj::to_string() {
  if (_stack) {
    return var_to_string(_stack->get(_offset + _slot));
  }
  return var_to_string(_closed);
}

} // namespace dwt
