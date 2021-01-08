// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_PARAMETERS_HPP
#define GUARD_DWT_IR_PARAMETERS_HPP

#include <dwt/ir/ast.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class parameters : public ast {
public:
  parameters(token_ref tok);
  parameters();
  virtual ~parameters();
  virtual void accept(ir::visitor &visitor);

  size_t count() {
    return nr_children();
  }
};

} // namespace ir
} // namespace dwt

#endif