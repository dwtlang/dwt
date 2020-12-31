// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/constants.hpp>
#include <dwt/decompiler.hpp>
#include <dwt/feedback.hpp>
#include <dwt/globals.hpp>
#include <dwt/scope.hpp>

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdlib>
#include <stack>
#include <string>

namespace dwt {

decompiler::decompiler(function_obj *function_obj)
  : _fun_obj(function_obj)
  , _ip(0)
  , _op(0)
  , _pass(1) {
}

decompiler::~decompiler() {
}

void decompiler::mark_jmp(int32_t addr) {
  unsigned int num = _labels.size();
  std::string label = TERM_BOLD ".L";
  label += std::to_string(num);
  label += TERM_RESET;

  _labels.add(kv_pair(NUM_AS_VAR(_ip + (addr - 2)), to_var(label)));
}

std::string decompiler::to_label(int32_t addr) {
  std::string l;

  if (addr < 0) {
    addr = _ip + (addr - 2);
  } else {
    addr = _ip + (addr - 2);
  }

  kv_pair *kvp = _labels.get(NUM_AS_VAR(addr));
  if (kvp) {
    l = static_cast<string_obj *>(VAR_AS_OBJ(kvp->value))->text();
  } else {
    l = std::to_string(addr);
  }

  return l;
}

void decompiler::read(int32_t &v) {
  uint8_t *ptr = _fun_obj->code().addr_at(_ip);

  v = *ptr++;
  v = v | (*ptr << 8);
  _ip += 2;
}

void decompiler::read(uint8_t &v) {
  uint8_t *ptr = _fun_obj->code().addr_at(_ip);
  v = *ptr;
  _ip++;
}

void decompiler::emit(std::string op) {
  if (_pass == 2) {
    char strbuf[256];
    std::string s;

    if (op == "SKIP") {
      s += TERM_GREY;
    }

    snprintf(strbuf, 256, "\t%04x\t%s\n", (unsigned int) _ip - 1, op.c_str());
    s += strbuf;

    if (op == "SKIP") {
      s += TERM_RESET;
    }

    err(s);
  }
}

void decompiler::emit(std::string op, std::string operand) {
  if (_pass == 2) {
    char strbuf[256];
    size_t offset = op == "CALL" ? _ip - 2 : _ip - 3;

    snprintf(strbuf,
             256,
             "\t%04x\t%s\t%s\n",
             (unsigned int) offset,
             op.c_str(),
             operand.c_str());

    err(strbuf);
  }
}

void decompiler::op_loop() {
  int32_t addr;
  read(addr);

  if (_pass == 1) {
    mark_jmp(-addr);
  } else {
    std::string label = to_label(-addr);
    emit(decode(OP_LOOP), label);
  }
}

void decompiler::op_bra() {
  int32_t addr;
  read(addr);

  if (_pass == 1) {
    mark_jmp(addr);
  } else {
    std::string label = to_label(addr);
    emit(decode(OP_BRA), label);
  }
}

void decompiler::op_brz() {
  int32_t addr;
  read(addr);

  if (_pass == 1) {
    mark_jmp(addr);
  } else {
    std::string label = to_label(addr);
    emit(decode(OP_BRZ), label);
  }
}

void decompiler::op_bnz() {
  int32_t addr;
  read(addr);

  if (_pass == 1) {
    mark_jmp(addr);
  } else {
    std::string label = to_label(addr);
    emit(decode(OP_BNZ), label);
  }
}

void decompiler::op_call() {
  uint8_t operand;
  read(operand);
  std::string oper_str = std::to_string(operand);

  if (_pass == 2) {
    emit(decode(OP_CALL), oper_str);
  }
}

void decompiler::op_closure() {
  int32_t operand;
  read(operand);
  var v = constants::table().get(operand);
  std::string oper_str = var_to_string(v);

  if (_pass == 2) {
    emit(decode(OP_CLOSURE), oper_str);
  }
}

void decompiler::op_load_g() {
  int32_t operand;
  read(operand);
  std::string oper_str = globals::table().name_at(operand);

  if (_pass == 2) {
    emit(decode(OP_GLOBAL), oper_str);
  }
}

void decompiler::op_load_c() {
  int32_t operand;
  read(operand);
  std::string oper_str = var_to_string(constants::table().get(operand));

  if (_pass == 2) {
    emit(decode(OP_CONST), oper_str);
  }
}

void decompiler::op_mbrget() {
  int32_t operand;
  read(operand);
  std::string oper_str = var_to_string(constants::table().get(operand));

  if (_pass == 2) {
    emit(decode(OP_MBRGET), oper_str);
  }
}

void decompiler::op_mbrset() {
  int32_t operand;
  read(operand);
  std::string oper_str = var_to_string(constants::table().get(operand));

  if (_pass == 2) {
    emit(decode(OP_MBRSET), oper_str);
  }
}

void decompiler::op_store() {
  int32_t operand;
  read(operand);
  std::string oper_str = globals::table().name_at(operand);

  if (_pass == 2) {
    emit(decode(OP_STORE), oper_str);
  }
}

void decompiler::op_get() {
  int32_t operand;
  read(operand);
  std::string oper_str = std::to_string(operand);
  if (_pass == 2) {
    emit(decode(OP_GET), oper_str);
  }
}

void decompiler::op_set() {
  int32_t operand;
  read(operand);
  std::string oper_str = std::to_string(operand);
  if (_pass == 2) {
    emit(decode(OP_SET), oper_str);
  }
}

void decompiler::op_upvget() {
  int32_t operand;
  read(operand);
  std::string oper_str = std::to_string(operand);
  if (_pass == 2) {
    emit(decode(OP_UPVGET), oper_str);
  }
}

void decompiler::op_upvset() {
  int32_t operand;
  read(operand);
  std::string oper_str = std::to_string(operand);
  if (_pass == 2) {
    emit(decode(OP_UPVSET), oper_str);
  }
}

void decompiler::op_popn() {
  uint8_t operand;
  read(operand);
  std::string oper_str = std::to_string(operand);
  if (_pass == 2) {
    emit(decode(OP_POPN), oper_str);
  }
}

void decompiler::op_tailcall() {
  uint8_t operand;
  read(operand);
  std::string oper_str = std::to_string(operand);
  if (_pass == 2) {
    emit(decode(OP_TAILCALL), oper_str);
  }
}

void decompiler::decompile() {
  _pass = 1;
  pass();
  _pass = 2;
  pass();
}

void decompiler::pass() {
  _ip = 0;
  uint8_t op;

  if (_pass == 2) {
    std::string s;

    s += TERM_BOLD "<fun ";
    s += _fun_obj->name();
    s += ">";
    s += TERM_RESET "\n";

    err(s);
  }

  do {
    switch (op = accept()) {
    case OP_LOOP:
      op_loop();
      break;
    case OP_BRA:
      op_bra();
      break;
    case OP_BRZ:
      op_brz();
      break;
    case OP_BNZ:
      op_bnz();
      break;
    case OP_CALL:
      op_call();
      break;
    case OP_GET:
      op_get();
      break;
    case OP_SET:
      op_set();
      break;
    case OP_UPVGET:
      op_upvget();
      break;
    case OP_UPVSET:
      op_upvset();
      break;
    case OP_CLOSURE:
      op_closure();
      break;
    case OP_GLOBAL:
      op_load_g();
      break;
    case OP_CONST:
      op_load_c();
      break;
    case OP_MBRGET:
      op_mbrget();
      break;
    case OP_MBRSET:
      op_mbrset();
      break;
    case OP_STORE:
      op_store();
      break;
    case OP_POPN:
      op_popn();
      break;
    case OP_TAILCALL:
      op_tailcall();
      break;
    default:
      emit(decode(op));
      break;
    }
  } while (_ip < _fun_obj->code().size());
}

} // namespace dwt
