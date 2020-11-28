// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_IR_CALL_EXPR_HPP
#define GUARD_DWT_IR_CALL_EXPR_HPP

#include <dwt/ir/arguments.hpp>
#include <dwt/ir/expr.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class call_expr : public expr {
public:
  call_expr(expr *, arguments *);
  virtual ~call_expr();
  virtual void accept(ir::visitor &visitor);

  expr *callee() {
    return _callee;
  }

  arguments *args() {
    return _args;
  }

  size_t num_args() const {
    size_t count = 0;

    if (_args) {
      count = _args->nr_children();
    }

    return count;
  }

private:
  expr *_callee = nullptr;
  arguments *_args = nullptr;
};

} // namespace ir
} // namespace dwt

#endif
