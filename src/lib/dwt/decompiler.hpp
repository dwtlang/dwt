// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_DECOMPILER_HPP
#define GUARD_DWT_DECOMPILER_HPP

#include <dwt/feedback.hpp>
#include <dwt/function_obj.hpp>
#include <dwt/hash_map.hpp>

#include <memory>

#ifndef JENKINS
#define TERM_BOLD "\x1b[1m"
#define TERM_RESET "\x1b[0m"
#else
#define TERM_BOLD ""
#define TERM_RESET ""
#endif

namespace dwt {

class decompiler {
public:
  decompiler(function_obj *function_obj);

  virtual ~decompiler();

  void decompile();

private:
  void pass();

  bool peek(uint8_t s) const {
    return _op == s;
  }

  uint8_t peek() const {
    return _fun_obj->code().base()[_ip];
  }

  uint8_t accept() {
    uint8_t op = peek();
    if (_pass == 2) {
      kv_pair *kvp = _labels.get(NUM_AS_VAR(_ip));

      if (kvp) {
        auto label = static_cast<string_obj *>(VAR_AS_OBJ(kvp->value));
        err(TERM_BOLD + label->text() + ":\n" + TERM_RESET);
      }
    }
    advance();
    return op;
  }

  void advance() {
    _ip++;
  }

  void mark_jmp(int32_t addr);

  std::string to_label(int32_t addr);

  void read(int32_t &);
  void read(uint8_t &);

  void emit(std::string op);

  void emit(std::string, std::string operand);

  void emit(std::string, double operand);

  void op_loop();
  void op_bra();
  void op_brz();
  void op_bnz();
  void op_call();
  void op_const();
  void op_get();
  void op_set();
  void op_upvget();
  void op_upvset();
  void op_closure();
  void op_load_g();
  void op_load_c();
  void op_mbrget();
  void op_mbrset();
  void op_store();
  void op_popn();
  void op_tailcall();

  hash_map _labels;

  function_obj *_fun_obj;
  size_t _ip;
  uint8_t _op;
  unsigned int _pass;
};

} // namespace dwt

#endif
