// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_CONDVAR_HPP
#define GUARD_DWT_CONDVAR_HPP

#include <condition_variable>
#include <string>

namespace dwt {

class condvar {
public:
  condvar();
  condvar(condvar &&);
  virtual ~condvar();

  template <typename Fn> void signal_all(Fn signal_fn) {
    bool condition_met = false;
    {
      std::unique_lock<std::mutex> lock(_mutex);
      condition_met = signal_fn();
    }

    if (condition_met) {
      _condvar.notify_all();
    }
  }

  void signal_all();

  template <typename Fn> void signal(Fn signal_fn) {
    bool condition_met = false;
    {
      std::unique_lock<std::mutex> lock(_mutex);
      condition_met = signal_fn();
    }

    if (condition_met) {
      _condvar.notify_one();
    }
  }

  void signal();

  template <typename Fn> void await(Fn await_fn) {
    std::unique_lock<std::mutex> lock(_mutex);
    if (!await_fn()) {
      _condvar.wait(lock, await_fn);
    }
  }

  void await();

  template <typename Fn> void reset(Fn reset_fn) {
    std::unique_lock<std::mutex> lock(_mutex);
    reset_fn();
  }

  void reset();

private:
  bool _cond;
  std::mutex _mutex;
  std::condition_variable _condvar;
};

} // namespace dwt

#endif
