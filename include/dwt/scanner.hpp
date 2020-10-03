// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_SCANNER_HPP
#define GUARD_DWT_SCANNER_HPP

#include <dwt/token_ref.hpp>
#include <dwt/token_seq.hpp>
#include <dwt/utf8_source.hpp>

#include <memory>

namespace dwt {

class scanner {
public:
  explicit scanner(utf8_source &);
  virtual ~scanner();

  token_ref next_token(bool skip_whitespace = true);

private:
  int peek_char();
  int next_char();

  void alphanumeric(std::string &, token_type &);
  bool isidchar(int);
  bool ishexdigit(int);
  bool isoctdigit(int);
  void octal(std::string &, token_type &);
  void decimal(std::string &, token_type &, bool);
  void hexadecimal(std::string &, token_type &);
  void escape_seq(std::string &, token_type &);
  void string_literal(std::string &, token_type &);
  bool comment(std::string &);
  bool block_comment(std::string &);
  void line_comment(std::string &);
  void whitespace(std::string &);
  size_t starting_column(std::string &);
  token_ref bad_token(std::string &lexeme);

  utf8_source &_chars;
  size_t _lineno;
  size_t _column;
  size_t _prev_column;
  std::shared_ptr<token_seq> _tokens;
};

} // namespace dwt

#endif
