// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/condvar_obj.hpp>

#include <string>

namespace dwt {

condvar_obj::condvar_obj() {
}

condvar_obj::~condvar_obj() {
}

obj_type condvar_obj::type() {
  return OBJ_CONDVAR;
}

obj *condvar_obj::clone() {
  return this;
}

void condvar_obj::mark_immutable() {
  _immutable = true;
}

void condvar_obj::blacken() {
}

std::string condvar_obj::to_string() {
  return "<condvar>";
}

} // namespace dwt
