/***************************************************************************
 *   Copyright (C) 2007 by Jean-Philippe Dubus                             *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <string>

#include <cxxtest/TestSuite.h>

#include <agrum/core/bijection.h>

using namespace std;
using namespace gum;

class BijectionTestSuite: public CxxTest::TestSuite {
public:

  void test_basic() {
    Bijection<int, int> bijection;
    TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(1, 2));
    TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(3, 4));
    TS_GUM_ASSERT_THROWS_NOTHING(bijection.insert(5, 6));
    TS_ASSERT(bijection.first(2) == 1);
    TS_ASSERT(bijection.first(4) == 3);
    TS_ASSERT(bijection.first(6) == 5);
    TS_ASSERT(bijection.second(1) == 2);
    TS_ASSERT(bijection.second(3) == 4);
    TS_ASSERT(bijection.second(5) == 6);
    TS_ASSERT(bijection.existsFirst(1));
    TS_ASSERT(bijection.existsFirst(3));
    TS_ASSERT(bijection.existsFirst(5));
    TS_ASSERT(bijection.existsSecond(2));
    TS_ASSERT(bijection.existsSecond(4));
    TS_ASSERT(bijection.existsSecond(6));
    TS_ASSERT(bijection.size() == 3);
    TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseFirst(1));
    TS_ASSERT(! bijection.existsSecond(2));
    TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseFirst(3));
    TS_GUM_ASSERT_THROWS_NOTHING(bijection.eraseSecond(6));
    TS_ASSERT(bijection.empty());
  }

  void test_ph() {
    Bijection<int,int> carre;
    carre.insert( 1,1 );
    carre.insert( 2,4 );
    carre.insert( 3,9 );
    try {
      carre.insert(4, 1);
    } catch(DuplicateElement&) { }
    TS_ASSERT(! carre.existsFirst(4));

  }

protected:

private:

};

