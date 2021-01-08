// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_DECLARATION_HPP
#define GUARD_DWT_IR_DECLARATION_HPP

#include <dwt/ir/ast.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class declaration : public ast {
public:
  declaration(token_ref tok);
  declaration();
  virtual ~declaration();
  virtual void accept(ir::visitor &visitor);
  bool is_constant() const;
  size_t index_of() const;
  void index_of(size_t);
  void is_global(bool);
  void is_constant(bool);

  void is_api(bool toggle) {
    _api = toggle;
  }

  bool is_api() const {
    return _api;
  }

private:
  size_t _index_of;
  bool _is_global;
  bool _is_constant;
  bool _api = false;
};

} // namespace ir
} // namespace dwt

#endif
