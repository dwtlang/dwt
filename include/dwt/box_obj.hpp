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

enum box_type { BOX_EMPTY, BOX_SHARED, BOX_RAW };

class box_obj : public obj {
public:
  box_obj(std::shared_ptr<void> boxed_obj);
  box_obj(void *boxed_obj);
  box_obj();
  box_obj(const box_obj &);
  virtual ~box_obj();

  void get_contents(std::shared_ptr<void> &);
  void get_contents(void *&);
  void set_contents(std::shared_ptr<void> &);
  void set_contents(void *&);

  box_type peek() const {
    return _type;
  }

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void blacken() override;
  virtual std::string to_string() override;

private:
  std::shared_ptr<void> _sp;
  void *_rp;
  box_type _type;
};

} // namespace dwt

#endif
