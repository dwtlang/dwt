// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/reporting.hpp>

#include <regex>
#include <sstream>

namespace dwt {

void remove_whitespace(std::string &str) {
  auto itr = std::remove(str.begin(), str.end(), ' ');
  str.erase(itr, str.end());
  itr = std::remove(str.begin(), str.end(), '\t');
  str.erase(itr, str.end());
  itr = std::remove(str.begin(), str.end(), '\n');
  str.erase(itr, str.end());
  itr = std::remove(str.begin(), str.end(), '\r');
  str.erase(itr, str.end());
}

std::string ui_msg(std::string fmt, int idx, std::string idstr) {
  std::stringstream ss;
  size_t pos = 0;
  size_t end = fmt.find_first_of('@', 2);

  if (end != std::string::npos) {
    end -= 2;

    while (end > 0 && (fmt[end] == ' ' || fmt[end] == '\t')) {
      --end;
    }
  }

  while (fmt[pos] == ' ' || fmt[pos] == '\t') {
    ++pos;
  }

  remove_whitespace(idstr);

  std::string tag("\\$" + std::to_string(idx));
  std::string msg = fmt.substr(pos, end);
  ss << std::regex_replace(msg, std::regex(tag), idstr);
  ss << "\n";

  return ss.str();
}

std::string ui_msgfmt(std::string fmt, int idx, std::string idstr) {
  size_t pos = 0;
  std::stringstream ss;
  std::string so_far;

  while (1) {
    std::string tag("@" + std::to_string(idx));
    pos = fmt.find(tag, pos);
    if (pos != std::string::npos) {
      switch (fmt[pos - 1]) {
      case 'w':
        ss << TERM_BOLD << "warning: " << TERM_RESET;
        break;
      case 'e':
        ss << TERM_BOLD << "error: " << TERM_RESET;
        break;
      case 'n':
        ss << TERM_BOLD << "note: " << TERM_RESET;
        break;
      default:
        break;
      }
      ss << ui_msg(fmt.substr(pos + 2), idx, idstr);
      pos += 2;
    } else {
      break;
    }

    so_far += ss.str();
  }

  return ss.str();
}

} // namespace dwt
