// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_FOR_STMT_HPP
#define GUARD_DWT_IR_FOR_STMT_HPP

#include <dwt/ir/stmt.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class yield_stmt : public stmt {
public:
  yield_stmt(token_ref tok);
  yield_stmt();
  virtual ~yield_stmt();
  virtual void accept(ir::visitor &visitor);
};

} // namespace ir
} // namespace dwt

#endif
