// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_SYSCALL_OBJ_HPP
#define GUARD_DWT_SYSCALL_OBJ_HPP

#include <dwt/obj.hpp>
#include <dwt/stack.hpp>
#include <dwt/syscall.hpp>
#include <dwt/var.hpp>

#include <atomic>
#include <string>
#include <vector>

namespace dwt {

class string_obj;

class syscall_obj : public obj {
public:
  syscall_obj(syscall, string_obj *name);
  syscall_obj(const syscall_obj &);
  virtual ~syscall_obj();

  syscall &impl() {
    return _impl;
  }

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void mark_immutable() override;
  virtual void blacken() override;

  virtual std::string to_string() override;

private:
  syscall _impl;
  string_obj *_name;
};

} // namespace dwt

#endif
