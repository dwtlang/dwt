// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt.hpp>
#include <dwt/feedback.hpp>
#include <dwt/garbage_collector.hpp>

#include <cstring>
#include <ctime>
#include <memory>

using namespace dwt;

var ping(size_t nr_args, var *args) {
  var n = *args;
  printf("ping\n");

  if (VAR_IS_NUM(n)) {
    n = var_dec(n);
    ffi::call("::pong", &n, 1);
  }

  return n;
}

int main(int argc, char **argv) {
  const char *filename = nullptr;
  int ret = 0;

  if (argc > 1) {
    filename = argv[1];
  } else {
    return ret = 1;
  }

  try {
    ffi::bind("::ping", ping);
    interpret(filename);
  } catch (std::exception &e) {
    err(e.what());
    ret = 1;
  } catch (...) {
    ret = 1;
  }

  garbage_collector::get().sweep();

  return ret;
}
