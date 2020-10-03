// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/class_obj.hpp>
#include <dwt/closure_obj.hpp>
#include <dwt/constants.hpp>
#include <dwt/decompiler.hpp>
#include <dwt/feedback.hpp>
#include <dwt/future_obj.hpp>
#include <dwt/garbage_collector.hpp>
#include <dwt/globals.hpp>
#include <dwt/instance_obj.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/opcode.hpp>
#include <dwt/reporting.hpp>
#include <dwt/scope.hpp>
#include <dwt/var.hpp>

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <ios>
#include <map>
#include <sstream>

namespace dwt {

interpreter::interpreter()
  : exec_stack(4096) {
}

interpreter::~interpreter() {
}

void interpreter::mark_roots(std::vector<obj *> &grey_objs) {
  exec_stack.for_all([&](auto &v) {
    if (VAR_IS_OBJ(v)) {
      obj *o = VAR_AS_OBJ(v);
      o->mark_as(MARK_GREY);
      grey_objs.push_back(o);
    }
  });

  call_stack.for_all([&](auto &f) {
    f.fn->mark_as(MARK_GREY);
    grey_objs.push_back(f.fn);

    if (f.closure) {
      f.closure->mark_as(MARK_GREY);
      grey_objs.push_back(f.closure);
    }

    if (f.map) {
      f.map->mark_as(MARK_GREY);
      grey_objs.push_back(f.map);
    }
  });

  for (auto upv : open_upvars) {
    upv->mark_as(MARK_GREY);
    grey_objs.push_back(upv);
  }
}

void interpreter::print(var v) {
  out(var_to_string(v));
  out("\n");
}

upvar_obj *interpreter::capture_upvar(size_t slot, size_t fp) {
  auto itr = std::find_if(open_upvars.begin(),
                          open_upvars.end(),
                          [slot](auto upv) { return upv->slot() == slot; });

  if (itr != open_upvars.end()) {
    return *itr;
  }

  upvar_obj *upvar = new upvar_obj(&exec_stack, fp, slot);

  open_upvars.push_front(upvar);

  return upvar;
}

inline void interpreter::close_upvars(size_t slot) {
  open_upvars.remove_if([slot](auto upv) {
    if (upv->slot() >= slot) {
      upv->close();
      return true;
    }
    return false;
  });
}

closure_obj *interpreter::op_closure(uint32_t operand, size_t fp) {
  function_obj *fun_obj =
    static_cast<function_obj *>(VAR_AS_OBJ(constants::table().get(operand)));
  closure_obj *closure = new closure_obj(fun_obj);
  auto upvars = fun_obj->upvars();
  closure_obj *parent = nullptr;

  if (call_stack.size() > 0) {
    parent = static_cast<closure_obj *>(call_stack.top_ref().closure);
  }

  for (size_t i = 0; i < upvars.size(); ++i) {
    if (upvars[i].is_local()) {
      closure->upvars()[i] = capture_upvar(upvars[i].slot(), fp);
    } else if (parent) {
      closure->upvars()[i] = parent->upvars()[i];
    } else {
      BUG();
    }
  }

  if (call_stack.top_ref().fn->type() == OBJ_CLASS) {
    if (fun_obj->is_api()) {
      static_cast<map_obj *>(call_stack.top_ref().map)
        ->op_keyset(OBJ_AS_VAR(fun_obj->short_name()), OBJ_AS_VAR(closure));
    }
  }

  return closure;
}

#if 0
void interpreter::dump_frame(std::stringstream &ss, frame &fr)
{
    auto fun = fr._fun_obj;
    ss << scope::prettify_name(fun->name()) << "(";

    for (size_t j = 0; j < fun->arity(); ++j) {
        ss << fun->locals().at(j).short_name();
        ss << "=" << var_to_string(exec_stack.get(fr.exec_stack_pos + j + 1));
        if (j < (fun->arity() - 1)) {
            ss << ", ";
        }
    }

    ss << ")\n";
}

std::string interpreter::stack_trace()
{
    std::stringstream ss;
    ss << "trace:\n";

    size_t line_digits = digits(frames.size());

    for (size_t i = 0; i < frames.size(); ++i) {
        if (i == 0) {
            ss << TERM_BOLD << "~>  ";
            ss << format_decimal(frames.size() - 1 - i, line_digits);
            ss << "  ";
            ss << TERM_RESET;
        }
        else {
            ss << TERM_BOLD;
            ss << "    ";
            ss << format_decimal(frames.size() - 1 - i, line_digits);
            ss << "  ";
            ss << TERM_RESET;
        }

        // dump_frame(ss, frames.top(i));
    }

    ss << "\n";

    return ss.str();
}

void interpreter::dump_state(frame *frame, uint8_t *&op)
{
    std::stringstream ss;
    ss << "state:\n";
    ss << "   interpreter: interpreter" << std::to_string(id()) << " op: " << decode(*op)
       << std::setfill('0') << std::setw(4) << std::hex
       << " irq: " << std::setw(2) << std::hex << interpreter::irq
       << " oob: " << std::setw(2) << std::hex << oob << "\n";

    ss << "stack:\n";

    for (int i = exec_stack.size() - 1; i >= 0; --i) {
        if (i == exec_stack.size() - 1) {
            ss << " ~>  ";
        }
        else {
            ss << "     ";
        }
        ss << std::setfill('0') << std::setw(4) << std::hex << i;
        ss << "  ";
        ss << var_to_string(exec_stack.get(i));
        ss << "\n";
    }

    err(ss);

    // decompiler decompiler(frame->_fun_obj);
    // decompiler.decompile();
}

#endif

token_ref interpreter::get_op_token(function_obj *fun_obj, uint8_t *op_ptr) {
  uintptr_t op_idx = op_ptr - fun_obj->bytecode().entry();

  return fun_obj->bytecode().token_at(op_idx);
}

void interpreter::interpret(function_obj *fun_obj) {
  exec_stack.push(OBJ_AS_VAR(fun_obj));
  invoke(fun_obj, 0);

  uint8_t *op = call_stack.top_ref().ip;
  size_t fp = 0;

  // "registers"...
  int o0;
  bool b0, b1;
  var v0, v1;

#include <vm/interpreter.inc>

  try {

    DISPATCH_LOOP {

      CASE_OP(SKIP) {
        DISPATCH();
      }

      CASE_OP(LOOP) {
        if (unlikely(garbage_collector::get().is_waiting)) {
          garbage_collector::get().collect_garbage(this);
        }
        op -= OPERAND(op);
        DISPATCH();
      }

      CASE_OP(BRA) {
        op += OPERAND(op);
        DISPATCH();
      }

      CASE_OP(BRZ) {
        if (var_eqz(exec_stack.top_and_pop())) {
          op += OPERAND(op);
        } else {
          op += 2;
        }
        DISPATCH();
      }

      CASE_OP(BNZ) {
        if (var_neqz(exec_stack.top_and_pop())) {
          op += OPERAND(op);
        } else {
          op += 2;
        }
        DISPATCH();
      }

      CASE_OP(CALL) {
        if (unlikely(garbage_collector::get().is_waiting)) {
          garbage_collector::get().collect_garbage(this);
        }
        o0 = *op++;
        v0 = exec_stack.top(o0);
        if (VAR_IS_OBJ(v0)) {
          call_stack.top_ref().ip = op;
          VAR_AS_OBJ(v0)->call(*this, o0);
          op = call_stack.top_ref().ip;
          fp = call_stack.top_ref().sp;
        } else {
          throw std::logic_error("e@1 value is not callable");
        }
        DISPATCH();
      }

      CASE_OP(RET) {
        close_upvars(0);
        exec_stack.pop_and_swap(exec_stack.size() - (fp + 1), exec_stack.top());
        call_stack.pop();

        if (call_stack.size() > 0) {
          op = call_stack.top_ref().ip;
          fp = call_stack.top_ref().sp;
        } else {
          return;
        }
        DISPATCH();
      }

      CASE_OP(SUPER) {
        v0 = exec_stack.top_and_pop();
        if (VAR_IS_OBJ(v0) && (VAR_AS_OBJ(v0)->type() == OBJ_INSTANCE)) {
          auto o = static_cast<instance_obj *>(
            static_cast<map_obj *>(call_stack.top_ref().map));
          o->super(static_cast<instance_obj *>(VAR_AS_OBJ(v0)));
        } else {
          throw std::logic_error("e@1 after '$1' expected an object instance");
        }

        DISPATCH();
      }

      CASE_OP(NIL) {
        exec_stack.push(nil);
        DISPATCH();
      }

      CASE_OP(TRUE) {
        exec_stack.push(BOOL_AS_VAR(true));
        DISPATCH();
      }

      CASE_OP(FALSE) {
        exec_stack.push(BOOL_AS_VAR(false));
        DISPATCH();
      }

      CASE_OP(ZERO) {
        exec_stack.push(NUM_AS_VAR(0.0));
        DISPATCH();
      }

      CASE_OP(ONE) {
        exec_stack.push(NUM_AS_VAR(1.0));
        DISPATCH();
      }

      CASE_OP(TWO) {
        exec_stack.push(NUM_AS_VAR(2.0));
        DISPATCH();
      }

      CASE_OP(POP) {
        exec_stack.pop();
        DISPATCH();
      }

      CASE_OP(POPN) {
        exec_stack.pop(*op++);
        DISPATCH();
      }

      CASE_OP(TAILCALL) {
        exec_stack.squash(*op);
        op = call_stack.top_ref().fn->bytecode().entry();
        if (unlikely(garbage_collector::get().is_waiting)) {
          garbage_collector::get().collect_garbage(this);
        }
        DISPATCH();
      }

      CASE_OP(GET) {
        exec_stack.push(exec_stack.get(fp + OPERAND(op)));
        op += 2;
        DISPATCH();
      }

      CASE_OP(SET) {
        exec_stack.set(fp + OPERAND(op), exec_stack.top());
        op += 2;
        DISPATCH();
      }

      CASE_OP(CLOSE) {
        close_upvars(exec_stack.size() - 1);
        DISPATCH();
      }

      CASE_OP(UPVGET) {
        exec_stack.push(static_cast<closure_obj *>(call_stack.top_ref().closure)
                          ->upvars()[OPERAND(op)]
                          ->get());
        op += 2;
        DISPATCH();
      }

      CASE_OP(UPVSET) {
        static_cast<closure_obj *>(call_stack.top_ref().closure)
          ->upvars()[OPERAND(op)]
          ->set(exec_stack.top());
        op += 2;
        DISPATCH();
      }

      CASE_OP(MBRGET) {
        v0 = constants::table().get(OPERAND(op));
        v1 = exec_stack.top();

        if (VAR_IS_OBJ(v1)) {
          exec_stack.push(VAR_AS_OBJ(v1)->op_mbrget(v0));
        }

        op += 2;
        DISPATCH();
      }

      CASE_OP(MBRSET) {
        v0 = constants::table().get(OPERAND(op));
        v1 = exec_stack.top(1);

        if (VAR_IS_OBJ(v1)) {
          VAR_AS_OBJ(v1)->op_mbrset(v0, exec_stack.top());
        }

        op += 2;
        DISPATCH();
      }

      CASE_OP(KEYGET) {
        v1 = exec_stack.top(1);
        v0 = exec_stack.top();

        if (VAR_IS_OBJ(v1)) {
          exec_stack.push(VAR_AS_OBJ(v1)->op_keyget(v0));
        }

        DISPATCH();
      }

      CASE_OP(KEYSET) {
        v1 = exec_stack.top(2);
        v0 = exec_stack.top(1);

        if (VAR_IS_OBJ(v1)) {
          VAR_AS_OBJ(v1)->op_keyset(v0, exec_stack.top());
        }

        DISPATCH();
      }

      CASE_OP(PAIR) {
        v0 = exec_stack.top(1);
        v1 = exec_stack.top();
        static_cast<map_obj *>(call_stack.top_ref().map)->op_keyset(v0, v1);

        DISPATCH();
      }

      CASE_OP(CLOSURE) {
        exec_stack.push(OBJ_AS_VAR(op_closure(OPERAND(op), fp)));
        op += 2;
        DISPATCH();
      }

      CASE_OP(GLOBAL) {
        exec_stack.push(var(globals::table().get(OPERAND(op))));
        op += 2;
        DISPATCH();
      }

      CASE_OP(CONST) {
        exec_stack.push(var(constants::table().get(OPERAND(op))));
        op += 2;
        DISPATCH();
      }

      CASE_OP(STORE) {
        globals::table().set(OPERAND(op), exec_stack.top());
        op += 2;
        DISPATCH();
      }

      CASE_OP(ADD) {
        exec_stack.pop_and_swap(var_add(exec_stack.top(1), exec_stack.top()));
        DISPATCH();
      }

      CASE_OP(SUB) {
        exec_stack.pop_and_swap(var_sub(exec_stack.top(1), exec_stack.top()));
        DISPATCH();
      }

      CASE_OP(MUL) {
        exec_stack.pop_and_swap(var_mul(exec_stack.top(1), exec_stack.top()));
        DISPATCH();
      }

      CASE_OP(DIV) {
        exec_stack.pop_and_swap(var_div(exec_stack.top(1), exec_stack.top()));
        DISPATCH();
      }

      CASE_OP(INC) {
        exec_stack.top_swap(var_inc(exec_stack.top()));
        DISPATCH();
      }

      CASE_OP(DEC) {
        exec_stack.top_swap(var_dec(exec_stack.top()));
        DISPATCH();
      }

      CASE_OP(NEG) {
        exec_stack.top_swap(var_neg(exec_stack.top()));
        DISPATCH();
      }

      CASE_OP(LT) {
        exec_stack.pop_and_swap(
          BOOL_AS_VAR(var_lt(exec_stack.top(1), exec_stack.top())));
        DISPATCH();
      }

      CASE_OP(LTEQ) {
        exec_stack.pop_and_swap(
          BOOL_AS_VAR(var_lteq(exec_stack.top(1), exec_stack.top())));
        DISPATCH();
      }

      CASE_OP(GT) {
        exec_stack.pop_and_swap(
          BOOL_AS_VAR(var_gt(exec_stack.top(1), exec_stack.top())));
        DISPATCH();
      }

      CASE_OP(GTEQ) {
        exec_stack.pop_and_swap(
          BOOL_AS_VAR(var_gteq(exec_stack.top(1), exec_stack.top())));
        DISPATCH();
      }

      CASE_OP(EQ) {
        exec_stack.pop_and_swap(
          BOOL_AS_VAR(var_eq(exec_stack.top(1), exec_stack.top())));
        DISPATCH();
      }

      CASE_OP(NEQ) {
        exec_stack.pop_and_swap(
          BOOL_AS_VAR(var_neq(exec_stack.top(1), exec_stack.top())));
        DISPATCH();
      }

      CASE_OP(IS) {
        exec_stack.pop_and_swap(
          BOOL_AS_VAR(var_is(exec_stack.top(1), exec_stack.top())));
        DISPATCH();
      }

      CASE_OP(AND) {
        b0 = var_eqz(exec_stack.top(1));
        b1 = var_eqz(exec_stack.top());
        exec_stack.pop_and_swap(BOOL_AS_VAR(!b0 && !b1));
        DISPATCH();
      }

      CASE_OP(OR) {
        b0 = var_eqz(exec_stack.top(1));
        b1 = var_eqz(exec_stack.top());
        exec_stack.pop_and_swap(BOOL_AS_VAR(!b0 || !b1));
        DISPATCH();
      }

      CASE_OP(XOR) {
        b0 = var_eqz(exec_stack.top(1));
        b1 = var_eqz(exec_stack.top());
        exec_stack.pop_and_swap(BOOL_AS_VAR((!b0 && b1) || (b0 && !b1)));
        DISPATCH();
      }

      CASE_OP(OBJ) {
        exec_stack.push(
          OBJ_AS_VAR(static_cast<map_obj *>(call_stack.top_ref().map)));
        DISPATCH();
      }

      CASE_OP(MAP) {
        exec_stack.push(
          OBJ_AS_VAR(static_cast<map_obj *>(call_stack.top_ref().map)));
        DISPATCH();
      }

      CASE_OP(PRINT) {
        print(exec_stack.top());
        exec_stack.pop();
        DISPATCH();
      }
    }

  } catch (std::runtime_error &e) {
    err(e.what());
    oops("n@1 from...", get_op_token(fun_obj, op - 1));
  } catch (std::logic_error &e) {
    oops(e.what(), get_op_token(fun_obj, op - 1));
  }
}

} // namespace dwt
