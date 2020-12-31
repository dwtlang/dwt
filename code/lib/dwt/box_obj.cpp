// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/box_obj.hpp>
#include <dwt/exception.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/string_obj.hpp>
#include <dwt/var.hpp>

namespace dwt {

box_obj::box_obj(std::shared_ptr<void> boxed_obj)
  : _sp(boxed_obj)
  , _rp(nullptr)
  , _type(BOX_SHARED) {
}

box_obj::box_obj(void *boxed_obj)
  : _rp(boxed_obj)
  , _type(BOX_RAW) {
}

box_obj::box_obj()
  : _type(BOX_EMPTY) {
}

box_obj::box_obj(const box_obj &other)
  : _sp(other._sp)
  , _rp(other._rp)
  , _type(other._type) {
}

box_obj::~box_obj() {
}

void box_obj::get_contents(std::shared_ptr<void> &sp) {
  if (_type == BOX_SHARED) {
    sp = _sp;
  } else {
    throw exception("box does not contain a shared_ptr");
  }
}

void box_obj::get_contents(void *&rp) {
  if (_type == BOX_RAW) {
    rp = _rp;
  } else {
    throw exception("box does not contain a raw pointer");
  }
}

void box_obj::set_contents(std::shared_ptr<void> &sp) {
  _sp = sp;
  _rp = nullptr;

  if (sp) {
    _type = BOX_SHARED;
  } else {
    _type = BOX_EMPTY;
  }
}

void box_obj::set_contents(void *&rp) {
  _sp = nullptr;
  _rp = rp;

  if (rp) {
    _type = BOX_RAW;
  } else {
    _type = BOX_EMPTY;
  }
}

obj_type box_obj::type() {
  return OBJ_BOX;
}

obj *box_obj::clone() {
  return new box_obj(*this);
}

void box_obj::blacken() {
}

std::string box_obj::to_string() {
  return "<box>";
}

} // namespace dwt
