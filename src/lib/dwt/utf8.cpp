// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/debug.hpp>
#include <dwt/utf8.hpp>

#include <algorithm>
#include <cassert>

namespace dwt {

size_t utf8_strlen(std::string &str) {
  return std::count_if(str.begin(), str.end(), [](char ch) {
    return (static_cast<unsigned char>(ch) & 0xc0) != 0x80;
  });
}

std::string utf8_encode(int char_code) {
  std::string str;

  if (char_code < 0x80) {
    str += char_code;
  } else if (char_code <= 0x7ff) {
    str += 0xc0 | ((char_code & 0x7c0) >> 6);
    str += 0x80 | (char_code & 0x3f);
  } else if (char_code <= 0xffff) {
    str += 0xe0 | ((char_code & 0xf000) >> 12);
    str += 0x80 | ((char_code & 0xfc0) >> 6);
    str += 0x80 | (char_code & 0x3f);
  } else if (char_code <= 0x10ffff) {
    str += 0xf0 | ((char_code & 0x1c0000) >> 18);
    str += 0x80 | ((char_code & 0x3f000) >> 12);
    str += 0x80 | ((char_code & 0xfc0) >> 6);
    str += 0x80 | (char_code & 0x3f);
  } else {
    BUG();
  }

  return str;
}

int utf8_decode(uint8_t *code_point,
                unsigned int bytes_remaining,
                int *nr_codes) {
  BUG_UNLESS(bytes_remaining > 0);
  int char_code = -1;

  do {
    if (*code_point < 0x80) {
      char_code = *code_point;
      bytes_remaining = 0;
      break;
    } else if ((*code_point & 0xe0) == 0xc0) {
      if (bytes_remaining < 2) {
        break;
      }
      char_code = *code_point & 0x1f;
      bytes_remaining = 1;
    } else if ((*code_point & 0xf0) == 0xe0) {
      if (bytes_remaining < 3) {
        break;
      }
      char_code = *code_point & 0x0f;
      bytes_remaining = 2;
    } else if ((*code_point & 0xf8) == 0xf0) {
      if (bytes_remaining < 3) {
        break;
      }
      char_code = *code_point & 0x07;
      bytes_remaining = 3;
    }
  } while (0);

  if (nr_codes) {
    *nr_codes = bytes_remaining + 1;
  }

  while (bytes_remaining-- && char_code != -1) {
    if ((*++code_point & 0xc0) == 0x80) {
      char_code = (char_code << 6) | (*code_point & 0x3f);
    } else {
      char_code = -1;
    }
  }

  return char_code;
}

} // namespace dwt
