// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/debug.hpp>
#include <dwt/feedback.hpp>
#include <dwt/globals.hpp>
#include <dwt/inbuilt.hpp>
#include <dwt/ir/lazy_visitor.hpp>
#include <dwt/ir/string_spec.hpp>
#include <dwt/ir/unary_expr.hpp>
#include <dwt/parser.hpp>
#include <dwt/reporting.hpp>
#include <dwt/scope.hpp>

namespace dwt {

using namespace ir;

parser::parser(utf8_source &&utf8)
  : scanner(utf8)
  , _token_cache(utf8.tokens()) {

  inbuilt::get(); // register inbuilt functions
}

parser::~parser() {
}

/**
 * Retrieve the next token from the scanner.
 *
 * The current token is retained as a reference to the previous token.
 */
void parser::advance() {
  _prev_token = _this_token;
  _this_token = next_token();
}

/**
 * Accept a given token if present.
 *
 * Consumes the current token if it matches the argument.
 *
 * @param tok The argument to match against
 * @return true if `tok` matches, false otherwise.
 */
bool parser::accept(token_type tok) {
  if (peek(tok)) {
    debug {
      dbg("accept ");
      std::string l = _this_token.text();
      if (l == "\n") {
        l = "\\n";
      }
      dbg(l + "\n");
    }

    advance();

    return true;
  }

  return false;
}

/**
 * Accept any valid token.
 */
void parser::accept() {
  accept(peek());
}

/**
 * Expect a given token.
 *
 * Throw an exception if the token does not match.
 *
 * @param tok The token to expect.
 */
void parser::expect(token_type tok) {
#ifndef NDEBUG
  dbg("expect " + symtext(tok) + "\n");
#endif

  if (!accept(tok)) {
    debug {
      dbg("reject ");
      std::string l = _this_token.text();
      if (l == "\n") {
        l = "\\n";
      }
      dbg(l + "\n");
    }

    oops("e@1 unexpected symbol '$1'", _this_token);
  }
}

/**
 * Skip any number of the given token.
 *
 * @param tok Token to skip.
 */
void parser::skip_any(token_type tok) {
  while (accept(tok))
    ;
}

/**
 * Expect any valid token that can end a statement.
 */
void parser::stmt_end() {
  if (!peek_any(TOK_LCURLY, TOK_RCURLY)) {
    if (_comma_stmt_sep) {
      expect_any(TOK_COMMA, TOK_BREAK);
    } else {
      expect_any(TOK_SEMICOLON, TOK_BREAK);
    }
  }
  skip_any(TOK_BREAK);
}

/**
 * Parse the input into an AST.
 *
 * @return The AST.
 */
std::unique_ptr<ast> parser::parse() {
  advance();

  return std::unique_ptr<ast>(parse_script());
}

/**
 * Parse a script.
 *
 * @return The script AST
 */
std::unique_ptr<script> parser::parse_script() {
  auto scr = std::make_unique<script>();

  push_self(scope::current);

  while (!peek(TOK_EOF)) {
    scr->splice(parse_declaration());
  }

  pop_self();

  return scr;
}

/**
 * Parse a declaration.
 *
 * @return The declaration AST.
 */
std::unique_ptr<declaration> parser::parse_declaration() {
  skip_any(TOK_BREAK);

  std::unique_ptr<declaration> decl;
  bool api = accept(KW_API);

  if (api) {
    skip_any(TOK_BREAK);
  }

  switch (peek()) {
  case KW_MOD:
    if (api) {
      oops("e@1 keyword 'api' has no meaning here", gettok());
    }
    decl = parse_module_decl();
    break;
  case KW_FUN:
    decl = parse_function_decl();
    break;
  case KW_OBJ:
    decl = parse_object_decl();
    break;
  case KW_FFI:
    decl = parse_ffi_decl();
    break;
  case KW_VAR:
    decl = parse_var_decl();
    break;
  default:
    if (api) {
      oops("e@1 keyword 'api' has no meaning here", gettok());
    }
    decl = parse_stmt();
    break;
  }

  decl->is_api(api);

  return decl;
}

/**
 * Parse a declaration allowed within an object definition.
 *
 * @return The declaration AST.
 */
std::unique_ptr<object_decl> parser::parse_object_decl() {
  expect(KW_OBJ);
  skip_any(TOK_BREAK);
  expect(TOK_IDENT);
  auto decl = std::make_unique<object_decl>(gettok());
  scope::open(gettok(), SCOPE_EXCLUSIVE | SCOPE_CREATE);
  skip_any(TOK_BREAK);

  push_self(scope::current);

  auto impl = parse_object();

  scope::close();

  if (impl->params()) {
    decl->arity(impl->params()->nr_children());
  } else {
    decl->arity(0);
  }

  decl->splice(std::move(impl));

  skip_any(TOK_BREAK);

  pop_self();

  return decl;
}

/**
 * Parse a lambda expression.
 *
 * @return The lambda expression AST.
 */
std::unique_ptr<lambda_expr> parser::parse_lambda_expr() {
  auto exp = std::make_unique<lambda_expr>();

  expect_any(TOK_BSLASH, KW_LAMBDA);
  skip_any(TOK_BREAK);
  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);
  if (!accept(TOK_RPAREN)) {
    exp->args(parse_arguments());
    expect(TOK_RPAREN);
  } else {
    exp->args(std::make_unique<arguments>());
  }
  skip_any(TOK_BREAK);

