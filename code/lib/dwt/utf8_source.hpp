// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_UTF8_SOURCE_HPP
#define GUARD_DWT_UTF8_SOURCE_HPP

#include <dwt/sha3.hpp>
#include <dwt/token_cache.hpp>
#include <dwt/uncopyable.hpp>

#include <cstdio>
#include <memory>

namespace dwt {

class utf8_source : public uncopyable {
public:
  explicit utf8_source(std::string filename);
  virtual ~utf8_source();
  int peek();
  int get();
  int peek(int &nr_bytes);
  int get(int &nr_bytes);
  std::string name() const;
  std::shared_ptr<token_cache> tokens();

  sha3_digest<256> digest() {
    return _sha3.final();
  }

private:
  int get_byte();
  int decode();

  sha3<256> _sha3;
  FILE *_bytes;
  size_t _bytes_read;
  int _char_code;
  int _char_size;
  std::string _name;
  std::shared_ptr<token_cache> _tokens;
};

} // namespace dwt

#endif
