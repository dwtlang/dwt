// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_HASH_MAP_HPP
#define GUARD_DWT_HASH_MAP_HPP

#include <dwt/fnv1a.hpp>
#include <dwt/kv_pair.hpp>
#include <dwt/stack.hpp>
#include <dwt/var.hpp>

#include <memory>

namespace dwt {

#define LOAD_FACTOR 0.75

class hash_map {
  friend class map_obj;
  friend class code_obj;

public:
  hash_map(size_t capacity = 16);
  hash_map(const hash_map &);
  virtual ~hash_map();

  kv_pair *add(kv_pair);
  bool del(var key);
  kv_pair *get(var key) const;

  template <typename Fn> void for_all(Fn f) {
    for (size_t i = 0; i < _capacity; ++i) {
      auto entry = &_buckets[i];

      if (!VAR_IS_NIL(entry->key)) {
        f(entry);
      }
    }
  }

  size_t size() const {
    return _entries;
  }

protected:
  // destructive assignment operator for internal use only
  hash_map &operator=(hash_map &&other) {
    _buckets.swap(other._buckets);
    _capacity = other._capacity;
    _entries = other._entries;
    other._buckets = nullptr;
    other._capacity = 0;
    other._entries = 0;
    return *this;
  }

  void grow();

  std::unique_ptr<kv_pair[]> _buckets;
  size_t _capacity;
  size_t _entries;
};

} // namespace dwt

#endif
