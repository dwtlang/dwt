// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/box_obj.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/string_obj.hpp>
#include <dwt/var.hpp>

namespace dwt {

box_obj::box_obj(std::shared_ptr<void> boxed_obj)
  : _boxed_obj(boxed_obj) {
}

box_obj::box_obj(const box_obj &other)
  : _boxed_obj(other._boxed_obj) {
}

box_obj::~box_obj() {
}

obj_type box_obj::type() {
  return OBJ_BOX;
}

obj *box_obj::clone() {
  return new box_obj(*this);
}

void box_obj::mark_immutable() {
  _immutable = true;
}

void box_obj::blacken() {
}

std::string box_obj::to_string() {
  return "<box>";
}

std::shared_ptr<void> box_obj::contents() {
  return _boxed_obj;
}

} // namespace dwt
