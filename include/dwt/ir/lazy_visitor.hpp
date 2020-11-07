// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_IR_LAZY_VISITOR_HPP
#define GUARD_DWT_IR_LAZY_VISITOR_HPP

#include <dwt/ir/visitor.hpp>

namespace dwt {
namespace ir {

class lazy_visitor : public visitor {
protected:
  lazy_visitor();
  virtual ~lazy_visitor();

public:
  virtual void visit(ir::numeric_expr &) override;
  virtual void visit(ir::stmt &) override;
  virtual void visit(ir::scoped_name &) override;
  virtual void visit(ir::parameter &) override;
  virtual void visit(ir::string_spec &) override;
  virtual void visit(ir::use_stmt &) override;
  virtual void visit(ir::lambda_decl &) override;
  virtual void visit(ir::primary_expr &) override;
  virtual void visit(ir::call_expr &) override;
  virtual void visit(ir::unary_expr &) override;
  virtual void visit(ir::mult_expr &) override;
  virtual void visit(ir::add_expr &) override;
  virtual void visit(ir::compare_expr &) override;
  virtual void visit(ir::equality_expr &) override;
  virtual void visit(ir::is_expr &) override;
  virtual void visit(ir::and_expr &) override;
  virtual void visit(ir::or_expr &) override;
  virtual void visit(ir::assign_expr &) override;
  virtual void visit(ir::expr &) override;
  virtual void visit(ir::arguments &) override;
  virtual void visit(ir::return_stmt &) override;
  virtual void visit(ir::type1 &) override;
  virtual void visit(ir::print_stmt &) override;
  virtual void visit(ir::if_stmt &) override;
  virtual void visit(ir::for_stmt &) override;
  virtual void visit(ir::expr_stmt &) override;
  virtual void visit(ir::block &) override;
  virtual void visit(ir::parameters &) override;
  virtual void visit(ir::function &) override;
  virtual void visit(ir::function_decl &) override;
  virtual void visit(ir::var_decl &) override;
  virtual void visit(ir::object_decl &) override;
  virtual void visit(ir::declaration &) override;
  virtual void visit(ir::script &) override;
  virtual void visit(ir::type2 &) override;
  virtual void visit(ir::type3 &) override;
  virtual void visit(ir::super_expr &) override;
  virtual void visit(ir::nil_expr &) override;
  virtual void visit(ir::map_impl &) override;
  virtual void visit(ir::kv_pair &) override;
  virtual void visit(ir::type4 &) override;
  virtual void visit(ir::object_body &) override;
  virtual void visit(ir::member_expr &) override;
  virtual void visit(ir::subscript_expr &) override;
  virtual void visit(ir::type5 &) override;
  virtual void visit(ir::object &) override;
  virtual void visit(ir::loop_stmt &) override;
  virtual void visit(ir::module_decl &) override;
  virtual void visit(ir::ffi_decl &) override;
  virtual void visit(ir::xor_expr &) override;
  virtual void visit(ir::boolean &) override;
  virtual void visit(ir::lambda_expr &) override;
  virtual void visit(ir::lambda &) override;
  virtual void visit(ir::break_stmt &) override;
  virtual void visit(ir::continue_stmt &) override;
  virtual void visit(ir::map_expr &) override;

  virtual void visit(ir::ast &) override;
};

} // namespace ir
} // namespace dwt

#endif
