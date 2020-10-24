// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/token_type.hpp>

#include <vector>

namespace dwt {

#ifndef NDEBUG
namespace {

const std::vector<std::pair<token_type, std::string>> token_info = {

  { TOK_EOF, "end-of-file" },
  { TOK_INV, "unexpected" },
  { TOK_BREAK, "newline" },
  { TOK_WS, "whitespace" },
  { TOK_IDENT, "identifier" },
  { TOK_INT, "integer" },
  { TOK_DEC, "decimal" },
  { TOK_HEX, "hexadecimal" },
  { TOK_OCT, "octal" },
  { TOK_DOT, "." },
  { TOK_COLON, ":" },
  { TOK_SEMICOLON, ";" },
  { TOK_OR, "|" },
  { TOK_XOR, "^" },
  { TOK_AND, "&" },
  { TOK_TILDE, "~" },
  { TOK_LSHIFT, "<<" },
  { TOK_RSHIFT, ">>" },
  { TOK_MODULO, "%" },
  { TOK_LSQUARE, "[" },
  { TOK_RSQUARE, "]" },
  { TOK_DOUBLE_LSQ, "[[" },
  { TOK_DOUBLE_RSQ, "]]" },
  { TOK_LCURLY, "{" },
  { TOK_RCURLY, "}" },
  { TOK_LT, "<" },
  { TOK_GT, ">" },
  { TOK_LPAREN, "(" },
  { TOK_RPAREN, ")" },
  { TOK_COMMA, "," },
  { TOK_ASSIGN, "=" },
  { TOK_QMARK, "?" },
  { TOK_STAR, "*" },
  { TOK_FWDSLASH, "/" },
  { TOK_BSLASH, "\\" },
  { TOK_PLUS, "+" },
  { TOK_MINUS, "-" },
  { TOK_BANG, "!" },
  { TOK_LOG_AND, "&&" },
  { TOK_LOG_OR, "||" },
  { TOK_LT_EQ, "<=" },
  { TOK_GT_EQ, ">=" },
  { TOK_NOT_EQ, "!=" },
  { TOK_EQUALS, "==" },
  { TOK_SCOPE, "::" },
  { TOK_SQUOTE, "\'" },
  { TOK_DQUOTE, "\"" },
  { TOK_STRING, "string" },
  { TOK_COMMENT, "comment" },

  { KW_USE, "use" },
  { KW_API, "api" },
  { KW_IS, "is" },
  { KW_MOD, "mod" },
  { KW_FUN, "fun" },
  { KW_FFI, "ffi" },
  { KW_VAR, "var" },
  { KW_TRUE, "true" },
  { KW_FALSE, "false" },
  { KW_NIL, "nil" },
  { KW_OBJ, "obj" },
  { KW_SELF, "self" },
  { KW_FOR, "for" },
  { KW_IN, "in" },
  { KW_IF, "if" },
  { KW_ELSE, "else" },
  { KW_LOOP, "loop" },
  { KW_WHILE, "while" },
  { KW_PRINT, "print" },
  { KW_UNTIL, "until" },
  { KW_RET, "return" },
  { KW_BREAK, "break" },
  { KW_CONTINUE, "continue" },
  { KW_LAMBDA, u8"λ" },
  { KW_AS, "as" },
  { KW_ENUM, "enum" },
  { KW_AND, "and" },
  { KW_OR, "or" },
  { KW_XOR, "xor" }
};

} // namespace

std::string symtext(token_type sym) {
  std::string s;
  for (auto &i : token_info) {
    if (i.first == sym) {
      s = i.second;
      break;
    }
  }

  return s;
}

#endif

namespace {

const std::vector<std::pair<token_type, std::string>> keywords = {
  { KW_USE, "use" },
  { KW_API, "api" },
  { KW_IS, "is" },
  { KW_MOD, "mod" },
  { KW_FUN, "fun" },
  { KW_FFI, "ffi" },
  { KW_VAR, "var" },
  { KW_TRUE, "true" },
  { KW_FALSE, "false" },
  { KW_NIL, "nil" },
  { KW_OBJ, "obj" },
  { KW_SELF, "self" },
  { KW_FOR, "for" },
  { KW_IN, "in" },
  { KW_IF, "if" },
  { KW_ELSE, "else" },
  { KW_LOOP, "loop" },
  { KW_WHILE, "while" },
  { KW_PRINT, "print" },
  { KW_UNTIL, "until" },
  { KW_RET, "return" },
  { KW_BREAK, "break" },
  { KW_CONTINUE, "continue" },
  { KW_LAMBDA, u8"λ" },
  { KW_AS, "as" },
  { KW_ENUM, "enum" },
  { KW_AND, "and" },
  { KW_OR, "or" },
  { KW_XOR, "xor" }
};

}

bool is_keyword(std::string text, token_type &sym) {
  for (auto &p : keywords) {
    if (p.second == text) {
      sym = p.first;
      return true;
    }
  }

  return false;
}

} // namespace dwt
