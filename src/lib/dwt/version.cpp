// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/feedback.hpp>
#include <dwt/version.hpp>

#include <cstring>

namespace dwt {

unsigned int version::major_no() {
  return MAJOR_VER;
}

unsigned int version::minor_no() {
  return MINOR_VER;
}

unsigned int version::patch_no() {
  return PATCH_VER;
}

const char *version::tag() {
  return BUILD_TAG;
}

std::string version::to_string() {
  std::string s;

  s += std::to_string(MAJOR_VER);
  s += ".";
  s += std::to_string(MINOR_VER);
  s += ".";
  s += std::to_string(PATCH_VER);

  if (strlen(tag()) > 0) {
    s += "-";
    s += tag();
  }

  s += " (profile=" BUILD_PROF ")";

  return s;
}

std::string version::notice() {
  std::string year_range = "2020";
  if (year_range != BUILD_YEAR) {
    year_range += "-";
    year_range += BUILD_YEAR;
  }
  std::string notice_str("    " TERM_BLUE TERM_BOLD ">~<" TERM_RESET
                         "    Dwt ");
  notice_str += version::to_string();
  notice_str += TERM_BOLD "\n   (A" TERM_RESET "|" TERM_BOLD "A)" TERM_RESET
                          "   Copyright (C) ";
  notice_str += year_range + " Andrew Scott-Jones and Contributors\n";
  notice_str += TERM_BOLD " <(  " TERM_YELLOW TERM_BOLD "V" TERM_RESET TERM_BOLD
                          "  )>" TERM_RESET " All rights reserved.\n";
  notice_str += "    " TERM_YELLOW TERM_BOLD "^ ^" TERM_RESET
                "    Licensed under the terms of the MPL v2.0\n";

  return notice_str;
}

} // namespace dwt
