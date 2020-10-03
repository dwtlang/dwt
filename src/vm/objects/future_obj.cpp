// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/closure_obj.hpp>
#include <dwt/future_obj.hpp>
#include <dwt/var.hpp>

#include <stdexcept>

namespace dwt {

future_obj::future_obj(std::shared_future<var> fut_var, closure_obj *cls)
  : _ready(false)
  , _fut_var(fut_var)
  , _closure(cls) {
}

future_obj::future_obj(const future_obj &other)
  : _ready(other._ready)
  , _fut_var(other._fut_var)
  , _closure(static_cast<closure_obj *>(other._closure->clone())) {
}

future_obj::~future_obj() {
}

obj_type future_obj::type() {
  return OBJ_FUTURE;
}

obj *future_obj::clone() {
  return new future_obj(*this);
}

void future_obj::mark_immutable() {
  _immutable = true;
}

void future_obj::blacken() {
  if (_ready) {
    if (VAR_IS_OBJ(_var)) {
      VAR_AS_OBJ(_var)->mark_as(MARK_GREY);
    }
  }
  _closure->mark_as(MARK_GREY);
}

void future_obj::call(interpreter &interpreter, int nr_args) {
  var v = get();
  if (VAR_IS_OBJ(v)) {
    VAR_AS_OBJ(v)->call(interpreter, nr_args);
  } else {
    throw std::logic_error("e@1 value is not callable");
  }
}

bool future_obj::op_eq(var v, bool rhs) {
  if (rhs) {
    return var_eq(v, get());
  }
  return var_eq(get(), v);
}

bool future_obj::op_neq(var v, bool rhs) {
  if (rhs) {
    return var_neq(v, get());
  }
  return var_neq(get(), v);
}

bool future_obj::op_gt(var v, bool rhs) {
  if (rhs) {
    return var_gt(v, get());
  }
  return var_gt(get(), v);
}

bool future_obj::op_gteq(var v, bool rhs) {
  if (rhs) {
    return var_gteq(v, get());
  }
  return var_gteq(get(), v);
}

bool future_obj::op_lt(var v, bool rhs) {
  if (rhs) {
    return var_lt(v, get());
  }
  return var_lt(get(), v);
}

bool future_obj::op_lteq(var v, bool rhs) {
  if (rhs) {
    return var_lteq(v, get());
  }
  return var_lteq(get(), v);
}

var future_obj::op_add(var v, bool rhs) {
  if (rhs) {
    return var_add(v, get());
  }
  return var_add(get(), v);
}

var future_obj::op_inc() {
  return var_inc(get());
}

var future_obj::op_dec() {
  return var_dec(get());
}

var future_obj::op_sub(var v, bool rhs) {
  if (rhs) {
    return var_sub(v, get());
  }
  return var_sub(get(), v);
}

var future_obj::op_mul(var v, bool rhs) {
  if (rhs) {
    return var_mul(v, get());
  }
  return var_mul(get(), v);
}

var future_obj::op_div(var v, bool rhs) {
  if (rhs) {
    return var_div(v, get());
  }
  return var_div(get(), v);
}

var future_obj::op_neg() {
  return var_neg(get());
}

std::string future_obj::to_string() {
  return var_to_string(get());
}

} // namespace dwt
