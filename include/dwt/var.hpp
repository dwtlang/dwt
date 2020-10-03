// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_VAR_HPP
#define GUARD_DWT_VAR_HPP

#include <dwt/debug.hpp>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>

#define SIGN_BIT (0x8000000000000000ull)
#define QNAN (0x7ffc000000000000ull)
#define OBJ_MASK (0xFFFFFFFFFFFFull)

#define OBJ_AS_VAR(o) (SIGN_BIT | QNAN | reinterpret_cast<uint64_t>(o))
#define VAR_AS_OBJ(v) \
  reinterpret_cast<obj *>(((v) &OBJ_MASK) & ~(SIGN_BIT | QNAN))
#define VAR_IS_OBJ(v) (((v) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))
#define NUM_AS_VAR(n) num_as_var(n)
#define VAR_AS_NUM(v) var_as_num(v)
#define VAR_AS_INT(v) var_as_int(v)
#define VAR_IS_NUM(v) (((v) &QNAN) != QNAN)
#define TAG_NIL (1ull)
#define TAG_FALSE (2ull)
#define TAG_TRUE (3ull)
#define VAR_TRUE static_cast<uint64_t>(QNAN | TAG_TRUE)
#define VAR_FALSE static_cast<uint64_t>(QNAN | TAG_FALSE)
#define VAR_AS_BOOL(v) ((v) == VAR_TRUE)
#define VAR_IS_BOOL(v) (((v) &VAR_FALSE) == VAR_FALSE)
#define BOOL_AS_VAR(b) ((b) ? VAR_TRUE : VAR_FALSE)
#define nil static_cast<uint64_t>(QNAN | TAG_NIL)
#define VAR_IS_NIL(v) ((v) == nil)

namespace dwt {

typedef uint64_t var;

class obj;

std::string var_to_string(var v);

#if USE_DOUBLE_PRECISION_FP
inline var num_as_var(double n) {
  static_assert(sizeof(uint64_t) == sizeof(double));
  uint64_t bits;
  memcpy(&bits, &n, 8);
  return bits;
}
#else
inline var num_as_var(float n) {
  static_assert(sizeof(uint32_t) == sizeof(float));
  uint32_t bits;
  memcpy(&bits, &n, 4);
  return bits;
}
#endif

#if USE_DOUBLE_PRECISION_FP
inline double var_as_num(var v) {
  static_assert(sizeof(uint64_t) == sizeof(double));
  double d;
  memcpy(&d, &v, 8);
  return d;
}
#else
inline float var_as_num(var v) {
  static_assert(sizeof(uint32_t) == sizeof(float));
  float f;
  memcpy(&f, &v, 4);
  return f;
}
#endif

#if USE_DOUBLE_PRECISION_FP
inline double var_as_int(var v) {
  return trunc(var_as_num(v));
}
#else
inline float var_as_int(var v) {
  return trunc(var_as_num(v));
}
#endif

inline bool var_eqz(var v0) {
  if (VAR_IS_NUM(v0)) {
    return v0 == NUM_AS_VAR(0.0);
  }

  if (VAR_IS_BOOL(v0)) {
    return v0 == BOOL_AS_VAR(false);
  }

  return v0 == nil;
}

inline bool var_neqz(var v0) {
  if (VAR_IS_NUM(v0)) {
    return v0 != NUM_AS_VAR(0.0);
  }

  if (VAR_IS_BOOL(v0)) {
    return v0 != BOOL_AS_VAR(false);
  }

  return v0 != nil;
}

bool obj_var_eq(var v0, var v1);
bool obj_var_neq(var v0, var v1);
bool obj_var_gt(var v0, var v1);
bool obj_var_gteq(var v0, var v1);
bool obj_var_lt(var v0, var v1);
bool obj_var_lteq(var v0, var v1);
bool obj_var_is(var v0, var v1);

var obj_var_add(var v0, var v1);
var obj_var_inc(var v0);
var obj_var_sub(var v0, var v1);
var obj_var_dec(var v0);
var obj_var_mul(var v0, var v1);
var obj_var_div(var v0, var v1);
var obj_var_neg(var v0);

inline bool var_eq(var v0, var v1) {
#if USE_STRICT_IEEE_754
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return VAR_AS_NUM(v0) == VAR_AS_NUM(v1);
  }
#endif

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_eq(v0, v1);
  }

  return v0 == v1;
}

