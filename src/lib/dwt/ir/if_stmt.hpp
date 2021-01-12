// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

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

  void cond(std::unique_ptr<ir::expr> e) {
    _cond = e.get();
    splice(std::move(e));
  }

  void if_body(std::unique_ptr<ir::stmt> s) {
    _true = s.get();
    splice(std::move(s));
  }

  void else_body(std::unique_ptr<ir::stmt> s) {
    _false = s.get();
    splice(std::move(s));
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
