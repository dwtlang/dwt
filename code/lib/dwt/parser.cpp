// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

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
std::unique_ptr<ir::ast> parser::parse() {
  advance();

  return std::unique_ptr<ir::ast>(script());
}

/**
 * Parse a script.
 *
 * @return The script AST
 */
std::unique_ptr<ir::script> parser::script() {
  auto scr = std::make_unique<ir::script>();

  push_self(scope::current);

  while (!peek(TOK_EOF)) {
    scr->splice(declaration());
  }

  pop_self();

  return scr;
}

/**
 * Parse a declaration.
 *
 * @return The declaration AST.
 */
std::unique_ptr<ir::declaration> parser::declaration() {
  skip_any(TOK_BREAK);

  std::unique_ptr<ir::declaration> decl;
  bool api = accept(KW_API);

  if (api) {
    skip_any(TOK_BREAK);
  }

  switch (peek()) {
  case KW_MOD:
    if (api) {
      oops("e@1 keyword 'api' has no meaning here", gettok());
    }
    decl = module_decl();
    break;
  case KW_FUN:
    decl = function_decl();
    break;
  case KW_OBJ:
    decl = object_decl();
    break;
  case KW_FFI:
    decl = ffi_decl();
    break;
  case KW_VAR:
    decl = var_decl();
    break;
  default:
    if (api) {
      oops("e@1 keyword 'api' has no meaning here", gettok());
    }
    decl = stmt();
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
std::unique_ptr<ir::object_decl> parser::object_decl() {
  expect(KW_OBJ);
  skip_any(TOK_BREAK);
  expect(TOK_IDENT);
  auto decl = std::make_unique<ir::object_decl>(gettok());
  scope::open(gettok(), SCOPE_EXCLUSIVE | SCOPE_CREATE);
  skip_any(TOK_BREAK);

  push_self(scope::current);

  auto impl = object();

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
std::unique_ptr<ir::lambda_expr> parser::lambda_expr() {
  auto exp = std::make_unique<ir::lambda_expr>();

  expect_any(TOK_BSLASH, KW_LAMBDA);
  skip_any(TOK_BREAK);
  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);
  if (!accept(TOK_RPAREN)) {
    exp->args(arguments());
    expect(TOK_RPAREN);
  } else {
    exp->args(std::make_unique<ir::arguments>());
  }
  skip_any(TOK_BREAK);

  exp->impl(lambda_decl());

  return exp;
}

/**
 * Parse a lambda definition.
 *
 * @return The lambda definition AST.
 */
std::unique_ptr<ir::lambda> parser::lambda() {
  auto lam = std::make_unique<ir::lambda>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  if (accept(TOK_OR)) {
    skip_any(TOK_BREAK);
    lam->params(parameters());
    skip_any(TOK_BREAK);
    expect(TOK_OR);
    skip_any(TOK_BREAK);
  }

  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  auto body = std::make_unique<ir::function_body>();

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);
    body->splice(declaration());
  }

  lam->body(std::move(body));

  scope::close();

  return lam;
}

/**
 * Parse a foreign function interface (FFI) declaration.
 *
 * @return The FFI declaration AST.
 */
std::unique_ptr<ir::ffi_decl> parser::ffi_decl() {
  expect(KW_FFI);
  expect(TOK_IDENT);

  auto decl = std::make_unique<ir::ffi_decl>(gettok());
  scope::add(gettok(), SCOPE_CREATE);

  skip_any(TOK_BREAK);
  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RPAREN)) {
    skip_any(TOK_BREAK);
    decl->splice(parameters());
  }

  stmt_end();

  return decl;
}

/**
 * Parse a module declaration.
 *
 * @return The module declaration AST.
 */