  exp->impl(parse_lambda_decl());

  return exp;
}

/**
 * Parse a lambda definition.
 *
 * @return The lambda definition AST.
 */
std::unique_ptr<lambda> parser::parse_lambda() {
  auto lam = std::make_unique<lambda>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  if (accept(TOK_OR)) {
    skip_any(TOK_BREAK);
    lam->params(parse_parameters());
    skip_any(TOK_BREAK);
    expect(TOK_OR);
    skip_any(TOK_BREAK);
  }

  lam->body(parse_function_body());

  scope::close();

  return lam;
}

/**
 * Parse a foreign function interface (FFI) declaration.
 *
 * @return The FFI declaration AST.
 */
std::unique_ptr<ffi_decl> parser::parse_ffi_decl() {
  expect(KW_FFI);
  expect(TOK_IDENT);

  auto decl = std::make_unique<ffi_decl>(gettok());
  scope::add(gettok(), SCOPE_CREATE);

  skip_any(TOK_BREAK);
  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RPAREN)) {
    skip_any(TOK_BREAK);
    decl->splice(parse_parameters());
  }

  stmt_end();

  return decl;
}

/**
 * Parse a module declaration.
 *
 * @return The module declaration AST.
 */
std::unique_ptr<module_decl> parser::parse_module_decl() {
  expect(KW_MOD);
  expect(TOK_IDENT);
  auto module = std::make_unique<module_decl>(gettok());
  scope::open(gettok(), SCOPE_APPEND | SCOPE_CREATE);

  skip_any(TOK_BREAK);
  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);
    module->splice(parse_declaration());
  }

  skip_any(TOK_BREAK);

  scope::close();

  return module;
}

/**
 * Parse a lambda declaration.
 *
 * @return The lambda declaration AST.
 */
std::unique_ptr<lambda_decl> parser::parse_lambda_decl() {
  auto decl = std::make_unique<lambda_decl>();
  scope::open(decl->name_tok(), SCOPE_CREATE);

  push_self(scope::current);

  auto impl = parse_lambda();

  scope::close();

  if (impl->params()) {
    decl->arity(impl->params()->nr_children());
  } else {
    decl->arity(0);
  }

  decl->splice(std::move(impl));

  pop_self();

  return decl;
}

/**
 * Parse a map definition.
 *
 * @return The map definition AST.
 */
std::unique_ptr<map_impl> parser::parse_map_impl() {
  auto impl = std::make_unique<map_impl>();
  scope::open(impl->name_tok(), SCOPE_CREATE);

  push_self(scope::current);

  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  if (!accept(TOK_RCURLY)) {
    do {
      skip_any(TOK_BREAK);
      impl->splice(parse_kv_pair(impl->nr_children()));
      skip_any(TOK_BREAK);
    } while (accept(TOK_COMMA));

    expect(TOK_RCURLY);
  }

  pop_self();

  return impl;
}

/**
 * Parse a function declaration.
 *
 * @return The function declaration AST.
 */
