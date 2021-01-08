// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_IR_PRINTER_HPP
#define GUARD_DWT_IR_PRINTER_HPP

#include <dwt/ir/visitor.hpp>

#include <map>
#include <stack>
#include <string>

namespace dwt {
namespace ir {

class printer : public ir::visitor {
public:
  printer();
  virtual ~printer();

  void print(ir::script *);

  virtual void visit(ir::numeric_expr &);
  virtual void visit(ir::stmt &);
  virtual void visit(ir::scoped_name &);
  virtual void visit(ir::parameter &);
  virtual void visit(ir::string_spec &);
  virtual void visit(ir::lambda_decl &);
  virtual void visit(ir::primary_expr &);
  virtual void visit(ir::call_expr &);
  virtual void visit(ir::unary_expr &);
  virtual void visit(ir::mult_expr &);
  virtual void visit(ir::add_expr &);
  virtual void visit(ir::compare_expr &);
  virtual void visit(ir::equality_expr &);
  virtual void visit(ir::and_expr &);
  virtual void visit(ir::or_expr &);
  virtual void visit(ir::assign_expr &);
  virtual void visit(ir::expr &);
  virtual void visit(ir::arguments &);
  virtual void visit(ir::return_stmt &);
  virtual void visit(ir::type1 &);
  virtual void visit(ir::print_stmt &);
  virtual void visit(ir::if_stmt &);
  virtual void visit(ir::yield_stmt &);
  virtual void visit(ir::expr_stmt &);
  virtual void visit(ir::block &);
  virtual void visit(ir::parameters &);
  virtual void visit(ir::function &);
  virtual void visit(ir::function_decl &);
  virtual void visit(ir::var_decl &);
  virtual void visit(ir::object_decl &);
  virtual void visit(ir::declaration &);
  virtual void visit(ir::script &);
  virtual void visit(ir::type2 &);
  virtual void visit(ir::super_expr &);
  virtual void visit(ir::nil_expr &);
  virtual void visit(ir::map_impl &);
  virtual void visit(ir::kv_pair &);
  virtual void visit(ir::object_body &);
  virtual void visit(ir::member_expr &);
  virtual void visit(ir::subscript_expr &);
  virtual void visit(ir::type5 &);
  virtual void visit(ir::object &);
  virtual void visit(ir::loop_stmt &);
  virtual void visit(ir::module_decl &);
  virtual void visit(ir::ffi_decl &);
  virtual void visit(ir::xor_expr &);
  virtual void visit(ir::boolean &);
  virtual void visit(ir::lambda_expr &);
  virtual void visit(ir::lambda &);
  virtual void visit(ir::break_stmt &);
  virtual void visit(ir::continue_stmt &);
  virtual void visit(ir::map_expr &);

private:
  void walk(ir::ast &);

  void write(ir::ast &node, std::string info = "");
  void push(const char *s);
  void pop();
  std::string prefix;
};

} // namespace ir
} // namespace dwt

#endif
