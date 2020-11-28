// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_ITERATOR_OBJ_HPP
#define GUARD_DWT_ITERATOR_OBJ_HPP

#include <dwt/obj.hpp>

namespace dwt {

class iterator_obj : public obj {
public:
  iterator_obj();
  iterator_obj(const iterator_obj &);
  ~iterator_obj();

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void blacken() override;
  virtual std::string to_string() override;

private:
};

} // namespace dwt

#endif