std::unique_ptr<declaration> parser::parse_function_decl() {
  expect(KW_FUN);
  expect(TOK_IDENT);
  auto decl = std::make_unique<function_decl>(gettok());
  scope::open(gettok(), SCOPE_EXCLUSIVE | SCOPE_CREATE);
  skip_any(TOK_BREAK);

  push_self(scope::current);

  auto impl = parse_function();

  scope::close();

  if (impl->params()) {
    decl->arity(impl->params()->nr_children());
  } else {
    decl->arity(0);
  }

  decl->splice(std::move(impl));

  skip_any(TOK_BREAK);

  pop_self();

  return decl;
}

/**
 * Parse a function definition.
 *
 * @return The function definition AST.
 */
std::unique_ptr<function> parser::parse_function() {
  auto fun = std::make_unique<function>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);

  if (!accept(TOK_RPAREN)) {
    skip_any(TOK_BREAK);
    fun->params(parse_parameters());
    expect(TOK_RPAREN);
  }

  fun->body(parse_function_body());

  scope::close();

  return fun;
}

/**
 * Parse an object definition.
 *
 * @return The object definiton AST.
 */
std::unique_ptr<object> parser::parse_object() {
  auto obj = std::make_unique<object>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);

  if (!accept(TOK_RPAREN)) {
    skip_any(TOK_BREAK);
    obj->params(parse_parameters());
    expect(TOK_RPAREN);
  }

  skip_any(TOK_BREAK);

  if (peek(TOK_COLON)) {
    obj->super(parse_super_expr());
    skip_any(TOK_BREAK);
  }

  obj->body(parse_object_body());

  scope::close();

  return obj;
}

/**
 * Parse function parameters.
 *
 * @return The parameters AST.
 */
std::unique_ptr<parameters> parser::parse_parameters() {
  auto params = std::make_unique<parameters>();
  do {
    if (prev(TOK_COMMA)) {
      skip_any(TOK_BREAK);
    }
    accept(KW_VAR);
    expect(TOK_IDENT);
    auto identifier = gettok();
    auto param = std::make_unique<parameter>(gettok());
    params->splice(std::move(param));
    skip_any(TOK_BREAK);
    scope::add(identifier, SCOPE_EXCLUSIVE | SCOPE_CREATE);
  } while (accept(TOK_COMMA));

  skip_any(TOK_BREAK);

  return params;
}

/**
 * Parse a scoped block.
 *
 * @return The scoped block AST.
 */
std::unique_ptr<block> parser::parse_block() {
  auto blk = std::make_unique<block>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);
    blk->splice(parse_declaration());
  }

  skip_any(TOK_BREAK);

  scope::close();

  return blk;
}

/**
 * Parse a function body.
 *
 * @return The function body AST.
 */
std::unique_ptr<function_body> parser::parse_function_body() {
  auto body = std::make_unique<function_body>();

  if (accept(TOK_BREAK)) {
    skip_any(TOK_BREAK);
    expect(TOK_LCURLY);
  } else if (!accept(TOK_LCURLY)) {
    body->splice(parse_stmt());
    return body;
  }

  skip_any(TOK_BREAK);

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);
    body->splice(parse_declaration());
  }

  return body;
}

/**
 * Parse an object body.
 *
 * @return The object body AST.
 */
std::unique_ptr<object_body> parser::parse_object_body() {
  auto body = std::make_unique<object_body>();

  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);

    std::unique_ptr<declaration> decl;
    bool api = accept(KW_API);

    if (api) {
      skip_any(TOK_BREAK);
    }

    switch (peek()) {
    case KW_FUN:
      decl = parse_function_decl();
      break;
    case KW_OBJ:
      decl = parse_object_decl();
      break;
    case KW_VAR:
      decl = parse_var_decl();
      break;
    default:
      if (api) {
        oops("e@1 keyword 'api' has no meaning here", gettok());
      }
      decl = parse_stmt();
      break;
    }

    decl->is_api(api);
    body->splice(std::move(decl));
  }

  skip_any(TOK_BREAK);

  return body;
}

/**
 * Parse a variable declaration.
 *
 * @return The variable declaration AST.
 */
