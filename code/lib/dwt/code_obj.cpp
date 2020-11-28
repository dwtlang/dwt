// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/code_obj.hpp>
#include <dwt/ffi.hpp>

namespace dwt {

code_obj::code_obj() {
}

code_obj::~code_obj() {
}

code_obj::code_obj(const code_obj &other)
  : _bytes(other._bytes)
  , _token_map(other._token_map) {
}

obj_type code_obj::type() {
  return OBJ_CODE;
}

obj *code_obj::clone() {
  return new code_obj(*this);
}

void code_obj::blacken() {
  for (size_t i = 0; i < _token_map._capacity; ++i) {
    auto &entry = _token_map._buckets[i];
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

std::string code_obj::to_string() {
  return "<code>";
}

void code_obj::emit(uint8_t octet) {
  _bytes.push_back(octet);
}

void code_obj::emit(uint8_t octet, token_ref t) {
  if (t.type() != TOK_INV) {
    _token_map.add(kv_pair(NUM_AS_VAR(_bytes.size()), ffi::any(t)));
  }
  emit(octet);
}

void code_obj::token_at(size_t idx, token_ref t) {
  _token_map.add(kv_pair(NUM_AS_VAR(idx), ffi::any(t)));
}

token_ref code_obj::token_at(size_t idx) {
  kv_pair *kvp = _token_map.get(NUM_AS_VAR(idx));
  token_ref t;

  if (kvp) {
    if (!(VAR_IS_NIL(kvp->value))) {
      std::shared_ptr<void> sp;
      ffi::unbox(sp, kvp->value);
      t = *reinterpret_cast<token_ref *>(sp.get());
    }
  }

  return t;
}

void code_obj::unmap_token_at(size_t idx) {
  _token_map.add(kv_pair(NUM_AS_VAR(idx), nil));
}

} // namespace dwt
