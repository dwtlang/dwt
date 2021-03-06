// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_IR_MAP_IMPL_HPP
#define GUARD_DWT_IR_MAP_IMPL_HPP

#include <dwt/ir/lambda_decl.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class map_impl : public lambda_decl {
public:
  map_impl(token_ref tok);
  map_impl();
  virtual ~map_impl();
  virtual void accept(ir::visitor &visitor);
};

} // namespace ir
} // namespace dwt

#endif