std::unique_ptr<var_decl> parser::parse_var_decl() {
  expect(KW_VAR);
  expect(TOK_IDENT);

  auto decl = std::make_unique<var_decl>(gettok());
  scope::add(gettok(), SCOPE_EXCLUSIVE | SCOPE_CREATE);

  if (accept(TOK_ASSIGN)) {
    skip_any(TOK_BREAK);
    decl->splice(parse_expr());
  } else if (accept(TOK_WALRUS)) {
    oops(
      "e@1 did you mean '='? Use ':=' only for assignment within expressions",
      gettok());
  }

  stmt_end();

  return decl;
}

/**
 * Parse a statement.
 *
 * @return The statement.
 */
std::unique_ptr<stmt> parser::parse_stmt() {
  switch (peek()) {
  case KW_RETURN:
    return parse_return_stmt();
    break;
  case KW_YIELD:
    return parse_yield_stmt();
    break;
  case KW_PRINTLN:
  case KW_PRINT:
    return parse_print_stmt();
    break;
  case KW_IF:
    return parse_if_stmt();
    break;
  case KW_LOOP:
  case KW_WHILE:
  case KW_UNTIL:
  case KW_FOR:
    return parse_loop_stmt();
    break;
  case KW_USE:
    return parse_use_stmt();
    break;
  case KW_CONTINUE:
    return parse_continue_stmt();
    break;
  case KW_BREAK:
    return parse_break_stmt();
    break;
  case TOK_LCURLY:
    return parse_block();
    break;
  default:
    return parse_expr_stmt();
    break;
  }

  BUG(); // should be unreachable

  return nullptr;
}

/**
 * Parse an expression statement.
 *
 * @return The expression statement.
 */
std::unique_ptr<expr_stmt> parser::parse_expr_stmt() {
  auto stmt = std::make_unique<expr_stmt>();
  stmt->splice(parse_expr());

  stmt_end();

  return stmt;
}

/**
 * Parse a "use" statement.
 *
 * @return The statement AST.
 */
std::unique_ptr<use_stmt> parser::parse_use_stmt() {
  expect(KW_USE);

  auto stmt = std::make_unique<use_stmt>();

  expect(TOK_STRING);

  auto file = std::make_unique<string_spec>(gettok());

  stmt->splice(std::move(file));

  stmt_end();

  return stmt;
}

/**
 * Parse an "if" statement.
 *
 * @return the statement AST.
 */
std::unique_ptr<if_stmt> parser::parse_if_stmt() {
  auto statement = std::make_unique<if_stmt>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  expect(KW_IF);
  statement->cond(parse_expr());
  skip_any(TOK_BREAK);
  if (peek_any(KW_LOOP, KW_WHILE, KW_UNTIL)) {
    statement->if_body(parse_loop_stmt());
  } else {
    statement->if_body(parse_stmt());
  }

  if (accept(KW_ELSE)) {
    skip_any(TOK_BREAK);
    if (peek_any(KW_LOOP, KW_WHILE, KW_UNTIL)) {
      statement->else_body(parse_loop_stmt());
    } else {
      statement->else_body(parse_stmt());
    }
  }

  skip_any(TOK_BREAK);

  scope::close();

  return statement;
}

/**
 * Parse a print statement.
 *
 * @return The statement AST.
 */
std::unique_ptr<print_stmt> parser::parse_print_stmt() {
  expect_any(KW_PRINT, KW_PRINTLN);
  auto stmt = std::make_unique<print_stmt>(gettok());
  skip_any(TOK_BREAK);
  stmt->splice(parse_expr());
  stmt_end();

  return stmt;
}

/**
 * Parse a return statement.
 *
 * @return The statement AST.
 */
std::unique_ptr<return_stmt> parser::parse_return_stmt() {
  expect(KW_RETURN);

  auto stmt = std::make_unique<return_stmt>(gettok());

  if (!accept_any(TOK_SEMICOLON, TOK_BREAK)) {
    stmt->splice(parse_expr());
    stmt_end();
  }

  skip_any(TOK_BREAK);

  return stmt;
}

/**
 * Parse a yield statement.
 *
 * @return The statement AST.
 */
std::unique_ptr<yield_stmt> parser::parse_yield_stmt() {
  expect(KW_YIELD);

  auto stmt = std::make_unique<yield_stmt>(gettok());

  if (!accept_any(TOK_SEMICOLON, TOK_BREAK)) {
    stmt->splice(parse_expr());
    stmt_end();
  }

  skip_any(TOK_BREAK);

  return stmt;
}

