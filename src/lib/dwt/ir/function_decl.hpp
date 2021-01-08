// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_FUNCTION_DECL_HPP
#define GUARD_DWT_IR_FUNCTION_DECL_HPP

#include <dwt/ir/declaration.hpp>
#include <dwt/ir/function.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class function_decl : public declaration {
public:
  function_decl(token_ref tok);
  function_decl();
  virtual ~function_decl();
  virtual void accept(ir::visitor &visitor);
  void arity(size_t);
  size_t arity() const;

private:
  size_t _arity;
};

} // namespace ir
} // namespace dwt

#endif
