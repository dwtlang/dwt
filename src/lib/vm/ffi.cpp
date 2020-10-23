// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/box_obj.hpp>
#include <dwt/ffi.hpp>
#include <dwt/globals.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/scope.hpp>
#include <dwt/string_mgr.hpp>
#include <dwt/string_obj.hpp>
#include <dwt/syscall_obj.hpp>

#include <stdexcept>

namespace dwt {
namespace ffi {

var bind(std::string scope_str, syscall call) {
  var call_obj =
    OBJ_AS_VAR(new syscall_obj(call, string_mgr::get().add_r(scope_str)));

  int idx = globals::table().index_of(scope_str);
  if (idx < 0) {
    idx = globals::table().add_r(scope_str);
  }

  globals::table().set_r(idx, call_obj);

  return call_obj;
}

var find(std::string identifier) {
  int idx = globals::table().index_of(identifier);
  var val = nil;

  if (idx >= 0) {
    val = globals::table().get_r(idx);
  }

  return val;
}

var call(std::string fn_identifier, var *args, size_t nr_args) {
  return call(find(fn_identifier), args, nr_args);
}

var call(var fn, var *args, size_t nr_args) {
  interpreter interpreter;

  if (VAR_IS_OBJ(fn)) {
    switch (VAR_AS_OBJ(fn)->type()) {
    case OBJ_FUNCTION:
    case OBJ_CLOSURE:
    case OBJ_SYSCALL:
    case OBJ_CLASS:
    case OBJ_INSTANCE:
      break;
    default:
      throw std::invalid_argument("object is not callable");
      break;
    }
  } else {
    throw std::invalid_argument("value is not callable");
  }

  return interpreter.interpret(VAR_AS_OBJ(fn), args, nr_args);
}

std::shared_ptr<void> unbox(var box) {
  std::shared_ptr<void> opaque_obj;

  if (VAR_IS_OBJ(box)) {
    if (VAR_AS_OBJ(box)->type() == OBJ_BOX) {
      box_obj *boxobj = static_cast<box_obj *>(VAR_AS_OBJ(box));
      opaque_obj = boxobj->contents();
    } else {
      throw std::invalid_argument("object is not a box");
    }
  } else {
    throw std::invalid_argument("value is not a box");
  }

  return opaque_obj;
}

} // namespace ffi
} // namespace dwt
