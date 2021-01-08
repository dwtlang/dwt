// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/interpreter.hpp>
#include <dwt/string_obj.hpp>
#include <dwt/syscall_obj.hpp>
#include <dwt/var.hpp>

namespace dwt {

syscall_obj::syscall_obj(ffi::syscall impl, string_obj *name)
  : _impl(impl)
  , _name(name) {
}

syscall_obj::syscall_obj(const syscall_obj &other)
  : _impl(other._impl)
  , _name(static_cast<string_obj *>(other._name->clone())) {
}

syscall_obj::~syscall_obj() {
}

obj_type syscall_obj::type() {
  return OBJ_SYSCALL;
}

obj *syscall_obj::clone() {
  return new syscall_obj(*this);
}

void syscall_obj::blacken() {
  _name->mark_as(MARK_GREY);
}

void syscall_obj::call(interpreter &interpreter, int nr_args) {
  interpreter.invoke(this, nr_args);
}

std::string syscall_obj::to_string() {
  return "<syscall " + _name->text() + ">";
}

} // namespace dwt
