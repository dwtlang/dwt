// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/debug.hpp>
#include <dwt/feedback.hpp>

#include <cassert>
#include <iostream>

namespace dwt {

void out(std::stringstream &ss) {
  std::cout << ss.str();
}

void out(std::string s) {
  std::cout << s;
}

void out(const char c) {
  std::cout << c;
}

void out(const char *s) {
  std::cout << s;
}

void err(std::stringstream &ss) {
  std::cerr << ss.str();
}

void err(std::string s) {
  std::cerr << s;
}

void err(const char c) {
  std::cerr << c;
}

void err(const char *s) {
  std::cerr << s;
}

void dbg(std::stringstream &ss) {
  debug { std::cerr << ss.str(); }
}

void dbg(std::string s) {
  debug { std::cerr << s; }
}

void dbg(const char c) {
  debug { std::cerr << c; }
}

void dbg(const char *s) {
  debug { std::cerr << s; }
}

std::string format_decimal(size_t dec, size_t max) {
  BUG_UNLESS(digits(dec) <= max);
  std::string decstr;
  size_t i = max - digits(dec);

  while (i--) {
    decstr += " ";
  }
  decstr += std::to_string(dec);

  return decstr;
}

} // namespace dwt
