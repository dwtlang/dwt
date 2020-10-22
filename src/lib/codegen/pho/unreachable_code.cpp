// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/debug.hpp>
#include <dwt/feedback.hpp>
#include <dwt/pho/unreachable_code.hpp>

#define OPERAND(op) ((*(op)) | ((*((op) + 1)) << 8))

namespace dwt {
namespace pho {

namespace {

static const int offsets[] = {
#define OP(_, __, off) off,
#include <dwt/opcodes.inc>
#undef OP
};

} // namespace

unreachable_code::unreachable_code(code_obj &code)
  : peephole({ { { OP_RET }, 1 } })
  , _code(code) {

  (*this)(code);
}

unreachable_code::~unreachable_code() {
}

size_t unreachable_code::first_jump_after(size_t pos) {
  std::vector<uint8_t> &ops = _code.byte_vec();
  size_t off = 0;
  uint8_t *op = &ops[0];
  size_t jmp_off = 0;

  while (off < _code.size()) {
    uint16_t operand = 0;

    switch (*op) {
    case OP_BRA:
      operand = OPERAND(op + 1);
      break;
    case OP_BRZ:
      operand = OPERAND(op + 1);
      break;
    case OP_BNZ:
      operand = OPERAND(op + 1);
      break;
    default:
      break;
    }

    if (operand) {
      size_t joff = operand + off;

      if (joff > pos) {
        if (jmp_off == 0 || joff < jmp_off) {
          jmp_off = joff;
        }
      }
    }

    off += 1 + offsets[*op];
    op += 1 + offsets[*op];
  }

  return jmp_off;
}

void unreachable_code::peep(uint8_t *op, size_t extent) {
  std::vector<uint8_t> &ops = _code.byte_vec();
  uintptr_t pos = op - &ops[0];
  size_t jmp_off = first_jump_after(pos);

  if (jmp_off == 0) {
    while (--extent) {
      *++op = OP_SKIP;
    }
  } else {
    while (pos++ < jmp_off) {
      *++op = OP_SKIP;
    }
  }
}

} // namespace pho
} // namespace dwt
