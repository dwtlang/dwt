// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_INBUILT_HPP
#define GUARD_DWT_INBUILT_HPP

#include <dwt/stack.hpp>
#include <dwt/uncopyable.hpp>

namespace dwt {

class inbuilt : public uncopyable {
public:
  inbuilt();
  virtual ~inbuilt();

  static inbuilt &get() {
    static inbuilt instance;
    return instance;
  }
};

} // namespace dwt

#endif