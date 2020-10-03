// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_CONDVAR_OBJ_HPP
#define GUARD_DWT_CONDVAR_OBJ_HPP

#include <dwt/condvar.hpp>
#include <dwt/obj.hpp>

namespace dwt {

class condvar_obj : public obj {
public:
  condvar_obj();
  virtual ~condvar_obj();

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void mark_immutable() override;
  virtual void blacken() override;
  virtual std::string to_string() override;

  void signal_all() {
    _condvar.signal_all();
  }

  void signal() {
    _condvar.signal();
  }

  void await() {
    _condvar.await();
  }

private:
  condvar _condvar;
};

} // namespace dwt

#endif
