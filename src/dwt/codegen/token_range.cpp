// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/feedback.hpp>
#include <dwt/token.hpp>
#include <dwt/token_range.hpp>

#include <sstream>

namespace dwt {

std::string token_range::in_context(int ctx_lines) {
  std::stringstream ss;

  ss << _seq->to_string() << ":" << from_line() << ":" << leftmost_column()
     << ":\n";
  int start_line = from_line() < ctx_lines + 1 ? 0 : from_line() - ctx_lines;
  int ln_digits = digits(_seq->size());
  int margin = ln_digits + 6;

  _seq->for_all([&](auto tok) {
    if (tok.line() >= start_line && tok.line() <= to_line()) {
      if (tok.column() == 0) {
        ss << TERM_BOLD;
        if (tok.line() == to_line()) {
          ss << " ~> ";
        } else {
          ss << "    ";
        }
        ss << TERM_RESET;
        ss << format_decimal(tok.line(), ln_digits) << "  ";
      }
      ss << tok.text();
    }
  });

  int i = 0;
  for (; i < leftmost_column() + margin; ++i) {
    ss << " ";
  }
  ss << TERM_BOLD << "^";

  i = width();
  while (--i) {
    ss << "~";
  }
  ss << TERM_RESET << "\n";

  return ss.str();
}

} // namespace dwt
