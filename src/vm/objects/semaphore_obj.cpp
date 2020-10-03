// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/semaphore_obj.hpp>

#include <string>

namespace dwt {

semaphore_obj::semaphore_obj(size_t initial_count)
  : _count(initial_count) {
}

semaphore_obj::~semaphore_obj() {
}

obj_type semaphore_obj::type() {
  return OBJ_SEMAPHORE;
}

obj *semaphore_obj::clone() {
  return this;
}

void semaphore_obj::mark_immutable() {
  _immutable = true;
}

void semaphore_obj::blacken() {
}

std::string semaphore_obj::to_string() {
  return "<semaphore>";
}

void semaphore_obj::up(size_t count) {
  {
    std::unique_lock<std::mutex> lock(_mutex);
    _count += count;
    count = _count;
  }

  if (count > 0) {
    if (count > 1) {
      _condvar.notify_all();
    } else {
      _condvar.notify_one();
    }
  }
}

void semaphore_obj::down() {
  std::unique_lock<std::mutex> lock(_mutex);

  if (_count < 0) {
    _condvar.wait(lock, [this] { return _count > 0; });
  }

  --_count;
}

size_t semaphore_obj::count() {
  std::unique_lock<std::mutex> lock(_mutex);
  return _count;
}

} // namespace dwt
