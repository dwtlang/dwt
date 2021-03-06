// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_TOKEN_TYPE_H
#define GUARD_DWT_TOKEN_TYPE_H

#include <string>

namespace dwt {

typedef enum {
  TOK_EOF = 0,
  TOK_INV,
  TOK_BREAK,
  TOK_WS,
  TOK_IDENT,
  TOK_INT,
  TOK_DEC,
  TOK_HEX,
  TOK_OCT,
  TOK_DOT,
  TOK_COLON,
  TOK_SEMICOLON,
  TOK_OR,
  TOK_XOR,
  TOK_AND,
  TOK_TILDE,
  TOK_LSHIFT,
  TOK_RSHIFT,
  TOK_MODULO,
  TOK_LSQUARE,
  TOK_RSQUARE,
  TOK_DOUBLE_LSQ,
  TOK_DOUBLE_RSQ,
  TOK_LCURLY,
  TOK_RCURLY,
  TOK_LT,
  TOK_GT,
  TOK_LPAREN,
  TOK_RPAREN,
  TOK_COMMA,
  TOK_ASSIGN,
  TOK_WALRUS,
  TOK_QMARK,
  TOK_STAR,
  TOK_FWDSLASH,
  TOK_BSLASH,
  TOK_PLUS,
  TOK_MINUS,
  TOK_BANG,
  TOK_LOG_AND,
  TOK_LOG_OR,
  TOK_LT_EQ,
  TOK_GT_EQ,
  TOK_NOT_EQ,
  TOK_EQUALS,
  TOK_RANGE,
  TOK_SCOPE,
  TOK_SQUOTE,
  TOK_DQUOTE,
  TOK_STRING,
  TOK_COMMENT,

  FFI_VER,
  FFI_DUP,
  FFI_STR,
  FFI_LEN,
  FFI_GC,
  FFI_SLEEP,

  KW_USE,
  KW_API,
  KW_IS,
  KW_MOD,
  KW_FUN,
  KW_FFI,
  KW_VAR,
  KW_TRUE,
  KW_FALSE,
  KW_NIL,
  KW_OBJ,
  KW_SELF,
  KW_PRINT,
  KW_PRINTLN,
  KW_FOR,
  KW_IN,
  KW_IF,
  KW_ELSE,
  KW_LOOP,
  KW_WHILE,
  KW_UNTIL,
  KW_RETURN,
  KW_YIELD,
  KW_BREAK,
  KW_CONTINUE,

  KW_LAMBDA,
  KW_AND,
  KW_OR,
  KW_XOR
} token_type;

#ifndef NDEBUG
std::string symtext(token_type s);
#endif

bool is_keyword(std::string s, token_type &sym);

} // namespace dwt

#endif
