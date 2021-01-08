// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/opcode.hpp>

namespace dwt {

const char *decode(opcode opcode) {

  static const char *op_str[] = {
#define OP(op, _, __) #op,
#include <dwt/opcodes.inc>
#undef OP
  };

  return opcode[op_str];
}

int opcode_operand_bytes(opcode op) {
  static const int operand_bytes[] = {
#define OP(_, __, operbytes) operbytes,
#include <dwt/opcodes.inc>
#undef OP
  };
  return operand_bytes[op];
}

int opcode_stack_effect(opcode op) {
  static const int stack_effect[] = {
#define OP(_, effect, __) effect,
#include <dwt/opcodes.inc>
#undef OP
  };
  return stack_effect[op];
}

} // namespace dwt
