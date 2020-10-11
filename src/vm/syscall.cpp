// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/globals.hpp>
#include <dwt/scope.hpp>
#include <dwt/string_mgr.hpp>
#include <dwt/string_obj.hpp>
#include <dwt/syscall.hpp>
#include <dwt/syscall_obj.hpp>

#include <iostream>

namespace dwt {

void add_syscall(std::string scope_str, syscall call) {
  var call_obj =
    OBJ_AS_VAR(new syscall_obj(call, string_mgr::get().add_r(scope_str)));

  int idx = globals::table().index_of(scope_str);
  if (idx < 0) {
    idx = globals::table().add_r(scope_str);
  }

  globals::table().set_r(idx, call_obj);
}

} // namespace dwt
