// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

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
  inline call_frame(function_obj *fn, size_t sp)
    : fn(fn)
    , ip(fn->code().entry())
    , sp(sp)
    , closure(nullptr)
    , map(nullptr) {
  }

  inline call_frame(closure_obj *closure, size_t sp)
    : fn(closure->fun_obj())
    , ip(fn->code().entry())
    , sp(sp)
    , closure(closure)
    , map(fn->type() == OBJ_CLASS
            ? new instance_obj(static_cast<class_obj *>(fn))
            : nullptr) {
  }

  inline call_frame(class_obj *klass, size_t sp)
    : fn(klass)
    , ip(fn->code().entry())
    , sp(sp)
    , closure(nullptr)
    , map(new instance_obj(klass)) {
  }

  inline call_frame(mapfn_obj *mapfn, size_t sp)
    : fn(mapfn)
    , ip(fn->code().entry())
    , sp(sp)
    , closure(nullptr)
    , map(new map_obj) {
  }

  inline call_frame() {
  }

  function_obj *fn;
  uint8_t *ip;
  size_t sp;
  closure_obj *closure;
  map_obj *map;
};

} // namespace dwt

#endif
