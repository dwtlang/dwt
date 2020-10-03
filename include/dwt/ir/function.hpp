// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_IR_FUNCTION_HPP
#define GUARD_DWT_IR_FUNCTION_HPP

#include <dwt/ir/function_body.hpp>
#include <dwt/ir/parameters.hpp>
#include <dwt/token.hpp>
#include <vector>

namespace dwt {
namespace ir {

class function : public ast {
public:
  function(token_ref tok);
  function();
  virtual ~function();
  virtual void accept(ir::visitor &visitor);

  std::shared_ptr<ir::parameters> params() {
    return _params;
  }

  std::shared_ptr<ir::function_body> body() {
    return _body;
  }

  void params(std::shared_ptr<ir::parameters> params) {
    splice(params);
    _params = params;
  }

  void body(std::shared_ptr<ir::function_body> block) {
    splice(block);
    _body = block;
  }

private:
  std::shared_ptr<ir::parameters> _params;
  std::shared_ptr<ir::function_body> _body;
};

} // namespace ir
} // namespace dwt

#endif
