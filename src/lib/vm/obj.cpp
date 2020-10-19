// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/garbage_collector.hpp>
#include <dwt/obj.hpp>

#include <cstddef>
#include <cstring>
#include <stdexcept>

namespace dwt {

std::atomic<uint64_t> obj::next_oid = 0;

void *obj::operator new(size_t size) {
  /* Offset allocation by default new alignment and store
     the memory footprint as an integer in the reserved
     space. This information will be used by delete to
     update heap statistics used by the garbage collector */
  size = ((size + __STDCPP_DEFAULT_NEW_ALIGNMENT__) + sizeof(size_t)) &
         ~(__STDCPP_DEFAULT_NEW_ALIGNMENT__ - 1);

  uint8_t *buf = static_cast<uint8_t *>(malloc(size));
  if (!buf) {
    throw std::bad_alloc();
  } else {
    size_t *footprint = reinterpret_cast<size_t *>(buf);
    *footprint = size;
    buf += __STDCPP_DEFAULT_NEW_ALIGNMENT__;

    garbage_collector::get().update_heap_size(*footprint);
  }

  return buf;
}

void obj::operator delete(void *p) {
  if (p) {
    uint8_t *buf = static_cast<uint8_t *>(p);
    buf -= __STDCPP_DEFAULT_NEW_ALIGNMENT__;

    size_t footprint = *reinterpret_cast<size_t *>(buf);

    debug {
      // encourage a crash in case of use after free
      memset(buf, 0xFF, footprint);
    }

    garbage_collector::get().update_heap_size(-footprint);

    free(buf);
  }
}

obj::obj()
  : ob_oid(next_oid++)
  , _mark(MARK_WHITE)
  , _prev(nullptr)
  , _next(nullptr) {

  garbage_collector::get().track(this);
}

obj::obj(const obj &other)
  : ob_oid(next_oid++)
  , _mark(MARK_WHITE)
  , _prev(nullptr)
  , _next(nullptr) {

  garbage_collector::get().track(this);
}

obj::~obj() {
}

hash_t obj::hash() {
  return 0;
}

bool obj::operator<(const obj &other) const {
  BUG();
  return false;
}

std::string obj::printable_string() {
  return to_string();
}

std::string obj::to_string() {
  return "<obj>";
}

void obj::call(interpreter &interpreter, int nr_args) {
  throw std::logic_error("e@1 value is not callable");
}

bool obj::op_eq(var v, bool rhs) {
  throw std::logic_error("e@1 == operator not supported for object");
  return false;
}

bool obj::op_neq(var v, bool rhs) {
  throw std::logic_error("e@1 != operator not supported for object");
  return false;
}

bool obj::op_gt(var v, bool rhs) {
  throw std::logic_error("e@1 > operator not supported for object");
  return false;
}

bool obj::op_gteq(var v, bool rhs) {
  throw std::logic_error("e@1 >= operator not supported for object");
  return false;
}

bool obj::op_lt(var v, bool rhs) {
  throw std::logic_error("e@1 < operator not supported for object");
  return false;
}

bool obj::op_lteq(var v, bool rhs) {
  throw std::logic_error("e@1 <= operator not supported for object");
  return false;
}

bool obj::op_is(var v, bool rhs) {
  if (VAR_IS_OBJ(v)) {
    return VAR_AS_OBJ(v) == this;
  }
  return false;
}

var obj::op_add(var v, bool rhs) {
  throw std::logic_error("e@1 + operator not supported for object");
  return v;
}

var obj::op_inc() {
  throw std::logic_error("e@1 ++ operator not supported for object");
  return 0;
}

var obj::op_dec() {
  throw std::logic_error("e@1 -- operator not supported for object");
  return 0;
}

var obj::op_sub(var v, bool rhs) {
  throw std::logic_error("e@1 - operator not supported for object");
  return v;
}

var obj::op_mul(var v, bool rhs) {
  throw std::logic_error("e@1 * operator not supported for object");
  return v;
}

var obj::op_div(var v, bool rhs) {
  throw std::logic_error("e@1 / operator not supported for object");
  return v;
}

var obj::op_neg() {
  throw std::logic_error("e@1 cannot negate object");
  return 0;
}

var obj::op_mbrget(var v) {
  throw std::logic_error("e@1 > operator not supported for object");
  return 0;
}

void obj::op_mbrset(var name, var v) {
  throw std::logic_error("e@1 object does not support member access");
}

var obj::op_keyget(var k) {
  throw std::logic_error("e@1 object is not a map");
  return 0;
}

void obj::op_keyset(var k, var v) {
  throw std::logic_error("e@1 object is not a map");
}

std::string decode(obj *obj) {
  char addrbuf[(sizeof(uint64_t) * 2) + 3];
  std::string s;

  if (obj) {
    s += decode(obj->type());
    s += " @ ";
  } else {
    s += "nil @ ";
  }

  sprintf(addrbuf, "0x%p", obj);

  s += addrbuf;

  return s;
}

} // namespace dwt
