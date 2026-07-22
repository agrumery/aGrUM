/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once

#include <optional>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/hashTable.h>

#include <agrum/base/core/optional_ref.h>
#include <type_traits>

namespace gum_tests {

  struct OptionalRefTestSuite {
    public:
    // ========================================================================
    // C++26 guard: if std::optional<T&> becomes valid, this class loses its
    // raison d'etre and should be replaced by std::optional<T&>.
    // ========================================================================


    // ========================================================================
    // Construction
    // ========================================================================


    // ========================================================================
    // Observers
    // ========================================================================


    // ========================================================================
    // Reference semantics: the optional_ref does NOT own a copy
    // ========================================================================


    // ========================================================================
    // Copy/assign semantics
    // ========================================================================


    // ========================================================================
    // Use with if-init-statement (C++17 pattern)
    // ========================================================================


    // ========================================================================
    // Use with HashTable::tryGet
    // ========================================================================
  };

  GUM_TEST(Cpp26Guard) {
#if __cplusplus > 202302L && __cpp_lib_optional >= 202406L
    // C++26 supports std::optional<T&>.
    // gum::optional_ref<T> should be replaced by std::optional<T&>.
    WARN("C++26 std::optional<T&> is available: gum::optional_ref<T> is now redundant.");

    // Verify that std::optional<T&> actually works as expected
    int                   x = 42;
    std::optional< int& > opt(x);
    CHECK(opt.has_value());
    CHECK(*opt == 42);
    x = 99;
    CHECK(*opt == 99);
#else
    // std::optional<T&> is NOT supported: gum::optional_ref<T> is needed.
    CHECK(true);
#endif
  }

  GUM_TEST(DefaultConstruction) {
    gum::optional_ref< int > empty;
    CHECK(!empty);
    CHECK(!empty.has_value());
  }

  GUM_TEST(NulloptConstruction) {
    gum::optional_ref< int > empty(std::nullopt);
    CHECK(!empty);
    CHECK(!empty.has_value());
  }

  GUM_TEST(LvalueConstruction) {
    int                      x = 42;
    gum::optional_ref< int > ref(x);
    CHECK(ref.has_value());
    CHECK(static_cast< bool >(ref));
    CHECK(*ref == 42);
  }

  GUM_TEST(RvalueDeleted) {
    // gum::optional_ref<int>(42) should not compile.
    // We verify the delete via type traits.
    CHECK(!(std::is_constructible< gum::optional_ref< int >, int&& >::value));
    CHECK(!(std::is_constructible< gum::optional_ref< std::string >, std::string&& >::value));
  }

  GUM_TEST(ConstRef) {
    const int                      x = 7;
    gum::optional_ref< const int > ref(x);
    CHECK(ref.has_value());
    CHECK(*ref == 7);
  }

  GUM_TEST(OperatorBool) {
    gum::optional_ref< int > empty;
    CHECK(!empty);

    int                      x = 1;
    gum::optional_ref< int > full(x);
    CHECK(static_cast< bool >(full));

    // explicit: must not participate in implicit conversions
    CHECK(!(std::is_convertible< gum::optional_ref< int >, bool >::value));
  }

  GUM_TEST(HasValue) {
    gum::optional_ref< double > empty;
    CHECK(!empty.has_value());

    double                      d = 3.14;
    gum::optional_ref< double > full(d);
    CHECK(full.has_value());
  }

  GUM_TEST(ValueSuccess) {
    int                      x = 42;
    gum::optional_ref< int > ref(x);
    CHECK(ref.value() == 42);
  }

  GUM_TEST(ValueThrows) {
    gum::optional_ref< int > empty;
    CHECK_THROWS_AS(empty.value(), const std::bad_optional_access&);
  }

  GUM_TEST(Dereference) {
    int                      x = 10;
    gum::optional_ref< int > ref(x);
    CHECK(*ref == 10);

    // Modification through the reference
    *ref = 20;
    CHECK(x == 20);
  }

  GUM_TEST(Arrow) {
    std::string                      s = "hello";
    gum::optional_ref< std::string > ref(s);
    CHECK(ref->size() == 5);
    ref->append(" world");
    CHECK(s == "hello world");
  }

  GUM_TEST(ReferenceSemantics) {
    int                      x = 1;
    gum::optional_ref< int > ref(x);
    CHECK(*ref == 1);

    x = 999;
    CHECK(*ref == 999);

    *ref = -1;
    CHECK(x == -1);
  }

  GUM_TEST(ConstReferenceSemantics) {
    int                            x = 5;
    gum::optional_ref< const int > ref(x);
    CHECK(*ref == 5);

    x = 100;
    CHECK(*ref == 100);

    // *ref = 200; // should not compile (const)
    CHECK((std::is_same< decltype(*ref), const int& >::value));
  }

  GUM_TEST(Copy) {
    int                      x = 42;
    gum::optional_ref< int > ref1(x);
    gum::optional_ref< int > ref2(ref1);

    CHECK(ref2.has_value());
    CHECK(*ref2 == 42);
    CHECK(&(*ref1) == &(*ref2));
  }

  GUM_TEST(Assignment) {
    int                      x = 1, y = 2;
    gum::optional_ref< int > ref(x);
    CHECK(*ref == 1);

    gum::optional_ref< int > ref2(y);
    ref = ref2;
    CHECK(*ref == 2);
    CHECK(&(*ref) == &y);
  }

  GUM_TEST(AssignFromEmpty) {
    int                      x = 1;
    gum::optional_ref< int > ref(x);
    CHECK(ref.has_value());

    gum::optional_ref< int > empty;
    ref = empty;
    CHECK(!ref.has_value());
  }

  GUM_TEST(IfInitStatement) {
    gum::HashTable< std::string, int > ht;
    ht.insert("a", 1);
    ht.insert("b", 2);

    if (auto p = ht.tryGet("a")) {
      CHECK(*p == 1);
    } else {
      FAIL("should have found key 'a'");
    }

    if (auto p = ht.tryGet("z")) {
      FAIL("should NOT have found key 'z'");
    } else {
      CHECK(true);
    }
  }

  GUM_TEST(WithHashTable) {
    gum::HashTable< int, std::string > ht;
    ht.insert(1, "one");
    ht.insert(2, "two");

    auto found = ht.tryGet(1);
    CHECK(found.has_value());
    CHECK(*found == "one");

    auto notfound = ht.tryGet(42);
    CHECK(!notfound);

    // Mutable access
    if (auto p = ht.tryGet(2)) { *p = "deux"; }
    CHECK(ht[2] == "deux");

    // Const access
    const auto& cht    = ht;
    auto        cfound = cht.tryGet(1);
    CHECK(cfound.has_value());
    CHECK(*cfound == "one");
    CHECK((std::is_same< decltype(*cfound), const std::string& >::value));
  }

  GUM_TEST(DerefEmptyThrows) {
    gum::optional_ref< int > empty;
    CHECK_THROWS_AS(*empty, const std::bad_optional_access&);

    gum::optional_ref< std::string > emptyStr;
    CHECK_THROWS_AS(*emptyStr, const std::bad_optional_access&);
  }
}   // namespace gum_tests
