// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_BOX_OBJ_HPP
#define GUARD_DWT_BOX_OBJ_HPP

#include <dwt/obj.hpp>
#include <dwt/var.hpp>

#include <memory>

namespace dwt {

class box_obj : public obj {
public:
  box_obj(std::shared_ptr<void> boxed_obj);
  box_obj(const box_obj &);
  virtual ~box_obj();

  std::shared_ptr<void> contents();

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void mark_immutable() override;
  virtual void blacken() override;
  virtual std::string to_string() override;

private:
  std::shared_ptr<void> _boxed_obj;
};

} // namespace dwt

#endif
