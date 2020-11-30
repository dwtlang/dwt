// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt.hpp>
#include <dwt/compiler.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/parser.hpp>
#include <dwt/utf8_source.hpp>
#include <dwt/var.hpp>

namespace dwt {

var interpret(const char *filename) {
  utf8_source source(filename);
  parser parser(std::move(source));
  compiler compiler;
  auto fn = compiler.compile(parser.parse());
  interpreter interpreter;
  return interpreter.interpret(fn, nullptr, 0);
}

} // namespace dwt
