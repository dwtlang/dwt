// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_OBJ_HPP
#define GUARD_DWT_OBJ_HPP

#include <dwt/debug.hpp>
#include <dwt/feedback.hpp>
#include <dwt/fnv1a.hpp>
#include <dwt/obj_type.hpp>
#include <dwt/var.hpp>

#include <atomic>
#include <cassert>
#include <string>
#include <vector>

namespace dwt {

enum obj_mark { MARK_WHITE, MARK_GREY };

std::string decode(class obj *);

class interpreter;

class obj {
protected:
  obj();
  obj(const obj &);

public:
  void *operator new(size_t size);
  void operator delete(void *p);

  virtual ~obj();

  virtual bool operator<(const obj &other) const;

  virtual obj_type type() = 0;

  virtual obj *clone() = 0;

  virtual hash_t hash();

  uint64_t oid() const {
    return ob_oid;
  }

  virtual std::string to_string();

  virtual void blacken() {
  }

  void mark_as(obj_mark mark) {
    _mark = mark;
  }

  obj_mark marked_as() const {
    return _mark;
  }

  void next(obj *obj) {
    _next = obj;
  }

  obj *next() const {
    return _next;
  }

  void prev(obj *obj) {
    _prev = obj;
  }

  obj *prev() const {
    return _prev;
  }

  virtual std::string printable_string();

  virtual void call(interpreter &, int);

  virtual bool op_eq(var v, bool rhs = false);
  virtual bool op_neq(var v, bool rhs = false);
  virtual bool op_gt(var v, bool rhs = false);
  virtual bool op_gteq(var v, bool rhs = false);
  virtual bool op_lt(var v, bool rhs = false);
  virtual bool op_lteq(var v, bool rhs = false);
  virtual bool op_is(var v, bool rhs = false);
  virtual var op_add(var v, bool rhs = false);
  virtual var op_inc();
  virtual var op_dec();
  virtual var op_sub(var v, bool rhs = false);
  virtual var op_mul(var v, bool rhs = false);
  virtual var op_div(var v, bool rhs = false);
  virtual var op_neg();
  virtual var op_mbrget(var v);
  virtual void op_mbrset(var name, var v);
  virtual var op_keyget(var key);
  virtual void op_keyset(var key, var v);

private:
  uint64_t ob_oid;
  obj_mark _mark;
  obj *_prev;
  obj *_next;
  static std::atomic<uint64_t> next_oid;
};

} // namespace dwt

#endif
