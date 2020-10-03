// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/class_obj.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/string_obj.hpp>
#include <dwt/var.hpp>

namespace dwt {

class_obj::class_obj(size_t arity, string_obj *name)
  : function_obj(FN_OBJECT, arity, name) {
}

class_obj::class_obj(const class_obj &other)
  : function_obj(other) {
}

class_obj::~class_obj() {
}

obj_type class_obj::type() {
  return OBJ_CLASS;
}

obj *class_obj::clone() {
  return new class_obj(*this);
}

void class_obj::mark_immutable() {
  _immutable = true;
}

void class_obj::blacken() {
  function_obj::blacken();
}

void class_obj::call(interpreter &interpreter, int nr_args) {
  interpreter.invoke(this, nr_args);
}

std::string class_obj::to_string() {
  return "<obj " + name() + ">";
}

} // namespace dwt
