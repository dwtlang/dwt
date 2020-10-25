// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/constants.hpp>
#include <dwt/function_obj.hpp>
#include <dwt/globals.hpp>
#include <dwt/interpret_exception.hpp>
#include <dwt/pho/constant_folding.hpp>

#define OPERAND(op) ((*(op)) | ((*((op) + 1)) << 8))

namespace dwt {
namespace pho {

constant_folding::constant_folding(code_obj &code)
  : peephole({ { { OP_CONST }, 3 },
               { { OP_ZERO }, 1 },
               { { OP_ONE }, 1 },
               { { OP_TWO }, 1 },
               { { OP_NIL }, 1 },
               { { OP_TRUE }, 1 },
               { { OP_FALSE }, 1 } }) {
  do {
    _repeat = false;
    (*this)(code);
  } while (_repeat);
}

constant_folding::~constant_folding() {
}

void constant_folding::fold(uint8_t *op, size_t off, var v) {
  int remaining_bytes = off;

  if (v == NUM_AS_VAR(0.0)) {
    op[off] = OP_ZERO;
  } else if (v == NUM_AS_VAR(1.0)) {
    op[off] = OP_ONE;
  } else if (v == NUM_AS_VAR(2.0)) {
    op[off] = OP_TWO;
  } else if (v == nil) {
    op[off] = OP_NIL;
  } else if (v == VAR_TRUE) {
    op[off] = OP_TRUE;
  } else if (v == VAR_FALSE) {
    op[off] = OP_FALSE;
  } else {
    uint16_t idx = constants::table().add_r(v);
    op[off - 2] = OP_CONST;
    op[off - 1] = idx & 0xFF;
    op[off] = (idx >> 8) & 0xFF;

    if (remaining_bytes > 2) {
      remaining_bytes -= 2;
    } else {
      remaining_bytes = 0;
    }
  }

  while (remaining_bytes--) {
    *op++ = OP_SKIP;
  }

  _repeat = true;
}

void constant_folding::peep(uint8_t *op, size_t extent) {
  var x = OBJ_AS_VAR(nullptr);
  var y = OBJ_AS_VAR(nullptr);
  size_t off = 0;

  switch (op[0]) {
  case OP_CONST:
    x = constants::table().get(OPERAND(&op[1]));
    off += 3;
    break;
  case OP_ZERO:
    x = NUM_AS_VAR(0.0);
    break;
  case OP_ONE:
    x = NUM_AS_VAR(1.0);
    ++off;
    break;
  case OP_TWO:
    x = NUM_AS_VAR(2.0);
    ++off;
    break;
  case OP_NIL:
    x = nil;
    ++off;
    break;
  case OP_TRUE:
    x = VAR_TRUE;
    ++off;
    break;
  case OP_FALSE:
    x = VAR_FALSE;
    ++off;
    break;
  default:
    BUG();
    break;
  }

  while (off < extent) {
    if (op[off] == OP_SKIP) {
      ++off;
    } else {
      break;
    }
  }

  if (off == extent) {
    return;
  }

  try {
    switch (op[off]) {
    case OP_CONST:
      y = constants::table().get(OPERAND(&op[off + 1]));
      off += 3;
      break;
    case OP_ZERO:
      y = NUM_AS_VAR(0.0);
      break;
    case OP_ONE:
      y = NUM_AS_VAR(1.0);
      ++off;
      break;
    case OP_TWO:
      y = NUM_AS_VAR(2.0);
      ++off;
      break;
    case OP_NIL:
      y = nil;
      ++off;
      break;
    case OP_TRUE:
      y = VAR_TRUE;
      ++off;
      break;
    case OP_FALSE:
      y = VAR_FALSE;
      ++off;
      break;
    default:
      switch (op[off]) {
      case OP_INC:
        fold(op, off, var_inc(x));
        return;
      case OP_DEC:
        fold(op, off, var_dec(x));
        return;
      default:
        return;
      }
      return;
    }

    switch (op[off]) {
    case OP_ADD:
      fold(op, off, var_add(x, y));
      return;
    case OP_SUB:
      fold(op, off, var_sub(x, y));
      return;
    case OP_MUL:
      fold(op, off, var_mul(x, y));
      return;
    case OP_DIV:
      fold(op, off, var_div(x, y));
      return;
    case OP_LT:
      break;
    case OP_LTEQ:
      break;
    case OP_GT:
      break;
    case OP_GTEQ:
      break;
    case OP_EQ:
      break;
    case OP_NEQ:
      break;
    case OP_AND:
      break;
    case OP_OR:
      break;
    case OP_XOR:
      break;
    default:
      break;
    }
  } catch (interpret_exception &e) {
  }
}

} // namespace pho
} // namespace dwt
