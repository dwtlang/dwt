// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_STRING_MGR_HPP
#define GUARD_DWT_STRING_MGR_HPP

#include <dwt/hash_map.hpp>
#include <dwt/string_obj.hpp>

#include <mutex>

namespace dwt {

class string_mgr : public hash_map {
private:
  string_mgr();
  virtual ~string_mgr();

  std::mutex _add_mutex;
  std::mutex _get_mutex;

public:
  static string_mgr &get_global();
  static string_mgr &get();

  void merge(string_mgr &);

  string_obj *add(std::string);
  string_obj *get(std::string);
  string_obj *get(std::string &, hash_t);

  void sweep();
};

} // namespace dwt

#endif
