// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_GLOBALS_HPP
#define GUARD_DWT_GLOBALS_HPP

#include <dwt/stack.hpp>
#include <dwt/uncopyable.hpp>

#include <cassert>
#include <mutex>
#include <unordered_map>
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
    std::scoped_lock hold(_mutex);
    int idx = vars.size();
    var v = nil;
    vars.push(v);
    auto entry = std::make_pair(name, idx);
    index_map.insert(entry);
    _names.push_back(name);

    return idx;
  }

  inline var get_r(int idx) {
    std::scoped_lock hold(_mutex);
    return vars.get(idx);
  }

  inline var get(int idx) const {
    return vars.get(idx);
  }

  std::string name_at(int idx) {
    std::scoped_lock hold(_mutex);
    return _names.at(idx);
  }

  int index_of(std::string name) {
    std::scoped_lock hold(_mutex);
    int index = -1;
    auto it = index_map.find(name);
    if (it != index_map.end()) {
      index = it->second;
    }

    return index;
  }

  inline void set_r(int idx, var v) {
    std::scoped_lock hold(_mutex);
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
  std::mutex _mutex;
  stack<var> vars;
  std::vector<std::string> _names;
  std::unordered_map<std::string, int> index_map;
};

} // namespace dwt

#endif
