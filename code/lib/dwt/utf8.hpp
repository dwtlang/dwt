// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_UTF8_HPP
#define GUARD_DWT_UTF8_HPP

#include <cstddef>
#include <cstdint>
#include <cwchar>
#include <string>

namespace dwt {

size_t utf8_strlen(std::string &str);

int utf8_decode(uint8_t *code_point,
                unsigned int bytes_remaining,
                int *nr_codes = nullptr);

std::string utf8_encode(int char_code);

} // namespace dwt

#endif
