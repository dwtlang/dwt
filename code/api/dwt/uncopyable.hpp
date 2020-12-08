// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_UNCOPYABLE_HPP
#define GUARD_DWT_UNCOPYABLE_HPP

#include <mutex>

namespace dwt {

class uncopyable {
protected:
  uncopyable() = default;
  virtual ~uncopyable() = default;

private:
  uncopyable(const uncopyable &) = delete;
  uncopyable &operator=(const uncopyable &) = delete;
  uncopyable(uncopyable &&) = delete;
};

} // namespace dwt

#endif