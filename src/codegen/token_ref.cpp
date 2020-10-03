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
  std::stringstream ss;

  BUG_UNLESS(_seq);

  ss << _seq->to_string() << ":" << line() << ":" << column() << ":\n";
  int from_line = line() < ctx_lines + 1 ? 0 : line() - ctx_lines;
  int ln_digits = digits(_seq->size());
  int margin = ln_digits + 6;

  _seq->for_all([&](auto tok) {
    if (tok.line() >= from_line && tok.line() <= line()) {
      if (tok.column() == 0) {
        ss << TERM_BOLD;
        if (tok.line() == line()) {
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

  if (ss.str().back() != '\n' && ss.str().back() != '\r') {
    ss << TERM_BOLD << "\t ...\n" << TERM_RESET;
  }

  int i = 0;
  for (; i < column() + margin; ++i) {
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
