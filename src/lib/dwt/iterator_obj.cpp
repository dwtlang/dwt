// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ffi.hpp>
#include <dwt/iterator_obj.hpp>

namespace dwt {

iterator_obj::iterator_obj() {
}

iterator_obj::~iterator_obj() {
}

iterator_obj::iterator_obj(const iterator_obj &other) {
}

obj_type iterator_obj::type() {
  return OBJ_ITERATOR;
}

obj *iterator_obj::clone() {
  return new iterator_obj(*this);
}

void iterator_obj::blacken() {
}

std::string iterator_obj::to_string() {
  return "<iterator>";
}

} // namespace dwt
