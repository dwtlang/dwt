// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_SEMAPHORE_OBJ_HPP
#define GUARD_DWT_SEMAPHORE_OBJ_HPP

#include <dwt/obj.hpp>

#include <condition_variable>
#include <string>

namespace dwt {

class semaphore_obj : public obj {
public:
  semaphore_obj(size_t initial_count = 0);
  virtual ~semaphore_obj();

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void mark_immutable() override;
  virtual void blacken() override;
  virtual std::string to_string() override;

  void up(size_t count);
  void down();
  size_t count();

private:
  size_t _count;
  std::mutex _mutex;
  std::condition_variable _condvar;
};

} // namespace dwt

#endif
