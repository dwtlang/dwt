// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_OR_EXPR_HPP
#define GUARD_DWT_IR_OR_EXPR_HPP

#include <dwt/ir/expr.hpp>
#include <dwt/token_ref.hpp>
#include <vector>

namespace dwt {
namespace ir {

class or_expr : public expr {
public:
  or_expr(std::unique_ptr<expr>, std::unique_ptr<expr>, token_ref);
  virtual ~or_expr();
  virtual void accept(ir::visitor &visitor);
};

} // namespace ir
} // namespace dwt

#endif
