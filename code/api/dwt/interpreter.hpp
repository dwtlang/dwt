// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_INTERPRETER_HPP
#define GUARD_DWT_INTERPRETER_HPP

#include <dwt/call_frame.hpp>
#include <dwt/closure_obj.hpp>
#include <dwt/code_obj.hpp>
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

#include <vector>

namespace dwt {

class interpreter {
  friend class function_obj;

public:
  explicit interpreter();
  interpreter(interpreter &&) = default;
  interpreter(const interpreter &) = delete;
  virtual ~interpreter();

  inline void invoke(function_obj *fun_obj, unsigned int num_args) {
    call_stack.push(call_frame(fun_obj, exec_stack.size() - (num_args + 1)));
  }

  inline void invoke(closure_obj *closure, unsigned int num_args) {
    call_stack.push(call_frame(closure, exec_stack.size() - (num_args + 1)));
  }

  inline void invoke(class_obj *klass, unsigned int num_args) {
    call_stack.push(call_frame(klass, exec_stack.size() - (num_args + 1)));
  }

  inline void invoke(instance_obj *inst_obj, unsigned int num_args) {
  }

  inline void invoke(mapfn_obj *mapfn, unsigned int nr_args) {
    call_stack.push(call_frame(mapfn, exec_stack.size() - (nr_args + 1)));
  }

  inline void invoke(syscall_obj *syscall, unsigned int num_args) {
    size_t fp = exec_stack.size() - (num_args + 1);
    var *args = exec_stack.top_ptr(num_args - 1);
    var r = syscall->impl()(num_args, args);
    exec_stack.pop(exec_stack.size() - fp);
    exec_stack.push(r);
  }

  void mark_roots(std::vector<obj *> &grey_objs);

  var interpret(obj *callable_obj, var *args, size_t nr_args);

  inline void next(interpreter *vm) {
    _next = vm;
  }

  inline void prev(interpreter *vm) {
    _prev = vm;
  }

  inline interpreter *next() const {
    return _next;
  }

  inline interpreter *prev() const {
    return _prev;
  }

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
  upvar_obj *open_upvars = nullptr;
  interpreter *_next = nullptr;
  interpreter *_prev = nullptr;
};

} // namespace dwt

#endif
