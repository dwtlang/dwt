// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/class_obj.hpp>
#include <dwt/closure_obj.hpp>
#include <dwt/compiler.hpp>
#include <dwt/constants.hpp>
#include <dwt/debug.hpp>
#include <dwt/decompiler.hpp>
#include <dwt/ffi.hpp>
#include <dwt/globals.hpp>
#include <dwt/inbuilt.hpp>
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
#include <dwt/ir/for_stmt.hpp>
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
#include <dwt/mapfn_obj.hpp>
#include <dwt/opcode.hpp>
#include <dwt/reporting.hpp>
#include <dwt/string_mgr.hpp>
#include <dwt/var.hpp>

#if USE_BYTECODE_OPTIMISER
#include <dwt/constant_folding.hpp>
#include <dwt/merge_pops.hpp>
#include <dwt/set_pop_get.hpp>
#include <dwt/tail_calls.hpp>
#include <dwt/unreachable_code.hpp>
#include <dwt/zero_branching.hpp>
#endif

#include <memory>

#define TBD 0
#define OPERAND(op) ((*(op)) | ((*((op) + 1)) << 8))

namespace dwt {

namespace {

#if USE_BYTECODE_OPTIMISER

int count_nops(uint8_t *op, size_t extent) {
  size_t off = 0;
  int nops = 0;

  while (off <= extent) {
    if (*op == OP_SKIP) {
      ++nops;
    }

    off += 1 + opcode_operand_bytes(*op);
    op += 1 + opcode_operand_bytes(*op);
  }

  return nops;
}

void patch_jumps(code_obj &code) {
  size_t off = 0;
  uint8_t *op = code.entry();

  while (off < code.size()) {
    uint16_t operand = 0;
    int nops = 0;

    switch (*op) {
    case OP_LOOP:
      operand = OPERAND(op + 1);
      nops = count_nops((op + 1) - (operand), operand);
      break;
    case OP_BRA:
      operand = OPERAND(op + 1);
      nops = count_nops(op, operand);
      break;
    case OP_BRZ:
      operand = OPERAND(op + 1);
      nops = count_nops(op, operand);
      break;
    case OP_BNZ:
      operand = OPERAND(op + 1);
      nops = count_nops(op, operand);
      break;
    default:
      break;
    }

    if (nops) {
      operand -= nops;
      op[1] = operand & 0xFF;
      op[2] = (operand >> 8) & 0xFF;
    }

    off += 1 + opcode_operand_bytes(*op);
    op += 1 + opcode_operand_bytes(*op);
  }
}

void remove_skips(code_obj &code) {
  std::vector<uint8_t> new_code;
  uint8_t *op = code.entry();
  size_t off = 0;
  size_t skips_removed = 0;

  while (off < code.size()) {
    if (*op != OP_SKIP) {
      new_code.push_back(*op);
      for (int i = 0; i < opcode_operand_bytes(*op); ++i) {
        new_code.push_back(op[1 + i]);
      }
      auto tok_ref = code.token_at(off);
      code.unmap_token_at(off);
      code.token_at(new_code.size() - 1, tok_ref);
    } else {
      ++skips_removed;
    }
    off += 1 + opcode_operand_bytes(*op);
    op += 1 + opcode_operand_bytes(*op);
  }

  code.byte_vec().swap(new_code);
}
#endif

loop_info *find_loop_info(std::string identifier,
                          std::vector<loop_info> &info_stack) {
  for (auto &info : info_stack) {
    if (info.name() == identifier) {
      return &info;
    }
  }
  return nullptr;
}

} // namespace

std::atomic<unsigned int> compiler::concurrency = 0;

compiler::compiler(function_obj *fun_obj, compiler *enclosing, bool concurrent)
  : _enclosing(enclosing)
  , _fun_obj(fun_obj)
  , _concurrent(concurrent)
  , _stack_pos(1)
  , _prev_op(OP_CALL) {

  if (concurrent) {
    ++concurrency;
  }
}

compiler::compiler()
  : _enclosing(nullptr)
  , _fun_obj(new function_obj(FN_NORMAL, 0, string_mgr::get().add_r("::")))
  , _concurrent(false)
  , _stack_pos(1)
  , _prev_op(OP_CALL) {
}

compiler::compiler(compiler &&other)
  : _enclosing(other._enclosing)
  , _fun_obj(other._fun_obj)
  , _concurrent(other._concurrent)
  , _stack_pos(other._stack_pos)
  , _prev_op(other._prev_op)
  , _continue_stack(other._continue_stack)
  , _break_stack(other._break_stack) {
}

compiler::~compiler() {
}

/**
 * Add a const and emit a reference to it in the code_obj.
 *
 * @param var The const value to add.
 */
void compiler::emit_const(var &v) {
  auto idx = constants::table().add_r(v);

  emit_op(OP_CONST);
  emit_operand(idx);
}

/**
 * Add a const std::string and emit a reference to it in the code_obj.
 *
 * @param s The const string to add.
 */
void compiler::emit_const(std::string &s) {
  obj *obj = string_mgr::get().add_r(s);
  emit_const(obj);
}

/**
 * Add a const object and emit a reference to it in the code_obj.
 *
 * @param obj The object to add.
 */
void compiler::emit_const(obj *obj) {
  var v = OBJ_AS_VAR(obj);
  auto idx = constants::table().add_r(v);

  emit_op(OP_CONST);
  emit_operand(idx);
}

/**
 * Add a const double and emit a reference to it in the code_obj.
 *
 * @param d The double to add.
 */
void compiler::emit_const(double d) {
  /* Skips are added to provide some inline workspace for the
   * optimiser which it will potentially need to perform
   * iterative constant folding.
   *
   * All skip ops are eventually removed from the optimised code_obj.
   */
  if (d == 0) {
#if USE_BYTECODE_OPTIMISER
    emit_op(OP_SKIP);
    emit_op(OP_SKIP);
#endif
    emit_op(OP_ZERO);
  } else if (d == 1) {
#if USE_BYTECODE_OPTIMISER
    emit_op(OP_SKIP);
    emit_op(OP_SKIP);
#endif
    emit_op(OP_ONE);
  } else if (d == 2) {
#if USE_BYTECODE_OPTIMISER
    emit_op(OP_SKIP);
    emit_op(OP_SKIP);
#endif
    emit_op(OP_TWO);
  } else {
    var v;
    v = NUM_AS_VAR(d);
    emit_const(v);
  }
}

/**
 * Emit an opcode and map to its corresponding input token.
 *
 * @param opcode The opcode to emit
 * @param tok The input token to map against this opcode.
 */
void compiler::emit_op(opcode opcode, token_ref tok) {

  _stack_pos += opcode_stack_effect(opcode);
  _prev_op = opcode;

  emit_byte(opcode, tok);
}

/**
 * Emit an opcode
 *
 * @param opcode The opcode to emit.
 */
void compiler::emit_op(opcode opcode) {
  _stack_pos += opcode_stack_effect(opcode);
  _prev_op = opcode;

  emit_byte(opcode);
}

/**
 * Emit a raw byte and map to its corresponding input token.
 *
 * @param octet The byte to emit.
 * @param tok The input token to map against this byte.
 */
void compiler::emit_byte(uint8_t octet, token_ref tok) {
  current_code_obj().emit(octet, tok);
}

/**
 * Emit a raw byte.
 *
 * @param octet The byte to emit.
 */
void compiler::emit_byte(uint8_t octet) {
  current_code_obj().emit(octet);
}

/**
 * Emit an operand.
 *
 * @param
 */
void compiler::emit_operand(uint16_t operand) {
  emit_byte(operand & 0xFF);
  emit_byte((operand >> 8) & 0xFF);
}

/**
 * Emit a jump instruction.
 *
 * @param jmp The jump opcode.
 * @param pos The absolute jump destination.
 * @return The offset of the jump instruction emitted.
 */
size_t compiler::mark_jump(opcode jmp, uint32_t pos) {
  emit_op(jmp);
  size_t off = code_obj_pos();

  /* Jump position provided is absolute but is translated into a
   * relative offset when committed to code_obj.
   */
  if (pos > off) {
    emit_operand(pos - off);
  } else {
    emit_operand(off - pos);
  }

  return off;
}

/**
 * Patch a jump instruction, given the offset of the jump opcode.
 *
 * @param off The jump instruction offset.
 * @param pos The new absolute jump destination.
 */
void compiler::patch_jump(size_t off, size_t pos) {
  uint8_t *byte_pos = current_code_obj().addr_at(off);
  uint16_t operand = 0;

  // Convert to relative jump offset.
  if (pos > off) {
    operand = (pos - off);
  } else {
    operand = (off - pos);
  }

  *byte_pos++ = operand & 0xFF;
  *byte_pos = (operand >> 8) & 0xFF;
}

/**
 * Patch a jump instruction, given the offset of the jump opcode.
 * Use the current code_obj position as the jump destination.
 *
 * @param off The jump instruction offset.
 */
void compiler::patch_jump(size_t off) {
  patch_jump(off, code_obj_pos());
}

/**
 * Patch the opcode at the given offset with the given opcode.
 * The input token mapping in the code_obj is also replaced.
 *
 * @param off The offset of the opcode to patch.
 * @param op The new opcode.
 * @param tok The input token to map against the new opcode.
 */
void compiler::patch_op(size_t off, opcode op, token_ref tok) {
  uint8_t *byte_pos = current_code_obj().addr_at(off);
  *byte_pos = op;
  current_code_obj().token_at(off, tok);
}

/**
 * Patch the opcode at the given offset with the given opcode.
 *
 * @param off The offset of the opcode to patch.
 * @param op The new opcode.
 */
void compiler::patch_op(size_t off, opcode op) {
  uint8_t *byte_pos = current_code_obj().addr_at(off);
  *byte_pos = op;
}

/**
 * Get the opcode at a given offset.
 *
 * @param off The opcode offset.
 * @return The opcode at the given offset.
 */
opcode compiler::op_at(size_t off) {
  return current_code_obj().opcode_at(off);
}

/**
 * Get a reference to the current code_obj chunk.
 *
 * @return The current code_obj chunk.
 */
code_obj &compiler::current_code_obj() {
  return _fun_obj->code();
}

/**
 * Compile the given AST into an executable function object.
 *
 * @param tree The AST.
 * @return The executable function object.
 */
function_obj *compiler::compile(std::unique_ptr<ir::ast> &&tree) {
  inbuilt::get(); // registers built-in functions
  return (*this)(tree.get());
}

#if USE_BYTECODE_OPTIMISER
void compiler::optimise(code_obj &code) {
  { unreachable_code pass(code); }
  { merge_pops pass(code); }
  { tail_calls pass(code); }
  { zero_branching pass(code); }
  { set_pop_get pass(code); }
  { constant_folding pass(code); }

  patch_jumps(code);
  remove_skips(code);
}
#endif

/**
 * Patch OP_CONST with OP_CLOSURE.
 *
 * A function is determined to be a closure if it contains any upvalues
 * and this is not known until the function has been compiled. Once
 * this has been determined this function will replace the OP_CONST
 * opcode to OP_CLOSURE.
 *
 * @param fun_obj The function object to patch.
 */
void compiler::patch_closure(function_obj *fun_obj) {
  // patch opcode for closures to save the interpreter some deduction work
  if (op_at(fun_obj->get_patchpoint()) == OP_CONST) {
    patch_op(fun_obj->get_patchpoint(), OP_CLOSURE);
  }
}

/**
 * Compile the given AST into an executable function object.
 *
 * @param tree The AST.
 * @return The executable function object.
 */
function_obj *compiler::operator()(ir::ast *tree) {
  walk(tree);

  if (_fun_obj->type() == OBJ_CLASS || _fun_obj->type() == OBJ_MAPINI) {
    emit_op(OP_MAP);
  } else {
#if USE_BYTECODE_OPTIMISER
    emit_op(OP_SKIP);
    emit_op(OP_SKIP);
#endif
    emit_op(OP_NIL);
  }

  emit_op(OP_RET);

#if USE_THREADED_COMPILER
  await();
#endif

#if USE_BYTECODE_OPTIMISER
  optimise(_fun_obj->code());
#endif

  debug {
    decompiler decompiler(_fun_obj);
    decompiler.decompile();
  }

  if (_concurrent) {
    --concurrency;
  }

  return _fun_obj;
}

/**
 * Subcompile the given function from the AST provided. Concurrency may
 * be used dependent on the current number of threads being employed.
 *
 * @param fun_obj The function object to compile.
 * @param node The function AST.
 */
void compiler::subcompile(function_obj *fun_obj, ir::ast *node) {
#if USE_THREADED_COMPILER
  if (concurrency > std::thread::hardware_concurrency()) {
    compiler c(fun_obj, this, false /* concurrent */);
    finalise(c(node));
  } else {
    compiler c(fun_obj, this, true /* concurrent */);
    defer(std::move(c), node);
  }
#else
  compiler c(fun_obj, this, false /* concurrent */);
  finalise(c(node));
#endif
}

/**
 * Finalise the given function object after compilation. This potentially
 * involves patching the closure opcode and compacting the code_obj
 * vector.
 *
 * @param fun_obj The function object to finalise.
 */
void compiler::finalise(function_obj *fun_obj) {
  if (_fun_obj->type() == OBJ_CLASS || fun_obj->upvars().size() > 0) {
    patch_closure(fun_obj);
  }

  fun_obj->code().compact();
}

#if USE_THREADED_COMPILER

/**
 * Move a sub-compiler instance into an async thread for deferred execution.
 * The compiled function object resulting from this is pushed to a queue
 * of futures in the enclosing compiler instance.
 *
 * @param c The compiler to move.
 * @param node The AST.
 */
void compiler::defer(compiler &&c, ir::ast *node) {
  _fun_objs.push_back(std::async(std::launch::async, std::move(c), node));
}

/**
 * Block until the future function object has been compiled. Then finalise
 * the function object in the current thread.
 *
 * @param future_obj The future function object.
 */
void compiler::await(std::shared_future<function_obj *> &future_obj) {
  finalise(future_obj.get());
}

/**
 * Block until all future function objects have been compiled.
 */
void compiler::await() {
  // wait for compilation of enclosed functions to complete
  for (auto &future_obj : _fun_objs) {
    await(future_obj);
  }
}

#endif

/**
 * Compile a numeric expression.
 *
 * @param num The numeric expression AST.
 */
void compiler::visit(ir::numeric_expr &num) {
  walk(num.children_of());
  emit_const(num.val());
}

/**
 * Compile a statement.
 *
 * @param stmt The statement AST.
 */
void compiler::visit(ir::stmt &stmt) {
  walk(stmt.children_of());
}

/**
 * Add a local variable and map to its fully scoped identifier.
 *
 * @param id_str The fully scoped identifier string.
 */
void compiler::add_local(std::string id_str) {
#if USE_THREADED_COMPILER
  std::scoped_lock hold(_mutex);
#endif
  _fun_obj->add_local(local_var(id_str, _stack_pos));
}

/**
 * Declare a variable, might be local or global.
 *
 * @param var The variable AST.
 */
void compiler::declare_variable(ir::ast &var) {
  auto var_scope = var.get_scope();
  BUG_UNLESS(var_scope);

  if (!var_scope->is_global()) {
    add_local(var.qualified_name());
  }
}

/**
 * Compile a scoped name.
 *
 * @param name The scoped name AST.
 */
void compiler::visit(ir::scoped_name &name) {
  auto ref_scope = name.get_scope();

  if (!ref_scope) {
    oops("e@1 unknown identifier '$1'", name.get_token_range());
  }
  auto id_str = ref_scope->qualified_name();

  if (name.is_setter()) {
    walk(name.parent_of()->child_at(1));

    if (ref_scope->is_global()) {
      emit_op(OP_STORE);
      emit_operand(find_global(id_str));
    } else {
      int idx = find_local(id_str);
      if (idx < 0) {
        emit_op(OP_UPVSET);
        idx = find_upvar(id_str);
        BUG_UNLESS(idx >= 0);
        emit_operand(idx);
      } else {
        emit_op(OP_SET);
        emit_operand(idx);
      }
    }
  } else {
    if (ref_scope->is_global()) {
      emit_op(OP_GLOBAL);
      emit_operand(find_global(id_str));
    } else {
      int idx = find_local(id_str);
      if (idx < 0) {
        emit_op(OP_UPVGET);
        emit_operand(find_upvar(id_str));
      } else {
        emit_op(OP_GET);
        emit_operand(idx);
      }
    }
  }
}

/**
 * Compile a parameter.
 *
 * @param param The parameter AST.
 */
void compiler::visit(ir::parameter &param) {
  declare_variable(param);
  push();
}

/**
 * Compile a string specification.
 *
 * @param str The string specification AST.
 */
void compiler::visit(ir::string_spec &str) {
  walk(str.children_of());
  emit_const(str.text());
}

/**
 * Compile a "use" statement.
 *
 * @param spec The statement specification AST.
 */
void compiler::visit(ir::use_stmt &spec) {
  walk(spec.children_of());
}

/**
 * Compile a "loop { .. } until" statement.
 *
 * @param loop The loop AST.
 */
void compiler::loop_until(ir::loop_stmt &loop) {
  auto instr_before_loop_body = code_obj_pos();
  walk(loop.body());
  auto instr_after_loop_body = code_obj_pos();

  walk(loop.cond());
  auto instr_after_cond = mark_jump(OP_BNZ, TBD);
  mark_jump(OP_LOOP, instr_before_loop_body);
  patch_jump(instr_after_cond);

  if (loop.is_tagged()) {
    auto info = find_loop_info(loop.name(), _continue_stack);

    for (auto pp : info->patch_points()) {
      // because continue is a jump forward, cannot use OP_LOOP
      patch_op(pp - 1, OP_BRA);
      patch_jump(pp, instr_after_loop_body);
    }
  }

  for (auto cp : _continue_stack.back().patch_points()) {
    // because continue is a jump forward, cannot use OP_LOOP
    patch_op(cp - 1, OP_BRA);
    patch_jump(cp, instr_after_loop_body);
  }
}

/**
 * Compile a "loop { .. } while" statement.
 *
 * @param loop The loop AST.
 */
void compiler::loop_while(ir::loop_stmt &loop) {
  auto instr_before_loop_body = code_obj_pos();
  walk(loop.body());
  auto instr_after_loop_body = code_obj_pos();

  walk(loop.cond());
  auto instr_after_cond = mark_jump(OP_BRZ, TBD);
  mark_jump(OP_LOOP, instr_before_loop_body);
  patch_jump(instr_after_cond);

  if (loop.is_tagged()) {
    auto info = find_loop_info(loop.name(), _continue_stack);

    for (auto pp : info->patch_points()) {
      // because continue is a jump forward, cannot use OP_LOOP
      patch_op(pp - 1, OP_BRA);
      patch_jump(pp, instr_after_loop_body);
    }
  }

  for (auto cp : _continue_stack.back().patch_points()) {
    // because continue is a jump forward, cannot use OP_LOOP
    patch_op(cp - 1, OP_BRA);
    patch_jump(cp, instr_after_loop_body);
  }
}

/**
 * Compile a basic loop.
 *
 * @param loop The loop AST.
 */
void compiler::basic_loop(ir::loop_stmt &loop) {
  auto instr_before_loop_body = code_obj_pos();
  walk(loop.body());
  mark_jump(OP_LOOP, instr_before_loop_body);

  if (loop.is_tagged()) {
    auto info = find_loop_info(loop.name(), _continue_stack);

    for (auto pp : info->patch_points()) {
      patch_jump(pp, instr_before_loop_body);
    }
  }

  for (auto cp : _continue_stack.back().patch_points()) {
    patch_jump(cp, instr_before_loop_body);
  }
}

/**
 * Compile a lambda declaration.
 *
 * @param decl The lambda declaration AST.
 */
void compiler::visit(ir::lambda_decl &decl) {
  std::string name = decl.qualified_name();

  function_obj *fun_obj =
    new function_obj(FN_NORMAL, decl.arity(), string_mgr::get().add_r(name));

  auto decl_scope = decl.get_scope();

  if (decl_scope->is_global()) {
    globals::table().set_r(find_global(name), OBJ_AS_VAR(fun_obj));
    emit_op(OP_GLOBAL);
    emit_operand(find_global(name));
  } else {
    fun_obj->set_patchpoint(code_obj_pos());
    declare_variable(decl);
    emit_const(fun_obj);
  }

  subcompile(fun_obj, decl.child_at(0));
}

/**
 * Compile a unary expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::unary_expr &expr) {
  walk(expr.children_of());

  switch (expr.name_tok().type()) {
  case TOK_MINUS:
    emit_op(OP_NEG, expr.name_tok());
    break;
  default:
    break;
  }
}

/**
 * Compile a primary expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::primary_expr &expr) {
  walk(expr.children_of());
}

/**
 * Compile a call expression.
 *
 * @param call The expression AST.
 */
void compiler::visit(ir::call_expr &call) {
  walk(call.callee());

  size_t stack_pos = _stack_pos;

  if (call.args()) {
    walk(call.args());
  }

  emit_op(OP_CALL, call.name_tok());
  emit_byte(call.num_args());
  current_code_obj().token_at(current_code_obj().size() - 1, call.name_tok());

  _stack_pos = stack_pos;
}

/**
 * Compile a multiply expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::mult_expr &expr) {
  walk(expr.children_of());

  switch (expr.gettok().type()) {
  case TOK_FWDSLASH:
    emit_op(OP_DIV, expr.gettok());
    break;
  case TOK_STAR:
    emit_op(OP_MUL, expr.gettok());
    break;
  default:
    break;
  }
}

/**
 * Compile an add expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::add_expr &expr) {
  walk(expr.children_of());

  switch (expr.gettok().type()) {
  case TOK_PLUS:
    if (_prev_op == OP_ONE) {
      patch_op(code_obj_pos() - 1, OP_INC, expr.gettok());
      pop();
    } else {
      emit_op(OP_ADD, expr.gettok());
    }
    break;
  case TOK_MINUS:
    if (_prev_op == OP_ONE) {
      patch_op(code_obj_pos() - 1, OP_DEC, expr.gettok());
      pop();
    } else {
      emit_op(OP_SUB, expr.gettok());
    }
    break;
  default:
    break;
  }
}

/**
 * Compile a compare expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::compare_expr &expr) {
  walk(expr.children_of());

  switch (expr.gettok().type()) {
  case TOK_GT:
    emit_op(OP_GT, expr.gettok());
    break;
  case TOK_GT_EQ:
    emit_op(OP_GTEQ, expr.gettok());
    break;
  case TOK_LT:
    emit_op(OP_LT, expr.gettok());
    break;
  case TOK_LT_EQ:
    emit_op(OP_LTEQ, expr.gettok());
    break;
  default:
    break;
  }
}

/**
 * Compile an equality expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::equality_expr &expr) {
  walk(expr.children_of());

  switch (expr.name_tok().type()) {
  case TOK_EQUALS:
    emit_op(OP_EQ, expr.name_tok());
    break;
  case TOK_NOT_EQ:
    emit_op(OP_NEQ, expr.name_tok());
    break;
  default:
    break;
  }
}

/**
 * Compile an "is" expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::is_expr &expr) {
  walk(expr.children_of());
  emit_op(OP_IS, expr.name_tok());
}

/**
 * Compile an "and" expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::and_expr &expr) {
  walk(expr.children_of());
  emit_op(OP_AND, expr.name_tok());
}

/**
 * Compile an "or" expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::or_expr &expr) {
  walk(expr.children_of());
  emit_op(OP_OR, expr.name_tok());
}

/**
 * Find a global variable from a fully scoped identifier.
 *
 * @param id_str The fully qualified identifier string.
 * @return The index of the global variable.
 */
size_t compiler::find_global(std::string id_str) {
  BUG_UNLESS(id_str.size() > 0);
  return globals::table().index_of(id_str);
}

/**
 * Find a local variable from its fully scoped identifier.
 *
 * @param id_str The fully qualified identifier string.
 * @return The stack position, or -1 if not found.
 */
int compiler::find_local(std::string id_str) {
  BUG_UNLESS(id_str.size() > 0);
#if USE_THREADED_COMPILER
  std::scoped_lock hold(_mutex);
#endif
  int slot = -1;

  for (auto &local : _fun_obj->locals()) {
    if (local.name().compare(id_str) == 0) {
      slot = local.slot();
      break;
    }
  }

  return slot;
}

/**
 * Find a local variable by probing its stack index.
 *
 * @param idx The stack index to probe.
 * @return The local_var* descriptor, or nullptr.
 */
local_var *compiler::find_local(size_t idx) {
#if USE_THREADED_COMPILER
  std::scoped_lock hold(_mutex);
#endif
  local_var *var = nullptr;

  for (auto &local : _fun_obj->locals()) {
    if (local.slot() == idx) {
      var = &local;
      break;
    }
  }

  return var;
}

/**
 * Capture a local variable on behalf of a closure.
 *
 * @param slot The stack position to capture.
 */
void compiler::capture_local(size_t slot) {
#if USE_THREADED_COMPILER
  std::scoped_lock hold(_mutex);
#endif
  for (auto &l : _fun_obj->locals()) {
    if (l.slot() == slot) {
      l.is_captured(true);
      break;
    }
  }
}

/**
 * Find an upvalue from its fully scoped identifier.
 *
 * @param id_str The fully scoped identifier string.
 * @return The stack index if found, or -1 otherwise.
 */
int compiler::find_upvar(std::string id_str) {
#if USE_THREADED_COMPILER
  std::scoped_lock hold(_mutex);
#endif
  BUG_UNLESS(id_str.size() > 0);
  int slot = -1;

  if (_enclosing) {
    slot = _enclosing->find_local(id_str);
    if (slot >= 0) {
      _enclosing->capture_local(slot);
      return _fun_obj->add_upvar(slot, true);
    }

    slot = _enclosing->find_upvar(id_str);
    if (slot >= 0) {
      return _fun_obj->add_upvar(slot, false);
    }
  }

  return slot;
}

/**
 * Compile an assignment expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::assign_expr &expr) {
  walk(expr.child_at(0));
}

/**
 * Compile an expression.
 *
 * @param The expression AST.
 */
void compiler::visit(ir::expr &expr) {
  walk(expr.children_of());
}

/**
 * Compile an argument list.
 *
 * @param args The arguments AST.
 */
void compiler::visit(ir::arguments &args) {
  walk(args.children_of());
}

/**
 * Compile a return statement.
 *
 * @param stmt The statement AST.
 */
void compiler::visit(ir::return_stmt &stmt) {
  class is_object_body : public ir::lazy_visitor {
  public:
    is_object_body() = default;
    virtual ~is_object_body() = default;

    bool answer = false;

    virtual void visit(ir::object_body &body) override {
      answer = true;
    }

    virtual void visit(ir::ast &node) override {
    }
  };

  is_object_body is_object_body;
  BUG_UNLESS(stmt.parent_of());
  stmt.parent_of()->accept(is_object_body);

  if (is_object_body.answer == true) {
    oops("e@1 explicit return not allowed in object definition",
         stmt.name_tok());
  }

  if (stmt.nr_children() > 0) {
    walk(stmt.children_of());
  } else {
    emit_op(OP_NIL);
  }
  emit_op(OP_RET);
}

/**
 * Compile a "loop while { .. }" statement.
 *
 * @param loop The loop AST.
 */
void compiler::while_loop(ir::loop_stmt &loop) {
  auto instr_before_loop = code_obj_pos();
  walk(loop.cond());
  auto instr_after_cond = mark_jump(OP_BRZ, TBD);
  walk(loop.body());
  mark_jump(OP_LOOP, instr_before_loop);
  patch_jump(instr_after_cond);

  if (loop.is_tagged()) {
    auto info = find_loop_info(loop.name(), _continue_stack);

    for (auto pp : info->patch_points()) {
      patch_jump(pp, instr_before_loop);
    }
  }

  for (auto cp : _continue_stack.back().patch_points()) {
    patch_jump(cp, instr_before_loop);
  }

  for (auto bp : _break_stack.back().patch_points()) {
    patch_jump(bp);
  }
}

/**
 * Compile a "loop until { .. }" statement.
 *
 * @param loop The statement AST.
 */
void compiler::until_loop(ir::loop_stmt &loop) {
  auto instr_before_loop = code_obj_pos();
  walk(loop.cond());
  auto instr_after_cond = mark_jump(OP_BNZ, TBD);
  walk(loop.body());
  mark_jump(OP_LOOP, instr_before_loop);
  patch_jump(instr_after_cond);

  if (loop.is_tagged()) {
    auto info = find_loop_info(loop.name(), _continue_stack);

    for (auto pp : info->patch_points()) {
      patch_jump(pp, instr_before_loop);
    }
  }

  for (auto cp : _continue_stack.back().patch_points()) {
    patch_jump(cp, instr_before_loop);
  }

  for (auto bp : _break_stack.back().patch_points()) {
    patch_jump(bp);
  }
}

void compiler::visit(ir::type1 &expr) {
  walk(expr.children_of());
}

/**
 * Compile a print statement.
 *
 * @param The statement AST.
 */
void compiler::visit(ir::print_stmt &stmt) {
  walk(stmt.children_of());

  emit_op(OP_PRINT);
}

/**
 * Compile an "if" statement.
 *
 * @param stmt The statement AST.
 */
void compiler::visit(ir::if_stmt &stmt) {
  walk(stmt.cond());
  auto instr_before_if = mark_jump(OP_BRZ, TBD);
  walk(stmt.if_body());

  if (stmt.else_body()) {
    auto instr_after_if_body = mark_jump(OP_BRA, TBD);
    patch_jump(instr_before_if);
    walk(stmt.else_body());
    patch_jump(instr_after_if_body);
  } else {
    patch_jump(instr_before_if);
  }
}

/**
 * Compile a "for" statement.
 *
 * @param stmt The statement AST.
 */
void compiler::visit(ir::for_stmt &stmt) {
  walk(stmt.children_of());
}

/**
 * Compile an expression statement.
 *
 * @param stmt The statement AST.
 */
void compiler::visit(ir::expr_stmt &stmt) {
  walk(stmt.children_of());
}

/**
 * Compile a block statement.
 *
 * @param block The statement AST.
 */
void compiler::visit(ir::block &block) {
  size_t pos = begin_scope();

  walk(block.children_of());

  end_scope(pos);
}

/**
 * Compile a parameter list.
 *
 * @param params The parameters AST.
 */
void compiler::visit(ir::parameters &params) {
  walk(params.children_of());
}

/**
 * End the current scope by closing or popping variables
 * declared within the closing scope.
 *
 * @param prev_pos The stack size before the scope was opened.
 */
void compiler::end_scope(size_t prev_pos) {
  while (_stack_pos > prev_pos) {

    local_var *var = find_local(_stack_pos - 1);

    if (var && var->is_captured()) {
      emit_op(OP_CLOSE);
    } else {
      emit_op(OP_POP);
    }
  }
}

/**
 * Compile a function definition.
 *
 * @param fun The function AST.
 */
void compiler::visit(ir::function &fun) {
  if (fun.params()) {
    walk(fun.params());
  }
  walk(fun.body());
}

/**
 * Compile a function declaration.
 *
 * @param decl The function declaration AST.
 */
void compiler::visit(ir::function_decl &decl) {
  std::string name = decl.qualified_name();

  function_obj *fun_obj =
    new function_obj(FN_NORMAL, decl.arity(), string_mgr::get().add_r(name));

  fun_obj->is_api(decl.is_api());

  if (decl.get_scope()->is_global()) {
    globals::table().set_r(find_global(name), OBJ_AS_VAR(fun_obj));
  } else {
    fun_obj->set_patchpoint(code_obj_pos());
    declare_variable(decl);
    emit_const(fun_obj);
  }

  subcompile(fun_obj, decl.child_at(0));
}

/**
 * Compile a variable declaration.
 *
 * @param decl The variable declaration AST.
 */
void compiler::visit(ir::var_decl &decl) {
  declare_variable(decl);

  auto id_str = decl.qualified_name();

  if (decl.children_of().size() == 0) {
    emit_op(OP_NIL);
  } else {
    walk(decl.children_of());
  }

  auto var_scope = decl.get_scope();

  if (var_scope->is_global()) {
    emit_op(OP_STORE);
    emit_operand(find_global(id_str));
    emit_op(OP_POP);
  }
}

void compiler::visit(ir::type5 &decl) {
  walk(decl.children_of());
}

/**
 * Compile an object declaration.
 *
 * @param decl The object declaration AST.
 */
void compiler::visit(ir::object_decl &decl) {
  std::string name = decl.qualified_name();

  class_obj *klass = new class_obj(decl.arity(), string_mgr::get().add_r(name));

  if (decl.get_scope()->is_global()) {
    globals::table().set_r(find_global(name), OBJ_AS_VAR(klass));
  } else {
    klass->set_patchpoint(code_obj_pos());
    declare_variable(decl);
    emit_const(klass);
  }

  subcompile(klass, decl.child_at(0));
}

/**
 * Compile a declaration.
 *
 * @param decl The declaration AST.
 */
void compiler::visit(ir::declaration &decl) {
  walk(decl.children_of());
}

/**
 * Compile a script.
 *
 * @param scr The script AST.
 */
void compiler::visit(ir::script &scr) {
  globals::table().set_r(find_global("::"), OBJ_AS_VAR(_fun_obj));
  walk(scr.children_of());
}

/**
 * Compile a type2.
 *
 * @param decl The type2 AST.
 */
void compiler::visit(ir::type2 &expr) {
  walk(expr.children_of());
}

/**
 * Compile a type3.
 *
 * @param decl The type3 AST.
 */
void compiler::visit(ir::type3 &decl) {
  walk(decl.children_of());
}

/**
 * Compile a "super" expression.
 *
 * @param decl The expression AST.
 */
void compiler::visit(ir::super_expr &decl) {
  walk(decl.children_of());
  emit_op(OP_SUPER, decl.name_tok());
}

/**
 * Compile a nil expression.
 *
 * @param decl The expression AST.
 */
void compiler::visit(ir::nil_expr &decl) {
  walk(decl.children_of());
  emit_op(OP_NIL);
}

/**
 * Compile a map definition.
 *
 * @param impl The definition AST.
 */
void compiler::visit(ir::map_impl &impl) {
  walk(impl.children_of());
}

/**
 * Compile a key/value pair.
 *
 * @param pair The pair AST.
 */
void compiler::visit(ir::kv_pair &pair) {
  walk(pair.children_of());
  emit_op(OP_PAIR);
}

/**
 * Compile a key/value pair.
 *
 * @param pair The pair AST.
 */
void compiler::visit(ir::type4 &decl) {
  walk(decl.children_of());
}

/**
 * Compile an object body.
 *
 * @param body The object body AST.
 */
void compiler::visit(ir::object_body &body) {
  walk(body.children_of());
}

/**
 * Compile a member expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::member_expr &expr) {
  obj *obj = string_mgr::get().add_r(expr.name());

  walk(expr.children_of());

  if (expr.is_setter()) {
    walk(expr.parent_of()->child_at(1));
    emit_op(OP_MBRSET);
    emit_operand(constants::table().add_r(OBJ_AS_VAR(obj)));
  } else {
    emit_op(OP_MBRGET);
    emit_operand(constants::table().add_r(OBJ_AS_VAR(obj)));
  }
}

/**
 * Compile a subscript expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::subscript_expr &expr) {
  walk(expr.children_of());

  if (expr.is_setter()) {
    walk(expr.parent_of()->child_at(1));
    emit_op(OP_KEYSET);
  } else {
    emit_op(OP_KEYGET);
  }
}

/**
 * Compile an object definition.
 *
 * @param obj The object AST.
 */
void compiler::visit(ir::object &obj) {
  if (obj.params()) {
    walk(obj.params());
  }

  if (obj.super()) {
    walk(obj.super());
  }

  walk(obj.body());
}

/**
 * Compile a "loop { .. }" statement.
 *
 * @param loop The loop AST.
 */
void compiler::visit(ir::loop_stmt &loop) {
  if (loop.is_tagged()) {
    _continue_stack.push_back(loop_info(loop.name(), _stack_pos));
    _break_stack.push_back(loop_info(loop.name(), _stack_pos));
  } else {
    _continue_stack.push_back(loop_info("", _stack_pos));
    _break_stack.push_back(loop_info("", _stack_pos));
  }

  switch (loop.get_type()) {
  case ir::WHILE_LOOP:
    while_loop(loop);
    break;
  case ir::LOOP_WHILE:
    loop_while(loop);
    break;
  case ir::LOOP_UNTIL:
    loop_until(loop);
    break;
  case ir::UNTIL_LOOP:
    until_loop(loop);
    break;
  case ir::BASIC_LOOP:
    basic_loop(loop);
    break;
  default:
    break;
  }

  if (loop.is_tagged()) {
    auto info = find_loop_info(loop.name(), _break_stack);

    for (auto pp : info->patch_points()) {
      patch_jump(pp);
    }
  }

  for (auto bp : _break_stack.back().patch_points()) {
    patch_jump(bp);
  }

  _continue_stack.pop_back();
  _break_stack.pop_back();
}

/**
 * Compile a module declaration.
 *
 * @param decl The declaration AST.
 */
void compiler::visit(ir::module_decl &decl) {
  walk(decl.children_of());
}

/**
 * Compile an FFI declaration.
 *
 * @param decl The FFI AST.
 */
void compiler::visit(ir::ffi_decl &decl) {
  walk(decl.children_of());
}

/**
 * Compile an "xor" expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::xor_expr &expr) {
  walk(expr.children_of());
  emit_op(OP_XOR);
}

/**
 * Compile a boolean expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::boolean &expr) {
  switch (expr.name_tok().type()) {
  case KW_TRUE:
#if USE_BYTECODE_OPTIMISER
    emit_op(OP_SKIP);
    emit_op(OP_SKIP);
#endif
    emit_op(OP_TRUE);
    break;
  case KW_FALSE:
#if USE_BYTECODE_OPTIMISER
    emit_op(OP_SKIP);
    emit_op(OP_SKIP);
#endif
    emit_op(OP_FALSE);
    break;
  default:
    BUG();
    break;
  }
}

/**
 * Compile a lambda expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::lambda_expr &expr) {
  walk(expr.impl());
  walk(expr.args());
  emit_op(OP_CALL);
  emit_byte(expr.args()->nr_children());
}

/**
 * Compile a lambda definition.
 *
 * @param lambda The lambda AST.
 */
void compiler::visit(ir::lambda &lambda) {
  walk(lambda.children_of());
}

/**
 * Compile a break statement.
 *
 * @param stmt The statement AST.
 */
void compiler::visit(ir::break_stmt &stmt) {
  if (stmt.name() != "") {
    auto info = find_loop_info(stmt.name(), _break_stack);

    auto nr_pops = _stack_pos - info->base_pos();

    while (nr_pops--) {
      emit_op(OP_POP);
    }

    info->add_patch_point(mark_jump(OP_BRA, TBD));
  } else {
    auto nr_pops = _stack_pos - _break_stack.back().base_pos();

    while (nr_pops--) {
      emit_op(OP_POP);
    }

    auto jump_pos = mark_jump(OP_BRA, TBD);
    _break_stack.back().add_patch_point(jump_pos);
  }
}

/**
 * Compile a continue statement.
 *
 * @param stmt The statement AST.
 */
void compiler::visit(ir::continue_stmt &stmt) {

  if (stmt.name() != "") {
    auto info = find_loop_info(stmt.name(), _continue_stack);

    auto nr_pops = _stack_pos - info->base_pos();

    while (nr_pops--) {
      emit_op(OP_POP);
    }

    info->add_patch_point(mark_jump(OP_LOOP, TBD));
  } else {
    auto nr_pops = _stack_pos - _continue_stack.back().base_pos();

    while (nr_pops--) {
      emit_op(OP_POP);
    }

    auto jump_pos = mark_jump(OP_LOOP, TBD);
    _continue_stack.back().add_patch_point(jump_pos);
  }
}

/**
 * Compile a map expression.
 *
 * @param expr The expression AST.
 */
void compiler::visit(ir::map_expr &expr) {
  std::string name = expr.qualified_name();

  mapfn_obj *map_obj = new mapfn_obj(string_mgr::get().add_r(name));

  map_obj->set_patchpoint(code_obj_pos());
  declare_variable(expr);
  emit_const(map_obj);
  subcompile(map_obj, expr.child_at(0));

  emit_op(OP_CALL);
  emit_byte(0); // 0 arguments
}

} // namespace dwt
