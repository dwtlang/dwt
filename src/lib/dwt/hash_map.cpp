// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/hash_map.hpp>
#include <dwt/obj.hpp>

namespace dwt {

hash_map::hash_map(size_t capacity)
  : _buckets(std::make_unique<kv_pair[]>(capacity))
  , _capacity(capacity)
  , _entries(0) {
}

hash_map::hash_map(const hash_map &other)
  : _buckets(std::make_unique<kv_pair[]>(other._capacity))
  , _capacity(other._capacity)
  , _entries(0) {
  for (size_t i = 0; i < other._capacity; ++i) {
    auto &entry = other._buckets[i];

    if (entry.key != nil) {
      var key, value;

      if (VAR_IS_OBJ(entry.key)) {
        key = OBJ_AS_VAR(VAR_AS_OBJ(entry.key)->clone());
      } else {
        key = entry.key;
      }

      if (VAR_IS_OBJ(entry.value)) {
        value = OBJ_AS_VAR(VAR_AS_OBJ(entry.value)->clone());
      } else {
        value = entry.value;
      }

      add(kv_pair(key, value));
    }
  }
}

hash_map::~hash_map() {
}

kv_pair *hash_map::add(kv_pair pair) {
  if ((_entries + 1) > _capacity * LOAD_FACTOR) {
    grow();
  }

  hash_t hash = pair.key;

  if (VAR_IS_OBJ(pair.key)) {
    if (VAR_AS_OBJ(pair.key)) {
      hash = VAR_AS_OBJ(pair.key)->hash();
    } else {
      return nullptr;
    }
  }

  auto pos = hash & (_capacity - 1);
  kv_pair *tombstone = nullptr;

  for (size_t i = 0; i < _capacity; ++i) {
    auto entry = &_buckets[pos];

    if (entry->is_tombstone()) {
      tombstone = entry;
    } else if (entry->key == nil) {
      if (tombstone) {
        entry = tombstone;
      } else {
        ++_entries;
      }
      *entry = pair;
      return entry;
    } else if (entry->key == pair.key) {
      *entry = pair;
      return entry;
    }

    pos = (pos + 1) & (_capacity - 1);
  }

  return nullptr;
}

bool hash_map::del(var key) {
  bool deleted = false;
  auto kv = get(key);

  if (kv->key != nil) {
    kv->mark_tombstone();
    deleted = true;
  }

  return deleted;
}

kv_pair *hash_map::get(var key) const {
  hash_t hash = 0;

  if (VAR_IS_OBJ(key)) {
    hash = VAR_AS_OBJ(key)->hash();
  } else {
    hash = key;
  }

  auto pos = hash & (_capacity - 1);

  for (size_t i = 0; i < _capacity; ++i) {
    auto entry = &_buckets[pos];

    if (entry->key == key) {
      return entry;
    }

    if (entry->key == nil && entry->value == nil) {
      break;
    }

    pos = (pos + 1) & (_capacity - 1);
  }

  return nullptr;
}

void hash_map::grow() {
  hash_map tmp(_capacity * 2);

  for (size_t i = 0; i < _capacity; ++i) {
    auto entry = &_buckets[i];

    if (entry->key != nil) {
      tmp.add(*entry);
    }
  }

  *this = std::move(tmp);
}

} // namespace dwt
