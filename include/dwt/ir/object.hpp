// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_IR_OBJECT_HPP
#define GUARD_DWT_IR_OBJECT_HPP

#include <dwt/ir/expr.hpp>
#include <dwt/ir/function.hpp>
#include <dwt/token.hpp>

#include <vector>

namespace dwt {
namespace ir {

class object : public function {
public:
  object(token_ref);
  object();
  virtual ~object();
  virtual void accept(ir::visitor &visitor);

  std::shared_ptr<expr> super() const;
  void super(std::shared_ptr<expr>);

private:
  std::shared_ptr<expr> _super;
};

} // namespace ir
} // namespace dwt

#endif
