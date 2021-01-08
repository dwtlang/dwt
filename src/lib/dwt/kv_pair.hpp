// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_KV_PAIR_HPP
#define GUARD_DWT_KV_PAIR_HPP

#include <dwt/var.hpp>

namespace dwt {

struct kv_pair {
  kv_pair(var k, var v)
    : key(k)
    , value(v) {
  }

  kv_pair()
    : key(nil)
    , value(nil) {
  }

  kv_pair(const kv_pair &other)
    : key(other.key)
    , value(other.value) {
  }

  inline bool operator==(const kv_pair &other) const {
    return (key == other.key) && (value == other.value);
  }

  inline bool is_tombstone() const {
    return (key == nil) && (value != nil);
  }

  inline void mark_tombstone() {
    key = nil;
    value = VAR_TRUE;
  }

  var key;
  var value;
};

} // namespace dwt

#endif
