// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/reporting.hpp>
#include <dwt/scanner.hpp>
#include <dwt/token_seq.hpp>
#include <dwt/utf8.hpp>

#include <cassert>
#include <iostream>

namespace dwt {

scanner::scanner(utf8_source &chars)
  : _chars(chars)
  , _lineno(1)
  , _column(0)
  , _prev_column(0)
  , _tokens(chars.tokens()) {
}

scanner::~scanner() {
}

int scanner::peek_char() {
  return _chars.peek();
}

int scanner::next_char() {
  _prev_column = _column;
  int ch = _chars.get();

  if (ch == '\n') {
    _column = 0;
    ++_lineno;
  } else if (ch != '\r') {
    if (ch == '\t') {
      // treat tab as 8 spaces, do we need to detect this?
      _column += 8;
    } else {
      ++_column;
    }
  } else {
    _column = 0;
  }

  return ch;
}

bool scanner::isidchar(int ch) {
  return isalpha(ch) || ch > 127;
}

void scanner::alphanumeric(std::string &lexeme, token_type &sym) {
  sym = TOK_IDENT;
  int ch;

  while (isidchar(ch = peek_char()) || isdigit(ch) || ch == '_') {
    ch = next_char();
    lexeme += utf8_encode(ch);
  }
}

void scanner::decimal(std::string &lexeme, token_type &sym, bool dp) {
  sym = TOK_INT;
  int ch;

  while (isalpha(ch = peek_char()) || isdigit(ch) || ch == '_' || ch == '.') {
    lexeme += utf8_encode(ch);

    if (ch == '.') {
      if (dp) {
        oops("e@1 more than 1 decimal point", bad_token(lexeme));
      } else {
        sym = TOK_DEC;
        dp = true;
      }

      next_char();

      if (peek_char() == '_') {
        oops("e@1 expected decimal literal", bad_token(lexeme));
      }
    } else if (ch == '_') {
      next_char();

      switch (peek_char()) {
      case '.':
      case '_':
        oops("e@1 expected decimal literal", bad_token(lexeme));
        break;
      default:
        break;
      }
    } else if (!isdigit(ch)) {
      next_char();
      oops("e@1 expected decimal literal", bad_token(lexeme));
    } else {
      next_char();
    }
  }

  if (ch == '_') {
    oops("e@1 expected decimal literal", bad_token(lexeme));
  }
}

bool scanner::ishexdigit(int ch) {
  if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') ||
      (ch >= 'A' && ch <= 'F')) {
    return true;
  }
  return false;
}

void scanner::hexadecimal(std::string &lexeme, token_type &sym) {
  sym = TOK_HEX;
  int ch;

  while (isalpha(ch = peek_char()) || isdigit(ch) || ch == '_') {
    lexeme += utf8_encode(ch);
    next_char();

    if (!ishexdigit(ch)) {
      oops("e@1 expected hexadecimal literal", bad_token(lexeme));
    }
  }
}

bool scanner::isoctdigit(int ch) {
  if (ch >= '0' && ch <= '7') {

    return true;
  }
  return false;
}

void scanner::octal(std::string &lexeme, token_type &sym) {
  sym = TOK_OCT;
  int ch;

  while (isalpha(ch = peek_char()) || isoctdigit(ch) || ch == '_') {
    lexeme += utf8_encode(ch);

    if (!isoctdigit(ch) && (isalpha(ch) || ch == '_') && sym != TOK_INV) {
      oops("e@1 expected octal literal", bad_token(lexeme));
    } else if (!isoctdigit(ch) && sym != TOK_INV) {
      oops("e@1 expected octal literal", bad_token(lexeme));
    }
    next_char();
  }
}

void scanner::escape_seq(std::string &lexeme, token_type &sym) {
  char ch = next_char();

  switch (ch) {
  case 'a':
    lexeme += '\a';
    break;
  case 'b':
    lexeme += '\b';
    break;
  case 'f':
    lexeme += '\f';
    break;
  case 'n':
    lexeme += '\n';
    break;
  case 'r':
    lexeme += '\r';
    break;
  case 't':
    lexeme += '\t';
    break;
  case 'v':
    lexeme += '\v';
    break;
  case '\\':
    lexeme += '\\';
    break;
  case '\'':
    lexeme += '\'';
    break;
  case '\"':
    lexeme += '\"';
    break;
  case 'u':
  case 'U':
  default:
    oops("e@1 unrecognised escape sequence '$1'", bad_token(lexeme));
    break;
  }
}

