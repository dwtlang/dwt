// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_PEEPHOLE_HPP
#define GUARD_DWT_PEEPHOLE_HPP

#include <dwt/code_obj.hpp>
#include <dwt/opcode.hpp>

namespace dwt {

struct ph_pattern {
  std::vector<uint8_t> ops;
  int extent;
};

class peephole {
public:
  virtual ~peephole();

protected:
  peephole(std::vector<ph_pattern> patterns);
  void operator()(code_obj &code);
  bool jumps_into_range(code_obj &code, size_t off, size_t extent);

private:
  virtual void peep(uint8_t *op, size_t extent) = 0;
  bool scan(std::vector<uint8_t> &code, int idx);

  std::vector<ph_pattern> _patterns;
  size_t _off;
};

} // namespace dwt

#endif
