// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_VERSION_HPP
#define GUARD_DWT_VERSION_HPP

#include <string>

namespace dwt {

class version {
public:
  static unsigned int major();
  static unsigned int minor();
  static unsigned int patch();
  static const char *tag();
  static std::string to_string();

private:
  version();
  version(version &&);
  version(const version &);
  version &operator=(const version &);
};

} // namespace dwt

#endif
