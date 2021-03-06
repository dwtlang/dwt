// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/constants.hpp>
#include <dwt/garbage_collector.hpp>
#include <dwt/globals.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/string_mgr.hpp>

namespace dwt {

bool garbage_collector::is_waiting = false;

garbage_collector::garbage_collector()
  : _threshold(0)
  , _heap_size(0)
  , _objs(nullptr) {
}

garbage_collector::~garbage_collector() {
}

garbage_collector &garbage_collector::get() {
  static garbage_collector instance;

  return instance;
}

void garbage_collector::add(interpreter *vm) {
  vm->next(_interpreters);
  vm->prev(nullptr);

  if (_interpreters) {
    _interpreters->prev(vm);
  }

  _interpreters = vm;
}

void garbage_collector::remove(interpreter *vm) {
  if (vm->prev()) {
    vm->prev()->next(vm->next());
  }
  if (vm->next()) {
    vm->next()->prev(vm->prev());
  }
  if (_interpreters == vm) {
    _interpreters = vm->next();
  }
}

void garbage_collector::update_heap_size(int64_t delta) {
  _heap_size += delta;

  if (_heap_size > _threshold) {
    is_waiting = true;
  }

  debug { is_waiting = true; }

  _threshold = _heap_size * 2;
}

void garbage_collector::track(obj *o) {
#if USE_THREADED_COMPILER
  std::scoped_lock lock(_mutex);
#endif
  if (_objs) {
    o->next(_objs);
    _objs->prev(o);
  }
  _objs = o;
}

void garbage_collector::collect_garbage() {
  dbg("-- marking constant roots\n");
  constants::table().get_all().for_all([this](auto &v) { mark(v); });
  dbg("-- marking global roots\n");
  globals::table().get_all().for_all([this](auto &v) { mark(v); });

  dbg("-- marking interpreter roots\n");

  interpreter *vm = _interpreters;
  while (vm) {
    vm->mark_roots(_grey_objs);
    vm = vm->next();
  }

  blacken();
  sweep();
}

void garbage_collector::mark(var v) {
  if (VAR_IS_OBJ(v)) {
    mark(VAR_AS_OBJ(v));
  }
}

void garbage_collector::mark(obj *o) {
  o->mark_as(MARK_GREY);
  _grey_objs.push_back(o);
}

void garbage_collector::blacken() {
  while (_grey_objs.size() > 0) {
    _grey_objs.back()->blacken();
    _grey_objs.pop_back();
  }
}

void garbage_collector::sweep() {
  obj *o = _objs;
  obj *p;

  string_mgr::get().sweep();

  while (o) {
    if (o->marked_as() == MARK_GREY) {
      o->mark_as(MARK_WHITE);
      o = o->next();
    } else {
      if (o->prev()) {
        o->prev()->next(o->next());
      }
      if (o->next()) {
        o->next()->prev(o->prev());
      }
      if (o == _objs) {
        _objs = o->next();
      }
      p = o;
      o = o->next();
      dbg(TERM_BOLD "-- deleting " TERM_RESET + decode(p) + "\n");
      delete p;
    }
  }

  is_waiting = false;
}

} // namespace dwt
