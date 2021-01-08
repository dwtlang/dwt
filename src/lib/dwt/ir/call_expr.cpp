// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/call_expr.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {
namespace ir {

call_expr::call_expr(std::unique_ptr<expr> callee,
                     std::unique_ptr<arguments> args)
  : _callee(callee.get())
  , _args(args.get()) {
  if (args) {
    splice(std::move(args));
  }

  splice(std::move(callee));
}

call_expr::~call_expr() {
}

void call_expr::accept(visitor &visitor) {
  visitor.visit(*this);
}

} // namespace ir
} // namespace dwt