std::unique_ptr<ir::module_decl> parser::module_decl() {
  expect(KW_MOD);
  expect(TOK_IDENT);
  auto module = std::make_unique<ir::module_decl>(gettok());
  scope::open(gettok(), SCOPE_APPEND | SCOPE_CREATE);

  skip_any(TOK_BREAK);
  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);
    module->splice(declaration());
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
std::unique_ptr<ir::lambda_decl> parser::lambda_decl() {
  auto decl = std::make_unique<ir::lambda_decl>();
  scope::open(decl->name_tok(), SCOPE_CREATE);

  push_self(scope::current);

  auto impl = lambda();

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
std::unique_ptr<ir::map_impl> parser::map_impl() {
  auto impl = std::make_unique<ir::map_impl>();
  scope::open(impl->name_tok(), SCOPE_CREATE);

  push_self(scope::current);

  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  if (!accept(TOK_RCURLY)) {
    do {
      skip_any(TOK_BREAK);
      impl->splice(kv_pair(impl->nr_children()));
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
std::unique_ptr<ir::declaration> parser::function_decl() {
  expect(KW_FUN);
  expect(TOK_IDENT);
  auto decl = std::make_unique<ir::function_decl>(gettok());
  scope::open(gettok(), SCOPE_EXCLUSIVE | SCOPE_CREATE);
  skip_any(TOK_BREAK);

  push_self(scope::current);

  auto impl = function();

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
std::unique_ptr<ir::function> parser::function() {
  auto fun = std::make_unique<ir::function>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);

  if (!accept(TOK_RPAREN)) {
    skip_any(TOK_BREAK);
    fun->params(parameters());
    expect(TOK_RPAREN);
  }

  skip_any(TOK_BREAK);
  fun->body(function_body());

  scope::close();

  return fun;
}

/**
 * Parse an object definition.
 *
 * @return The object definiton AST.
 */
std::unique_ptr<ir::object> parser::object() {
  auto obj = std::make_unique<ir::object>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);

  if (!accept(TOK_RPAREN)) {
    skip_any(TOK_BREAK);
    obj->params(parameters());
    expect(TOK_RPAREN);
  }

  skip_any(TOK_BREAK);

  if (peek(TOK_COLON)) {
    obj->super(super_expr());
    skip_any(TOK_BREAK);
  }

  obj->body(object_body());

  scope::close();

  return obj;
}

/**
 * Parse function parameters.
 *
 * @return The parameters AST.
 */
std::unique_ptr<ir::parameters> parser::parameters() {
  auto params = std::make_unique<ir::parameters>();
  do {
    if (prev(TOK_COMMA)) {
      skip_any(TOK_BREAK);
    }
    accept(KW_VAR);
    expect(TOK_IDENT);
    auto identifier = gettok();
    auto param = std::make_unique<ir::parameter>(gettok());
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
std::unique_ptr<ir::block> parser::block() {
  auto blk = std::make_unique<ir::block>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);
    blk->splice(declaration());
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
std::unique_ptr<ir::function_body> parser::function_body() {
  auto body = std::make_unique<ir::function_body>();

  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);
    body->splice(declaration());
  }

  skip_any(TOK_BREAK);

  return body;
}

/**
 * Parse an object body.
 *
 * @return The object body AST.
 */
std::unique_ptr<ir::object_body> parser::object_body() {
  auto body = std::make_unique<ir::object_body>();

  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);

    std::unique_ptr<ir::declaration> decl;
    bool api = accept(KW_API);

    if (api) {
      skip_any(TOK_BREAK);
    }

    switch (peek()) {
    case KW_FUN:
      decl = function_decl();
      break;
    case KW_OBJ:
      decl = object_decl();
      break;
    case KW_VAR:
      decl = var_decl();
      break;
    default:
      if (api) {
        oops("e@1 keyword 'api' has no meaning here", gettok());
      }
      decl = stmt();
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
std::unique_ptr<ir::var_decl> parser::var_decl() {
  expect(KW_VAR);
  expect(TOK_IDENT);

  auto decl = std::make_unique<ir::var_decl>(gettok());
  scope::add(gettok(), SCOPE_EXCLUSIVE | SCOPE_CREATE);

  if (accept(TOK_ASSIGN)) {
    skip_any(TOK_BREAK);
    decl->splice(expr());
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
std::unique_ptr<ir::stmt> parser::stmt() {
  switch (peek()) {
  case KW_RETURN:
    return return_stmt();
    break;
  case KW_YIELD:
    return yield_stmt();
    break;
  case KW_PRINT:
    return print_stmt();
    break;
  case KW_IF:
    return if_stmt();
    break;
  case KW_LOOP:
  case KW_WHILE:
  case KW_UNTIL:
  case KW_FOR:
    return loop_stmt();
    break;
  case KW_USE:
    return use_stmt();
    break;
  case KW_CONTINUE:
    return continue_stmt();
    break;
  case KW_BREAK:
    return break_stmt();
    break;
  case TOK_LCURLY:
    return block();
    break;
  default:
    return expr_stmt();
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
std::unique_ptr<ir::expr_stmt> parser::expr_stmt() {
  auto stmt = std::make_unique<ir::expr_stmt>();
  stmt->splice(expr());

  stmt_end();

  return stmt;
}

/**
 * Parse a "use" statement.
 *
 * @return The statement AST.
 */
std::unique_ptr<ir::use_stmt> parser::use_stmt() {
  expect(KW_USE);

  auto stmt = std::make_unique<ir::use_stmt>();

  expect(TOK_STRING);

  auto file = std::make_unique<ir::string_spec>(gettok());

  stmt->splice(std::move(file));

  stmt_end();

  return stmt;
}

/**
 * Parse an "if" statement.
 *
 * @return the statement AST.
 */
std::unique_ptr<ir::if_stmt> parser::if_stmt() {
  auto statement = std::make_unique<ir::if_stmt>();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  expect(KW_IF);
  statement->cond(expr());
  skip_any(TOK_BREAK);
  if (peek_any(KW_LOOP, KW_WHILE, KW_UNTIL)) {
    statement->if_body(loop_stmt());
  } else {
    statement->if_body(stmt());
  }

  if (accept(KW_ELSE)) {
    skip_any(TOK_BREAK);
    if (peek_any(KW_LOOP, KW_WHILE, KW_UNTIL)) {
      statement->else_body(loop_stmt());
    } else {
      statement->else_body(stmt());
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
std::unique_ptr<ir::print_stmt> parser::print_stmt() {
  auto stmt = std::make_unique<ir::print_stmt>();
  expect(KW_PRINT);
  skip_any(TOK_BREAK);
  stmt->splice(expr());
  stmt_end();

  return stmt;
}

/**
 * Parse a return statement.
 *
 * @return The statement AST.
 */
std::unique_ptr<ir::return_stmt> parser::return_stmt() {
  expect(KW_RETURN);

  auto stmt = std::make_unique<ir::return_stmt>(gettok());

  if (!accept_any(TOK_SEMICOLON, TOK_BREAK)) {
    stmt->splice(expr());
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
std::unique_ptr<ir::yield_stmt> parser::yield_stmt() {
  expect(KW_YIELD);

  auto stmt = std::make_unique<ir::yield_stmt>(gettok());

  if (!accept_any(TOK_SEMICOLON, TOK_BREAK)) {
    stmt->splice(expr());
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
std::unique_ptr<ir::continue_stmt> parser::continue_stmt() {
  std::unique_ptr<ir::continue_stmt> statement;

  expect(KW_CONTINUE);

  if (accept(TOK_IDENT)) {
    statement = std::make_unique<ir::continue_stmt>(gettok());
    stmt_end();
  } else {
    statement = std::make_unique<ir::continue_stmt>();
    stmt_end();
  }

  return statement;
}

/**
 * Parse a "break" statement.
 *
 * @return The statement AST.
 */
std::unique_ptr<ir::break_stmt> parser::break_stmt() {
  std::unique_ptr<ir::break_stmt> statement;

  expect(KW_BREAK);

  if (accept(TOK_IDENT)) {
    statement = std::make_unique<ir::break_stmt>(gettok());
    stmt_end();
  } else {
    statement = std::make_unique<ir::break_stmt>();
    stmt_end();
  }

  return statement;
}

/**
 * Parse a loop declaration.
 *
 * @return The loop declaration AST.
 */
std::unique_ptr<ir::loop_stmt> parser::loop_decl() {
  token_ref tag;

  if (accept(KW_LOOP)) {
    skip_any(TOK_BREAK);
    if (accept(TOK_IDENT)) {
      tag = gettok(); // tagged loop
      skip_any(TOK_BREAK);
    }
  }

  auto decl = std::make_unique<ir::loop_stmt>(tag);
  return decl;
}

/**
 * Parse a loop statement.
 *
 * @return The loop statement AST.
 */
std::unique_ptr<ir::loop_stmt> parser::loop_stmt() {
  ir::loop_type loop_type = ir::BASIC_LOOP;
  std::unique_ptr<ir::loop_stmt> loop;
  std::unique_ptr<ir::expr> cond;
  std::unique_ptr<ir::stmt> body;
  std::unique_ptr<ir::declaration> before;
  std::unique_ptr<ir::stmt> after;

  loop = loop_decl();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  if (accept(KW_WHILE)) {
    loop_type = ir::WHILE_LOOP;
    skip_any(TOK_BREAK);
    cond = expr();
    skip_any(TOK_BREAK);
    body = stmt();
  } else if (accept(KW_UNTIL)) {
    loop_type = ir::UNTIL_LOOP;
    skip_any(TOK_BREAK);
    cond = expr();
    skip_any(TOK_BREAK);
    body = stmt();
  } else if (accept(KW_FOR)) {
    loop_type = ir::FOR_LOOP;
    _comma_stmt_sep = true;
    skip_any(TOK_BREAK);
    if (!accept(TOK_SEMICOLON)) {
      if (peek(KW_VAR)) {
        before = var_decl();
      } else {
        before = expr_stmt();
      }
    }
    if (!accept(TOK_SEMICOLON)) {
      cond = expr();
      stmt_end();
    } else {
      skip_any(TOK_BREAK);
    }
    if (!accept(TOK_SEMICOLON)) {
      after = expr_stmt();
    } else {
      skip_any(TOK_BREAK);
    }
    _comma_stmt_sep = false;
    body = stmt();
  } else {
    body = stmt();
    skip_any(TOK_BREAK);

    if (accept(KW_WHILE)) {
      loop_type = ir::LOOP_WHILE;
      skip_any(TOK_BREAK);
      cond = expr();
      skip_any(TOK_BREAK);
    } else if (accept(KW_UNTIL)) {
      loop_type = ir::LOOP_UNTIL;
      skip_any(TOK_BREAK);
      cond = expr();
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
std::unique_ptr<ir::arguments> parser::arguments() {
  auto args = std::make_unique<ir::arguments>();
  do {
    skip_any(TOK_BREAK);
    args->splice(expr());
  } while (accept(TOK_COMMA));

  skip_any(TOK_BREAK);

  return args;
}

/**
 * Parse an expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::expr() {
  return assign_expr();
}

/**
 * Parse an assignment expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::assign_expr() {
  class setter : public ir::lazy_visitor {
  public:
    setter() = default;
    virtual ~setter() = default;

    virtual void visit(ir::scoped_name &name) override {
      name.is_setter(true);
    }

    virtual void visit(ir::member_expr &expr) override {
      expr.is_setter(true);
    }

    virtual void visit(ir::subscript_expr &expr) override {
      expr.is_setter(true);
    }

    virtual void visit(ir::ast &node) override {
      BUG();
    }
  };

  auto lhs = or_expr();
  if (accept(TOK_WALRUS)) {
    setter s;
    lhs->accept(s);
    skip_any(TOK_BREAK);
    auto expr = std::make_unique<ir::assign_expr>();
    expr->splice(std::move(lhs));
    auto rhs = assign_expr();
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
std::unique_ptr<ir::expr> parser::or_expr() {
  std::unique_ptr<ir::expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<ir::or_expr>(std::move(e), xor_expr(), t);
    } else {
      e = xor_expr();
    }
  } while (accept(KW_OR));

  return e;
}

/**
 * Parse an "xor" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::xor_expr() {
  std::unique_ptr<ir::expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<ir::xor_expr>(std::move(e), and_expr(), t);
    } else {
      e = and_expr();
    }
  } while (accept(KW_XOR));

  return e;
}

/**
 * Parse an "and" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::and_expr() {
  std::unique_ptr<ir::expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<ir::and_expr>(std::move(e), equality_expr(), t);
    } else {
      e = equality_expr();
    }
  } while (accept(KW_AND));

  return e;
}

/**
 * Parse an equality expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::equality_expr() {
  std::unique_ptr<ir::expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<ir::equality_expr>(std::move(e), is_expr(), t);
    } else {
      e = is_expr();
    }
  } while (accept_any(TOK_EQUALS, TOK_NOT_EQ));

  return e;
}

/**
 * Parse an "is" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::is_expr() {
  std::unique_ptr<ir::expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<ir::is_expr>(std::move(e), compare_expr(), t);
    } else {
      e = compare_expr();
    }
  } while (accept(KW_IS));

  return e;
}

/**
 * Parse a compare expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::compare_expr() {
  std::unique_ptr<ir::expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<ir::compare_expr>(std::move(e), add_expr(), t);
    } else {
      e = add_expr();
    }
  } while (accept_any(TOK_GT, TOK_GT_EQ, TOK_LT, TOK_LT_EQ));

  return e;
}

/**
 * Parse an "add" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::add_expr() {
  std::unique_ptr<ir::expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<ir::add_expr>(std::move(e), mult_expr(), t);
    } else {
      e = mult_expr();
    }
  } while (accept_any(TOK_MINUS, TOK_PLUS));

  return e;
}

/**
 * Parse a multiply expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::mult_expr() {
  std::unique_ptr<ir::expr> e;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = std::make_unique<ir::mult_expr>(std::move(e), unary_expr(), t);
    } else {
      e = unary_expr();
    }
  } while (accept_any(TOK_FWDSLASH, TOK_STAR));

  return e;
}

/**
 * Parse a unary expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::unary_expr() {
  if (accept_any(TOK_BANG, TOK_MINUS, TOK_PLUS)) {
    auto t = gettok();
    skip_any(TOK_BREAK);
    return std::make_unique<ir::unary_expr>(unary_expr(), t);
  } else {
    return call_expr();
  }
}

/**
 * Parse a "call" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::call_expr() {
  std::unique_ptr<ir::expr> e = primary_expr();
  token_ref lparen;

  while (1) {
    if (accept(TOK_LPAREN)) {
      std::unique_ptr<ir::arguments> args;
      auto tok = gettok();
      skip_any(TOK_BREAK);

      if (!accept(TOK_RPAREN)) {
        args = arguments();
        expect(TOK_RPAREN);
      }

      e = std::make_unique<ir::call_expr>(std::move(e), std::move(args));
      e->set_name(tok);
    } else if (accept(TOK_LSQUARE)) {
      skip_any(TOK_BREAK);
      e = std::make_unique<ir::subscript_expr>(std::move(e), expr());
      skip_any(TOK_BREAK);
      expect(TOK_RSQUARE);
    } else if (accept(TOK_DOT)) {
      skip_any(TOK_BREAK);
      expect(TOK_IDENT);
      e = std::make_unique<ir::member_expr>(std::move(e), gettok());
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
std::unique_ptr<ir::kv_pair> parser::kv_pair(size_t idx) {
  std::unique_ptr<ir::kv_pair> pair;

  auto k_or_v = expr();
  skip_any(TOK_BREAK);

  if (accept(TOK_COLON)) {
    skip_any(TOK_BREAK);
    pair = std::make_unique<ir::kv_pair>(std::move(k_or_v), expr());
  } else {
    auto k = std::make_unique<ir::numeric_expr>(idx);
    pair = std::make_unique<ir::kv_pair>(std::move(k), std::move(k_or_v));
  }

  return pair;
}

/**
 * Parse a map expression.
 *
 * @return the expression AST.
 */
std::unique_ptr<ir::expr> parser::map_expr() {
  auto map = std::make_unique<ir::map_expr>();

  map->splice(map_impl());

  return map;
}

/**
 * Parse a nil expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::nil_expr() {
  expect(KW_NIL);
  return std::make_unique<ir::nil_expr>(gettok());
}

/**
 * Parse a primary expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::primary_expr() {
  std::unique_ptr<ir::expr> e;

  switch (peek()) {
  case KW_NIL:
    e = nil_expr();
    break;
  case KW_TRUE:
  case KW_FALSE:
    accept();
    e = std::make_unique<ir::boolean>(gettok());
    break;
  case TOK_DEC:
  case TOK_INT:
  case TOK_HEX:
  case TOK_OCT:
    accept();
    e = std::make_unique<ir::numeric_expr>(gettok());
    break;
  case TOK_STRING:
    accept();
    e = std::make_unique<ir::string_spec>(gettok());
    break;
  case TOK_LPAREN:
    e = paren_expr();
    break;
  case TOK_BSLASH:
  case KW_LAMBDA:
    e = lambda_expr();
    break;
  case TOK_LCURLY:
    e = map_expr();
    break;
  case KW_VER:
  case KW_DUP:
  case KW_STR:
  case KW_LEN:
  case KW_GC:
  default:
    e = scoped_name();
    break;
  }

  return e;
}

/**
 * Parse a "super" expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::super_expr() {
  expect(TOK_COLON);
  auto e = std::make_unique<ir::super_expr>(gettok());
  skip_any(TOK_BREAK);
  e->splice(expr());

  return e;
}

/**
 * Parse a parentheses expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::expr> parser::paren_expr() {
  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);
  auto e = expr();
  skip_any(TOK_BREAK);
  expect(TOK_RPAREN);

  return e;
}

/**
 * Parse a scoped name expression.
 *
 * @return The expression AST.
 */
std::unique_ptr<ir::scoped_name> parser::scoped_name() {
  auto name = std::make_unique<ir::scoped_name>();
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

      expect_any(TOK_IDENT, KW_VER, KW_DUP, KW_STR, KW_LEN, KW_GC);
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
