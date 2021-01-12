// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_GARBAGE_COLLECTOR_HPP
#define GUARD_DWT_GARBAGE_COLLECTOR_HPP

#include <dwt/obj.hpp>
#include <dwt/uncopyable.hpp>

#if USE_THREADED_COMPILER
#include <mutex>
#endif
#include <vector>

namespace dwt {

class interpreter;

class garbage_collector : public uncopyable {
public:
  static garbage_collector &get();
  void track(obj *o);
  void collect_garbage();
  void sweep();
  void update_heap_size(int64_t delta);

  static bool is_waiting;

  void add(interpreter *vm);
  void remove(interpreter *vm);

private:
  garbage_collector();
  virtual ~garbage_collector();

  void mark(var v);
  void mark(obj *);
  void blacken();

#if USE_THREADED_COMPILER
  std::mutex _mutex;
#endif
  uint64_t _threshold;
  uint64_t _heap_size;
  obj *_objs;
  std::vector<obj *> _grey_objs;
  interpreter *_interpreters;
};

} // namespace dwt

#endif