/**
 * Parse a "continue" statement.
 *
 * @return The statement AST.
 */
std::unique_ptr<continue_stmt> parser::parse_continue_stmt() {
  std::unique_ptr<continue_stmt> statement;

  expect(KW_CONTINUE);

  if (accept(TOK_IDENT)) {
    statement = std::make_unique<continue_stmt>(gettok());
    stmt_end();
  } else {
    statement = std::make_unique<continue_stmt>();
    stmt_end();
  }

  return statement;
}

/**
 * Parse a "break" statement.
 *
 * @return The statement AST.
 */
std::unique_ptr<break_stmt> parser::parse_break_stmt() {
  std::unique_ptr<break_stmt> statement;

  expect(KW_BREAK);

  if (accept(TOK_IDENT)) {
    statement = std::make_unique<break_stmt>(gettok());
    stmt_end();
  } else {
    statement = std::make_unique<break_stmt>();
    stmt_end();
  }

  return statement;
}

/**
 * Parse a loop declaration.
 *
 * @return The loop declaration AST.
 */
std::unique_ptr<loop_stmt> parser::parse_loop_decl() {
  token_ref tag;

  if (accept(KW_LOOP)) {
    skip_any(TOK_BREAK);
    if (accept(TOK_IDENT)) {
      tag = gettok(); // tagged loop
      skip_any(TOK_BREAK);
    }
  }

  auto decl = std::make_unique<loop_stmt>(tag);
  return decl;
}

/**
 * Parse a loop statement.
 *
 * @return The loop statement AST.
 */
std::unique_ptr<loop_stmt> parser::parse_loop_stmt() {
  loop_type loop_type = BASIC_LOOP;
  std::unique_ptr<loop_stmt> loop;
  std::unique_ptr<expr> cond;
  std::unique_ptr<stmt> body;
  std::unique_ptr<declaration> before;
  std::unique_ptr<stmt> after;

  loop = parse_loop_decl();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  if (accept(KW_WHILE)) {
    loop_type = WHILE_LOOP;
    skip_any(TOK_BREAK);
    cond = parse_expr();
    skip_any(TOK_BREAK);
    body = parse_stmt();
  } else if (accept(KW_UNTIL)) {
    loop_type = UNTIL_LOOP;
    skip_any(TOK_BREAK);
    cond = parse_expr();
    skip_any(TOK_BREAK);
    body = parse_stmt();
  } else if (accept(KW_FOR)) {
    loop_type = FOR_LOOP;
    _comma_stmt_sep = true;
    skip_any(TOK_BREAK);
    if (!accept(TOK_SEMICOLON)) {
      if (peek(KW_VAR)) {
        before = parse_var_decl();
      } else {
        before = parse_expr_stmt();
      }
    }
    if (!accept(TOK_SEMICOLON)) {
      cond = parse_expr();
      stmt_end();
    } else {
      skip_any(TOK_BREAK);
    }
    if (!accept(TOK_SEMICOLON)) {
      after = parse_expr_stmt();
    } else {
      skip_any(TOK_BREAK);
    }
    _comma_stmt_sep = false;
    body = parse_stmt();
  } else {
    body = parse_stmt();
    skip_any(TOK_BREAK);

    if (accept(KW_WHILE)) {
      loop_type = LOOP_WHILE;
      skip_any(TOK_BREAK);
      cond = parse_expr();
      skip_any(TOK_BREAK);
    } else if (accept(KW_UNTIL)) {
      loop_type = LOOP_UNTIL;
      skip_any(TOK_BREAK);
      cond = parse_expr();
      skip_any(TOK_BREAK);
    }
  }

  loop->set_type(loop_type);

  if (before) {
    loop->before(std::move(before));
  }

  if (cond) {
    loop->cond(std::move(cond));
  }

  if (after) {
    loop->after(std::move(after));
  }

  loop->body(std::move(body));

  scope::close();

  return loop;
}

/**
 * Parse function arguments.
 *
 * @return The function arguments AST.
 */
std::unique_ptr<arguments> parser::parse_arguments() {
  auto args = std::make_unique<arguments>();
  do {
    skip_any(TOK_BREAK);
    args->splice(parse_expr());
  } while (accept(TOK_COMMA));

  skip_any(TOK_BREAK);

  return args;
}

