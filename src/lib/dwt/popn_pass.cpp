// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/popn_pass.hpp>

namespace dwt {

popn_pass::popn_pass(code_obj &code)
  : peephole({ { { OP_POP }, 1 } })
  , _code(code) {

  (*this)(code);
}

popn_pass::~popn_pass() {
}

void popn_pass::peep(uint8_t *op, size_t extent) {
  size_t off = op - _code.entry();
  size_t pop_count = 0;

  while (pop_count < extent && op[pop_count] == OP_POP) {
    if (jumps_into_range(_code, off, pop_count)) {
      // *never* merge pops that are divided by a jump destination!
      break;
    }
    if (++pop_count == 255) {
      break;
    }
  }

  if (pop_count > 1) {
    *op++ = OP_POPN;
    *op++ = pop_count & 0xFF;
    pop_count -= 2;
    while (pop_count--) {
      *op++ = OP_SKIP;
    }
  }
}

} // namespace dwt
