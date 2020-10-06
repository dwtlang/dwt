// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_STACK_HPP
#define GUARD_DWT_STACK_HPP

#include <dwt/var.hpp>

#include <algorithm>
#include <cstddef>
#include <cstring>

#ifndef unlikely
#define unlikely(cond) __builtin_expect(!!(cond), 0)
#endif

namespace dwt {

template <typename T> class stack {
public:
  stack(size_t space)
    : space(space)
    , sp(-1)
    , bp(space > 0 ? new T[space] : nullptr) {
  }

  stack()
    : space(0)
    , sp(-1)
    , bp(nullptr) {
  }

  stack(const stack &) = delete;

  stack(stack &&other)
    : space(other._space)
    , sp(other.sp)
    , bp(other.bp) {
    other.space = 0;
    other.sp = -1;
    other.bp = nullptr;
  }

  ~stack() {
    if (space > 0) {
      BUG_UNLESS(bp);
      delete[] bp;
    }
  }

  void resize() {
    if (unlikely(space == 0)) {
      bp = new T[16];
      space = 16;
    } else {
      T *b = new T[space << 1];
      std::copy(bp, bp + space, b);
      delete[] bp;
      bp = b;
      space <<= 1;
    }
  }

  void push(T v) {
    if (unlikely(++sp == space)) {
      resize();
    }
    bp[sp] = v;
  }

  inline void pop(int n) {
    BUG_UNLESS(sp >= (n - 1));

    sp -= n;
  }

  inline void pop_and_swap(int n, T v) {
    BUG_UNLESS(sp >= n);
    bp[sp -= n] = v;
  }

  inline void pop() {
    BUG_UNLESS(sp >= 0);
    --sp;
  }

  inline void pop_and_swap(T v) {
    BUG_UNLESS(sp > 0);
    bp[--sp] = v;
  }

  inline void pop_all() {
    sp = -1;
  }

  inline T top(size_t off) const {
    return bp[sp - off];
  }

  inline T &top_ref() {
    return bp[sp];
  }

  inline T top() const {
    return bp[sp];
  }

  inline T top_and_pop() {
    return bp[sp--];
  }

  inline void top_swap(T v) {
    bp[sp] = v;
  }

  inline T get(size_t off) const {
    return bp[off];
  }

  inline void set(size_t off, T v) {
    bp[off] = v;
  }

  inline void squash(size_t pos, size_t n) {
    size_t shift = (sp - n) - pos;

    while (n) {
      bp[sp - shift] = bp[sp];
      --n;
      --sp;
    }
  }

  template <typename Fn> void for_all(Fn f) {
    for (int i = 0; i <= sp; ++i) {
      f(bp[i]);
    }
  }

  inline size_t size() const {
    return sp + 1;
  }

private:
  int space;
  int sp;
  T *bp;
};

} // namespace dwt

#endif