void scanner::string_literal(std::string &lexeme, token_type &sym) {
  sym = TOK_INV;
  int ch;

  while ((ch = next_char()) != 0) {
    switch (ch) {
    case ' ':
    case '!':
    case '=':
      lexeme += utf8_encode(ch);
      break;

    case '"':
      sym = TOK_STRING;
      lexeme += utf8_encode(ch);
      return;

    case '\\':
      escape_seq(lexeme, sym);
      break;

    default:
      if ((ch >= 32 && ch < 127) || (ch > 127)) {
        lexeme += utf8_encode(ch);
      } else {
        oops("e@1 invalid character in string literal", bad_token(lexeme));
      }
    }
  }
}

void scanner::block_comment(std::string &lexeme) {
  int ch;

  while (1) {
    ch = peek_char();

    if (ch == '\n') {
      int col = starting_column(lexeme);
      lexeme += next_char();
      _tokens->add(token(TOK_COMMENT, lexeme, _lineno - 1, col));
      lexeme = "";
    } else if (ch == '*') {
      lexeme += ch;
      next_char();
      if (peek_char() == '/') {
        lexeme += next_char();
        _tokens->add(
          token(TOK_COMMENT, lexeme, _lineno, starting_column(lexeme)));
        break;
      }
    } else if (ch == EOF) {
      oops("e@1 expected '/'", bad_token(lexeme));
      break;
    } else {
      lexeme += ch;
      next_char();
    }
  }
}

void scanner::line_comment(std::string &lexeme) {
  int ch;

  while (1) {
    ch = peek_char();

    if (ch != '\n' && ch != EOF) {
      lexeme += utf8_encode(ch);
      next_char();
    } else {
      break;
    }
  }

  _tokens->add(token(TOK_COMMENT, lexeme, _lineno, starting_column(lexeme)));
}

bool scanner::comment(std::string &lexeme) {
  bool is_comment = false;
  int ch = peek_char();

  if (ch == '/') {
    is_comment = true;
    lexeme += utf8_encode(ch);
    ch = next_char();
    line_comment(lexeme);
  } else if (ch == '*') {
    is_comment = true;
    lexeme += utf8_encode(ch);
    next_char();
    block_comment(lexeme);
  }

  return is_comment;
}

void scanner::whitespace(std::string &lexeme) {
  // group adjacent tabs and spaces into a single token...
  while (1) {
    int ch = peek_char();

    if (ch == ' ' || ch == '\t') {
      lexeme += utf8_encode(ch);
      next_char();
    } else {
      break;
    }
  }
}

size_t scanner::starting_column(std::string &lexeme) {
  if (lexeme != "\n" && lexeme != "\r") {
    size_t lexlen = utf8_strlen(lexeme);

    return (_column - lexlen);
  } else {
    return _column;
  }
}

token_ref scanner::bad_token(std::string &lexeme) {
  _tokens->add(token(TOK_INV, lexeme, _lineno, starting_column(lexeme) - 1));
  return token_ref(_tokens, _tokens->size() - 1);
}

