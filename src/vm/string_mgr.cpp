// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/string_mgr.hpp>

namespace dwt {

string_mgr::string_mgr() {
}

string_mgr::~string_mgr() {
}

string_mgr &string_mgr::get_global() {
  static string_mgr instance;
  return instance;
}

string_mgr &string_mgr::get() {
  thread_local string_mgr instance;
  return instance;
}

string_obj *string_mgr::get(std::string str) {
  hash_t hash = fnv1a(str.c_str(), str.size());

  return get(str, hash);
}

void string_mgr::merge(string_mgr &other) {
  for (size_t i = 0; i < other._capacity; ++i) {
    auto entry = &other._buckets[i];

    if (!VAR_IS_NIL(entry->key)) {
      string_obj *key = static_cast<string_obj *>(VAR_AS_OBJ(entry->key));
      add(key->text());
      other.del(entry->key);
    }
  }
}

string_obj *string_mgr::add(std::string str) {
  std::scoped_lock hold(_add_mutex);
  hash_t hash = fnv1a(str.c_str(), str.size());
  auto obj = get(str, hash);

  if (!obj) {
    obj = new string_obj(str);
    kv_pair kv(OBJ_AS_VAR(obj), nil);
    hash_map::add(kv);
  }

  return obj;
}

string_obj *string_mgr::get(std::string &str, hash_t hash) {
  std::scoped_lock hold(_get_mutex);

  auto pos = hash & (_capacity - 1);

  for (size_t i = 0; i < _capacity; ++i) {
    auto entry = &_buckets[pos];
    string_obj *key = nullptr;

    if (VAR_IS_NIL(entry->key) && VAR_IS_NIL(entry->value)) {
      return nullptr;
    } else {
      key = static_cast<string_obj *>(VAR_AS_OBJ(entry->key));
    }

    if (key->text() == str) {
      return key;
    } else {
      pos = (pos + 1) & (_capacity - 1);
    }
  }

  return nullptr;
}

void string_mgr::sweep() {
  for (size_t i = 0; i < _capacity; ++i) {
    auto entry = &_buckets[i];

    if (!VAR_IS_NIL(entry->key) &&
        VAR_AS_OBJ(entry->key)->marked_as() == MARK_WHITE) {
      del(entry->key);
    }
  }
}

} // namespace dwt
