// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_UPVAR_HPP
#define GUARD_DWT_UPVAR_HPP

namespace dwt {

class upvar {
public:
  upvar(size_t slot, bool local)
    : _slot(slot)
    , _local(local) {
  }

  virtual ~upvar() = default;

  size_t slot() const {
    return _slot;
  }

  void is_local(bool local) {
    _local = local;
  }

  bool is_local() {
    return _local;
  }

private:
  size_t _slot;
  bool _local;
};

} // namespace dwt

#endif
