// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_UPVAR_OBJ_HPP
#define GUARD_DWT_UPVAR_OBJ_HPP

#include <dwt/obj.hpp>
#include <dwt/stack.hpp>
#include <dwt/var.hpp>

#include <atomic>
#include <string>
#include <vector>

namespace dwt {

class upvar_obj : public obj {
public:
  upvar_obj(stack<var> *, size_t, size_t, upvar_obj *next_upv = nullptr);
  upvar_obj(const upvar_obj &);
  virtual ~upvar_obj();

  virtual obj_type type() override;
  virtual obj *clone() override;

  var get() {
    if (_stack) {
      return _stack->get(_offset + _slot);
    }
    return _closed;
  }

  void set(var val) {
    if (_stack) {
      _stack->set(_offset + _slot, val);
    }
    _closed = val;
  }

  size_t pos() const {
    return _offset + _slot;
  }

  size_t slot() const {
    return _slot;
  }

  void next_upvar(upvar_obj *obj) {
    _next_upvar = obj;
  }

  upvar_obj *next_upvar() const {
    return _next_upvar;
  }

  void close() {
    if (_stack) {
      _closed = get();
      _stack = nullptr;
    }
  }

  virtual void blacken() override;
  virtual std::string to_string() override;

private:
  stack<var> *_stack;
  var _closed = nil;
  size_t _offset;
  size_t _slot;
  std::atomic<upvar_obj *> _next_upvar;
};

} // namespace dwt

#endif
