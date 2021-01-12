// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_TOKEN_HPP
#define GUARD_DWT_TOKEN_HPP

#include <dwt/token_type.hpp>
#include <dwt/utf8.hpp>

#include <string>

namespace dwt {

class token {
public:
  token(dwt::token_type sym, std::string text, int line, int col);

  virtual ~token();

  dwt::token_type type() const {
    return sym;
  }

  const std::string &text() const {
    return _text;
  }

  int line() const {
    return ln;
  }

  int column() const {
    return col;
  }

  int width() {
    return utf8_strlen(_text);
  }

private:
  dwt::token_type sym;
  std::string _text;
  int ln;
  int col;
};

} // namespace dwt

#endif
