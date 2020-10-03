// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_FUTURE_OBJ_HPP
#define GUARD_DWT_FUTURE_OBJ_HPP

#include <dwt/obj.hpp>
#include <dwt/var.hpp>

#include <future>

namespace dwt {
class closure_obj;
class future_obj : public obj {
public:
  future_obj(std::shared_future<var> fut_var, closure_obj *cls);
  future_obj(const future_obj &);
  virtual ~future_obj();

  virtual void call(interpreter &, int) override;
  virtual bool op_eq(var v, bool rhs = false) override;
  virtual bool op_neq(var v, bool rhs = false) override;
  virtual bool op_gt(var v, bool rhs = false) override;
  virtual bool op_gteq(var v, bool rhs = false) override;
  virtual bool op_lt(var v, bool rhs = false) override;
  virtual bool op_lteq(var v, bool rhs = false) override;
  virtual var op_add(var v, bool rhs = false) override;
  virtual var op_inc() override;
  virtual var op_dec() override;
  virtual var op_sub(var v, bool rhs = false) override;
  virtual var op_mul(var v, bool rhs = false) override;
  virtual var op_div(var v, bool rhs = false) override;
  virtual var op_neg() override;

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void mark_immutable() override;
  virtual void blacken() override;
  virtual std::string to_string() override;

private:
  var get() {
    if (!_ready) {
      _var = _fut_var.get();
      _ready = true;
    }

    return _var;
  }

  bool _ready = false;
  var _var;
  std::shared_future<var> _fut_var;
  closure_obj *_closure;
};

} // namespace dwt

#endif
