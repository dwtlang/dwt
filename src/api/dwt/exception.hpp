// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_EXCEPTION_HPP
#define GUARD_DWT_EXCEPTION_HPP

#include <string>

namespace dwt {

class exception {
public:
  exception(std::string what)
    : _what(what) {
  }

  exception(const exception &other)
    : _what(other._what) {
  }

  virtual ~exception() = default;

  const std::string &what() const {
    return _what;
  }

private:
  std::string _what;
};

} // namespace dwt

#endif
