// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/fnv1a.hpp>

#define FNV64_OFFSET_BASIS 0xcbf29ce484222325ull
#define FNV64_PRIME 1099511628211ull
#define FNV32_OFFSET_BASIS 0x811c9dc5ul
#define FNV32_PRIME 0x01000193ul

namespace dwt {

uint32_t fnv1a_32(const uint8_t *bytes, size_t nr_bytes) {
  uint32_t hash = FNV32_OFFSET_BASIS;

  while (nr_bytes--) {
    hash = hash ^ *bytes++;
    hash = hash * FNV32_PRIME;
  }

  return hash;
}

uint64_t fnv1a_64(const uint8_t *bytes, size_t nr_bytes) {
  uint64_t hash = FNV64_OFFSET_BASIS;

  while (nr_bytes--) {
    hash = hash ^ *bytes++;
    hash = hash * FNV64_PRIME;
  }

  return hash;
}

} // namespace dwt
