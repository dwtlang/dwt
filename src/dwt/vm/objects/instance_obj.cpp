// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/instance_obj.hpp>
#include <dwt/interpreter.hpp>

#include <iomanip>

namespace dwt {

instance_obj::instance_obj(class_obj *klass)
  : _super(nullptr)
  , _klass(klass) {
}

instance_obj::instance_obj(const instance_obj &other)
  : map_obj(other) {

  if (other._super) {
    _super = static_cast<instance_obj *>(other._super->clone());
  } else {
    _super = nullptr;
  }

  if (other._klass) {
    _klass = static_cast<class_obj *>(other._klass->clone());
  } else {
    _klass = nullptr;
  }
}

instance_obj::~instance_obj() {
}

obj_type instance_obj::type() {
  return OBJ_INSTANCE;
}

obj *instance_obj::clone() {
  return new instance_obj(*this);
}

void instance_obj::mark_immutable() {
  _immutable = true;
  if (_super) {
    _super->mark_immutable();
  }
  _klass->mark_immutable();
}

void instance_obj::super(instance_obj *s) {
  _super = s;
}

instance_obj *instance_obj::super() const {
  return _super;
}

std::string instance_obj::to_string() {
  return _klass->to_string();
}

void instance_obj::blacken() {
  if (_super) {
    _super->mark_as(MARK_GREY);
  }
  _klass->mark_as(MARK_GREY);

  map_obj::blacken();
}

void instance_obj::call(interpreter &interpreter, int nr_args) {
  interpreter.invoke(this, nr_args);
}

var instance_obj::op_mbrget(var key) {
  auto kv = _map.get(key);

  if (kv) {
    return kv->value;
  } else if (_super) {
    return _super->op_mbrget(key);
  }

  return nil;
}

void instance_obj::op_mbrset(var key, var value) {
  auto kv = _map.get(key);

  if (kv) {
    kv->value = value;
  } else if (_super) {
    _super->op_mbrset(key, value);
  }
}

} // namespace dwt