/**
 * Parse an expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_expr() {
  return parse_assign_expr();
}

/**
 * Parse an assignment expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_assign_expr() {
  class setter : public lazy_visitor {
  public:
    setter() = default;
    virtual ~setter() = default;

    virtual void visit(scoped_name &name) override {
      name.is_setter(true);
    }

    virtual void visit(member_expr &expr) override {
      expr.is_setter(true);
    }

    virtual void visit(subscript_expr &expr) override {
      expr.is_setter(true);
    }

    virtual void visit(ast &node) override {
      BUG();
    }
  };

  auto lhs = parse_or_expr();
  if (accept(TOK_WALRUS)) {
    setter s;
    lhs->accept(s);
    skip_any(TOK_BREAK);
    auto expr = std::make_unique<assign_expr>();
    expr->splice(std::move(lhs));
    auto rhs = parse_assign_expr();
    expr->splice(std::move(rhs));
    return expr;
  } else if (accept(TOK_ASSIGN)) {
    oops("e@1 use ':=' for assignment within expressions", gettok());
  }

  return lhs;
}

/**
 * Parse an "or" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_or_expr() {
  std::unique_ptr<expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<or_expr>(std::move(e), parse_xor_expr(), t);
    } else {
      e = parse_xor_expr();
    }
  } while (accept(KW_OR));

  return e;
}

/**
 * Parse an "xor" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_xor_expr() {
  std::unique_ptr<expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<xor_expr>(std::move(e), parse_and_expr(), t);
    } else {
      e = parse_and_expr();
    }
  } while (accept(KW_XOR));

  return e;
}

/**
 * Parse an "and" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_and_expr() {
  std::unique_ptr<expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<and_expr>(std::move(e), parse_equality_expr(), t);
    } else {
      e = parse_equality_expr();
    }
  } while (accept(KW_AND));

  return e;
}

/**
 * Parse an equality expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_equality_expr() {
  std::unique_ptr<expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<equality_expr>(std::move(e), parse_is_expr(), t);
    } else {
      e = parse_is_expr();
    }
  } while (accept_any(TOK_EQUALS, TOK_NOT_EQ));

  return e;
}

/**
 * Parse an "is" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_is_expr() {
  std::unique_ptr<expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<is_expr>(std::move(e), parse_compare_expr(), t);
    } else {
      e = parse_compare_expr();
    }
  } while (accept(KW_IS));

  return e;
}

/**
 * Parse a compare expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_compare_expr() {
  std::unique_ptr<expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<compare_expr>(std::move(e), parse_add_expr(), t);
    } else {
      e = parse_add_expr();
    }
  } while (accept_any(TOK_GT, TOK_GT_EQ, TOK_LT, TOK_LT_EQ));

  return e;
}

/**
 * Parse an "add" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_add_expr() {
  std::unique_ptr<expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<add_expr>(std::move(e), parse_mult_expr(), t);
    } else {
      e = parse_mult_expr();
    }
  } while (accept_any(TOK_MINUS, TOK_PLUS));

  return e;
}

/**
 * Parse a multiply expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_mult_expr() {
  std::unique_ptr<expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<mult_expr>(std::move(e), parse_unary_expr(), t);
    } else {
      e = parse_unary_expr();
    }
  } while (accept_any(TOK_FWDSLASH, TOK_STAR));

  return e;
}

/**
 * Parse a unary expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_unary_expr() {
  if (accept_any(TOK_BANG, TOK_MINUS, TOK_PLUS)) {
    auto t = gettok();
    skip_any(TOK_BREAK);
    return std::make_unique<unary_expr>(parse_unary_expr(), t);
  } else {
    return parse_call_expr();
  }
}

/**
 * Parse a "call" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_call_expr() {
  std::unique_ptr<expr> e = parse_primary_expr();
  token_ref lparen;

  while (1) {
    if (accept(TOK_LPAREN)) {
      std::unique_ptr<arguments> args;
      auto tok = gettok();
      skip_any(TOK_BREAK);

      if (!accept(TOK_RPAREN)) {
        args = parse_arguments();
        expect(TOK_RPAREN);
      }

      e = std::make_unique<call_expr>(std::move(e), std::move(args));
      e->set_name(tok);
    } else if (accept(TOK_LSQUARE)) {
      skip_any(TOK_BREAK);
      e = std::make_unique<subscript_expr>(std::move(e), parse_expr());
      skip_any(TOK_BREAK);
      expect(TOK_RSQUARE);
    } else if (accept(TOK_DOT)) {
      skip_any(TOK_BREAK);
      expect(TOK_IDENT);
      e = std::make_unique<member_expr>(std::move(e), gettok());
    } else {
      break;
    }
  }

  return e;
}

/**
 * Parse a key/value pair.
 *
 * @return The key/value pair AST.
 */
