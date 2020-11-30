// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_CONSTANT_FOLDING_HPP
#define GUARD_DWT_CONSTANT_FOLDING_HPP

#include <dwt/peephole.hpp>
#include <dwt/var.hpp>

namespace dwt {

class constant_folding : public peephole {
public:
  constant_folding(code_obj &);
  virtual ~constant_folding();

private:
  void fold(uint8_t *op, size_t off, var v);

  virtual void peep(uint8_t *op, size_t extent) override;

  bool _repeat;
};

} // namespace dwt

#endif
