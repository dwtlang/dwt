// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_IR_OBJECT_DECL_HPP
#define GUARD_DWT_IR_OBJECT_DECL_HPP

#include <dwt/ir/function_decl.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class object_decl : public function_decl {
public:
  object_decl(token_ref tok);
  object_decl();
  virtual ~object_decl();
  virtual void accept(ir::visitor &visitor);
};

} // namespace ir
} // namespace dwt

#endif
