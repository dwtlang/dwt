// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/feedback.hpp>
#include <dwt/token.hpp>
#include <dwt/token_range.hpp>

namespace dwt {

std::string token_range::in_context(int ctx_lines) {
  std::string s;

  s += _seq->to_string();
  s += ":";
  s += std::to_string(from_line());
  s += ":";
  s += std::to_string(leftmost_column());
  s += ":\n";

  int start_line = from_line() < ctx_lines + 1 ? 0 : from_line() - ctx_lines;
  int ln_digits = digits(_seq->size());
  int margin = ln_digits + 7;

  _seq->for_all([&](auto tok) {
    if (tok.line() >= start_line && tok.line() <= to_line()) {
      if (tok.column() == 0) {
        s += TERM_BOLD;
        if (tok.line() == to_line()) {
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

  if (s.size() > 0 && s.back() != '\n') {
    s += "\n";
  }

  int i = 0;
  for (; i < leftmost_column() + margin; ++i) {
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
