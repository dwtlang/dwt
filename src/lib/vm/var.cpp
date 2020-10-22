// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/box_obj.hpp>
#include <dwt/obj.hpp>
#include <dwt/string_mgr.hpp>
#include <dwt/var.hpp>

namespace dwt {

var to_var(std::shared_ptr<void> opaque_obj) {
  auto box = new box_obj(opaque_obj);
  return OBJ_AS_VAR(box);
}

var to_var(std::string cxx_str) {
  auto str_obj = string_mgr::get().add_r(cxx_str);
  return OBJ_AS_VAR(str_obj);
}

#undef RHS
#define RHS true

bool obj_var_eq(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_eq(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_eq(v0, RHS);
  }

  return VAR_IS_OBJ(v0) && VAR_IS_OBJ(v1);
}

bool obj_var_neq(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_neq(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_neq(v0, RHS);
  }

  return !VAR_IS_OBJ(v0) || !VAR_IS_OBJ(v1);
}

bool obj_var_gt(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_gt(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_gt(v0, RHS);
  }

  BUG();

  return false;
}

bool obj_var_gteq(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_gteq(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_gteq(v0, RHS);
  }

  BUG();

  return false;
}

bool obj_var_lt(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_lt(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_lt(v0, RHS);
  }

  BUG();

  return false;
}

bool obj_var_lteq(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_lteq(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_lteq(v0, RHS);
  }

  BUG();

  return false;
}

bool obj_var_is(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_is(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_is(v0, RHS);
  }

  return false;
}

var obj_var_add(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_add(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_add(v0, RHS);
  }

  BUG();

  return nil;
}

var obj_var_inc(var v) {
  if (VAR_AS_OBJ(v)) {
    return VAR_AS_OBJ(v)->op_inc();
  }

  BUG();

  return v;
}

var obj_var_dec(var v) {
  if (VAR_AS_OBJ(v)) {
    return VAR_AS_OBJ(v)->op_dec();
  }

  BUG();

  return v;
}

var obj_var_sub(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_sub(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_sub(v0, RHS);
  }

  BUG();

  return nil;
}

var obj_var_mul(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_mul(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_mul(v0, RHS);
  }

  BUG();

  return nil;
}

var obj_var_div(var v0, var v1) {
  if (VAR_IS_OBJ(v0)) {
    return VAR_AS_OBJ(v0)->op_div(v1);
  }

  if (VAR_IS_OBJ(v1)) {
    return VAR_AS_OBJ(v1)->op_div(v0, RHS);
  }

  BUG();

  return nil;
}

var obj_var_neg(var v) {
  if (VAR_AS_OBJ(v)) {
    return VAR_AS_OBJ(v)->op_neg();
  }

  BUG();
  return v;
}

std::string var_to_string(var v) {
  std::string s;

  if (VAR_IS_BOOL(v)) {
    s = VAR_AS_BOOL(v) ? "true" : "false";
  } else if (VAR_IS_NUM(v)) {
    s = std::to_string(VAR_AS_NUM(v));
    s.erase(s.find_last_not_of('0') + 1, std::string::npos);
    s.erase(s.find_last_not_of('.') + 1, std::string::npos);
  } else if (VAR_IS_NIL(v)) {
    s = "<nil>";
  } else {
    obj *o = VAR_AS_OBJ(v);
    s = o->to_string();
  }

  return s;
}

} // namespace dwt
