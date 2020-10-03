// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/sha3_algo.hpp>

#include <cstring>

namespace dwt {

namespace {

const uint8_t rho[24] = { 1,  3,  6,  10, 15, 21, 28, 36, 45, 55, 2,  14,
                          27, 41, 56, 8,  25, 43, 62, 18, 39, 61, 20, 44 };

const uint8_t pi[24] = { 10, 7,  11, 17, 18, 3, 5,  16, 8,  21, 24, 4,
                         15, 23, 19, 13, 12, 2, 20, 14, 22, 9,  6,  1 };

const uint64_t round_constants[24] = {
  0x0000000000000001ull, 0x0000000000008082ull, 0x800000000000808aull,
  0x8000000080008000ull, 0x000000000000808bull, 0x0000000080000001ull,
  0x8000000080008081ull, 0x8000000000008009ull, 0x000000000000008aull,
  0x0000000000000088ull, 0x0000000080008009ull, 0x000000008000000aull,
  0x000000008000808bull, 0x800000000000008bull, 0x8000000000008089ull,
  0x8000000000008003ull, 0x8000000000008002ull, 0x8000000000000080ull,
  0x000000000000800aull, 0x800000008000000aull, 0x8000000080008081ull,
  0x8000000000008080ull, 0x0000000080000001ull, 0x8000000080008008ull
};

inline uint64_t lrotate64(uint64_t value, int shift) {
  return (value << shift) | (value >> (64 - shift));
}

inline uint64_t swap64(uint64_t x) {
  return (
    ((x & 0xff00000000000000ull) >> 56) | ((x & 0x00ff000000000000ull) >> 40) |
    ((x & 0x0000ff0000000000ull) >> 24) | ((x & 0x000000ff00000000ull) >> 8) |
    ((x & 0x00000000ff000000ull) << 8) | ((x & 0x0000000000ff0000ull) << 24) |
    ((x & 0x000000000000ff00ull) << 40) | ((x & 0x00000000000000ffull) << 56));
}

} // namespace

sha3_algo::sha3_algo() {
  uint64_t one = 1;
  uint8_t *ptr = reinterpret_cast<uint8_t *>(&one);
  big_endian = (*ptr == 0);

  reset();
}

sha3_algo::~sha3_algo() {
  reset();
}

void sha3_algo::reset() {
  memset(state.bytes, 0, SHA3_STATE_BYTES);
}

void sha3_algo::transform() {
  uint64_t b[5];
  uint64_t t;

  if (big_endian) {
    for (unsigned int i = 0; i < SHA3_STATE_WORDS; ++i) {
      state.words[i] = swap64(state.words[i]);
    }
  }

  for (unsigned int r = 0; r < SHA3_ROUNDS; ++r) {
    // theta

    for (unsigned int i = 0; i < 5; ++i) {
      b[i] = state.words[i] ^ state.words[i + 5] ^ state.words[i + 10] ^
             state.words[i + 15] ^ state.words[i + 20];
    }

    for (unsigned int i = 0; i < 5; ++i) {
      t = b[(i + 4) % 5] ^ lrotate64(b[(i + 1) % 5], 1);

      for (unsigned int j = 0; j < 25; j += 5) {
        state.words[j + i] ^= t;
      }
    }

    // rho pi

    t = state.words[1];

    for (unsigned int i = 0; i < 24; ++i) {
      int j = pi[i];
      b[0] = state.words[j];
      state.words[j] = lrotate64(t, rho[i]);
      t = b[0];
    }

    // chi

    for (unsigned int j = 0; j < SHA3_STATE_WORDS; j += 5) {
      for (unsigned int i = 0; i < 5; ++i) {
        b[i] = state.words[j + i];
      }

      for (unsigned int i = 0; i < 5; ++i) {
        state.words[j + i] ^= (~b[(i + 1) % 5]) & b[(i + 2) % 5];
      }
    }

    // iota

    state.words[0] ^= round_constants[r];
  }

  if (big_endian) {
    for (unsigned int i = 0; i < SHA3_STATE_WORDS; ++i) {
      state.words[i] = swap64(state.words[i]);
    }
  }
}

} // namespace dwt
