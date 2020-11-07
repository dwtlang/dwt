// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_CONSTANTS_HPP
#define GUARD_DWT_CONSTANTS_HPP

#include <dwt/stack.hpp>
#include <dwt/uncopyable.hpp>
#include <dwt/var.hpp>

#if USE_THREADED_COMPILER
#include <mutex>
#endif

namespace dwt {

class constants : public uncopyable {
public:
  constants();
  virtual ~constants();

  static constants &table() {
    static constants instance;
    return instance;
  }

  size_t add_r(var v) {
#if USE_THREADED_COMPILER
    std::scoped_lock hold(_mutex);
#endif
    size_t idx = vars.size();
    vars.push(v);
    return idx;
  }

  inline var get_r(size_t idx) {
#if USE_THREADED_COMPILER
    std::scoped_lock hold(_mutex);
#endif
    return vars.get(idx);
  }

  inline var get(size_t idx) const {
    return vars.get(idx);
  }

  inline void set_r(size_t idx, var v) {
#if USE_THREADED_COMPILER
    std::scoped_lock hold(_mutex);
#endif
    vars.set(idx, v);
  }

  inline void set(size_t idx, var v) {
    vars.set(idx, v);
  }

  stack<var> &get_all() {
    return vars;
  }

  void drop() {
    vars.pop_all();
  }

private:
#if USE_THREADED_COMPILER
  std::mutex _mutex;
#endif
  stack<var> vars;
};

} // namespace dwt

#endif
