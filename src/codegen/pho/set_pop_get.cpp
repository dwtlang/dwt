// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/pho/set_pop_get.hpp>

#define OPERAND(op) ((*(op)) | ((*((op) + 1)) << 8))

namespace dwt {
namespace pho {

set_pop_get::set_pop_get(bytecode &code)
  : peephole({ { { OP_STORE, OP_POP, OP_GLOBAL }, 7 },
               { { OP_SET, OP_POP, OP_GET }, 7 } }) {

  (*this)(code);
}

set_pop_get::~set_pop_get() {
}

void set_pop_get::peep(uint8_t *op, size_t extent) {
  uint16_t idx0 = OPERAND(&op[1]);
  uint16_t idx1 = OPERAND(&op[5]);
  bool unnecessary = false;

  if (idx0 == idx1) {
    if (op[0] == OP_STORE && op[4] == OP_GLOBAL) {
      unnecessary = true;
    } else if (op[0] == OP_SET && op[4] == OP_GET) {
      unnecessary = true;
    }

    if (unnecessary) {
      for (size_t i = 3; i < 7; ++i) {
        op[i] = OP_SKIP;
      }
    }
  }
}

} // namespace pho
} // namespace dwt
