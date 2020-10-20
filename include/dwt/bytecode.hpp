// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_BYTECODE_HPP
#define GUARD_DWT_BYTECODE_HPP

#include <dwt/opcode.hpp>
#include <dwt/token_ref.hpp>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace dwt {

class bytecode {
public:
  bytecode();
  ~bytecode();

  inline opcode *base() {
    return &_bytes[0];
  }

  size_t offset(opcode *ip) {
    return ip - base();
  }

  uint8_t *addr_at(size_t off) {
    return &_bytes[off];
  }

  opcode *entry() {
    return base();
  }

  size_t size() {
    return _bytes.size();
  }

  opcode opcode_at(size_t off) {
    return _bytes.at(off);
  }

  void compact() {
    _bytes.shrink_to_fit();
  }

  std::vector<uint8_t> &byte_vec() {
    return _bytes;
  }

  void emit(uint8_t);
  void emit(uint8_t, token_ref);
  void token_at(size_t, token_ref);
  token_ref token_at(size_t);
  void unmap_token_at(size_t idx);

private:
  std::vector<uint8_t> _bytes;
  std::unordered_map<size_t, token_ref> _tokens;
};

} // namespace dwt

#endif
