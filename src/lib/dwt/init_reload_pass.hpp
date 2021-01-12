// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_INIT_RELOAD_PASS_HPP
#define GUARD_DWT_INIT_RELOAD_PASS_HPP

#include <dwt/peephole.hpp>

namespace dwt {

class init_reload_pass : public peephole {
public:
  init_reload_pass(code_obj &);
  virtual ~init_reload_pass();

private:
  virtual void peep(uint8_t *op, size_t extent) override;
};

} // namespace dwt

#endif
