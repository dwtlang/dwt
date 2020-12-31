// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

#include <dwt/ir/string_spec.hpp>
#include <dwt/ir/visitor.hpp>

namespace dwt {

ir::string_spec::string_spec(token_ref tok) {
  auto text = tok.text();

  if (text.size() > 2) {
    _text = text.substr(1, text.size() - 2);
  }

  set_name(tok);
}

ir::string_spec::string_spec() {
}

ir::string_spec::~string_spec() {
}

void ir::string_spec::accept(ir::visitor &visitor) {
  visitor.visit(*this);
}

} // namespace dwt
