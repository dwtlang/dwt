// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_SEMAPHORE_HPP
#define GUARD_DWT_SEMAPHORE_HPP

#include <dwt/uncopyable.hpp>

#include <condition_variable>
#include <mutex>

namespace dwt {

class semaphore : public uncopyable {
public:
  explicit semaphore(unsigned int count = 0);
  virtual ~semaphore();

  void up(unsigned int v = 1);
  void down();
  unsigned int count();

  static void swap(semaphore &, semaphore &);

private:
  unsigned int se_count;
  std::mutex se_mutex;
  std::condition_variable se_condvar;
};

} // namespace dwt

#endif
