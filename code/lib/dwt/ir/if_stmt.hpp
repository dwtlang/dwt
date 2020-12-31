// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_IF_STMT_HPP
#define GUARD_DWT_IR_IF_STMT_HPP

#include <dwt/ir/expr.hpp>
#include <dwt/ir/stmt.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class if_stmt : public stmt {
public:
  if_stmt(token_ref tok);
  if_stmt();

  virtual ~if_stmt();

  virtual void accept(ir::visitor &visitor);

  void cond(ir::expr *e) {
    splice(e);
    _cond = e;
  }

  void if_body(ir::stmt *s) {
    splice(s);
    _true = s;
  }

  void else_body(ir::stmt *s) {
    splice(s);
    _false = s;
  }

  ir::expr *cond() {
    return _cond;
  }

  ir::stmt *if_body() {
    return _true;
  }

  ir::stmt *else_body() {
    return _false;
  }

private:
  ir::expr *_cond = nullptr;
  ir::stmt *_true = nullptr;
  ir::stmt *_false = nullptr;
};

} // namespace ir
} // namespace dwt

#endif
