// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_IR_USE_STMT_HPP
#define GUARD_DWT_IR_USE_STMT_HPP

#include <dwt/ir/stmt.hpp>
#include <dwt/token.hpp>

#include <string>

namespace dwt {
namespace ir {

class use_stmt : public stmt {
public:
  use_stmt(token_ref tok);
  use_stmt();
  virtual ~use_stmt();
  virtual void accept(ir::visitor &visitor);
};

} // namespace ir
} // namespace dwt

#endif
