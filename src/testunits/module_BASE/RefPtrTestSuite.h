/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/core/refPtr.h>

namespace gum_tests {

  class [[maybe_unused]] RefPtrTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(Constructors) {
      gum::RefPtr< int > ptr1(new int(4));

      TS_ASSERT(ptr1)

      gum::RefPtr< int > ptr2 = ptr1, ptr3;

      TS_ASSERT_EQUALS(ptr1, ptr2)

      TS_ASSERT_DIFFERS(ptr1, ptr3)

      ptr3 = ptr1;

      TS_ASSERT_EQUALS(ptr1, ptr3)
    }

    GUM_ACTIVE_TEST(Modify) {
      gum::RefPtr< int > ptr1(new int(4));
      gum::RefPtr< int > ptr2 = ptr1, ptr3;
      ptr3                    = ptr1;

      TS_ASSERT_EQUALS(ptr1, ptr2)

      ptr2.clear();

      TS_ASSERT_DIFFERS(ptr1, ptr2)

      *ptr1 = 5;

      TS_ASSERT_EQUALS(*ptr1, 5)

      ptr1 = 0;

      TS_ASSERT_EQUALS(ptr1.refCount(), 0U)

      ptr2 = ptr1;

      TS_ASSERT_EQUALS(ptr2.refCount(), 0U)
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

    GUM_ACTIVE_TEST(Downcast) {
      gum::RefPtr< titi > ptr1(new titi);

      TS_ASSERT_EQUALS(ptr1->getY(), 3)

      gum::RefPtr< toto > ptr2(ptr1);

      TS_ASSERT_DIFFERS(ptr1->getY(), ptr2->getY())
    }
  };
}   // namespace gum_tests
