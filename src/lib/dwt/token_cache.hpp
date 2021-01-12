// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#ifndef GUARD_DWT_TOKEN_SEQ_HPP
#define GUARD_DWT_TOKEN_SEQ_HPP

#include <dwt/token.hpp>

#include <vector>

namespace dwt {

class token_cache {
public:
  explicit token_cache(std::string source)
    : _src(source) {
  }
  token_cache(const token_cache &) = default;
  virtual ~token_cache() = default;

  void add(token tok) {
    _seq.emplace_back(tok);
  }

  token operator[](size_t idx) {
    return _seq.at(idx);
  }

  size_t size() const {
    return _seq.size();
  }

  template <typename Fn> void for_all(Fn f) {
    for (auto &tok : _seq) {
      f(tok);
    }
  }

  std::string to_string() {
    return _src;
  }

private:
  std::string _src;
  std::vector<token> _seq;
};

} // namespace dwt

#endif
