// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_CALL_FRAME_HPP
#define GUARD_DWT_CALL_FRAME_HPP

#include <dwt/class_obj.hpp>
#include <dwt/closure_obj.hpp>
#include <dwt/function_obj.hpp>
#include <dwt/instance_obj.hpp>
#include <dwt/mapfn_obj.hpp>
#include <dwt/obj.hpp>

namespace dwt {

class call_frame {
public:
  inline call_frame(function_obj *fn, unsigned int sp)
    : fn(fn)
    , ip(fn->code().entry())
    , sp(sp)
    , closure(nullptr)
    , map(nullptr) {
  }

  inline call_frame(closure_obj *closure, unsigned int sp)
    : fn(closure->fun_obj())
    , ip(fn->code().entry())
    , sp(sp)
    , closure(closure)
    , map((fn->type() == OBJ_CLASS) || (fn->type() == OBJ_MAPINI)
            ? new instance_obj(static_cast<class_obj *>(fn))
            : nullptr) {
  }

  inline call_frame(class_obj *klass, unsigned int sp)
    : fn(klass)
    , ip(fn->code().entry())
    , sp(sp)
    , closure(nullptr)
    , map(new instance_obj(klass)) {
  }

  inline call_frame(mapfn_obj *mapfn, unsigned int sp)
    : fn(mapfn)
    , ip(fn->code().entry())
    , sp(sp)
    , closure(nullptr)
    , map(new map_obj) {
  }

  inline call_frame(unsigned int sp)
    : fn(nullptr)
    , ip(nullptr)
    , sp(sp)
    , closure(nullptr)
    , map(nullptr) {
  }

  inline call_frame() {
  }

  function_obj *fn;
  uint8_t *ip;
  unsigned int sp;
  closure_obj *closure;
  map_obj *map;
};

} // namespace dwt

#endif
