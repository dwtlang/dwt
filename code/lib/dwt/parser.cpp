// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/debug.hpp>
#include <dwt/feedback.hpp>
#include <dwt/globals.hpp>
#include <dwt/ir/lazy_visitor.hpp>
#include <dwt/ir/string_spec.hpp>
#include <dwt/ir/unary_expr.hpp>
#include <dwt/parser.hpp>
#include <dwt/reporting.hpp>
#include <dwt/scope.hpp>

namespace dwt {

parser::parser(utf8_source &&utf8)
  : scanner(utf8)
  , _token_seq(utf8.tokens()) {
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
  if (!peek(TOK_RCURLY)) {
    expect_any(TOK_SEMICOLON, TOK_BREAK);
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
ir::script *parser::script() {
  auto scr = new ir::script();

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
ir::declaration *parser::declaration() {
  skip_any(TOK_BREAK);

  ir::declaration *decl = nullptr;
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
ir::object_decl *parser::object_decl() {
  expect(KW_OBJ);
  skip_any(TOK_BREAK);
  expect(TOK_IDENT);
  auto decl = new ir::object_decl(gettok());
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

  decl->splice(impl);

  skip_any(TOK_BREAK);

  pop_self();

  return decl;
}

/**
 * Parse a lambda expression.
 *
 * @return The lambda expression AST.
 */
ir::lambda_expr *parser::lambda_expr() {
  auto exp = new ir::lambda_expr();

  expect_any(TOK_BSLASH, KW_LAMBDA);
  skip_any(TOK_BREAK);
  expect(TOK_LPAREN);
  skip_any(TOK_BREAK);
  if (!accept(TOK_RPAREN)) {
    exp->args(arguments());
    expect(TOK_RPAREN);
  } else {
    auto args = new ir::arguments();
    exp->args(args);
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
ir::lambda *parser::lambda() {
  auto lam = new ir::lambda();
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

  auto body = new ir::function_body();

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);
    body->splice(declaration());
  }

  lam->body(body);

  scope::close();

  return lam;
}

/**
 * Parse a foreign function interface (FFI) declaration.
 *
 * @return The FFI declaration AST.
 */
ir::ffi_decl *parser::ffi_decl() {
  expect(KW_FFI);
  expect(TOK_IDENT);

  auto decl = new ir::ffi_decl(gettok());
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
ir::module_decl *parser::module_decl() {
  expect(KW_MOD);
  expect(TOK_IDENT);
  auto module = new ir::module_decl(gettok());
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
ir::lambda_decl *parser::lambda_decl() {
  auto decl = new ir::lambda_decl();
  scope::open(decl->name_tok(), SCOPE_CREATE);

  push_self(scope::current);

  auto impl = lambda();

  scope::close();

  if (impl->params()) {
    decl->arity(impl->params()->nr_children());
  } else {
    decl->arity(0);
  }

  decl->splice(impl);

  pop_self();

  return decl;
}

/**
 * Parse a map definition.
 *
 * @return The map definition AST.
 */
ir::map_impl *parser::map_impl() {
  auto impl = new ir::map_impl();
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
 * Parse a vector definition.
 *
 * @return The vector definition AST.
 */
ir::type3 *parser::type3() {
  auto impl = new ir::type3();
  scope::open(impl->name_tok(), SCOPE_CREATE);

  push_self(scope::current);

  expect(TOK_LSQUARE);
  skip_any(TOK_BREAK);

  if (!accept(TOK_RSQUARE)) {
    do {
      skip_any(TOK_BREAK);
      impl->splice(type4());
      skip_any(TOK_BREAK);
    } while (accept(TOK_COMMA));

    expect(TOK_RSQUARE);
  }

  pop_self();

  return impl;
}

/**
 * Parse a function declaration.
 *
 * @return The function declaration AST.
 */
ir::declaration *parser::function_decl() {
  expect(KW_FUN);
  expect(TOK_IDENT);
  auto decl = new ir::function_decl(gettok());
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

  decl->splice(impl);

  skip_any(TOK_BREAK);

  pop_self();

  return decl;
}

/**
 * Parse a function definition.
 *
 * @return The function definition AST.
 */
ir::function *parser::function() {
  auto fun = new ir::function();
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
ir::object *parser::object() {
  auto obj = new ir::object();
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
ir::parameters *parser::parameters() {
  auto params = new ir::parameters();
  do {
    if (prev(TOK_COMMA)) {
      skip_any(TOK_BREAK);
    }
    accept(KW_VAR);
    expect(TOK_IDENT);
    auto identifier = gettok();
    auto param = new ir::parameter(gettok());
    params->splice(param);
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
ir::block *parser::block() {
  auto blk = new ir::block();
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
ir::function_body *parser::function_body() {
  auto body = new ir::function_body();

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
ir::object_body *parser::object_body() {
  auto body = new ir::object_body();

  expect(TOK_LCURLY);
  skip_any(TOK_BREAK);

  while (!accept(TOK_RCURLY)) {
    skip_any(TOK_BREAK);

    ir::declaration *decl = nullptr;
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
    body->splice(decl);
  }

  skip_any(TOK_BREAK);

  return body;
}

/**
 * Parse a variable declaration.
 *
 * @return The variable declaration AST.
 */
ir::var_decl *parser::var_decl() {
  expect(KW_VAR);
  expect(TOK_IDENT);

  auto decl = new ir::var_decl(gettok());
  scope::add(gettok(), SCOPE_EXCLUSIVE | SCOPE_CREATE);

  if (accept(TOK_ASSIGN)) {
    skip_any(TOK_BREAK);
    decl->splice(expr());
  }

  stmt_end();

  return decl;
}

/**
 * Parse a statement.
 *
 * @return The statement.
 */
ir::stmt *parser::stmt() {
  switch (peek()) {
  case KW_RET:
    return return_stmt();
  case KW_PRINT:
    return print_stmt();
    break;
  case KW_IF:
    return if_stmt();
    break;
  case KW_LOOP:
    return loop_stmt();
    break;
  case KW_FOR:
    return for_stmt();
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
ir::expr_stmt *parser::expr_stmt() {
  auto stmt = new ir::expr_stmt();
  stmt->splice(expr());

  stmt_end();

  return stmt;
}

/**
 * Parse a "for" statement.
 *
 * @return The statement AST.
 */
ir::for_stmt *parser::for_stmt() {
  auto forstmt = new ir::for_stmt();
  scope::open(SCOPE_CREATE | SCOPE_ANONYMOUS);

  expect(KW_FOR);
  skip_any(TOK_BREAK);
  expect(TOK_IDENT);
  skip_any(TOK_BREAK);
  // auto param = new ir::parameter(gettok());
  expect(KW_IN);
  skip_any(TOK_BREAK);
  expr();
  skip_any(TOK_BREAK);
  stmt();

  return forstmt;

#if 0


  expect(KW_FOR);
  expect(TOK_LPAREN);
  if (peek(KW_VAR)) {
    stmt->splice(var_decl());
  } else if (!accept(TOK_SEMICOLON)) {
    stmt->splice(expr_stmt());
  }

  if (!accept(TOK_SEMICOLON)) {
    expr();
    expect(TOK_SEMICOLON);
  }

  if (!accept(TOK_RPAREN)) {
    expr();
    expect(TOK_RPAREN);
  }

  scope::close();

  return stmt;
#endif
}

/**
 * Parse a "use" statement.
 *
 * @return The statement AST.
 */
ir::use_stmt *parser::use_stmt() {
  expect(KW_USE);

  auto stmt = new ir::use_stmt();

  expect(TOK_STRING);

  auto file = new ir::string_spec(gettok());

  stmt->splice(file);

  stmt_end();

  return stmt;
}

/**
 * Parse an "if" statement.
 *
 * @return the statement AST.
 */
ir::if_stmt *parser::if_stmt() {
  auto statement = new ir::if_stmt();
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
ir::print_stmt *parser::print_stmt() {
  auto stmt = new ir::print_stmt();
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
ir::return_stmt *parser::return_stmt() {
  expect(KW_RET);

  auto stmt = new ir::return_stmt(gettok());

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
ir::continue_stmt *parser::continue_stmt() {
  ir::continue_stmt *statement;

  expect(KW_CONTINUE);

  if (accept(TOK_IDENT)) {
    statement = new ir::continue_stmt(gettok());
    stmt_end();
  } else {
    statement = new ir::continue_stmt();
    stmt_end();
  }

  return statement;
}

/**
 * Parse a "break" statement.
 *
 * @return The statement AST.
 */
ir::break_stmt *parser::break_stmt() {
  ir::break_stmt *statement;

  expect(KW_BREAK);

  if (accept(TOK_IDENT)) {
    statement = new ir::break_stmt(gettok());
    stmt_end();
  } else {
    statement = new ir::break_stmt();
    stmt_end();
  }

  return statement;
}

/**
 * Parse a loop declaration.
 *
 * @return The loop declaration AST.
 */
ir::loop_stmt *parser::loop_decl() {
  token_ref tag;

  expect(KW_LOOP);
  skip_any(TOK_BREAK);
  if (accept(TOK_IDENT)) {
    tag = gettok(); // tagged loop
    skip_any(TOK_BREAK);
  }

  auto decl = new ir::loop_stmt(tag);
  return decl;
}

/**
 * Parse a loop statement.
 *
 * @return The loop statement AST.
 */
ir::loop_stmt *parser::loop_stmt() {
  ir::loop_type loop_type = ir::BASIC_LOOP;
  ir::loop_stmt *loop = nullptr;
  ir::expr *cond = nullptr;
  ir::stmt *body = nullptr;

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

  if (cond) {
    loop->cond(cond);
  }

  loop->body(body);

  scope::close();

  return loop;
}

/**
 * Parse function arguments.
 *
 * @return The function arguments AST.
 */
ir::arguments *parser::arguments() {
  auto args = new ir::arguments();
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
ir::expr *parser::expr() {
  return assign_expr();
}

/**
 * Parse an assignment expression.
 *
 * @return The expression AST.
 */
ir::expr *parser::assign_expr() {
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
  if (accept(TOK_ASSIGN)) {
    setter s;
    lhs->accept(s);
    skip_any(TOK_BREAK);
    auto expr = new ir::assign_expr();
    expr->splice(lhs);
    auto rhs = assign_expr();
    expr->splice(rhs);
    return expr;
  }

  return lhs;
}

/**
 * Parse an "or" expression.
 *
 * @return The expression AST.
 */
ir::expr *parser::or_expr() {
  ir::expr *e = nullptr;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = new ir::or_expr(e, xor_expr(), t);
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
ir::expr *parser::xor_expr() {
  ir::expr *e = nullptr;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = new ir::xor_expr(e, and_expr(), t);
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
ir::expr *parser::and_expr() {
  ir::expr *e = nullptr;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = new ir::and_expr(e, equality_expr(), t);
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
ir::expr *parser::equality_expr() {
  ir::expr *e = nullptr;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = new ir::equality_expr(e, is_expr(), t);
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
ir::expr *parser::is_expr() {
  ir::expr *e = nullptr;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = new ir::is_expr(e, compare_expr(), t);
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
ir::expr *parser::compare_expr() {
  ir::expr *e = nullptr;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = new ir::compare_expr(e, add_expr(), t);
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
ir::expr *parser::add_expr() {
  ir::expr *e = nullptr;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = new ir::add_expr(e, mult_expr(), t);
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
ir::expr *parser::mult_expr() {
  ir::expr *e = nullptr;
  token_ref t;

  do {
    if (e) {
      t = gettok();
      skip_any(TOK_BREAK);
      e = new ir::mult_expr(e, unary_expr(), t);
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
ir::expr *parser::unary_expr() {
  if (accept_any(TOK_BANG, TOK_MINUS, TOK_PLUS)) {
    auto t = gettok();
    skip_any(TOK_BREAK);
    return new ir::unary_expr(unary_expr(), t);
  } else {
    return call_expr();
  }
}

/**
 * Parse a "call" expression.
 *
 * @return The expression AST.
 */
ir::expr *parser::call_expr() {
  ir::expr *e = primary_expr();
  token_ref lparen;

  while (1) {
    if (accept(TOK_LPAREN)) {
      ir::arguments *args = nullptr;
      auto tok = gettok();
      skip_any(TOK_BREAK);

      if (!accept(TOK_RPAREN)) {
        args = arguments();
        expect(TOK_RPAREN);
      }

      e = new ir::call_expr(e, args);
      e->set_name(tok);
    } else if (accept(TOK_LSQUARE)) {
      skip_any(TOK_BREAK);
      e = new ir::subscript_expr(e, expr());
      skip_any(TOK_BREAK);
      expect(TOK_RSQUARE);
    } else if (accept(TOK_DOT)) {
      skip_any(TOK_BREAK);
      expect(TOK_IDENT);
      e = new ir::member_expr(e, gettok());
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
ir::kv_pair *parser::kv_pair(size_t idx) {
  ir::kv_pair *pair;

  auto k_or_v = expr();
  skip_any(TOK_BREAK);

  if (accept(TOK_COLON)) {
    skip_any(TOK_BREAK);
    pair = new ir::kv_pair(k_or_v, expr());
  } else {
    auto k = new ir::numeric_expr(idx);
    pair = new ir::kv_pair(k, k_or_v);
  }

  return pair;
}

/**
 * Parse a vector type4.
 *
 * @return The vector type4 AST.
 */
ir::type4 *parser::type4() {
  return new ir::type4(expr());
}

/**
 * Parse a map expression.
 *
 * @return the expression AST.
 */
ir::expr *parser::map_expr() {
  auto map = new ir::map_expr();

  map->splice(map_impl());

  return map;
}

/**
 * Parse a vector expression.
 *
 * @return the expression AST.
 */
ir::expr *parser::type2() {
  auto vec = new ir::type2();

  vec->splice(type3());

  return vec;
}

/**
 * Parse a nil expression.
 *
 * @return The expression AST.
 */
ir::expr *parser::nil_expr() {
  expect(KW_NIL);
  return new ir::nil_expr(gettok());
}

/**
 * Parse a primary expression.
 *
 * @return The expression AST.
 */
ir::expr *parser::primary_expr() {
  ir::expr *e;

  switch (peek()) {
  case KW_NIL:
    e = nil_expr();
    break;
  case KW_TRUE:
  case KW_FALSE:
    accept();
    e = new ir::boolean(gettok());
    break;
  case TOK_DEC:
  case TOK_INT:
  case TOK_HEX:
  case TOK_OCT:
    accept();
    e = new ir::numeric_expr(gettok());
    break;
  case TOK_STRING:
    accept();
    e = new ir::string_spec(gettok());
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
  case TOK_LSQUARE:
    e = type2();
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
ir::expr *parser::super_expr() {
  expect(TOK_COLON);
  auto e = new ir::super_expr(gettok());
  skip_any(TOK_BREAK);
  e->splice(expr());

  return e;
}

/**
 * Parse a parentheses expression.
 *
 * @return The expression AST.
 */
ir::expr *parser::paren_expr() {
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
ir::scoped_name *parser::scoped_name() {
  auto name = new ir::scoped_name();
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

  token_range range(_token_seq, begintok.pos(), gettok().pos());

  name->set_token_range(range);
  name->set_scope(scope::current);
  name->set_ident(s);

  return name;
}

} // namespace dwt
