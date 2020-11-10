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
#include <dwt/exception.hpp>
#include <dwt/feedback.hpp>
#include <dwt/garbage_collector.hpp>
#include <dwt/globals.hpp>
#include <dwt/instance_obj.hpp>
#include <dwt/interpret_exception.hpp>
#include <dwt/interpreter.hpp>
#include <dwt/opcode.hpp>
#include <dwt/reporting.hpp>
#include <dwt/scope.hpp>
#include <dwt/var.hpp>

#include <algorithm>
#include <cassert>
#include <map>

#define PUSH(x) exec_stack.push(x)
#define TOP() exec_stack.top()
#define TOPN(n) exec_stack.top(n)
#define TOP_SWAP(x) exec_stack.top_swap(x)
#define POP() exec_stack.pop()
#define POPN(n) exec_stack.pop(n)
#define TOP_AND_POP() exec_stack.top_and_pop()
#define POP_AND_SWAP(v) exec_stack.pop_and_swap(v)
#define POPN_AND_SWAP(n, v) exec_stack.pop_and_swap(n, v)
#define GET(n) exec_stack.get(n)
#define SET(n, v) exec_stack.set(n, v)

#define GC_MAYBE()                                    \
  do {                                                \
    if (unlikely(garbage_collector::is_waiting)) {    \
      garbage_collector::get().collect_garbage(this); \
    }                                                 \
  } while (0)

#define TOP_FRAME() call_stack.top_ref()
#define POP_FRAME() call_stack.pop()

#define SAVE_STATE()     \
  do {                   \
    TOP_FRAME().ip = op; \
    TOP_FRAME().sp = fp; \
  } while (0)

#define LOAD_STATE()     \
  do {                   \
    op = TOP_FRAME().ip; \
    fp = TOP_FRAME().sp; \
  } while (0)

