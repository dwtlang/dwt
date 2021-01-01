// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

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

  expr *super() const;
  void super(std::unique_ptr<expr>);

private:
  expr *_super = nullptr;
};

} // namespace ir
} // namespace dwt

#endif