token_ref scanner::next_token(bool skip_whitespace) {
  int ch = next_char();
  token_type sym = TOK_INV;
  std::string lexeme;
  size_t lineno;
  size_t col;

  while (ch == '\t' || ch == ' ' || ch == '/') {
    if (ch == '/') {
      lexeme += utf8_encode(ch);
      if (comment(lexeme)) {
        ch = next_char();
        continue;
      } else {
        break;
      }
    } else {
      if (skip_whitespace) {
        sym = TOK_WS;
        lexeme += utf8_encode(ch);
        whitespace(lexeme);
        ch = next_char();
      } else {
        break;
      }
    }

    _tokens->add(token(sym, lexeme, _lineno, starting_column(lexeme) - 1));
  }

  sym = TOK_INV;
  lexeme = "";

  switch (ch) {
  case '\t':
  case ' ':
    sym = TOK_WS;
    lexeme += utf8_encode(ch);
    whitespace(lexeme);
    break;

  case '\n':
    sym = TOK_BREAK;
    lexeme += utf8_encode(ch);

    if (peek_char() == '\r') {
      lexeme += next_char();
    }
    break;

  case '\\':
    sym = TOK_BSLASH;
    lexeme += utf8_encode(ch);
    break;

  case '\'':
    sym = TOK_SQUOTE;
    lexeme += utf8_encode(ch);
    break;

  case '/':
    lexeme += utf8_encode(ch);
    sym = TOK_FWDSLASH;
    break;

  case '+':
    sym = TOK_PLUS;
    lexeme += utf8_encode(ch);
    break;

  case '-':
    sym = TOK_MINUS;
    lexeme += utf8_encode(ch);
    break;

  case '.':
    lexeme += utf8_encode(ch);

    if (peek_char() == '.') {
      ch = next_char();
      lexeme += utf8_encode(ch);
      sym = TOK_RANGE;
    } else if (isdigit(peek_char())) {
      sym = TOK_DEC;
      decimal(lexeme, sym, true); // dp consumed
    } else {
      sym = TOK_DOT;
    }
    break;

  case '~':
    lexeme += utf8_encode(ch);
    sym = TOK_TILDE;
    break;

  case '(':
    sym = TOK_LPAREN;
    lexeme += utf8_encode(ch);
    break;

  case ')':
    sym = TOK_RPAREN;
    lexeme += utf8_encode(ch);
    break;

  case ':':
    lexeme += utf8_encode(ch);

    if (peek_char() == ':') {
      sym = TOK_SCOPE;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else if (peek_char() == '=') {
      sym = TOK_ASSIGN;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else {
      sym = TOK_COLON;
    }
    break;

  case '|':
    lexeme += utf8_encode(ch);

    if (peek_char() == '|') {
      sym = TOK_LOG_OR;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else {
      sym = TOK_OR;
    }
    break;

  case '*':
    sym = TOK_STAR;
    lexeme += utf8_encode(ch);
    break;

  case '^':
    sym = TOK_XOR;
    lexeme += utf8_encode(ch);
    break;

  case '&':
    lexeme += utf8_encode(ch);

    if (peek_char() == '&') {
      sym = TOK_LOG_AND;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else {
      sym = TOK_AND;
    }
    break;

  case '<':
    lexeme += utf8_encode(ch);

    if (peek_char() == '<') {
      sym = TOK_LSHIFT;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else if (peek_char() == '=') {
      sym = TOK_LT_EQ;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else {
      sym = TOK_LT;
    }
    break;

  case '>':
    lexeme += utf8_encode(ch);

    if (peek_char() == '>') {
      sym = TOK_RSHIFT;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else if (peek_char() == '=') {
      sym = TOK_GT_EQ;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else {
      sym = TOK_GT;
    }
    break;

  case '%':
    sym = TOK_MODULO;
    lexeme += utf8_encode(ch);
    break;

  case '!':
    lexeme += utf8_encode(ch);

    if (peek_char() == '=') {
      sym = TOK_NOT_EQ;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else {
      sym = TOK_BANG;
    }
    break;

  case ';':
    sym = TOK_SEMICOLON;
    lexeme += utf8_encode(ch);
    break;

  case '[':
    lexeme += utf8_encode(ch);

    if (peek_char() == '[') {
      sym = TOK_DOUBLE_LSQ;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else {
      sym = TOK_LSQUARE;
    }
    break;

  case ']':
    lexeme += utf8_encode(ch);

    if (peek_char() == ']') {
      sym = TOK_DOUBLE_RSQ;
      ch = next_char();
      lexeme += utf8_encode(ch);
    } else {
      sym = TOK_RSQUARE;
    }
    break;

  case '{':
    sym = TOK_LCURLY;
    lexeme += utf8_encode(ch);
    break;

  case '}':
    sym = TOK_RCURLY;
    lexeme += utf8_encode(ch);
    break;

  case ',':
    sym = TOK_COMMA;
    lexeme += utf8_encode(ch);
    break;

  case '=':
    lexeme += utf8_encode(ch);

    if (peek_char() == '=') {
      sym = TOK_EQUALS;
      ch = next_char();
      lexeme += utf8_encode(ch);
    }
    break;

  case '?':
    lexeme += utf8_encode(ch);
    sym = TOK_QMARK;
    break;

  case '0':
    lexeme += utf8_encode(ch);

    if (peek_char() == 'x' || ch == 'X') {
      lexeme += next_char();
      hexadecimal(lexeme, sym);
    } else if (peek_char() == '.') {
      decimal(lexeme, sym, false);
    } else if (!isdigit(peek_char())) {
      sym = TOK_DEC;
    } else {
      octal(lexeme, sym);
    }
    break;

  case '"':
    lexeme += utf8_encode(ch);
    string_literal(lexeme, sym);
    break;

  case -1:
  case 0:
    sym = TOK_EOF;
    lexeme += utf8_encode(ch);
    break;

  case 0x3bb:

    lexeme += utf8_encode(ch);
    sym = KW_LAMBDA;
    break;

  default:
    if (isidchar(ch)) {
      lexeme += utf8_encode(ch);
      alphanumeric(lexeme, sym);
      is_keyword(lexeme, sym);
    } else if (isdigit(ch)) {
      lexeme += utf8_encode(ch);
      decimal(lexeme, sym, false);
    } else {
      lexeme += utf8_encode(ch);
      oops("e@1 unsupported character '$1'", bad_token(lexeme));
    }
    break;
  }

  if (sym == TOK_BREAK) {
    lineno = _lineno - 1;
    col = _prev_column;
  } else {
    lineno = _lineno;
    col = starting_column(lexeme);
  }

  _tokens->add(token(sym, lexeme, lineno, col));

  return token_ref(_tokens, _tokens->size() - 1);
}

} // namespace dwt
