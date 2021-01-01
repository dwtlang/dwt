// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_LOOP_STMT_HPP
#define GUARD_DWT_IR_LOOP_STMT_HPP

#include <dwt/ir/expr.hpp>
#include <dwt/ir/stmt.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

enum loop_type {
  BASIC_LOOP = 0,
  LOOP_WHILE,
  WHILE_LOOP,
  FOR_LOOP,
  LOOP_UNTIL,
  UNTIL_LOOP
};

class loop_stmt : public stmt {
public:
  loop_stmt(token_ref tok, loop_type = BASIC_LOOP);
  loop_stmt(loop_type = BASIC_LOOP);
  virtual ~loop_stmt();
  virtual void accept(ir::visitor &visitor);

  void set_type(loop_type ltype) {
    _type = ltype;
  }

  loop_type get_type() const {
    return _type;
  }

  void before(stmt *s) {
    splice(s);
    _before = s;
  }

  void after(stmt *s) {
    splice(s);
    _after = s;
  }

  void cond(expr *e) {
    splice(e);
    _cond = e;
  }

  void body(stmt *s) {
    splice(s);
    _body = s;
  }

  stmt *before() {
    return _before;
  }

  expr *cond() {
    return _cond;
  }

  stmt *body() {
    return _body;
  }

  stmt *after() {
    return _after;
  }

  bool is_tagged() {
    return name() != "";
  }

private:
  loop_type _type;
  stmt *_before = nullptr;
  expr *_cond = nullptr;
  stmt *_body = nullptr;
  stmt *_after = nullptr;
};

} // namespace ir
} // namespace dwt

#endif
