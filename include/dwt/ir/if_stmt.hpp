// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

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

  void cond(std::shared_ptr<ir::expr> e) {
    splice(e);
    _cond = e;
  }

  void if_body(std::shared_ptr<ir::stmt> s) {
    splice(s);
    _true = s;
  }

  void else_body(std::shared_ptr<ir::stmt> s) {
    splice(s);
    _false = s;
  }

  std::shared_ptr<ir::expr> cond() {
    return _cond;
  }

  std::shared_ptr<ir::stmt> if_body() {
    return _true;
  }

  std::shared_ptr<ir::stmt> else_body() {
    return _false;
  }

private:
  std::shared_ptr<ir::expr> _cond;
  std::shared_ptr<ir::stmt> _true;
  std::shared_ptr<ir::stmt> _false;
};

} // namespace ir
} // namespace dwt

#endif
