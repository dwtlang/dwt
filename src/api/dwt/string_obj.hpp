// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_STRING_OBJ_HPP
#define GUARD_DWT_STRING_OBJ_HPP

#include <dwt/obj.hpp>

#include <string>

namespace dwt {

class string_obj : public obj {
  friend class string_mgr;

public:
  virtual ~string_obj();

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual hash_t hash() override;
  virtual size_t length() override;

  virtual var op_add(var v, bool rhs = false) override;
  virtual var op_sub(var v, bool rhs = false) override;
  virtual var op_mul(var v, bool rhs = false) override;

  std::string &text() {
    return _text;
  }

  bool operator==(const string_obj &other) const {
    return this->_text == other._text;
  }

  virtual std::string printable_string() override;
  virtual std::string to_string() override;

private:
  string_obj(std::string);
  string_obj();
  string_obj(const string_obj &) = delete;
  std::string _text;
};

} // namespace dwt

#endif
