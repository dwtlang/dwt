// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#ifndef GUARD_DWT_STRING_MGR_HPP
#define GUARD_DWT_STRING_MGR_HPP

#include <dwt/hash_map.hpp>
#include <dwt/string_obj.hpp>

#if USE_THREADED_COMPILER
#include <mutex>
#endif

namespace dwt {

class string_mgr : public hash_map {
private:
  string_mgr();
  virtual ~string_mgr();

#if USE_THREADED_COMPILER
  std::mutex _mutex;
#endif

public:
  static string_mgr &get();

  string_obj *add_r(std::string);
  string_obj *get_r(std::string);
  string_obj *get_r(std::string &, hash_t);
  string_obj *add(std::string);
  string_obj *get(std::string);
  string_obj *get(std::string &, hash_t);

  void sweep();
};

} // namespace dwt

#endif
