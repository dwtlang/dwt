// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_MAP_OBJ_HPP
#define GUARD_DWT_MAP_OBJ_HPP

#include <dwt/hash_map.hpp>
#include <dwt/obj.hpp>

namespace dwt {

class map_obj : public obj {
public:
  map_obj();
  map_obj(const map_obj &);

  virtual ~map_obj();
  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void mark_immutable() override;
  virtual void blacken() override;
  virtual std::string to_string() override;

  virtual void op_keyset(var key, var val) override;
  virtual var op_keyget(var key) override;

protected:
  hash_map _map;
};

} // namespace dwt

#endif
