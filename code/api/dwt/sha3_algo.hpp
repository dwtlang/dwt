// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_SHA3_ALGO_HPP
#define GUARD_DWT_SHA3_ALGO_HPP

#include <cstddef>
#include <cstdint>

namespace dwt {

#define SHA3_STATE_WORDS 25
#define SHA3_STATE_BYTES 200
#define SHA3_ROUNDS 24

class sha3_algo {
protected:
  sha3_algo();
  virtual ~sha3_algo();

  void transform();
  void reset();

  union {
    uint64_t words[SHA3_STATE_WORDS];
    uint8_t bytes[SHA3_STATE_BYTES];
  } state;

private:
  bool big_endian = false;
};

} // namespace dwt

#endif
