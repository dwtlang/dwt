// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_BUILTIN_HPP
#define GUARD_DWT_BUILTIN_HPP

#include <dwt/stack.hpp>
#include <dwt/uncopyable.hpp>

namespace dwt {

class built_in : public uncopyable {
public:
  built_in();
  virtual ~built_in();

  static built_in &get() {
    static built_in instance;
    return instance;
  }
};

} // namespace dwt

#endif
