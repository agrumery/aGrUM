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


#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/labelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  LabelizedVariable
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct LabelizedVariableTestSuite {
    public:
    static void testCopy() {
      gum::LabelizedVariable var1("var1", "this is var1", 2);
      gum::LabelizedVariable var2("var2", "this is var2", 2);

      gum::LabelizedVariable var3(var1);
      gum::LabelizedVariable var4("var4", "this is var4");
      GUM_CHECK_ASSERT_THROWS_NOTHING(var4 = var2);

      CHECK((var4) == (var2));
      CHECK((var1) == (var3));
      CHECK((var4) != (var1));
    }   // namespace gum_tests

    static void testLabels() {
      gum::LabelizedVariable var1("var1", "this is var1", 0);
      CHECK((var1.domainSize()) == (static_cast< gum::Size >(0)));
      CHECK(var1.empty());
      var1.addLabel("4").addLabel("3").addLabel("2").addLabel("1");

      CHECK((var1.domainSize()) == (static_cast< gum::Size >(4)));
      CHECK((var1.label(1)) == ("3"));
      CHECK((var1["3"]) == (static_cast< gum::Idx >(1)));

      CHECK_THROWS_AS(var1.addLabel("3"), const gum::DuplicateElement&);

      std::stringstream s;
      s << var1;
      CHECK((s.str()) == ("var1:Labelized({4|3|2|1})"));

      CHECK((var1.toString()) == ("var1:Labelized({4|3|2|1})"));
    }

    static void testChangeLabel() {
      gum::LabelizedVariable var1("var1", "this is var1", 0);
      var1.addLabel("4").addLabel("3").addLabel("2").addLabel("1");

      CHECK((var1.toString()) == ("var1:Labelized({4|3|2|1})"));

      var1.changeLabel(1, "x");
      CHECK((var1.toString()) == ("var1:Labelized({4|x|2|1})"));

      const gum::LabelizedVariable& var2 = var1;
      CHECK((var2.toString()) == ("var1:Labelized({4|x|2|1})"));
      var2.changeLabel(1, "y");
      CHECK((var2.toString()) == ("var1:Labelized({4|y|2|1})"));

      GUM_CHECK_ASSERT_THROWS_NOTHING(
          var1.changeLabel(1, "x"));   // should be OK since label 1 is already "x"
      CHECK_THROWS_AS(var1.changeLabel(0, "x"), const gum::DuplicateElement&);
      CHECK_THROWS_AS(var1.changeLabel(1000, "x"), const gum::OutOfBounds&);
    }

    static void testNumerical() {
      gum::LabelizedVariable var1("var1", "this is var1", 0);
      var1.addLabel("4").addLabel("3").addLabel("2").addLabel("1");

      CHECK((var1.numerical(0)) == (0));
      CHECK((var1.numerical(1)) == (1));
      CHECK((var1.numerical(2)) == (2));
      CHECK((var1.numerical(3)) == (3));
    }

    static void testAndConstructorWithLabels() {
      gum::LabelizedVariable var1("var1", "this is var1", {"rouge", "vert", "bleu"});
      CHECK((var1.toString()) == ("var1:Labelized({rouge|vert|bleu})"));
      CHECK((var1.posLabel("vert")) == (gum::Idx(1)));
    }
  };

  GUM_TEST_ACTIF(Copy)
  GUM_TEST_ACTIF(Labels)
  GUM_TEST_ACTIF(ChangeLabel)
  GUM_TEST_ACTIF(Numerical)
  GUM_TEST_ACTIF(AndConstructorWithLabels)
}   // namespace gum_tests
