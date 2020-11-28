// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_FEEDBACK_HPP
#define GUARD_DWT_FEEDBACK_HPP

#include <string>

#ifndef JENKINS
#define TERM_BOLD "\x1b[1m"
#define TERM_LIGHT "\x1b[2m"
#define TERM_ITALIC "\x1b[3m"
#define TERM_UNDERLINE "\x1b[4m"
#define TERM_RED "\x1b[31m"
#define TERM_GREEN "\x1b[32m"
#define TERM_YELLOW "\x1b[33m"
#define TERM_BLUE "\x1b[34m"
#define TERM_MAGENTA "\x1b[35m"
#define TERM_CYAN "\x1b[36m"
#define TERM_GREY "\x1b[37m"
#define TERM_RESET "\x1b[0m"
#else
#define TERM_BOLD ""
#define TERM_LIGHT ""
#define TERM_ITALIC ""
#define TERM_UNDERLINE ""
#define TERM_RED ""
#define TERM_GREEN ""
#define TERM_YELLOW ""
#define TERM_BLUE ""
#define TERM_MAGENTA ""
#define TERM_CYAN ""
#define TERM_GREY ""
#define TERM_RESET ""
#endif

namespace dwt {

void out(std::stringstream &);
void out(std::string);
void out(const char);
void out(const char *);
void dbg(std::stringstream &);
void dbg(std::string);
void dbg(const char);
void dbg(const char *);
void err(std::stringstream &);
void err(std::string);
void err(const char);
void err(const char *);

inline size_t digits(size_t dec) {
  size_t count = 0;

  do {
    dec /= 10;
    ++count;
  } while (dec != 0);

  return count;
}

std::string format_decimal(size_t dec, size_t max);

} // namespace dwt

#endif
