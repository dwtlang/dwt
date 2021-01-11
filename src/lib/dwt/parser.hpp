// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_PARSER_HPP
#define GUARD_DWT_PARSER_HPP

#include <dwt/ir/add_expr.hpp>
#include <dwt/ir/and_expr.hpp>
#include <dwt/ir/arguments.hpp>
#include <dwt/ir/assign_expr.hpp>
#include <dwt/ir/block.hpp>
#include <dwt/ir/boolean.hpp>
#include <dwt/ir/break_stmt.hpp>
#include <dwt/ir/call_expr.hpp>
#include <dwt/ir/compare_expr.hpp>
#include <dwt/ir/continue_stmt.hpp>
#include <dwt/ir/declaration.hpp>
#include <dwt/ir/equality_expr.hpp>
#include <dwt/ir/expr.hpp>
#include <dwt/ir/expr_stmt.hpp>
#include <dwt/ir/ffi_decl.hpp>
#include <dwt/ir/function.hpp>
#include <dwt/ir/function_body.hpp>
#include <dwt/ir/function_decl.hpp>
#include <dwt/ir/if_stmt.hpp>
#include <dwt/ir/is_expr.hpp>
#include <dwt/ir/kv_pair.hpp>
#include <dwt/ir/lambda.hpp>
#include <dwt/ir/lambda_decl.hpp>
#include <dwt/ir/lambda_expr.hpp>
#include <dwt/ir/loop_stmt.hpp>
#include <dwt/ir/map_expr.hpp>
#include <dwt/ir/map_impl.hpp>
#include <dwt/ir/member_expr.hpp>
#include <dwt/ir/module_decl.hpp>
#include <dwt/ir/mult_expr.hpp>
#include <dwt/ir/nil_expr.hpp>
#include <dwt/ir/numeric_expr.hpp>
#include <dwt/ir/object.hpp>
#include <dwt/ir/object_body.hpp>
#include <dwt/ir/object_decl.hpp>
#include <dwt/ir/or_expr.hpp>
#include <dwt/ir/parameter.hpp>
#include <dwt/ir/parameters.hpp>
#include <dwt/ir/primary_expr.hpp>
#include <dwt/ir/print_stmt.hpp>
#include <dwt/ir/return_stmt.hpp>
#include <dwt/ir/scoped_name.hpp>
#include <dwt/ir/script.hpp>
#include <dwt/ir/stmt.hpp>
#include <dwt/ir/subscript_expr.hpp>
#include <dwt/ir/super_expr.hpp>
#include <dwt/ir/use_stmt.hpp>
#include <dwt/ir/var_decl.hpp>
#include <dwt/ir/xor_expr.hpp>
#include <dwt/ir/yield_stmt.hpp>
#include <dwt/scanner.hpp>
#include <dwt/token.hpp>
#include <dwt/token_cache.hpp>
#include <dwt/token_type.hpp>

#include <cassert>
#include <memory>
#include <stack>

namespace dwt {

class parser : public scanner {
public:
  parser(utf8_source &&);
  virtual ~parser();

  std::unique_ptr<ir::ast> parse();

private:
  bool prev(token_type s) const {
    return _prev_token.type() == s;
  }

  token_type prev() const {
    return _prev_token.type();
  }

  bool peek(token_type s) const {
    return _this_token.type() == s;
  }

  token_type peek() const {
    return _this_token.type();
  }

  token_ref gettok() {
    return _prev_token;
  }

  void advance();

  bool accept(token_type s);

  void accept();

  void expect(token_type s);

  template <typename... Args> bool peek_any(Args... args) {
    bool seen = false;

    for (auto a : { args... }) {
      seen = peek(a);

      if (seen) {
        break;
      }
    }

    return seen;
  }

  template <typename... Args> bool scan_for(Args... args) {
    for (;;) {
      for (auto a : { args... }) {
        if (peek() == a) {
          return true;
        } else if (peek_any(TOK_INV)) {
          return false;
        }
      }
      advance();
    }
    return false;
  }

  template <typename... Args> bool accept_any(Args... args) {
    bool accepted = false;
    for (auto a : { args... }) {
      accepted = accept(a);

      if (accepted) {
        break;
      }
    }
    return accepted;
  }

