// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/debug.hpp>
#include <dwt/feedback.hpp>

#include <cassert>
#include <cstdio>

namespace dwt {

void out(std::string s) {
  printf("%s", s.c_str());
}

void out(const char c) {
  printf("%c", c);
}

void out(const char *s) {
  printf("%s", s);
}

void err(std::string s) {
  fprintf(stderr, "%s", s.c_str());
}

void err(const char c) {
  fprintf(stderr, "%c", c);
}

void err(const char *s) {
  fprintf(stderr, "%s", s);
}

void dbg(std::string s) {
  debug { fprintf(stderr, "%s", s.c_str()); }
}

void dbg(const char c) {
  debug { fprintf(stderr, "%c", c); }
}

void dbg(const char *s) {
  debug { fprintf(stderr, "%s", s); }
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
