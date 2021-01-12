// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/debug.hpp>
#include <dwt/peephole.hpp>

#define OPERAND(op) ((*(op)) | ((*((op) + 1)) << 8))

namespace dwt {

peephole::peephole(std::vector<ph_pattern> patterns)
  : _patterns(patterns)
  , _off(0) {
}

peephole::~peephole() {
}

bool peephole::jumps_into_range(code_obj &code, size_t off, size_t extent) {
  uint8_t *ops = code.entry();
  size_t pos = 0;
  uint16_t jmpoff;

  while (pos < code.size()) {
    switch (ops[pos]) {
    case OP_LOOP:
      jmpoff = OPERAND(&ops[pos + 1]);
      jmpoff = (pos + 1) - jmpoff;
      if ((jmpoff > off) && jmpoff < (off + extent)) {
        return true;
      }
      break;
    case OP_BRA:
      jmpoff = OPERAND(&ops[pos + 1]);
      jmpoff += pos;
      if (jmpoff > off && jmpoff < (off + extent)) {
        return true;
      }
      break;
    case OP_BRZ:
      jmpoff = OPERAND(&ops[pos + 1]);
      jmpoff += pos;
      if (jmpoff > off && jmpoff < (off + extent)) {
        return true;
      }
      break;
    case OP_BNZ:
      jmpoff = OPERAND(&ops[pos + 1]);
      jmpoff += pos;
      if (jmpoff > off && jmpoff < (off + extent)) {
        return true;
      }
      break;
    default:
      break;
    }
    pos += 1 + opcode_operand_bytes(ops[pos]);
  }

  return false;
}

void peephole::operator()(code_obj &code) {
  std::vector<uint8_t> &ops = code.byte_vec();

  for (size_t i = 0; i < _patterns.size(); ++i) {
    _off = 0;
    while (scan(ops, i)) {
      size_t off = _off - _patterns[i].extent;
      if (!jumps_into_range(code, off, _patterns[i].extent)) {
        peep(&ops[off], code.size() - off);
      }
    }
  }
}

bool peephole::scan(std::vector<uint8_t> &code, int idx) {

  size_t seqoff = 0;

  while (seqoff < _patterns[idx].ops.size() && _off < code.size()) {
    if (code[_off] == _patterns[idx].ops[seqoff]) {
      uint8_t *op = &code[_off];
      _off += 1 + opcode_operand_bytes(*op);
      ++seqoff;
    } else if (code[_off] == OP_SKIP) {
      ++_off;
    } else {
      _off += 1 + opcode_operand_bytes(code[_off]);
      seqoff = 0;
    }
  }

  return seqoff == _patterns[idx].ops.size();
}

} // namespace dwt
