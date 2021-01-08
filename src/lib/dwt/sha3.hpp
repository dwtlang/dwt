// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_SHA3_HPP
#define GUARD_DWT_SHA3_HPP

#include <dwt/sha3_algo.hpp>

#include <array>
#include <cstddef>
#include <cstdint>

namespace dwt {

template <size_t N> using sha3_digest = std::array<uint8_t, N / 8>;
template <size_t N = 256> class sha3 : public sha3_algo {
public:
  static_assert(N == 224 || N == 256 || N == 384 || N == 512,
                "SHA3 digest size must be 224, 256, 384, or 512");
  sha3()
    : pos(0)
    , rsiz(200 - 2 * (N / 8)) {
  }

  virtual ~sha3() {
    pos = rsiz = 0;
  }

  void update(uint8_t *bytes, size_t nr_bytes) {
    for (size_t i = 0; i < nr_bytes; ++i) {
      state.bytes[pos++] ^= bytes[i];

      if (pos == rsiz) {
        transform();
        pos = 0;
      }
    }
  }

  sha3_digest<N> final() {
    sha3_digest<N> md;

    state.bytes[pos] ^= 0x06;
    state.bytes[rsiz - 1] ^= 0x80;

    transform();

    for (unsigned int i = 0; i < (N / 8); ++i) {
      md[i] = state.bytes[i];
    }

    reset();

    return md;
  }

private:
  unsigned int pos;
  unsigned int rsiz;
};

} // namespace dwt

#endif
