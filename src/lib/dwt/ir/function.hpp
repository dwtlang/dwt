// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020-2021 Andrew Scott and Contributors

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

  ir::parameters *params() {
    return _params;
  }

  ir::function_body *body() {
    return _body;
  }

  void params(std::unique_ptr<ir::parameters> params) {
    _params = params.get();
    splice(std::move(params));
  }

  void body(std::unique_ptr<ir::function_body> block) {
    _body = block.get();
    splice(std::move(block));
  }

private:
  ir::parameters *_params = nullptr;
  ir::function_body *_body = nullptr;
};

} // namespace ir
} // namespace dwt

#endif
