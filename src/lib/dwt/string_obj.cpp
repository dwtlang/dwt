// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// SPDX-License-Identifier: MPL-2.0
//
// Copyright (C) 2020-2021 Andrew Scott-Jones and Contributors

#include <dwt/interpret_exception.hpp>
#include <dwt/string_mgr.hpp>
#include <dwt/string_obj.hpp>
#include <dwt/utf8.hpp>

namespace dwt {

string_obj::string_obj(std::string text)
  : _text(text) {
}

string_obj::string_obj() {
}

string_obj::~string_obj() {
}

obj_type string_obj::type() {
  return OBJ_STRING;
}

obj *string_obj::clone() {
  return string_mgr::get().add(to_string());
}

hash_t string_obj::hash() {
  return fnv1a(_text.c_str(), _text.size());
}

std::string string_obj::printable_string() {
  return _text;
}

std::string string_obj::to_string() {
  return _text;
}

size_t string_obj::length() {
  return utf8_strlen(text());
}

var string_obj::op_mul(var v, bool rhs) {
  if (VAR_IS_NUM(v)) {
    if (rhs) {
      throw interpret_exception("e@1 cannot multiply something by a string");
    } else if (VAR_AS_INT(v) != VAR_AS_NUM(v)) {
      throw interpret_exception(
        "e@1 a string can only be multiplied by an integer");
    } else {
      std::string s;
      double n = VAR_AS_INT(v);
      while (n--) {
        s += text();
      }
      return OBJ_AS_VAR(string_mgr::get().add(s));
    }
  } else {
    throw interpret_exception(
      "e@1 a string can only be multiplied by an integer");
  }

  return nil;
}

var string_obj::op_add(var v, bool rhs) {
  string_obj *s = nullptr;

  if (VAR_IS_OBJ(v)) {
    if (VAR_AS_OBJ(v)->type() == OBJ_STRING) {
      s = static_cast<string_obj *>(VAR_AS_OBJ(v));
    }
  }

  if (s) {
    if (rhs) {
      s = string_mgr::get().add(s->text() + text());
    } else {
      s = string_mgr::get().add(text() + s->text());
    }
  } else {
    throw interpret_exception("e@1 invalid operands");
  }

  if (s) {
    return OBJ_AS_VAR(s);
  } else {
    return nil;
  }
}

namespace {

dwt::string_obj *remove_substr(std::string original, std::string pattern) {
  size_t pos = std::string::npos;
  std::string s = original;

  while ((pos = s.find(pattern)) != std::string::npos) {
    s.erase(pos, pattern.length());
  }

  dwt::string_obj *obj = string_mgr::get().add(s);

  return obj;
}

} // namespace

var string_obj::op_sub(var v, bool rhs) {
  string_obj *s = nullptr;

  if (VAR_IS_OBJ(v)) {
    if (VAR_AS_OBJ(v)->type() == OBJ_STRING) {
      s = static_cast<string_obj *>(VAR_AS_OBJ(v));
    }
  }

  if (s) {
    if (rhs) {
      s = remove_substr(s->text(), text());
    } else {
      s = remove_substr(text(), s->text());
    }
  } else {
    throw interpret_exception("e@1 invalid operands");
  }

  if (s) {
    return OBJ_AS_VAR(s);
  } else {
    return nil;
  }
}

} // namespace dwt
