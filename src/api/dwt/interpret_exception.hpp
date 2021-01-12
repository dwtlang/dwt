// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_INTERPRET_EXCEPTION_HPP
#define GUARD_DWT_INTERPRET_EXCEPTION_HPP

#include <dwt/exception.hpp>

namespace dwt {

class interpret_exception : public exception {
public:
  interpret_exception(std::string what)
    : exception(what) {
  }

  interpret_exception(const interpret_exception &other) = default;
  virtual ~interpret_exception() = default;
};

} // namespace dwt

#endif
