// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/semaphore.hpp>

namespace dwt {

semaphore::semaphore(unsigned int count)
  : se_count(count) {
}

semaphore::~semaphore() {
}

void semaphore::up(unsigned int v) {
  {
    std::unique_lock<std::mutex> lock(se_mutex);
    se_count += v;
  }

  se_condvar.notify_all();
}

void semaphore::down() {
  std::unique_lock<std::mutex> lock(se_mutex);

  if (se_count == 0) {
    se_condvar.wait(lock, [this] { return se_count > 0; });
  }

  --se_count;
}

unsigned int semaphore::count() {
  std::unique_lock<std::mutex> lock(se_mutex);
  return se_count;
}

void semaphore::swap(semaphore &x, semaphore &y) {
  std::scoped_lock hold(x.se_mutex, y.se_mutex);
  std::swap(x.se_count, y.se_count);
}

} // namespace dwt
