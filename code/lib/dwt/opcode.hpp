// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_OPCODE_HPP
#define GUARD_DWT_OPCODE_HPP

#include <cstdint>

namespace dwt {

enum opcodes : uint8_t {
#define OP(op, _, __) OP_##op,
#include <dwt/opcodes.inc>
#undef OP
};

typedef uint8_t opcode;

const char *decode(opcode);

int opcode_operand_bytes(opcode op);
int opcode_stack_effect(opcode op);

} // namespace dwt

#endif
