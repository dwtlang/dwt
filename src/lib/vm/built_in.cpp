// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/built_in.hpp>
#include <dwt/ffi.hpp>
#include <dwt/interpret_exception.hpp>
#include <dwt/obj.hpp>
#include <dwt/scope.hpp>
#include <dwt/string_mgr.hpp>
#include <dwt/token_ref.hpp>

namespace dwt {

namespace {

var dup(size_t nr_args, var *args) {
  if (nr_args != 1) {
    throw interpret_exception("e@1 expected a single argument");
  }

  var v;

  if (is_obj(args[0])) {
    v = as_var(as_obj(args[0])->clone());
  } else {
    v = args[0];
  }

  return v;
}

var str(size_t nr_args, var *args) {
  if (nr_args != 1) {
    throw interpret_exception("e@1 expected a single argument");
  }

  return as_var(string_mgr::get().add(var_to_string(args[0])));
}

} // namespace

built_in::built_in() {
  scope::global->add(token_ref("dup"), SCOPE_CREATE | SCOPE_EXCLUSIVE);
  ffi::bind("::dup", dup);

  scope::global->add(token_ref("str"), SCOPE_CREATE | SCOPE_EXCLUSIVE);
  ffi::bind("::str", str);
}

built_in::~built_in() {
}

} // namespace dwt
