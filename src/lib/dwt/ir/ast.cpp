// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/ir/ast.hpp>
#include <dwt/ir/visitor.hpp>
#include <dwt/token.hpp>

#include <cassert>

namespace dwt {
namespace ir {

std::atomic<uint64_t> ast::node_number = 0;

ast::ast()
  : _parent(nullptr)
  , _node_number(++node_number) {
  set_scope(dwt::scope::current);
}

ast::~ast() {
}

void ast::set_name(token_ref name_token) {
  _name_token = name_token;
}

uint64_t ast::id() const {
  return _node_number;
}

std::string ast::name() {
  return _name_token.text();
}

token_ref ast::name_tok() {
  return _name_token;
}

ast *ast::parent_of() {
  return _parent;
}

void ast::splice(std::unique_ptr<ast> obj) {
  obj->_parent = this;
  _children.emplace_back(std::move(obj));
}

void ast::splice(ast *obj) {
  obj->_parent = this;
  _children.emplace_back(std::unique_ptr<ast>(obj));
}

size_t ast::nr_children() const {
  return _children.size();
}

std::vector<std::unique_ptr<ast>> &ast::children_of() {
  return _children;
}

ast *ast::child_at(size_t idx) {
  std::unique_ptr<ast> &child = _children.at(idx);

  return child.get();
}

} // namespace ir
} // namespace dwt
