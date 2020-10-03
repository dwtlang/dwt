// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_MAPFN_OBJ_HPP
#define GUARD_DWT_MAPFN_OBJ_HPP

#include <dwt/function_obj.hpp>
#include <dwt/string_obj.hpp>

namespace dwt {

class mapfn_obj : public function_obj {
public:
  mapfn_obj(string_obj *name);
  mapfn_obj(const mapfn_obj &);
  virtual ~mapfn_obj();

  virtual void call(interpreter &, int) override;
  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void mark_immutable() override;
  virtual void blacken() override;
  virtual std::string to_string() override;
};

} // namespace dwt

#endif
