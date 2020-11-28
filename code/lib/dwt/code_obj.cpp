// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/code_obj.hpp>

namespace dwt {

code_obj::code_obj() {
}

code_obj::~code_obj() {
}

code_obj::code_obj(const code_obj &other)
  : _bytes(other._bytes)
  , _tokens(other._tokens) {
}

obj_type code_obj::type() {
  return OBJ_CODE;
}

obj *code_obj::clone() {
  return new code_obj(*this);
}

void code_obj::blacken() {
}

std::string code_obj::to_string() {
  return "<code>";
}

void code_obj::emit(uint8_t octet, token_ref t) {
  _tokens.push_back(t);
  _bytes.push_back(octet);
}

void code_obj::token_at(size_t idx, token_ref t) {
  _tokens[idx] = t;
}

token_ref code_obj::token_at(size_t idx) {
  return _tokens[idx];
}

void code_obj::unmap_token_at(size_t idx) {
  _tokens[idx] = token_ref();
}

} // namespace dwt
