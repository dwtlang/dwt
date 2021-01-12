// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_LOCAL_VAR_HPP
#define GUARD_DWT_LOCAL_VAR_HPP

#include <string>

namespace dwt {

class local_var {
public:
  local_var(std::string qualified_name, size_t slot)
    : _name(qualified_name)
    , _slot(slot)
    , _captured(false) {
  }

  virtual ~local_var() = default;

  std::string &name() {
    return _name;
  }

  std::string short_name() {
    return _name.substr(_name.find_last_of(':') + 1, std::string::npos);
  }

  size_t slot() const {
    return _slot;
  }

  void is_captured(bool captured) {
    _captured = captured;
  }

  bool is_captured() {
    return _captured;
  }

private:
  std::string _name;
  size_t _slot;
  bool _captured;
};

} // namespace dwt

#endif
