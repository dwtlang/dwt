// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_GLOBALS_HPP
#define GUARD_DWT_GLOBALS_HPP

#include <dwt/stack.hpp>
#include <dwt/uncopyable.hpp>

#include <cassert>
#include <mutex>
#include <vector>

namespace dwt {

class globals : public uncopyable {
public:
  globals();
  virtual ~globals();

  static globals &table() {
    static globals instance;
    return instance;
  }

  int add_r(std::string name) {
#if USE_THREADED_COMPILER
    std::scoped_lock hold(_mutex);
#endif
    int idx = vars.size();
    var v = nil;
    vars.push(v);
    _names.push_back(name);

    return idx;
  }

  inline var get_r(int idx) {
#if USE_THREADED_COMPILER
    std::scoped_lock hold(_mutex);
#endif
    return vars.get(idx);
  }

  inline var get(int idx) const {
    return vars.get(idx);
  }

  std::string name_at(int idx) {
#if USE_THREADED_COMPILER
    std::scoped_lock hold(_mutex);
#endif
    return _names.at(idx);
  }

  int index_of(std::string name) {
#if USE_THREADED_COMPILER
    std::scoped_lock hold(_mutex);
#endif
    int index = -1;

    for (size_t i = 0; i < _names.size(); ++i) {
      if (_names[i] == name) {
        index = i;
        break;
      }
    }

    return index;
  }

  inline void set_r(int idx, var v) {
#if USE_THREADED_COMPILER
    std::scoped_lock hold(_mutex);
#endif
    vars.set(idx, v);
  }

  inline void set(int idx, var v) {
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
  std::vector<std::string> _names;
};

} // namespace dwt

#endif
