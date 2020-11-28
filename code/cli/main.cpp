// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/compiler.hpp>
#include <dwt/debug.hpp>
#include <dwt/exception.hpp>
#include <dwt/feedback.hpp>
#include <dwt/function_obj.hpp>
#include <dwt/garbage_collector.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/ir/printer.hpp>
#include <dwt/ir/script.hpp>
#include <dwt/parser.hpp>
#include <dwt/scope.hpp>
#include <dwt/string_mgr.hpp>
#include <dwt/version.hpp>

#include <cstring>
#include <ctime>
#include <memory>
#include <random>

using namespace dwt;

int main(int argc, char **argv) {
  std::string filename;
  int ret = 0;

  if (argc > 1) {
    filename = argv[1];
  } else {
    err(version::notice());
    return ret;
  }

  try {
    utf8_source src(filename);
    parser p(std::move(src));

    compiler c;
    auto fn = c.compile(p.parse());

    interpreter interpreter;
    interpreter.interpret(fn, nullptr, 0);

  } catch (exception &e) {
    err(e.what());
    ret = 1;
  } catch (...) {
    ret = 1;
  }

  garbage_collector::get().sweep();

  return ret;
}
