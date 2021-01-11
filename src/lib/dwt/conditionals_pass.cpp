// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/conditionals_pass.hpp>

namespace dwt {

conditionals_pass::conditionals_pass(code_obj &code)
  : peephole({ { { OP_ZERO, OP_EQ, OP_BRZ }, 5 },
               { { OP_ZERO, OP_EQ, OP_BNZ }, 5 } }) {

  (*this)(code);
}

conditionals_pass::~conditionals_pass() {
}

void conditionals_pass::peep(uint8_t *op, size_t extent) {
  op[0] = OP_SKIP;
  op[1] = OP_SKIP;

  if (op[2] == OP_BRZ) {
    op[2] = OP_BNZ;
  } else if (op[2] == OP_BNZ) {
    op[2] = OP_BRZ;
  }
}

} // namespace dwt
