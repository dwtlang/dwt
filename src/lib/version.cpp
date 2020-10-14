// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/version.hpp>

#include <cstring>
#include <sstream>

namespace dwt {

unsigned int version::major() {
  return MAJOR_VER;
}

unsigned int version::minor() {
  return MINOR_VER;
}

unsigned int version::patch() {
  return PATCH_VER;
}

const char *version::tag() {
  return BUILD_TAG;
}

std::string version::to_string() {
  std::stringstream ss;
  ss << MAJOR_VER << "." << MINOR_VER << "." << PATCH_VER;

  if (strlen(tag()) > 0) {
    ss << "-" << tag();
  }

  return ss.str();
}

} // namespace dwt
