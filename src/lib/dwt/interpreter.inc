// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#define FETCH() (*op++)

#define OPERAND(op) ((*(op)) | ((*((op) + 1)) << 8))

#if USE_COMPUTED_GOTO

#define DISPATCH_TABLE static void *op_vtable[] =

#define CASE_OP(op) L_OP_##op:

#define DISPATCH()            \
  do {                        \
    goto *op_vtable[FETCH()]; \
  } while (0)

#define DISPATCH_LOOP DISPATCH();

#else

#define CASE_OP(op) case OP_##op:

#define DISPATCH() break

#define DISPATCH_LOOP \
  while (1)           \
    switch (FETCH())

#endif

#if USE_COMPUTED_GOTO

DISPATCH_TABLE {
#define OP(op, _, __) &&L_OP_##op,
#include "dwt/opcodes.inc"
#undef OP
};

#endif