  template <typename F, typename... Args>
  void expect_any(F first, Args... args) {
    bool accepted = accept(first);

    if (!accepted) {
      for (auto a : { args... }) {
        accepted = accept(a);

        if (accepted) {
          break;
        }
      }
    }

    if (!accepted) {
      expect(first);
    }
  }

  void skip_any(token_type s);
  void stmt_end();

  std::unique_ptr<ir::script> parse_script();
  std::unique_ptr<ir::declaration> parse_declaration();
  std::unique_ptr<ir::object_decl> parse_object_decl();
  std::unique_ptr<ir::object_body> parse_object_body();
  std::unique_ptr<ir::object> parse_object();
  std::unique_ptr<ir::subscript_expr> parse_subscript_expr();
  std::unique_ptr<ir::declaration> parse_function_decl();
  std::unique_ptr<ir::lambda_decl> parse_lambda_decl();
  std::unique_ptr<ir::map_impl> parse_map_impl();
  std::unique_ptr<ir::ffi_decl> parse_ffi_decl();
  std::unique_ptr<ir::function> parse_function();
  std::unique_ptr<ir::module_decl> parse_module_decl();
  std::unique_ptr<ir::lambda_expr> parse_lambda_expr();
  std::unique_ptr<ir::lambda> parse_lambda();
  std::unique_ptr<ir::parameters> parse_parameters();
  std::unique_ptr<ir::kv_pair> parse_kv_pair(size_t idx);
  std::unique_ptr<ir::block> parse_block();
  std::unique_ptr<ir::function_body> parse_function_body();
  std::unique_ptr<ir::var_decl> parse_var_decl();
  std::unique_ptr<ir::arguments> parse_arguments();
  std::unique_ptr<ir::stmt> parse_stmt();
  std::unique_ptr<ir::expr_stmt> parse_expr_stmt();
  std::unique_ptr<ir::use_stmt> parse_use_stmt();
  std::unique_ptr<ir::if_stmt> parse_if_stmt();
  std::unique_ptr<ir::print_stmt> parse_print_stmt();
  std::unique_ptr<ir::loop_stmt> parse_loop_decl();
  std::unique_ptr<ir::loop_stmt> parse_loop_stmt();
  std::unique_ptr<ir::continue_stmt> parse_continue_stmt();
  std::unique_ptr<ir::break_stmt> parse_break_stmt();
  std::unique_ptr<ir::return_stmt> parse_return_stmt();
  std::unique_ptr<ir::yield_stmt> parse_yield_stmt();
  std::unique_ptr<ir::expr> parse_expr();
  std::unique_ptr<ir::expr> parse_assign_expr();
  std::unique_ptr<ir::expr> parse_or_expr();
  std::unique_ptr<ir::expr> parse_xor_expr();
  std::unique_ptr<ir::expr> parse_and_expr();
  std::unique_ptr<ir::expr> parse_equality_expr();
  std::unique_ptr<ir::expr> parse_is_expr();
  std::unique_ptr<ir::expr> parse_compare_expr();
  std::unique_ptr<ir::expr> parse_add_expr();
  std::unique_ptr<ir::expr> parse_mult_expr();
  std::unique_ptr<ir::expr> parse_unary_expr();
  std::unique_ptr<ir::expr> parse_call_expr();
  std::unique_ptr<ir::expr> parse_primary_expr();
  std::unique_ptr<ir::expr> parse_paren_expr();
  std::unique_ptr<ir::expr> parse_member_expr();
  std::unique_ptr<ir::expr> parse_map_expr();
  std::unique_ptr<ir::expr> parse_nil_expr();
  std::unique_ptr<ir::expr> parse_super_expr();
  std::unique_ptr<ir::scoped_name> parse_scoped_name();

  void push_self(scope *scope_ptr) {
    _self_stack.push_back(scope_ptr);
  }

  void pop_self() {
    _self_stack.pop_back();
  }

  scope *self() {
    return _self_stack.back();
  }

  bool _comma_stmt_sep = false;
  std::vector<scope *> _self_stack;
  token_ref _this_token;
  token_ref _prev_token;
  std::shared_ptr<token_cache> _token_cache;
};

} // namespace dwt

#endif
