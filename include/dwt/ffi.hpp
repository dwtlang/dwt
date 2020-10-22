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

typedef std::function<var(size_t nr_args, var *args)> syscall;

var ffi_bind(std::string identifier, syscall);
var ffi_find(std::string identifier);
var ffi_call(std::string identifier, var *args, size_t nr_args);
var ffi_call(var callable, var *args, size_t nr_args);

inline var ffi_box(std::shared_ptr<void> opaque_obj) {
  return to_var(opaque_obj);
}

std::shared_ptr<void> ffi_unbox(var box);

} // namespace dwt

#endif
