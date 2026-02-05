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


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/refPtr.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  RefPtr
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct RefPtrTestSuite {
    public:
    static void testConstructors() {
      gum::RefPtr< int > ptr1(new int(4));

      CHECK(ptr1);

      gum::RefPtr< int > ptr2 = ptr1, ptr3;

      CHECK((ptr1) == (ptr2));

      CHECK((ptr1) != (ptr3));

      ptr3 = ptr1;

      CHECK((ptr1) == (ptr3));
    }

    static void testModify() {
      gum::RefPtr< int > ptr1(new int(4));
      gum::RefPtr< int > ptr2 = ptr1, ptr3;
      ptr3                    = ptr1;

      CHECK((ptr1) == (ptr2));

      ptr2.clear();

      CHECK((ptr1) != (ptr2));

      *ptr1 = 5;

      CHECK((*ptr1) == (5));

      ptr1 = 0;

      CHECK((ptr1.refCount()) == (0U));

      ptr2 = ptr1;

      CHECK((ptr2.refCount()) == (0U));
    }

    struct toto {
      int xxx;
      toto() : xxx(4) {};

      int getY() { return xxx; }
    };

    struct titi: public toto {
      int yyy;
      titi() : yyy(3) {};

      int getY() { return yyy; }
    };

    static void testDowncast() {
      gum::RefPtr< titi > ptr1(new titi);

      CHECK((ptr1->getY()) == (3));

      gum::RefPtr< toto > ptr2(ptr1);

      CHECK((ptr1->getY()) != (ptr2->getY()));
    }
  };

  GUM_TEST_ACTIF(Constructors)
  GUM_TEST_ACTIF(Modify)
  GUM_TEST_ACTIF(Downcast)
}   // namespace gum_tests
