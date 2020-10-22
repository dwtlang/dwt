// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_FUNCTION_OBJ_HPP
#define GUARD_DWT_FUNCTION_OBJ_HPP

#include <dwt/code_obj.hpp>
#include <dwt/function_type.hpp>
#include <dwt/local_var.hpp>
#include <dwt/obj.hpp>
#include <dwt/string_obj.hpp>
#include <dwt/upvar.hpp>

#include <vector>

namespace dwt {

class function_obj : public obj {
public:
  function_obj(function_type, size_t, string_obj *);
  function_obj(const function_obj &);
  virtual ~function_obj();

  virtual obj_type type() override;
  virtual obj *clone() override;
  virtual void mark_immutable() override;

  size_t arity() {
    return _arity;
  }

  inline code_obj &code() {
    return *_code;
  }

  std::string name() {
    return _name->text();
  }

  string_obj *short_name() {
    return _short_name;
  }

  function_type type() const {
    return _type;
  }

  void set_patchpoint(size_t patchpoint) {
    _patchpoint = patchpoint;
  }

  size_t get_patchpoint() const {
    return _patchpoint;
  }

  std::vector<local_var> &locals() {
    return _locals;
  }

  std::vector<upvar> &upvars() {
    return _upvars;
  }

  void add_local(local_var v) {
    _locals.push_back(v);
  }

  int add_upvar(size_t, bool);

  virtual void call(interpreter &, int) override;

  virtual void blacken() override;

  virtual std::string to_string() override;

  void optimised(bool tog) {
    _optimised = tog;
  }

  bool optimised() const {
    return _optimised;
  }

  void is_api(bool toggle) {
    _is_api = toggle;
  }

  bool is_api() const {
    return _is_api;
  }

private:
  function_type _type;
  size_t _arity;
  std::vector<local_var> _locals;
  std::vector<upvar> _upvars;
  code_obj *_code;

protected:
  string_obj *_short_name;
  string_obj *_name;

private:
  size_t _patchpoint = 0;
  bool _optimised = false;
  bool _is_api = false;
};

} // namespace dwt

#endif
