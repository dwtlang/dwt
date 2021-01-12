// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt.hpp>
#include <dwt/debug.hpp>
#include <dwt/exception.hpp>
#include <dwt/feedback.hpp>
#include <dwt/garbage_collector.hpp>
#include <dwt/version.hpp>

#include <cstring>
#include <ctime>

using namespace dwt;

int main(int argc, char **argv) {
  const char *filename = nullptr;
  int ret = 0;

  if (argc > 1) {
    filename = argv[1];
  } else {
    err(version::notice());
    return ret;
  }

  try {
    interpret(filename);
  } catch (exception &e) {
    err(e.what());
    ret = 1;
  } catch (...) {
    ret = 1;
  }

  garbage_collector::get().sweep();

  return ret;
}
