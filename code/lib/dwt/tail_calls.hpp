// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_TAIL_CALLS_HPP
#define GUARD_DWT_TAIL_CALLS_HPP

#include <dwt/peephole.hpp>

namespace dwt {

class tail_calls : public peephole {
public:
  tail_calls(code_obj &);
  virtual ~tail_calls();

private:
  uint8_t *prev_op(uint8_t *this_op);

  virtual void peep(uint8_t *op, size_t extent) override;

  code_obj &_code;
};

} // namespace dwt

#endif
