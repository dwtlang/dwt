// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/bytecode.hpp>

namespace dwt {

bytecode::bytecode() {
}

bytecode::~bytecode() {
}

void bytecode::emit(uint8_t octet) {
  _bytes.push_back(octet);
}

void bytecode::emit(uint8_t octet, token_ref t) {
  if (t.type() != TOK_INV) {
    _tokens.insert({ _bytes.size(), t });
  }
  emit(octet);
}

void bytecode::token_at(size_t idx, token_ref t) {
  auto existing = _tokens.find(idx);

  if (existing != _tokens.end()) {
    _tokens.erase(existing);
  }

  if (t.type() != TOK_INV) {
    _tokens.insert({ idx, t });
  }
}

token_ref bytecode::token_at(size_t idx) {
  token_ref t;

  auto itr = _tokens.find(idx);
  if (itr != _tokens.end()) {
    t = itr->second;
  }

  return t;
}

void bytecode::unmap_token_at(size_t idx) {
  auto itr = _tokens.find(idx);
  if (itr != _tokens.end()) {
    _tokens.erase(itr);
  }
}

} // namespace dwt
