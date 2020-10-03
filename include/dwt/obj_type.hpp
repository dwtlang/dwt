// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_OBJ_TYPE_HPP
#define GUARD_DWT_OBJ_TYPE_HPP

namespace dwt {

enum obj_type {
  OBJ_FUTURE,
  OBJ_STRING,
  OBJ_FUNCTION,
  OBJ_CLOSURE,
  OBJ_UPVALUE,
  OBJ_SYSCALL,
  OBJ_CLASS,
  OBJ_INSTANCE,
  OBJ_MAP,
  OBJ_VECTOR,
  OBJ_MAPINI,
  OBJ_VECINI,
  OBJ_CONDVAR,
  OBJ_SEMAPHORE
};

const char *decode(obj_type);

} // namespace dwt

#endif
