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

code_obj::code_obj()
  : _token_map(new map_obj) {
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
  _token_map->mark_as(MARK_GREY);
  _token_map->blacken();
}

std::string code_obj::to_string() {
  return "<code>";
}

void code_obj::emit(uint8_t octet) {
  _bytes.push_back(octet);
}

void code_obj::emit(uint8_t octet, token_ref t) {
  if (t.type() != TOK_INV) {
    _token_map->op_keyset(NUM_AS_VAR(_bytes.size()), ffi::any(t));
  }
  emit(octet);
}

void code_obj::token_at(size_t idx, token_ref t) {
  _token_map->op_keyset(NUM_AS_VAR(idx), ffi::any(t));
}

token_ref code_obj::token_at(size_t idx) {
  token_ref t;

  var value = _token_map->op_keyget(NUM_AS_VAR(idx));

  if (!VAR_IS_NIL(value)) {
    t = *std::reinterpret_pointer_cast<token_ref>(ffi::unbox(value));
  }

  return t;
}

void code_obj::unmap_token_at(size_t idx) {
  _token_map->op_keyset(NUM_AS_VAR(idx), nil);
}

} // namespace dwt
