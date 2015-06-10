/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/refPtr.h>

namespace gum_tests {

  class RefPtrTestSuite : public CxxTest::TestSuite {
    public:
    void testConstructors() {
      gum::RefPtr<int> ptr1(new int(4));

      TS_ASSERT(ptr1);

      gum::RefPtr<int> ptr2 = ptr1, ptr3;

      TS_ASSERT_EQUALS(ptr1, ptr2);

      TS_ASSERT_DIFFERS(ptr1, ptr3);

      ptr3 = ptr1;

      TS_ASSERT_EQUALS(ptr1, ptr3);
    }

    void testModify() {
      gum::RefPtr<int> ptr1(new int(4));
      gum::RefPtr<int> ptr2 = ptr1, ptr3;
      ptr3 = ptr1;

      TS_ASSERT_EQUALS(ptr1, ptr2);

      ptr2.clear();

      TS_ASSERT_DIFFERS(ptr1, ptr2);

      *ptr1 = 5;

      TS_ASSERT_EQUALS(*ptr1, 5);

      ptr1 = 0;

      TS_ASSERT_EQUALS(ptr1.refCount(), 0U);

      ptr2 = ptr1;

      TS_ASSERT_EQUALS(ptr2.refCount(), 0U);
    }

    struct toto {
      int xxx;
      toto() : xxx(4){};

      int getY() { return xxx; }
    };

    struct titi : public toto {
      int yyy;
      titi() : yyy(3){};

      int getY() { return yyy; }
    };

    void testDowncast() {
      gum::RefPtr<titi> ptr1(new titi);

      TS_ASSERT_EQUALS(ptr1->getY(), 3);

      gum::RefPtr<toto> ptr2(ptr1);

      TS_ASSERT_DIFFERS(ptr1->getY(), ptr2->getY());
    }
  };
}
