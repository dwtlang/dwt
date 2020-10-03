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

void register_syscall(std::string scope_str, syscall call) {
  /*    var v = OBJ_AS_VAR(heap::get().new_obj<syscall_obj>(
          call, string_mgr::get().add(scope_str)));

      auto s = scope::add(scope_str, SCOPE_CREATE | SCOPE_APPEND);
      globals::table().set(s->lookup(), v);

      scope::current = scope::global;*/
}

} // namespace dwt
