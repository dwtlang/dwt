// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_INSTANCE_OBJ_HPP
#define GUARD_DWT_INSTANCE_OBJ_HPP

#include <dwt/class_obj.hpp>
#include <dwt/closure_obj.hpp>
#include <dwt/map_obj.hpp>
#include <dwt/obj.hpp>

#include <vector>

namespace dwt {

class instance_obj : public map_obj {
public:
  instance_obj(class_obj *);
  instance_obj(const instance_obj &);
  virtual ~instance_obj();

  class_obj *klass() const {
    return _klass;
  }

  void super(instance_obj *);
  instance_obj *super() const;

  inline void add(string_obj *str, var v) {
    op_keyset(OBJ_AS_VAR(str), v);
  }

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void blacken() override;
  virtual void call(interpreter &, int) override;
  virtual std::string to_string() override;
  virtual var op_mbrget(var) override;
  virtual void op_mbrset(var, var) override;

private:
  instance_obj *_super;
  class_obj *_klass;
};

} // namespace dwt

#endif
