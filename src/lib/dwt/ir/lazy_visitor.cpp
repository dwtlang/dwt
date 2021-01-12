// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

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
#include <dwt/ir/equality_expr.hpp>
#include <dwt/ir/expr_stmt.hpp>
#include <dwt/ir/ffi_decl.hpp>
#include <dwt/ir/function.hpp>
#include <dwt/ir/function_decl.hpp>
#include <dwt/ir/if_stmt.hpp>
#include <dwt/ir/is_expr.hpp>
#include <dwt/ir/kv_pair.hpp>
#include <dwt/ir/lambda.hpp>
#include <dwt/ir/lambda_decl.hpp>
#include <dwt/ir/lambda_expr.hpp>
#include <dwt/ir/lazy_visitor.hpp>
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
#include <dwt/ir/print_stmt.hpp>
#include <dwt/ir/return_stmt.hpp>
#include <dwt/ir/scoped_name.hpp>
#include <dwt/ir/script.hpp>
#include <dwt/ir/string_spec.hpp>
#include <dwt/ir/subscript_expr.hpp>
#include <dwt/ir/super_expr.hpp>
#include <dwt/ir/type1.hpp>
#include <dwt/ir/type2.hpp>
#include <dwt/ir/type3.hpp>
#include <dwt/ir/type4.hpp>
#include <dwt/ir/type5.hpp>
#include <dwt/ir/unary_expr.hpp>
#include <dwt/ir/use_stmt.hpp>
#include <dwt/ir/var_decl.hpp>
#include <dwt/ir/xor_expr.hpp>
#include <dwt/ir/yield_stmt.hpp>

namespace dwt {
namespace ir {

lazy_visitor::lazy_visitor() {
}

lazy_visitor::~lazy_visitor() {
}

void lazy_visitor::visit(ir::numeric_expr &num) {
  visit(static_cast<ir::ast &>(num));
}

void lazy_visitor::visit(ir::stmt &stmt) {
  visit(static_cast<ir::ast &>(stmt));
}

void lazy_visitor::visit(ir::scoped_name &name) {
  visit(static_cast<ir::ast &>(name));
}

void lazy_visitor::visit(ir::parameter &param) {
  visit(static_cast<ir::ast &>(param));
}

void lazy_visitor::visit(ir::string_spec &str) {
  visit(static_cast<ir::ast &>(str));
}

void lazy_visitor::visit(ir::use_stmt &spec) {
  visit(static_cast<ir::ast &>(spec));
}

void lazy_visitor::visit(ir::lambda_decl &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::unary_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::primary_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::call_expr &call) {
  visit(static_cast<ir::ast &>(call));
}

void lazy_visitor::visit(ir::mult_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::add_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::compare_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::equality_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::is_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::and_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::or_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::assign_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::arguments &args) {
  visit(static_cast<ir::ast &>(args));
}

void lazy_visitor::visit(ir::return_stmt &stmt) {
  visit(static_cast<ir::ast &>(stmt));
}

void lazy_visitor::visit(ir::type1 &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::print_stmt &stmt) {
  visit(static_cast<ir::ast &>(stmt));
}

void lazy_visitor::visit(ir::if_stmt &stmt) {
  visit(static_cast<ir::ast &>(stmt));
}

void lazy_visitor::visit(ir::yield_stmt &stmt) {
  visit(static_cast<ir::ast &>(stmt));
}

void lazy_visitor::visit(ir::expr_stmt &stmt) {
  visit(static_cast<ir::ast &>(stmt));
}

void lazy_visitor::visit(ir::block &block) {
  visit(static_cast<ir::ast &>(block));
}

void lazy_visitor::visit(ir::parameters &params) {
  visit(static_cast<ir::ast &>(params));
}

void lazy_visitor::visit(ir::function &fun) {
  visit(static_cast<ir::ast &>(fun));
}

void lazy_visitor::visit(ir::function_decl &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::var_decl &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::object_decl &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::declaration &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::script &scr) {
  visit(static_cast<ir::ast &>(scr));
}

void lazy_visitor::visit(ir::type2 &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::type3 &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::super_expr &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::nil_expr &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::map_impl &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::kv_pair &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::type4 &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::object_body &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::member_expr &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::subscript_expr &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::type5 &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::object &obj) {
  visit(static_cast<ir::ast &>(obj));
}

void lazy_visitor::visit(ir::loop_stmt &loop) {
  visit(static_cast<ir::ast &>(loop));
}

void lazy_visitor::visit(ir::module_decl &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::ffi_decl &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::xor_expr &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::boolean &expr) {
  visit(static_cast<ir::ast &>(expr));
}

void lazy_visitor::visit(ir::lambda_expr &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::lambda &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::break_stmt &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::continue_stmt &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::map_expr &decl) {
  visit(static_cast<ir::ast &>(decl));
}

void lazy_visitor::visit(ir::ast &decl) {
}

} // namespace ir
} // namespace dwt
