// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_MAP_EXPR_HPP
#define GUARD_DWT_IR_MAP_EXPR_HPP

#include <dwt/ir/expr.hpp>
#include <vector>

namespace dwt {
namespace ir {

class map_expr : public expr {
public:
  map_expr();
  virtual ~map_expr();
  virtual void accept(ir::visitor &visitor) override;
};

} // namespace ir
} // namespace dwt

#endif
