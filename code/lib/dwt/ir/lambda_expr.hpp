// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_LAMBDA_EXPR_HPP
#define GUARD_DWT_IR_LAMBDA_EXPR_HPP

#include <dwt/ir/arguments.hpp>
#include <dwt/ir/expr.hpp>
#include <dwt/ir/lambda_decl.hpp>
#include <dwt/ir/parameters.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class lambda_expr : public expr {
public:
  lambda_expr(token_ref tok);
  lambda_expr();
  virtual ~lambda_expr();
  virtual void accept(ir::visitor &visitor);

  ir::arguments *args() {
    return _args;
  }

  ir::lambda_decl *impl() {
    return _impl;
  }

  void args(std::unique_ptr<ir::arguments> args) {
    _args = args.get();
    splice(std::move(args));
  }

  void impl(std::unique_ptr<ir::lambda_decl> lambda) {
    _impl = lambda.get();
    splice(std::move(lambda));
  }

private:
  ir::arguments *_args = nullptr;
  ir::lambda_decl *_impl = nullptr;
};

} // namespace ir
} // namespace dwt

#endif
