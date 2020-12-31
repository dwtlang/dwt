// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_VISITOR_HPP
#define GUARD_DWT_IR_VISITOR_HPP

#include <dwt/ir/ast.hpp>

#include <mutex>

namespace dwt {
namespace ir {

class stmt;
class scoped_name;
class numeric_expr;
class parameter;
class string_spec;
class use_stmt;
class lambda_decl;
class primary_expr;
class call_expr;
class unary_expr;
class mult_expr;
class add_expr;
class compare_expr;
class equality_expr;
class is_expr;
class and_expr;
class or_expr;
class assign_expr;
class expr;
class arguments;
class return_stmt;
class type1;
class print_stmt;
class if_stmt;
class for_stmt;
class expr_stmt;
class block;
class parameters;
class function;
class function_decl;
class var_decl;
class object_decl;
class declaration;
class script;
class type2;
class type3;
class super_expr;
class nil_expr;
class map_impl;
class kv_pair;
class type4;
class object_body;
class member_expr;
class subscript_expr;
class type5;
class object;
class loop_stmt;
class module_decl;
class ffi_decl;
class xor_expr;
class boolean;
class lambda_expr;
class lambda;
class break_stmt;
class continue_stmt;
class map_expr;

class visitor {
protected:
  visitor();
  virtual ~visitor();

public:
  virtual void visit(ir::numeric_expr &) = 0;
  virtual void visit(ir::stmt &) = 0;
  virtual void visit(ir::scoped_name &) = 0;
  virtual void visit(ir::parameter &) = 0;
  virtual void visit(ir::string_spec &) = 0;
  virtual void visit(ir::use_stmt &) = 0;
  virtual void visit(ir::lambda_decl &) = 0;
  virtual void visit(ir::primary_expr &) = 0;
  virtual void visit(ir::call_expr &) = 0;
  virtual void visit(ir::unary_expr &) = 0;
  virtual void visit(ir::mult_expr &) = 0;
  virtual void visit(ir::add_expr &) = 0;
  virtual void visit(ir::compare_expr &) = 0;
  virtual void visit(ir::equality_expr &) = 0;
  virtual void visit(ir::is_expr &) = 0;
  virtual void visit(ir::and_expr &) = 0;
  virtual void visit(ir::or_expr &) = 0;
  virtual void visit(ir::assign_expr &) = 0;
  virtual void visit(ir::expr &) = 0;
  virtual void visit(ir::arguments &) = 0;
  virtual void visit(ir::return_stmt &) = 0;
  virtual void visit(ir::type1 &) = 0;
  virtual void visit(ir::print_stmt &) = 0;
  virtual void visit(ir::if_stmt &) = 0;
  virtual void visit(ir::for_stmt &) = 0;
  virtual void visit(ir::expr_stmt &) = 0;
  virtual void visit(ir::block &) = 0;
  virtual void visit(ir::parameters &) = 0;
  virtual void visit(ir::function &) = 0;
  virtual void visit(ir::function_decl &) = 0;
  virtual void visit(ir::var_decl &) = 0;
  virtual void visit(ir::object_decl &) = 0;
  virtual void visit(ir::declaration &) = 0;
  virtual void visit(ir::script &) = 0;
  virtual void visit(ir::type2 &) = 0;
  virtual void visit(ir::type3 &) = 0;
  virtual void visit(ir::super_expr &) = 0;
  virtual void visit(ir::nil_expr &) = 0;
  virtual void visit(ir::map_impl &) = 0;
  virtual void visit(ir::kv_pair &) = 0;
  virtual void visit(ir::type4 &) = 0;
  virtual void visit(ir::object_body &) = 0;
  virtual void visit(ir::member_expr &) = 0;
  virtual void visit(ir::subscript_expr &) = 0;
  virtual void visit(ir::type5 &) = 0;
  virtual void visit(ir::object &) = 0;
  virtual void visit(ir::loop_stmt &) = 0;
  virtual void visit(ir::module_decl &) = 0;
  virtual void visit(ir::ffi_decl &) = 0;
  virtual void visit(ir::xor_expr &) = 0;
  virtual void visit(ir::boolean &) = 0;
  virtual void visit(ir::lambda_expr &) = 0;
  virtual void visit(ir::lambda &) = 0;
  virtual void visit(ir::break_stmt &) = 0;
  virtual void visit(ir::continue_stmt &) = 0;
  virtual void visit(ir::map_expr &) = 0;
  virtual void visit(ir::ast &);
};

} // namespace ir
} // namespace dwt

#endif
