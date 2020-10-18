// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_COMPILER_HPP
#define GUARD_DWT_COMPILER_HPP

#include <dwt/function_obj.hpp>
#include <dwt/ir/visitor.hpp>
#include <dwt/local_var.hpp>
#include <dwt/opcode.hpp>
#include <dwt/var.hpp>

#include <future>
#include <map>
#include <mutex>
#include <stack>
#include <string>
#include <unordered_map>

namespace dwt {

class bytecode;
class function_obj;

class loop_info {
public:
  loop_info(size_t base_pos)
    : _base_pos(base_pos) {
  }

  void add_patch_point(size_t patch_point) {
    _patch_points.push_back(patch_point);
  }

  std::vector<size_t> &patch_points() {
    return _patch_points;
  }

  size_t base_pos() const {
    return _base_pos;
  }

private:
  size_t _base_pos;
  std::vector<size_t> _patch_points;
};

class compiler : public ir::visitor {
public:
  compiler(function_obj *, compiler *, bool concurrent);
  compiler();
  compiler(compiler &&);
  virtual ~compiler();

  function_obj *compile(std::shared_ptr<ir::ast>);
  function_obj *operator()(ir::ast *);

#if USE_BYTECODE_OPTIMISER
  void optimise(bytecode &);
#endif

  static std::atomic<unsigned int> peak_concurrency;
  static std::atomic<unsigned int> peak_serial;

  virtual void visit(ir::numeric_expr &);
  virtual void visit(ir::stmt &);
  virtual void visit(ir::scoped_name &);
  virtual void visit(ir::parameter &);
  virtual void visit(ir::string_spec &);
  virtual void visit(ir::use_stmt &);
  virtual void visit(ir::lambda_decl &);
  virtual void visit(ir::primary_expr &);
  virtual void visit(ir::call_expr &);
  virtual void visit(ir::unary_expr &);
  virtual void visit(ir::mult_expr &);
  virtual void visit(ir::add_expr &);
  virtual void visit(ir::compare_expr &);
  virtual void visit(ir::equality_expr &);
  virtual void visit(ir::is_expr &);
  virtual void visit(ir::and_expr &);
  virtual void visit(ir::or_expr &);
  virtual void visit(ir::assign_expr &);
  virtual void visit(ir::expr &);
  virtual void visit(ir::arguments &);
  virtual void visit(ir::return_stmt &);
  virtual void visit(ir::type1 &);
  virtual void visit(ir::print_stmt &);
  virtual void visit(ir::if_stmt &);
  virtual void visit(ir::for_stmt &);
  virtual void visit(ir::expr_stmt &);
  virtual void visit(ir::block &);
  virtual void visit(ir::parameters &);
  virtual void visit(ir::function &);
  virtual void visit(ir::function_decl &);
  virtual void visit(ir::var_decl &);
  virtual void visit(ir::type5 &);
  virtual void visit(ir::object_decl &);
  virtual void visit(ir::declaration &);
  virtual void visit(ir::script &);
  virtual void visit(ir::type2 &);
  virtual void visit(ir::type3 &);
  virtual void visit(ir::super_expr &);
  virtual void visit(ir::nil_expr &);
  virtual void visit(ir::map_impl &);
  virtual void visit(ir::kv_pair &);
  virtual void visit(ir::type4 &);
  virtual void visit(ir::object_body &);
  virtual void visit(ir::member_expr &);
  virtual void visit(ir::subscript_expr &);
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
  void subcompile(function_obj *, ir::ast *);
  void defer(compiler &&, ir::ast *node);
  void finalise(function_obj *);
  void await();
  void await(std::shared_future<function_obj *> &);

  void walk(std::shared_ptr<ir::ast> obj) {
    walk(obj.get());
  }

  void walk(ir::ast *obj) {
    if (obj) {
      walk(*obj);
    }
  }

  void walk(ir::ast &obj) {
    obj.accept(*this);
  }

  void walk(std::vector<std::shared_ptr<ir::ast>> &v) {
    for (auto &p : v) {
      walk(p);
    }
  }

  void emit_const(obj *);
  void emit_const(var &);
  void emit_const(std::string &);
  void emit_const(double);
  void emit_number(double);
  void emit_op(opcode, token_ref);
  void emit_op(opcode);
  void emit_byte(uint8_t, token_ref);
  void emit_byte(uint8_t);
  void emit_operand(uint16_t);
  size_t mark_jump(opcode, uint32_t);
  void patch_jump(size_t, size_t);
  void patch_jump(size_t);
  void patch_op(size_t, opcode, token_ref);
  void patch_op(size_t, opcode);
  void patch_upvar(uint32_t);
  void patch_closure(function_obj *);
  opcode op_at(size_t off);
  bytecode &current_bytecode();

  size_t bytecode_pos() {
    return current_bytecode().size();
  }

  void push(int sp = 1) {
    _stack_pos += sp;
  }

  void pop(int sp = -1) {
    _stack_pos += sp;
  }

  size_t begin_scope() {
    return _stack_pos;
  }

  void end_scope(size_t);

  void declare_variable(ir::ast &);
  void add_local(std::string);
  void capture_local(size_t);
  int add_upvar(size_t, bool);
  int find_local(std::string);
  local_var *find_local(size_t);
  int find_upvar(std::string);
  size_t find_global(std::string);

  void while_loop(ir::loop_stmt &);
  void loop_while(ir::loop_stmt &);
  void loop_until(ir::loop_stmt &);
  void until_loop(ir::loop_stmt &);
  void basic_loop(ir::loop_stmt &);

  compiler *_enclosing;
  function_obj *_fun_obj;
  bool _concurrent;
  size_t _stack_pos;
  uint8_t _prev_op;
  std::mutex _mutex;

  static std::atomic<unsigned int> concurrency;

  std::map<std::string, loop_info> _continue_map;
  std::map<std::string, loop_info> _break_map;
  std::stack<loop_info> _continue_stack;
  std::stack<loop_info> _break_stack;
  std::vector<std::shared_future<function_obj *>> _fun_objs;
};

} // namespace dwt

#endif
