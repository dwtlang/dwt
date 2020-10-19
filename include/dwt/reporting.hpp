// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_REPORTING_HPP
#define GUARD_DWT_REPORTING_HPP

#include <dwt/feedback.hpp>
#include <dwt/token_range.hpp>
#include <dwt/token_ref.hpp>

#include <stdexcept>

namespace dwt {

std::string ui_msgfmt(std::string fmt, int idx, std::string idstr);

template <typename... Args> void oops(std::string fmt, Args... toks) {
  std::string s;
  int tok_line = -1;
  int ctx_lines = 3;
  int idx = 1;

  for (auto tok : { toks... }) {
    s += ui_msgfmt(fmt, idx++, tok.text());
    if (tok_line > 0) {
      ctx_lines = tok.line() - tok_line;
      if (ctx_lines < 2) {
        ctx_lines = 0;
      }
      s += tok.in_context(ctx_lines);
    } else {
      s += tok.in_context(ctx_lines);
    }
    tok_line = tok.line();
  }

  throw std::runtime_error(s);
}

} // namespace dwt

#endif
