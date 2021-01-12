// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_IR_EXPR_HPP
#define GUARD_DWT_IR_EXPR_HPP

#include <dwt/ir/ast.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class expr : public ast {
public:
  expr(token_ref tok);
  expr();
  virtual ~expr();
  virtual void accept(ir::visitor &visitor);
};

} // namespace ir
} // namespace dwt

#endif
