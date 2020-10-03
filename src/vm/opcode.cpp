// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

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

} // namespace dwt
