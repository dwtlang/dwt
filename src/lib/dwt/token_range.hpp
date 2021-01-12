// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_TOKEN_RANGE_HPP
#define GUARD_DWT_TOKEN_RANGE_HPP

#include <dwt/token_cache.hpp>
#include <dwt/token_ref.hpp>

#include <climits>
#include <memory>

namespace dwt {

class token_range {
public:
  token_range(token_ref from, token_ref to)
    : _seq(from._seq)
    , _from(from.pos())
    , _to(to.pos()) {
  }

  token_range(std::shared_ptr<token_cache> seq, int from, int to)
    : _seq(seq)
    , _from(from)
    , _to(to) {
  }

  token_range() = default;

  token_range(const token_range &) = default;
  ~token_range() = default;

  std::string text() const {
    std::string lstr;

    int pos = _from;

    while (pos <= _to) {
      lstr += (*_seq)[pos++].text();
    }

    return lstr;
  }

  int line() const {
    return from_line();
  }

  int from_line() const {
    return (*_seq)[_from].line();
  }

  int to_line() const {
    return (*_seq)[_to].line();
  }

  int column() const {
    return (*_seq)[_from].column();
  }

  int leftmost_column() {
    int lmc = INT_MAX;

    int pos = _from;

    while (pos <= _to) {
      auto tok = (*_seq)[pos];
      int col = tok.column();

      switch (tok.type()) {
      case TOK_COMMENT:
      case TOK_WS:
      case TOK_BREAK:
        break;
      default:
        if (col < lmc) {
          lmc = col;
        }
        break;
      }

      ++pos;
    }

    return lmc;
  }

  int rightmost_column() {
    int rmc = -1;

    int pos = _from;

    while (pos <= _to) {
      auto tok = (*_seq)[pos];
      int col = tok.column() + tok.width();

      switch (tok.type()) {
      case TOK_COMMENT:
      case TOK_WS:
      case TOK_BREAK:
        break;
      default:
        if (col > rmc) {
          rmc = col;
        }
        break;
      }

      ++pos;
    }

    return rmc;
  }

  int width() {
    return rightmost_column() - leftmost_column();
  }

  std::string in_context(int ctx_lines = 3);

private:
  std::shared_ptr<token_cache> _seq;
  int _from;
  int _to;
};

} // namespace dwt

#endif
