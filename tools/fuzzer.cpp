#include <dwt/feedback.hpp>
#include <dwt/scanner.hpp>
#include <dwt/utf8_source.hpp>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>

using namespace dwt;

std::shared_ptr<std::mt19937> mersenne_twister;
std::shared_ptr<std::uniform_int_distribution<int>> dice;

bool newline_is_allowed_between(token_type sym_x, token_type sym_y) {
  bool allow = false;

  do {
    if (sym_x == TOK_INV) { // implies sym_y is the first token_type
      allow = true;
      break;
    }

    if (sym_x == TOK_RPAREN && sym_y == TOK_LCURLY) {
      allow = true;
      break;
    }

  } while (0);

  return allow;
}

bool newline_is_allowed_after(token_type sym) {
  bool allow = false;

  switch (sym) {
  case TOK_BREAK:
  case TOK_DOT:
  case TOK_COLON:
  case TOK_SEMICOLON:
  case TOK_OR:
  case TOK_XOR:
  case TOK_AND:
  case TOK_TILDE:
  case TOK_LSHIFT:
  case TOK_RSHIFT:
  case TOK_MODULO:
  case TOK_LSQUARE:
  case TOK_LCURLY:
  case TOK_LT:
  case TOK_GT:
  case TOK_LPAREN:
  case TOK_COMMA:
  case TOK_ASSIGN:
  case TOK_FWDSLASH:
  case TOK_BSLASH:
  case TOK_PLUS:
  case TOK_MINUS:
  case TOK_BANG:
  case TOK_LOG_AND:
  case TOK_LOG_OR:
  case TOK_LT_EQ:
  case TOK_GT_EQ:
  case TOK_NOT_EQ:
  case TOK_EQUALS:
  case TOK_RANGE:
  case TOK_SCOPE:
  case KW_AND:
  case KW_OR:
  case KW_XOR:

    allow = true;
    break;

  default:

    break;
  }

  return allow;
}

int roll_dice() {
  return (*dice)(*mersenne_twister);
}

bool maybe() {
  return roll_dice() & 1;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    return 1;
  }

  unsigned int seed = 0;

  if (argc == 3) {
    seed = strtoul(argv[2], nullptr, 16);
  } else {
    std::random_device rdev;
    seed = rdev();
  }

  dice = std::make_shared<std::uniform_int_distribution<int>>(1, 6);
  mersenne_twister = std::make_shared<std::mt19937>(seed);

  try {
    utf8_source src(argv[1]);
    scanner scanner(src);

    token_type prev_sym = TOK_INV;
    token_type next_sym = TOK_INV;

    while (1) {
      auto next_tok = scanner.next_token(false /* skip whitespace */);
      next_sym = next_tok.type();
      std::string text = next_tok.text();

      if (next_sym == TOK_EOF || next_sym == TOK_INV) {
        break;
      }

      if (next_sym == TOK_SEMICOLON) {
        if (maybe()) {
          next_sym = TOK_BREAK;
          text = "\n";
        }
      }

      if (newline_is_allowed_between(prev_sym, next_sym)) {
        if (maybe()) {
          out("\n");
        }
      }

      out(text);

      if (newline_is_allowed_after(next_sym)) {
        if (maybe()) {
          out("\n");
        }
      }

      if (next_sym != TOK_BREAK && next_sym != TOK_WS) {
        prev_sym = next_sym;
      }
    }

    out("\n");

  } catch (...) {
    return 1;
  }

  return 0;
}
