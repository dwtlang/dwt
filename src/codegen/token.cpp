// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/token.hpp>

dwt::token::token(dwt::token_type s, std::string text, int line, int col)
  : sym(s)
  , _text(text)
  , ln(line)
  , col(col) {
}

dwt::token::~token() {
}
