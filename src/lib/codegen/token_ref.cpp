// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/debug.hpp>
#include <dwt/feedback.hpp>
#include <dwt/token_ref.hpp>

namespace dwt {

std::string token_ref::in_context(int ctx_lines) {
  std::string s;

  BUG_UNLESS(_seq);

  s += _seq->to_string();
  s += ":";
  s += std::to_string(line());
  s += ":";
  s += std::to_string(column());
  s += ":\n";

  int from_line = line() < ctx_lines + 1 ? 0 : line() - ctx_lines;
  int ln_digits = digits(_seq->size());
  int margin = ln_digits + 7;

  _seq->for_all([&](auto tok) {
    if (tok.line() >= from_line && tok.line() <= line()) {
      if (tok.column() == 0) {
        s += TERM_BOLD;
        if (tok.line() == line()) {
          s += " ~> ";
        } else {
          s += "    ";
        }
        s += TERM_RESET;
        s += format_decimal(tok.line(), ln_digits);
        s += u8"│  ";
      }
      s += tok.text();
    }
  });

  int i = 0;
  for (; i < column() + margin; ++i) {
    s += " ";
  }
  s += TERM_BOLD "^";

  i = width();
  while (--i) {
    s += "~";
  }
  s += TERM_RESET "\n";

  return s;
}

} // namespace dwt
