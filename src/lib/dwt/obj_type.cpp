// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/obj_type.hpp>

namespace dwt {

const char *decode(obj_type obj_type) {
  static const char *type_str[] = { "string", "function", "closure",
                                    "upvar",  "syscall",  "code",
                                    "class",  "instance", "map",
                                    "mapfn",  "box",      "iterator" };

  return type_str[obj_type];
}

} // namespace dwt
