// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_SET_POP_GET_HPP
#define GUARD_DWT_SET_POP_GET_HPP

#include <dwt/peephole.hpp>

namespace dwt {

class set_pop_get : public peephole {
public:
  set_pop_get(code_obj &);
  virtual ~set_pop_get();

private:
  virtual void peep(uint8_t *op, size_t extent) override;
};

} // namespace dwt

#endif
