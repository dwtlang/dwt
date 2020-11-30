// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (c) 2020  Andrew Scott

#ifndef GUARD_DWT_IR_AST_HPP
#define GUARD_DWT_IR_AST_HPP

#include <atomic>
#include <dwt/scope.hpp>
#include <dwt/token_range.hpp>
#include <dwt/token_ref.hpp>

#include <memory>
#include <vector>

namespace dwt {

class token;
class token_range;

namespace ir {

class visitor;

class ast {
protected:
  ast();

public:
  ast(const ast &) = delete;
  ast(ast &&) = delete;
  ast &operator=(const ast &) = delete;
  ast &operator=(ast &&) = delete;

  virtual ~ast();

  virtual void accept(ir::visitor &visitor) = 0;
  void splice(ir::ast *ast);
  void set_name(token_ref tok);
  uint64_t id() const;
  std::vector<std::unique_ptr<ast>> &children_of();
  size_t nr_children() const;
  ir::ast *child_at(size_t i);
  ir::ast *parent_of();

  std::string qualified_name() {
    std::string qualified = get_scope()->qualified_name();

    if (name().size() > 0) {
      if (qualified.back() != ':') {
        qualified = qualified + "::" + name();
      } else {
        qualified = qualified + name();
      }
    }

    return qualified;
  }

  virtual std::string name();
  virtual token_ref name_tok();

  virtual void set_scope(scope *scope_ptr) {
    _scope = scope_ptr;
  }

  virtual dwt::scope *get_scope() {
    return _scope;
  }

  token_range get_token_range() const {
    return _token_range;
  }

  void set_token_range(token_range range) {
    _token_range = range;
  }

private:
  std::string _qualified_name;
  token_ref _name_token;
  token_range _token_range;
  scope *_scope = nullptr;
  ir::ast *_parent;
  std::vector<std::unique_ptr<ast>> _children;
  uint64_t _node_number;

  static std::atomic<uint64_t> node_number;
};

} // namespace ir
} // namespace dwt

#endif
