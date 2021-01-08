// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_LAMBDA_DECL_HPP
#define GUARD_DWT_IR_LAMBDA_DECL_HPP

#include <dwt/ir/function_decl.hpp>
#include <dwt/ir/stmt.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class lambda_decl : public function_decl {
public:
  lambda_decl(token_ref tok);
  lambda_decl();
  virtual ~lambda_decl();
  virtual void accept(ir::visitor &visitor);
};

} // namespace ir
} // namespace dwt

#endif
