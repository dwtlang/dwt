// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_FFI_HPP
#define GUARD_DWT_FFI_HPP

#include <dwt/var.hpp>

#include <functional>

namespace dwt {
namespace ffi {

typedef std::function<var(size_t nr_args, var *args)> syscall;

var bind(std::string identifier, syscall);
var find(std::string identifier);
var call(std::string identifier, var *args, size_t nr_args);
var call(var callable, var *args, size_t nr_args);

template <typename T> var any(T t) {
  return to_var(std::shared_ptr<void>(new T(t)));
}

inline var box(std::shared_ptr<void> opaque_obj) {
  return to_var(opaque_obj);
}

std::shared_ptr<void> unbox(var box);

} // namespace ffi
} // namespace dwt

#endif
