// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_FOLDING_PASS_HPP
#define GUARD_DWT_FOLDING_PASS_HPP

#include <dwt/peephole.hpp>
#include <dwt/var.hpp>

namespace dwt {

class folding_pass : public peephole {
public:
  folding_pass(code_obj &);
  virtual ~folding_pass();

private:
  void fold(uint8_t *op, size_t off, var v);

  virtual void peep(uint8_t *op, size_t extent) override;

  bool _repeat;
};

} // namespace dwt

#endif
