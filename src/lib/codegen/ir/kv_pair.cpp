// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#include <dwt/ir/kv_pair.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::kv_pair::kv_pair(expr *k, expr *v) {
  splice(k);
  splice(v);
}

ir::kv_pair::~kv_pair() {
}

void ir::kv_pair::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
