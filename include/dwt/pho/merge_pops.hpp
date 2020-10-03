// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_PHO_MERGE_POPS_HPP
#define GUARD_DWT_PHO_MERGE_POPS_HPP

#include <dwt/peephole.hpp>

namespace dwt {
namespace pho {

class merge_pops : public peephole {
public:
  merge_pops(bytecode &);
  virtual ~merge_pops();

private:
  virtual void peep(uint8_t *op, size_t extent) override;

  bytecode &_code;
};

} // namespace pho
} // namespace dwt

#endif
