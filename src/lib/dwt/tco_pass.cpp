// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/function_obj.hpp>
#include <dwt/globals.hpp>
#include <dwt/tco_pass.hpp>

#define OPERAND(op) ((*(op)) | ((*((op) + 1)) << 8))

namespace dwt {

tco_pass::tco_pass(code_obj &code)
  : peephole({ { { OP_CALL, OP_RET }, 3 } })
  , _code(code) {

  (*this)(code);
}

tco_pass::~tco_pass() {
}

uint8_t *tco_pass::prev_op(uint8_t *this_op) {
  uint8_t *op = _code.entry();
  uint8_t *prev_op = nullptr;

  while (op < this_op) {
    prev_op = op;
    op += 1 + opcode_operand_bytes(*op);
  }

  return prev_op;
}

void tco_pass::peep(uint8_t *op, size_t extent) {
  static int op_eff[] = {
#define OP(_, eff, __) eff,
#include <dwt/opcodes.inc>
#undef OP
  };

  int nr_args = op[1];
  int sp = nr_args + 1;
  uint8_t *ip = op;

  if (nr_args) {
    while (sp >= nr_args) {
      ip = prev_op(ip);
      BUG_UNLESS(ip);
      sp -= op_eff[*ip];
    }
  } else {
    ip = prev_op(ip);
  }
  BUG_UNLESS(ip);

  if (*ip != OP_GLOBAL) {
    return;
  }

  var v = globals::table().get(OPERAND(ip + 1));
  BUG_UNLESS(VAR_IS_OBJ(v));
  auto fun_obj = static_cast<function_obj *>(VAR_AS_OBJ(v));
  if (&fun_obj->code() == &_code) {
    op[0] = OP_TAILCALL;
  }
}

} // namespace dwt
