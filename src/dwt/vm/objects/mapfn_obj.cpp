// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/interpreter.hpp>
#include <dwt/mapfn_obj.hpp>
#include <dwt/string_obj.hpp>
#include <dwt/var.hpp>

namespace dwt {

mapfn_obj::mapfn_obj(string_obj *name)
  : function_obj(FN_MAP, 0, name) {
}

mapfn_obj::mapfn_obj(const mapfn_obj &other)
  : function_obj(other) {
}

mapfn_obj::~mapfn_obj() {
}

obj_type mapfn_obj::type() {
  return OBJ_MAPINI;
}

obj *mapfn_obj::clone() {
  return new mapfn_obj(*this);
}

void mapfn_obj::mark_immutable() {
  _immutable = true;
}

void mapfn_obj::blacken() {
}

void mapfn_obj::call(interpreter &interpreter, int nr_args) {
  interpreter.invoke(this, nr_args);
}

std::string mapfn_obj::to_string() {
  return "<obj " + name() + ">";
}

} // namespace dwt
