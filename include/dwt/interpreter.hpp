// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_INTERPRETER_HPP
#define GUARD_DWT_INTERPRETER_HPP

#include <dwt/bytecode.hpp>
#include <dwt/call_frame.hpp>
#include <dwt/closure_obj.hpp>
#include <dwt/debug.hpp>
#include <dwt/feedback.hpp>
#include <dwt/function_obj.hpp>
#include <dwt/instance_obj.hpp>
#include <dwt/map_obj.hpp>
#include <dwt/mapfn_obj.hpp>
#include <dwt/obj.hpp>
#include <dwt/stack.hpp>
#include <dwt/syscall_obj.hpp>
#include <dwt/upvar_obj.hpp>
#include <dwt/var.hpp>

#include <atomic>
#include <future>
#include <list>
#include <sstream>
#include <vector>

namespace dwt {

class interpreter {
  friend class function_obj;

public:
  explicit interpreter();
  interpreter(interpreter &&) = default;
  interpreter(const interpreter &) = delete;
  virtual ~interpreter();

  inline void invoke(function_obj *fun_obj, int num_args) {
    call_stack.push(call_frame(fun_obj, exec_stack.size() - (num_args + 1)));
  }

  inline void invoke(closure_obj *closure, int num_args) {
    call_stack.push(call_frame(closure, exec_stack.size() - (num_args + 1)));
  }

  inline void invoke(class_obj *klass, int num_args) {
    call_stack.push(call_frame(klass, exec_stack.size() - (num_args + 1)));
  }

  inline void invoke(instance_obj *inst_obj, int num_args) {
  }

  inline void invoke(mapfn_obj *mapfn, int nr_args) {
    call_stack.push(call_frame(mapfn, exec_stack.size() - (nr_args + 1)));
  }

  inline void invoke(syscall_obj *syscall, int num_args) {
    std::vector<var> args;

    while (num_args--) {
      args.emplace_back(exec_stack.top(num_args));
    }

    exec_stack.push(syscall->impl()(args));
  }

  void mark_roots(std::vector<obj *> &grey_objs);

  void interpret(function_obj *fun_obj);

private:
  upvar_obj *capture_upvar(size_t, size_t);
  void close_upvars(size_t);

  token_ref get_op_token(function_obj *fun_obj, uint8_t *op_ptr);
  // void dump_frame(std::stringstream &, frame &);
  // void dump_state(frame *, uint8_t *&);
  std::string stack_trace();

  void print(var);

  closure_obj *op_closure(uint32_t, size_t fp);

  stack<call_frame> call_stack;
  stack<var> exec_stack;
  std::list<upvar_obj *> open_upvars;
  // function_obj *_fun_obj;
};

} // namespace dwt

#endif
