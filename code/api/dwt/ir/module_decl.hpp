// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_IR_MODULE_DECL_HPP
#define GUARD_DWT_IR_MODULE_DECL_HPP

#include <dwt/ir/declaration.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class module_decl : public declaration {
public:
  module_decl(token_ref tok);
  module_decl();
  virtual ~module_decl();
  virtual void accept(ir::visitor &visitor);
};

} // namespace ir
} // namespace dwt

#endif
