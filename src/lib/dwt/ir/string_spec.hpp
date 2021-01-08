// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_STRING_SPEC_HPP
#define GUARD_DWT_IR_STRING_SPEC_HPP

#include <dwt/ir/primary_expr.hpp>
#include <dwt/token.hpp>

#include <string>

namespace dwt {
namespace ir {

class string_spec : public primary_expr {
public:
  string_spec(token_ref tok);
  string_spec();
  virtual ~string_spec();
  virtual void accept(ir::visitor &visitor);

  std::string &text() {
    return _text;
  }

private:
  std::string _text;
};

} // namespace ir
} // namespace dwt

#endif
