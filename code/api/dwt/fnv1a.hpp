// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_FNV1A_HPP
#define GUARD_DWT_FNV1A_HPP

#include <cstddef>
#include <cstdint>

namespace dwt {

#if USE_HASH32
typedef uint32_t hash_t;
#else
typedef uint64_t hash_t;
#endif

uint64_t fnv1a_64(const uint8_t *bytes, size_t nr_bytes);
uint32_t fnv1a_32(const uint8_t *bytes, size_t nr_bytes);

inline hash_t fnv1a(const uint8_t *bytes, size_t nr_bytes) {
#if USE_HASH32
  return fnv1a_32(bytes, nr_bytes);
#else
  return fnv1a_64(bytes, nr_bytes);
#endif
}

inline hash_t fnv1a(const char *bytes, size_t nr_bytes) {
#if USE_HASH32
  return fnv1a_32(reinterpret_cast<const uint8_t *>(bytes), nr_bytes);
#else
  return fnv1a_64(reinterpret_cast<const uint8_t *>(bytes), nr_bytes);
#endif
}

} // namespace dwt

#endif
