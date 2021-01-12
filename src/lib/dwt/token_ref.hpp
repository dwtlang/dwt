// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_TOKEN_REF_HPP
#define GUARD_DWT_TOKEN_REF_HPP

#include <dwt/token_cache.hpp>

#include <memory>

namespace dwt {

class token_ref {
  friend class token_range;

public:
  token_ref(std::shared_ptr<token_cache> seq, int idx)
    : _seq(seq)
    , _idx(idx) {
  }

  token_ref(std::string str)
    : _idx(0)
    , _text(str) {
  }

  token_ref()
    : _idx(0) {
  }

  token_ref(const token_ref &) = default;
  ~token_ref() = default;

  token_type type() const {
    token_type sym = TOK_INV;

    if (_seq) {
      sym = (*_seq)[_idx].type();
    }

    return sym;
  }

  std::string text() const {
    std::string lstr = _text;

    if (_seq) {
      lstr = (*_seq)[_idx].text();
    }

    return lstr;
  }

  int line() const {
    int ln = 0;

    if (_seq) {
      ln = (*_seq)[_idx].line();
    }

    return ln;
  }

  int column() const {
    int col = 0;

    if (_seq) {
      col = (*_seq)[_idx].column();
    }

    return col;
  }

  int width() {
    int w = 0;

    if (_seq) {
      w = (*_seq)[_idx].width();
    }

    return w;
  }

  int pos() const {
    return _idx;
  }

  std::string in_context(int ctx_lines = 3);

private:
  std::shared_ptr<token_cache> _seq;
  int _idx;
  std::string _text;
};

} // namespace dwt

#endif
