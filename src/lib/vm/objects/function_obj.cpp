// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/code_obj.hpp>
#include <dwt/decompiler.hpp>
#include <dwt/function_obj.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/scope.hpp>
#include <dwt/string_mgr.hpp>

namespace dwt {

function_obj::function_obj(function_type type,
                           size_t arity,
                           string_obj *name_str)
  : _type(type)
  , _arity(arity)
  , _code(new code_obj)
  , _name(name_str) {

  _short_name =
    string_mgr::get().add_r(name().substr(name().find_last_of(":") + 1));
}

function_obj::function_obj(const function_obj &other)
  : _type(other._type)
  , _arity(other._arity)
  , _locals(other._locals)
  , _upvars(other._upvars)
  , _code(other._code)
  , _name(static_cast<string_obj *>(other._name->clone()))
  , _patchpoint(other._patchpoint)
  , _optimised(other._optimised) {

  _short_name =
    string_mgr::get().add_r(name().substr(name().find_last_of(":") + 1));
}

function_obj::~function_obj() {
}

obj_type function_obj::type() {
  return OBJ_FUNCTION;
}

obj *function_obj::clone() {
  return new function_obj(*this);
}

void function_obj::mark_immutable() {
  _immutable = true;
}

int function_obj::add_upvar(size_t slot, bool is_local) {
  int idx = _upvars.size();

  for (size_t i = 0; i < _upvars.size(); ++i) {
    auto &upvar = _upvars[i];
    if (upvar.slot() == slot && upvar.is_local() == is_local) {
      return i;
    }
  }

  _upvars.emplace_back(upvar(slot, is_local));

  return idx;
}

void function_obj::call(interpreter &interpreter, int nr_args) {
  interpreter.invoke(this, nr_args);
}

void function_obj::blacken() {
  _name->mark_as(MARK_GREY);
  _short_name->mark_as(MARK_GREY);
  _code->mark_as(MARK_GREY);
  _code->blacken();
}

std::string function_obj::to_string() {
  return "<fun " + _name->text() + ">";
}

} // namespace dwt