std::unique_ptr<kv_pair> parser::parse_kv_pair(size_t idx) {
  std::unique_ptr<kv_pair> pair;

  auto k_or_v = parse_expr();
  skip_any(TOK_BREAK);

  if (accept(TOK_COLON)) {
    skip_any(TOK_BREAK);
    pair = std::make_unique<kv_pair>(std::move(k_or_v), parse_expr());
  } else {
    auto k = std::make_unique<numeric_expr>(idx);
    pair = std::make_unique<kv_pair>(std::move(k), std::move(k_or_v));
  }

  return pair;
}

/**
 * Parse a map expression.
 *
 * @return the expression AST.
 */
std::unique_ptr<expr> parser::parse_map_expr() {
  auto map = std::make_unique<map_expr>();

  map->splice(parse_map_impl());

  return map;
}

/**
 * Parse a nil expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_nil_expr() {
  expect(KW_NIL);
  return std::make_unique<nil_expr>(gettok());
}

/**
 * Parse a primary expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_primary_expr() {
  std::unique_ptr<expr> e;

  switch (peek()) {
  case KW_NIL:
    e = parse_nil_expr();
    break;
  case KW_TRUE:
  case KW_FALSE:
    accept();
    e = std::make_unique<boolean>(gettok());
    break;
  case TOK_DEC:
  case TOK_INT:
  case TOK_HEX:
  case TOK_OCT:
    accept();
    e = std::make_unique<numeric_expr>(gettok());
    break;
  case TOK_STRING:
    accept();
    e = std::make_unique<string_spec>(gettok());
    break;
  case TOK_LPAREN:
    e = parse_paren_expr();
    break;
  case TOK_BSLASH:
  case KW_LAMBDA:
    e = parse_lambda_expr();
    break;
  case TOK_LCURLY:
    e = parse_map_expr();
    break;
  case FFI_VER:
  case FFI_DUP:
  case FFI_STR:
  case FFI_LEN:
  case FFI_GC:
  case FFI_SLEEP:
  default:
    e = parse_scoped_name();
    break;
  }

  return e;
}

/**
 * Parse a "super" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_super_expr() {
  expect(TOK_COLON);
  auto e = std::make_unique<super_expr>(gettok());
  skip_any(TOK_BREAK);
  e->splice(parse_expr());

  return e;
}

/**
 * Parse a parentheses expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<expr> parser::parse_paren_expr() {
  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);
  auto e = parse_expr();
  skip_any(TOK_BREAK);
  expect(TOK_RPAREN);

  return e;
}

/**
 * Parse a scoped name expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<scoped_name> parser::parse_scoped_name() {
  auto name = std::make_unique<scoped_name>();
  token_ref begintok;
  std::string s;

  if (accept(KW_SELF)) {
    name->set_referenced_scope(self());
    name->is_abs(false);
    name->is_self(true);
    s += gettok().text();
    begintok = gettok();
  } else {
    name->is_abs(accept(TOK_SCOPE));

    if (name->is_abs()) {
      begintok = gettok();
    }

    do {
      if (prev(TOK_SCOPE)) {
        s += "::";
        skip_any(TOK_BREAK);
      }

      expect_any(
        TOK_IDENT, FFI_VER, FFI_DUP, FFI_STR, FFI_LEN, FFI_GC, FFI_SLEEP);
      s += gettok().text();

      if (begintok.type() == TOK_INV) {
        begintok = gettok();
      }

    } while (accept(TOK_SCOPE));
  }

  token_range range(_token_cache, begintok.pos(), gettok().pos());

  name->set_token_range(range);
  name->set_scope(scope::current);
  name->set_ident(s);

  return name;
}

} // namespace dwt