inline bool var_neq(var v0, var v1) {
#if USE_STRICT_IEEE_754
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return VAR_AS_NUM(v0) != VAR_AS_NUM(v1);
  }
#endif

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_neq(v0, v1);
  }

  return v0 != v1;
}

inline bool var_gt(var v0, var v1) {
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return VAR_AS_NUM(v0) > VAR_AS_NUM(v1);
  }

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_gt(v0, v1);
  }

  throw std::logic_error("e@1 invalid operands");
}

inline bool var_gteq(var v0, var v1) {
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return VAR_AS_NUM(v0) >= VAR_AS_NUM(v1);
  }

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_gteq(v0, v1);
  }

  throw std::logic_error("e@1 invalid operands");
}

inline bool var_lt(var v0, var v1) {
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return VAR_AS_NUM(v0) < VAR_AS_NUM(v1);
  }

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_lt(v0, v1);
  }

  throw std::logic_error("e@1 invalid operands");
}

inline bool var_lteq(var v0, var v1) {
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return VAR_AS_NUM(v0) <= VAR_AS_NUM(v1);
  }

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_lteq(v0, v1);
  }

  throw std::logic_error("e@1 invalid operands");
}

inline bool var_is(var v0, var v1) {
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return v0 == v1;
  }

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_is(v0, v1);
  }

  return false;
}

inline var var_add(var v0, var v1) {
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return NUM_AS_VAR(VAR_AS_NUM(v0) + VAR_AS_NUM(v1));
  }

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_add(v0, v1);
  }

  throw std::logic_error("e@1 invalid operands");
}

inline var var_inc(var v0) {
  if (VAR_IS_NUM(v0)) {
    return NUM_AS_VAR(VAR_AS_NUM(v0) + 1.0);
  }

  if (VAR_IS_OBJ(v0)) {
    return obj_var_inc(v0);
  }

  throw std::logic_error("e@1 invalid operand");
}

inline var var_dec(var v0) {
  if (VAR_IS_NUM(v0)) {
    return NUM_AS_VAR(VAR_AS_NUM(v0) - 1.0);
  }

  if (VAR_IS_OBJ(v0)) {
    return obj_var_dec(v0);
  }

  throw std::logic_error("e@1 invalid operand");
}

inline var var_sub(var v0, var v1) {
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return NUM_AS_VAR(VAR_AS_NUM(v0) - VAR_AS_NUM(v1));
  }

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_sub(v0, v1);
  }

  throw std::logic_error("e@1 invalid operands");
}

inline var var_mul(var v0, var v1) {
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return NUM_AS_VAR(VAR_AS_NUM(v0) * VAR_AS_NUM(v1));
  }

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_mul(v0, v1);
  }

  throw std::logic_error("e@1 invalid operands");
}

inline var var_div(var v0, var v1) {
  if (VAR_IS_NUM(v0) && VAR_IS_NUM(v1)) {
    return NUM_AS_VAR(VAR_AS_NUM(v0) / VAR_AS_NUM(v1));
  }

  if (VAR_IS_OBJ(v0) || VAR_IS_OBJ(v1)) {
    return obj_var_div(v0, v1);
  }

  throw std::logic_error("e@1 invalid operands");
}

inline var var_neg(var v0) {
  if (VAR_IS_NUM(v0)) {
    return NUM_AS_VAR(-VAR_AS_NUM(v0));
  }

  if (VAR_IS_OBJ(v0)) {
    return obj_var_neg(v0);
  }

  throw std::logic_error("e@1 invalid operand");
}

} // namespace dwt

#endif
