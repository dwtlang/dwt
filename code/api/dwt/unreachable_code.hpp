// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_UNREACHABLE_CODE_HPP
#define GUARD_DWT_UNREACHABLE_CODE_HPP

#include <dwt/peephole.hpp>

namespace dwt {

class unreachable_code : public peephole {
public:
  unreachable_code(code_obj &code);
  virtual ~unreachable_code();

private:
  size_t first_jump_after(size_t pos);

  virtual void peep(uint8_t *op, size_t extent) override;

  code_obj &_code;
};

} // namespace dwt

#endif
