// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/condvar.hpp>

#include <string>

namespace dwt {

condvar::condvar()
  : _cond(false) {
}

condvar::condvar(condvar &&other)
  : _cond(false) {
}

condvar::~condvar() {
}

void condvar::signal_all() {
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _cond = true;
  }
  _condvar.notify_all();
}

void condvar::signal() {
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _cond = true;
  }
  _condvar.notify_one();
}

void condvar::await() {
  std::unique_lock<std::mutex> lock(_mutex);

  if (!_cond) {
    _condvar.wait(lock, [this] { return _cond; });
  }
}

void condvar::reset() {
  std::unique_lock<std::mutex> lock(_mutex);
  _cond = false;
}

} // namespace dwt
