/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/scores_and_tests/cache4PartEntropy.h>

namespace gum_tests {

  class Cache4PartEntropyTestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      gum::learning::Cache4PartEntropy cache;

      std::vector< gum::Idx > v1{1, 6, 2};
      std::vector< gum::Idx > v2{1, 2, 6};
      std::vector< gum::Idx > v3;
      std::vector< gum::Idx > v4{1};

      cache.insert(v1, 1.5);
      TS_ASSERT_THROWS(cache.insert(v1, 2.5), gum::DuplicateElement);
      TS_ASSERT_THROWS(cache.insert(v2, 3.5), gum::DuplicateElement);
      TS_GUM_ASSERT_THROWS_NOTHING(cache.insert(v3, 4.5));
      // cache.insert( v3, 5 );
      // cache.insert( v4, 7 );

      TS_ASSERT_EQUALS(cache.score(v1), 1.5);
      // TS_ASSERT( cache.score( v1 ) == 4.5 );
      TS_ASSERT_THROWS(cache.score(v4), gum::NotFound);
      TS_ASSERT_EQUALS(cache.score(v3), 4.5);
      // TS_ASSERT( cache.score( v4 ) == 7 );

      cache.clear();
      TS_ASSERT_THROWS(cache.score(v2), gum::NotFound);
      TS_ASSERT_THROWS(cache.score(v3), gum::NotFound);

      cache.insert(v1, 1.5);
      TS_ASSERT_THROWS(cache.insert(v1, 2.5), gum::DuplicateElement);
      TS_ASSERT_THROWS(cache.insert(v2, 3.5), gum::DuplicateElement);
      TS_GUM_ASSERT_THROWS_NOTHING(cache.insert(v3, 4.5));
      // cache.insert( v4, 7 );

      TS_ASSERT_EQUALS(cache.score(v2), 1.5);
      TS_ASSERT_EQUALS(cache.score(v1), 1.5);
      TS_ASSERT_THROWS(cache.score(v4), gum::NotFound);
      TS_ASSERT_EQUALS(cache.score(v3), 4.5);
      // TS_ASSERT( cache.score( v4 ) == 7 );
    }
  };

} /* namespace gum_tests */
