// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ffi.hpp>
#include <dwt/garbage_collector.hpp>
#include <dwt/inbuilt.hpp>
#include <dwt/interpret_exception.hpp>
#include <dwt/obj.hpp>
#include <dwt/scope.hpp>
#include <dwt/string_mgr.hpp>
#include <dwt/token_ref.hpp>
#include <dwt/utf8.hpp>
#include <dwt/version.hpp>

#include <thread>
#include <chrono>

namespace dwt {

namespace {

var dup(size_t nr_args, var *args) {
  if (nr_args != 1) {
    throw interpret_exception("e@1 expected a single argument");
  }

  var v;

  if (is_obj(args[0])) {
    v = as_var(as_obj(args[0])->clone());
  } else {
    v = args[0];
  }

  return v;
}

var str(size_t nr_args, var *args) {
  if (nr_args != 1) {
    throw interpret_exception("e@1 expected a single argument");
  }

  return as_var(string_mgr::get().add(var_to_string(args[0])));
}

var len(size_t nr_args, var *args) {
  if (nr_args != 1) {
    throw interpret_exception("e@1 expected a single argument");
  }

  if (is_obj(args[0])) {
    return as_var(static_cast<double>(as_obj(args[0])->length()));
  }

  throw interpret_exception("e@1 value has no concept of length");
}

var ver(size_t nr_args, var *args) {
  if (nr_args != 0) {
    throw interpret_exception("e@1 expected no arguments");
  }

  return as_var(string_mgr::get().add(version::to_string()));
}

var gc(size_t nr_args, var *args) {
  if (nr_args != 0) {
    throw interpret_exception("e@1 expected no arguments");
  }

  garbage_collector::get().collect_garbage();

  return nil;
}

var sleep(size_t nr_args, var *args) {
  if (nr_args != 1) {
    throw interpret_exception("e@1 expected a single argument");
  }

  if (VAR_IS_NUM(args[0])) {
    double usecs = VAR_AS_NUM(args[0]) * 1000000;

    std::this_thread::sleep_for(
        std::chrono::microseconds(static_cast<unsigned int>(usecs)));
  } else {
    throw interpret_exception("e@1 sleep function expects a value in seconds");
  }

  return BOOL_AS_VAR(true);
}

void add_inbuilt_function(std::string name, ffi::syscall impl) {
  scope::global->add(name, SCOPE_CREATE | SCOPE_EXCLUSIVE);
  ffi::bind("::" + name, impl);
}

} // namespace

inbuilt::inbuilt() {
  add_inbuilt_function("ver", ver);
  add_inbuilt_function("dup", dup);
  add_inbuilt_function("str", str);
  add_inbuilt_function("len", len);
  add_inbuilt_function("gc", gc);
  add_inbuilt_function("sleep", sleep);
}

inbuilt::~inbuilt() {
}

} // namespace dwt
