// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_CLOSURE_OBJ_HPP
#define GUARD_DWT_CLOSURE_OBJ_HPP

#include <dwt/function_obj.hpp>
#include <dwt/upvar_obj.hpp>

#include <vector>

namespace dwt {

class function_obj;
class upvar_obj;

class closure_obj : public obj {
public:
  closure_obj(function_obj *);
  closure_obj(const closure_obj &);
  virtual ~closure_obj();

  function_obj *fun_obj() const {
    return _fun_obj;
  }

  std::vector<upvar_obj *> &upvars() {
    return _upvars;
  }

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void blacken() override;
  virtual void call(interpreter &, int) override;
  virtual std::string to_string() override;

private:
  function_obj *_fun_obj;
  std::vector<upvar_obj *> _upvars;
};

} // namespace dwt

#endif
