// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_MACROS_HPP
#define GUARD_DWT_MACROS_HPP

#include <dwt/debug.hpp>

#if defined(__GNUC__) || defined(__clang__)
#define likely(cond) __builtin_expect(!!(cond), 1)
#define unlikely(cond) __builtin_expect(!!(cond), 0)
#else
#define likely(cond) cond
#define unlikely(cond) cond
#endif

#endif
