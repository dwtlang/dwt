// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

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
#include <dwt/ir/for_stmt.hpp>
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
#include <dwt/ir/type2.hpp>
#include <dwt/ir/type3.hpp>
#include <dwt/ir/type4.hpp>
#include <dwt/ir/type5.hpp>
#include <dwt/ir/use_stmt.hpp>
#include <dwt/ir/var_decl.hpp>
#include <dwt/ir/xor_expr.hpp>
#include <dwt/scanner.hpp>
#include <dwt/token.hpp>
#include <dwt/token_seq.hpp>
#include <dwt/token_type.hpp>

#include <cassert>
#include <iostream>
#include <memory>
#include <stack>

namespace dwt {

class parser : public scanner {
public:
  parser(utf8_source &&);
  virtual ~parser();

  std::shared_ptr<ir::script> parse();

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

  std::shared_ptr<ir::script> script();
  std::shared_ptr<ir::declaration> declaration();
  std::shared_ptr<ir::object_decl> object_decl();
  std::shared_ptr<ir::object_body> object_body();
  std::shared_ptr<ir::object> object();
  std::shared_ptr<ir::subscript_expr> subscript_expr();
  std::shared_ptr<ir::declaration> function_decl(bool api);
  std::shared_ptr<ir::lambda_decl> lambda_decl();
  std::shared_ptr<ir::map_impl> map_impl();
  std::shared_ptr<ir::type3> type3();
  std::shared_ptr<ir::ffi_decl> ffi_decl();
  std::shared_ptr<ir::function> function();
  std::shared_ptr<ir::module_decl> module_decl();
  std::shared_ptr<ir::lambda_expr> lambda_expr();
  std::shared_ptr<ir::lambda> lambda();
  std::shared_ptr<ir::parameters> parameters();
  std::shared_ptr<ir::kv_pair> kv_pair(size_t idx);
  std::shared_ptr<ir::type4> type4();
  std::shared_ptr<ir::block> block();
  std::shared_ptr<ir::function_body> function_body();
  std::shared_ptr<ir::var_decl> var_decl();
  std::shared_ptr<ir::type5> type5();
  std::shared_ptr<ir::arguments> arguments();
  std::shared_ptr<ir::stmt> object_stmt();
  std::shared_ptr<ir::stmt> stmt();
  std::shared_ptr<ir::expr_stmt> expr_stmt();
  std::shared_ptr<ir::for_stmt> for_stmt();
  std::shared_ptr<ir::use_stmt> use_stmt();
  std::shared_ptr<ir::if_stmt> if_stmt();
  std::shared_ptr<ir::print_stmt> print_stmt();
  std::shared_ptr<ir::loop_stmt> loop_decl();
  std::shared_ptr<ir::loop_stmt> loop_stmt();
  std::shared_ptr<ir::continue_stmt> continue_stmt();
  std::shared_ptr<ir::break_stmt> break_stmt();
  std::shared_ptr<ir::return_stmt> return_stmt();
  std::shared_ptr<ir::expr> expr();
  std::shared_ptr<ir::expr> assign_expr();
  std::shared_ptr<ir::expr> or_expr();
  std::shared_ptr<ir::expr> xor_expr();
  std::shared_ptr<ir::expr> and_expr();
  std::shared_ptr<ir::expr> equality_expr();
  std::shared_ptr<ir::expr> is_expr();
  std::shared_ptr<ir::expr> compare_expr();
  std::shared_ptr<ir::expr> add_expr();
  std::shared_ptr<ir::expr> mult_expr();
  std::shared_ptr<ir::expr> unary_expr();
  std::shared_ptr<ir::expr> call_expr();
  std::shared_ptr<ir::expr> primary_expr();
  std::shared_ptr<ir::expr> paren_expr();
  std::shared_ptr<ir::expr> member_expr();
  std::shared_ptr<ir::expr> map_expr();
  std::shared_ptr<ir::expr> type2();
  std::shared_ptr<ir::expr> nil_expr();
  std::shared_ptr<ir::expr> super_expr();
  std::shared_ptr<ir::scoped_name> scoped_name();

  void push_self(std::shared_ptr<scope> ptr) {
    _self_stack.push_back(ptr);
  }

  void pop_self() {
    _self_stack.pop_back();
  }

  std::shared_ptr<scope> self() {
    return _self_stack.back();
  }

  std::vector<std::shared_ptr<scope>> _self_stack;
  token_ref _this_token;
  token_ref _prev_token;
  std::shared_ptr<token_seq> _token_seq;
};

} // namespace dwt

#endif