namespace dwt {

interpreter::interpreter()
  : exec_stack(1024) {
}

interpreter::~interpreter() {
}

void interpreter::mark_roots(std::vector<obj *> &grey_objs) {
  exec_stack.for_all([&](auto &v) {
    if (is_obj(v)) {
      obj *o = as_obj(v);
      o->mark_as(MARK_GREY);
      grey_objs.push_back(o);
    }
  });

  call_stack.for_all([&](auto &f) {
    if (f.fn) {
      f.fn->mark_as(MARK_GREY);
      grey_objs.push_back(f.fn);
    }

    if (f.closure) {
      f.closure->mark_as(MARK_GREY);
      grey_objs.push_back(f.closure);
    }

    if (f.map) {
      f.map->mark_as(MARK_GREY);
      grey_objs.push_back(f.map);
    }
  });

  auto upv = open_upvars;
  while (upv) {
    upv->mark_as(MARK_GREY);
    grey_objs.push_back(upv);
    upv = upv->next_upvar();
  }
}

void interpreter::print(var v) {
  out(var_to_string(v));
  out("\n");
}

upvar_obj *interpreter::capture_upvar(size_t slot, size_t fp) {
  upvar_obj *curr_upvar = open_upvars;
  upvar_obj *prev_upvar = open_upvars;
  size_t pos = fp + slot;

  while (curr_upvar) {
    if (pos < curr_upvar->pos()) {
      prev_upvar = curr_upvar;
      curr_upvar = curr_upvar->next_upvar();
    } else if (pos == curr_upvar->pos()) {
      return curr_upvar;
    } else {
      break;
    }
  }

  curr_upvar = new upvar_obj(&exec_stack, fp, slot, curr_upvar);
  if (prev_upvar) {
    prev_upvar->next_upvar(curr_upvar);
  } else {
    open_upvars = curr_upvar;
  }

  return curr_upvar;
}

inline void interpreter::close_upvars(size_t pos) {
  while (open_upvars) {
    if (open_upvars->pos() >= pos) {
      open_upvars->close();
      open_upvars = open_upvars->next_upvar();
    } else {
      break;
    }
  }
}

closure_obj *interpreter::op_closure(uint32_t operand, size_t fp) {
  function_obj *fun_obj =
    static_cast<function_obj *>(VAR_AS_OBJ(constants::table().get(operand)));
  closure_obj *closure = new closure_obj(fun_obj);
  auto upvars = fun_obj->upvars();
  closure_obj *parent = nullptr;

  if (call_stack.size() > 0) {
    parent = static_cast<closure_obj *>(TOP_FRAME().closure);
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

  if (TOP_FRAME().fn->type() == OBJ_CLASS) {
    if (fun_obj->is_api()) {
      static_cast<map_obj *>(TOP_FRAME().map)
        ->op_keyset(as_var(fun_obj->short_name()), as_var(closure));
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
  uintptr_t op_idx = op_ptr - fun_obj->code().entry();

  return fun_obj->code().token_at(op_idx);
}

var interpreter::interpret(obj *callable_obj, var *args, size_t nr_args) {
  PUSH(as_var(callable_obj));

  for (size_t i = 0; i < nr_args; ++i) {
    PUSH(args[i]);
  }

  callable_obj->call(*this, nr_args);

  if (callable_obj->type() == OBJ_SYSCALL) {
    return TOP_AND_POP();
  }

  uint8_t *op = TOP_FRAME().ip;
  unsigned int fp = 0;
  unsigned int o0;
  var v0, v1;
  var one = as_var(1.0);
  var two = as_var(2.0);
  var zero = as_var(0.0);
  var yes = as_var(true);
  var no = as_var(false);
  auto &consts = constants::table().get_all();
  auto &global_vars = globals::table().get_all();

#include <vm/interpreter.inc>

  try {

    DISPATCH_LOOP {

      CASE_OP(SKIP) {
        DISPATCH();
      }

      CASE_OP(LOOP) {
        op -= OPERAND(op);

        GC_MAYBE();
        DISPATCH();
      }

      CASE_OP(BRA) {
        op += OPERAND(op);

        DISPATCH();
      }

      CASE_OP(BRZ) {
        if (var_eqz(TOP_AND_POP())) {
          op += OPERAND(op);
        } else {
          op += 2;
        }

        DISPATCH();
      }

      CASE_OP(BNZ) {
        if (var_neqz(TOP_AND_POP())) {
          op += OPERAND(op);
        } else {
          op += 2;
        }

        DISPATCH();
      }

      CASE_OP(CALL) {
        v0 = TOPN(o0 = *op++);
        SAVE_STATE();
        as_obj(v0)->call(*this, o0);
        LOAD_STATE();

        GC_MAYBE();
        DISPATCH();
      }

      CASE_OP(RET) {
        o0 = exec_stack.size() - (fp + 1);
        if (open_upvars) {
          close_upvars(fp);
        }
        POPN_AND_SWAP(o0, TOP());
        POP_FRAME();

        if (likely(call_stack.size() > 0)) {
          LOAD_STATE();
        } else {
          return TOP_AND_POP();
        }

        DISPATCH();
      }

      CASE_OP(SUPER) {
        v0 = TOP_AND_POP();
        if (is_obj(v0) && (VAR_AS_OBJ(v0)->type() == OBJ_INSTANCE)) {
          auto o = static_cast<instance_obj *>(
            static_cast<map_obj *>(TOP_FRAME().map));
          o->super(static_cast<instance_obj *>(VAR_AS_OBJ(v0)));
        } else {
          throw interpret_exception(
            "e@1 after '$1' expected an object instance");
        }

        DISPATCH();
      }

      CASE_OP(NIL) {
        PUSH(nil);

        DISPATCH();
      }

      CASE_OP(TRUE) {
        PUSH(yes);

        DISPATCH();
      }

      CASE_OP(FALSE) {
        PUSH(no);

        DISPATCH();
      }

      CASE_OP(ZERO) {
        PUSH(zero);

        DISPATCH();
      }

      CASE_OP(ONE) {
        PUSH(one);

        DISPATCH();
      }

      CASE_OP(TWO) {
        PUSH(two);

        DISPATCH();
      }

      CASE_OP(POP) {
        POP();

        DISPATCH();
      }

      CASE_OP(POPN) {
        POPN(*op++);

        DISPATCH();
      }

      CASE_OP(TAILCALL) {
        o0 = *op++;
        v0 = TOPN(o0);
        if (as_obj(v0) == TOP_FRAME().fn) {
          exec_stack.squash(fp, o0);
          op = TOP_FRAME().fn->code().entry();
        } else {
          SAVE_STATE();
          // definitely an object otherwise previous as_obj call would throw
          VAR_AS_OBJ(v0)->call(*this, o0);
          LOAD_STATE();
        }

        GC_MAYBE();
        DISPATCH();
      }

      CASE_OP(GET) {
        PUSH(GET(fp + OPERAND(op)));
        op += 2;

        DISPATCH();
      }

      CASE_OP(SET) {
        SET(fp + OPERAND(op), TOP());
        op += 2;

        DISPATCH();
      }

      CASE_OP(CLOSE) {
        close_upvars(exec_stack.size() - 1);

        DISPATCH();
      }

      CASE_OP(UPVGET) {
        PUSH(static_cast<closure_obj *>(TOP_FRAME().closure)
               ->upvars()[OPERAND(op)]
               ->get());
        op += 2;

        DISPATCH();
      }

      CASE_OP(UPVSET) {
        static_cast<closure_obj *>(TOP_FRAME().closure)
          ->upvars()[OPERAND(op)]
          ->set(TOP());
        op += 2;

        DISPATCH();
      }

      CASE_OP(MBRGET) {
        v0 = consts.get(OPERAND(op));
        v1 = TOP();
        PUSH(as_obj(v1)->op_mbrget(v0));
        op += 2;

        DISPATCH();
      }

      CASE_OP(MBRSET) {
        v0 = consts.get(OPERAND(op));
        v1 = TOPN(1);
        as_obj(v1)->op_mbrset(v0, TOP());
        op += 2;

        DISPATCH();
      }

      CASE_OP(KEYGET) {
        v1 = TOPN(1);
        v0 = TOP();
        PUSH(as_obj(v1)->op_keyget(v0));

        DISPATCH();
      }

      CASE_OP(KEYSET) {
        v1 = TOPN(2);
        v0 = TOPN(1);
        as_obj(v1)->op_keyset(v0, TOP());

        DISPATCH();
      }

      CASE_OP(PAIR) {
        v0 = TOPN(1);
        v1 = TOP();
        static_cast<map_obj *>(TOP_FRAME().map)->op_keyset(v0, v1);

        DISPATCH();
      }

      CASE_OP(CLOSURE) {
        PUSH(as_var(op_closure(OPERAND(op), fp)));
        op += 2;

        DISPATCH();
      }

      CASE_OP(GLOBAL) {
        PUSH(var(global_vars.get(OPERAND(op))));
        op += 2;

        DISPATCH();
      }

      CASE_OP(CONST) {
        PUSH(var(consts.get(OPERAND(op))));
        op += 2;

        DISPATCH();
      }

      CASE_OP(STORE) {
        global_vars.set(OPERAND(op), TOP());
        op += 2;

        DISPATCH();
      }

      CASE_OP(ADD) {
        POP_AND_SWAP(var_add(TOPN(1), TOP()));

        DISPATCH();
      }

      CASE_OP(SUB) {
        POP_AND_SWAP(var_sub(TOPN(1), TOP()));

        DISPATCH();
      }

      CASE_OP(MUL) {
        POP_AND_SWAP(var_mul(TOPN(1), TOP()));

        DISPATCH();
      }

      CASE_OP(DIV) {
        POP_AND_SWAP(var_div(TOPN(1), TOP()));

        DISPATCH();
      }

      CASE_OP(INC) {
        TOP_SWAP(var_inc(TOP()));

        DISPATCH();
      }

      CASE_OP(DEC) {
        TOP_SWAP(var_dec(TOP()));

        DISPATCH();
      }

      CASE_OP(NEG) {
        TOP_SWAP(var_neg(TOP()));

        DISPATCH();
      }

      CASE_OP(LT) {
        POP_AND_SWAP(as_var(var_lt(TOPN(1), TOP())));

        DISPATCH();
      }

      CASE_OP(LTEQ) {
        POP_AND_SWAP(as_var(var_lteq(TOPN(1), TOP())));

        DISPATCH();
      }

      CASE_OP(GT) {
        POP_AND_SWAP(as_var(var_gt(TOPN(1), TOP())));

        DISPATCH();
      }

      CASE_OP(GTEQ) {
        POP_AND_SWAP(as_var(var_gteq(TOPN(1), TOP())));

        DISPATCH();
      }

      CASE_OP(EQ) {
        POP_AND_SWAP(as_var(var_eq(TOPN(1), TOP())));

        DISPATCH();
      }

      CASE_OP(NEQ) {
        POP_AND_SWAP(as_var(var_neq(TOPN(1), TOP())));

        DISPATCH();
      }

      CASE_OP(IS) {
        POP_AND_SWAP(as_var(var_is(TOPN(1), TOP())));

        DISPATCH();
      }

      CASE_OP(AND) {
        POP_AND_SWAP(as_var(!var_eqz(TOPN(1)) && !var_eqz(TOP())));

        DISPATCH();
      }

      CASE_OP(OR) {
        POP_AND_SWAP(as_var(!var_eqz(TOPN(1)) || !var_eqz(TOP())));

        DISPATCH();
      }

      CASE_OP(XOR) {
        POP_AND_SWAP(as_var((!var_eqz(TOPN(1)) && var_eqz(TOP())) ||
                            (var_eqz(TOPN(1)) && !var_eqz(TOP()))));

        DISPATCH();
      }

      CASE_OP(MAP) {
        PUSH(as_var(static_cast<map_obj *>(TOP_FRAME().map)));

        DISPATCH();
      }

      CASE_OP(PRINT) {
        print(TOP_AND_POP());

        DISPATCH();
      }
    }

  } catch (interpret_exception &e) {
    oops(e.what(), get_op_token(TOP_FRAME().fn, op - 1));
  } catch (exception &e) {
    err(e.what());
    oops("n@1 from...", get_op_token(TOP_FRAME().fn, op - 1));
  }

  return nil;
}

} // namespace dwt
