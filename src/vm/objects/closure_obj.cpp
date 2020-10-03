// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/closure_obj.hpp>
#include <dwt/interpreter.hpp>

#include <iomanip>
#include <sstream>

namespace dwt {

closure_obj::closure_obj(function_obj *fun_obj)
  : _fun_obj(fun_obj) {
  size_t nr_upvars = fun_obj->upvars().size();

  while (nr_upvars--) {
    _upvars.push_back(nullptr);
  }
}

closure_obj::closure_obj(const closure_obj &other)
  : _fun_obj(static_cast<function_obj *>(other._fun_obj->clone()))
  , _upvars(other._upvars) {
}

closure_obj::~closure_obj() {
}

obj_type closure_obj::type() {
  return OBJ_CLOSURE;
}

obj *closure_obj::clone() {
  return new closure_obj(*this);
}

void closure_obj::mark_immutable() {
  _immutable = true;
}

std::string closure_obj::to_string() {
  return _fun_obj->to_string();
}

void closure_obj::blacken() {
  for (auto upv : _upvars) {
    if (upv) {
      upv->mark_as(MARK_GREY);
    }
  }
  _fun_obj->mark_as(MARK_GREY);
}

void closure_obj::call(interpreter &interpreter, int nr_args) {
  interpreter.invoke(this, nr_args);
}

} // namespace dwt
