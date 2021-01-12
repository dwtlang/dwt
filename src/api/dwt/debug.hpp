// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_DEBUG_HPP
#define GUARD_DWT_DEBUG_HPP

#if defined(DEBUG) && !defined(NDEBUG)
#define debug if constexpr (true)
#include <cassert>
#define BUG_UNLESS(cond) assert(cond)
#define BUG_ON(cond) assert(!(cond))
#define BUG() assert(0)
#else
#define debug if constexpr (false)
#define BUG_UNLESS(cond)
#define BUG_ON(cond)
#define BUG()
#endif

#endif
