// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/exception.hpp>
#include <dwt/utf8_source.hpp>

namespace dwt {

utf8_source::utf8_source(std::string filename)
  : _bytes_read(0)
  , _char_code(0)
  , _char_size(0)
  , _name(filename) {

  _bytes = fopen(filename.c_str(), "rb");

  if (_bytes) {
    _char_code = decode();
    _char_size = _bytes_read;
  } else {
    throw exception("no such file: " + filename + "\n");
  }

  _tokens = std::make_shared<token_seq>(filename);
}

utf8_source::~utf8_source() {
  if (_bytes) {
    fclose(_bytes);
  }
}

std::string utf8_source::name() const {
  return _name;
}

std::shared_ptr<token_seq> utf8_source::tokens() {
  return _tokens;
}

int utf8_source::get(int &nr_bytes) {
  nr_bytes = _char_size;
  return get();
}

int utf8_source::peek(int &nr_bytes) {
  nr_bytes = _char_size;
  return peek();
}

int utf8_source::get() {
  int pos = _bytes_read;
  int code = _char_code;

  _char_code = decode();
  _char_size = _bytes_read - pos;

  return code;
}

int utf8_source::peek() {
  return _char_code;
}

int utf8_source::get_byte() {
  uint8_t octet;
  int val = -1;

  if (fread(&octet, 1, 1, _bytes) == 1) {
    _sha3.update(&octet, 1);
    ++_bytes_read;
    val = octet;
  }

  return val;
}

int utf8_source::decode() {
  int code_point = get_byte();
  int char_code = code_point;
  int bytes_remaining = 0;

  if ((code_point & 0xe0) == 0xc0) {
    bytes_remaining = 1;
    char_code &= 0x1f;
  } else if ((code_point & 0xf0) == 0xe0) {
    bytes_remaining = 2;
    char_code &= 0x0f;
  } else if ((code_point & 0xf8) == 0xf0) {
    bytes_remaining = 3;
    char_code &= 0x07;
  }

  while (bytes_remaining-- && char_code != -1) {
    code_point = get_byte();
    if (code_point > 0) {
      char_code = (char_code << 6) | (code_point & 0x3f);
    } else {
      char_code = code_point;
    }
  }

  return char_code;
}

} // namespace dwt
