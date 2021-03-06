// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_CODE_OBJ_HPP
#define GUARD_DWT_CODE_OBJ_HPP

#include <dwt/obj.hpp>
#include <dwt/opcode.hpp>
#include <dwt/token_ref.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace dwt {

class code_obj : public obj {
public:
  code_obj();
  code_obj(const code_obj &);
  ~code_obj();

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void blacken() override;
  virtual std::string to_string() override;

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

  void emit(uint8_t, token_ref ref = token_ref());
  void token_at(size_t, token_ref);
  token_ref token_at(size_t);
  void unmap_token_at(size_t idx);

private:
  std::vector<uint8_t> _bytes;
  std::vector<token_ref> _tokens;
};

} // namespace dwt

#endif
