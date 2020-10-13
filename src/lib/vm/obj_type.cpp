// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/obj_type.hpp>

namespace dwt {

const char *decode(obj_type obj_type) {
  static const char *type_str[] = {
    "future", "string",   "function", "closure", "upvar",   "syscall",
    "class",  "instance", "map",      "vector",  "condvar", "semaphore"
  };

  return type_str[obj_type];
}

} // namespace dwt
