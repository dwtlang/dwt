// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/interpreter.hpp>
#include <dwt/map_obj.hpp>

namespace dwt {

map_obj::map_obj() {
}

map_obj::map_obj(const map_obj &other) {
  for (size_t i = 0; i < other._map._capacity; ++i) {
    auto &entry = other._map._buckets[i];

    if (entry.key != nil) {
      var key, value;

      if (VAR_IS_OBJ(entry.key)) {
        key = OBJ_AS_VAR(VAR_AS_OBJ(entry.key)->clone());
      } else {
        key = entry.key;
      }

      if (VAR_IS_OBJ(entry.value)) {
        value = OBJ_AS_VAR(VAR_AS_OBJ(entry.value)->clone());
      } else {
        value = entry.value;
      }

      op_keyset(key, value);
    }
  }
}

map_obj::~map_obj() {
}

obj_type map_obj::type() {
  return OBJ_MAP;
}

obj *map_obj::clone() {
  return new map_obj(*this);
}

std::string map_obj::to_string() {
  return "<map>";
}

void map_obj::blacken() {
  for (size_t i = 0; i < _map._capacity; ++i) {
    auto &entry = _map._buckets[i];
    if (entry.key != nil) {
      if (VAR_IS_OBJ(entry.key)) {
        VAR_AS_OBJ(entry.key)->mark_as(MARK_GREY);
      }
      if (VAR_IS_OBJ(entry.value)) {
        VAR_AS_OBJ(entry.value)->mark_as(MARK_GREY);
      }
    }
  }
}

void map_obj::op_keyset(var key, var val) {
  _map.add(kv_pair(key, val));
}

var map_obj::op_keyget(var key) {
  auto kv = _map.get(key);

  if (kv) {
    return kv->value;
  } else {
    return nil;
  }
}

size_t map_obj::length() {
  return _map.size();
}

} // namespace dwt
