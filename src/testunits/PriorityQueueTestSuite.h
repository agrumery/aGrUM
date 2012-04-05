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
#include <string>
#include <agrum/core/priorityQueue.h>
#include <agrum/core/set.h>
#include <agrum/core/hashFunc.h>

namespace gum {
  template <>
  class HashFunc< gum::Set<int> > : public HashFuncBase< gum::Set<int> > {
    public:
      /// computes the hashed value of a key
      Size operator()( const gum::Set<int>& key ) const  {
        return ( ( key.size() * GUM_HASHTABLE_INT_GOLD ) & _hash_mask );
      }
  };
}

namespace gum_tests {

  class PriorityQueueTestSuite: public CxxTest::TestSuite {
    public:

      void testConstruct() {
        gum::PriorityQueue<std::string> queue1;
        TS_ASSERT( queue1.size() == 0 );
        TS_ASSERT( queue1.empty() == true );
        TS_ASSERT( queue1.contains( "ZZZ" ) == false );
        TS_ASSERT_THROWS( queue1.top(), gum::NotFound );
        TS_ASSERT_THROWS( queue1.pop(), gum::NotFound );
        TS_ASSERT_THROWS( queue1.topPriority(), gum::NotFound );

        queue1.insert( 8,  "AAA" );
        queue1.insert( 10, "BBB" );
        queue1.insert( 2,  "CCC" );
        queue1.insert( 23, "DDD" );
        queue1.insert( 24, "EEE" );
        queue1.insert( 10, "AAA" );

        TS_ASSERT( queue1.size() == 6 );
        TS_ASSERT( queue1.empty() == false );
        TS_ASSERT( queue1.contains( "AAA" ) == true );
        TS_ASSERT( queue1.contains( "ZZZ" ) == false );

        const std::string& str = queue1.top();
        TS_ASSERT( str == "CCC" );
        TS_ASSERT( queue1.topPriority() == 2 );

        std::string str2 = queue1.pop();
        TS_ASSERT( str2 == "CCC" );
        TS_ASSERT( queue1.top() == "AAA" );
        TS_ASSERT( queue1.topPriority() == 8 );

        queue1.setPriority( 0, 4 );
        TS_ASSERT( queue1.top() == "AAA" );
        TS_ASSERT( queue1.topPriority() == 4 );
        TS_ASSERT_THROWS( queue1.setPriority( 10, 4 ) ,gum::NotFound );

        queue1.setPriorityByVal( "BBB", 2 );
        TS_ASSERT( queue1.top() == "BBB" );
        TS_ASSERT( queue1.topPriority() == 2 );
        TS_GUM_ASSERT_THROWS_NOTHING( queue1.setPriorityByVal( "zzz", 4 ) );

        TS_ASSERT_THROWS( queue1.priorityByVal( "zzz" ), gum::NotFound );
        TS_ASSERT( queue1.priorityByVal( "BBB" ) == 2 );

        queue1.resize( 10 );
        TS_ASSERT( queue1.capacity() == 10 );

        TS_ASSERT( queue1.uniquenessPolicy() == false );
        queue1.setUniquenessPolicy( true );
        TS_ASSERT( queue1.uniquenessPolicy() == true );
        TS_ASSERT_THROWS( queue1.insert( 10, "AAA" ), gum::DuplicateElement );

        const gum::HashTable< std::string,std::vector<gum::Size> >&
        vals = queue1.allValues();
        TS_ASSERT( vals.size() == 4 );
      }

      void testCopy() {
        gum::PriorityQueue<std::string> queue1;
        queue1.insert( 8,  "AAA" );
        queue1.insert( 10, "BBB" );
        queue1.insert( 2,  "CCC" );
        queue1.insert( 23, "DDD" );
        queue1.insert( 24, "EEE" );
        queue1.insert( 10, "AAA" );

        gum::PriorityQueue<std::string> queue2 = queue1;
        TS_ASSERT( queue2.size() == 6 );
        TS_ASSERT( queue2.empty() == false );
        TS_ASSERT( queue2.contains( "AAA" ) == true );
        TS_ASSERT( queue2.contains( "ZZZ" ) == false );

        queue2.clear();
        TS_ASSERT( queue2.size() == 0 );
        TS_GUM_ASSERT_THROWS_NOTHING( queue2.erase( 10 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( queue2.eraseByVal( "RRR" ) );

        queue2 = queue1;
        queue2.eraseTop();
        queue2 = queue1;
        TS_ASSERT( queue2.size() == 6 );
        TS_ASSERT( queue2.empty() == false );
        TS_ASSERT( queue2.contains( "AAA" ) == true );
        TS_ASSERT( queue2.contains( "ZZZ" ) == false );
        queue2.setUniquenessPolicy( true );
        queue2 = queue1;
        TS_ASSERT( queue2.size() == 6 );
        TS_ASSERT( queue2.empty() == false );
        TS_ASSERT( queue2.contains( "AAA" ) == true );
        TS_ASSERT( queue2.contains( "ZZZ" ) == false );
        TS_ASSERT( queue2.uniquenessPolicy() == false );

        queue2.eraseTop();
        TS_ASSERT( queue2.topPriority() == 8 );
        queue2.erase( 0 );
        TS_ASSERT( queue2.topPriority() == 10 );
        queue2.erase( 3 );
        TS_ASSERT( queue2.size() == 3 );
        queue2.eraseByVal( "AAA" );
        TS_ASSERT( queue2.size() == 2 );

        const std::string& str1 = queue2[0];
        const std::string& str2 = queue2[1];
        TS_ASSERT( str1 == "BBB" );
        TS_ASSERT( str2 == "DDD" );
      }

      void testConstObj() {
        gum::Set<int> set1, set2, set3, set4, set5, set6;
        set1 << 1;
        set2 << 1 << 2;
        set3 << 1 << 2 << 3;
        set4 << 1 << 2 << 3 << 4;
        set5 << 1 << 2 << 3 << 4 << 5;

        gum::PriorityQueue< gum::Set<int> > queue1;
        TS_ASSERT( queue1.size() == 0 );
        TS_ASSERT( queue1.empty() == true );
        TS_ASSERT( queue1.contains( set1 ) == false );
        TS_ASSERT_THROWS( queue1.top(), gum::NotFound );
        TS_ASSERT_THROWS( queue1.pop(), gum::NotFound );
        TS_ASSERT_THROWS( queue1.topPriority(), gum::NotFound );

        queue1.insert( 8,  set1 );
        queue1.insert( 10, set2 );
        queue1.insert( 2,  set3 );
        queue1.insert( 23, set4 );
        queue1.insert( 24, set5 );
        queue1.insert( 10, set1 );

        TS_ASSERT( queue1.size() == 6 );
        TS_ASSERT( queue1.empty() == false );
        TS_ASSERT( queue1.contains( set1 ) == true );
        TS_ASSERT( queue1.contains( set6 ) == false );

        const gum::Set<int>& str = queue1.top();
        TS_ASSERT( str == set3 );
        TS_ASSERT( queue1.topPriority() == 2 );

        gum::Set<int> str2 = queue1.pop();
        TS_ASSERT( str2 == set3 );
        TS_ASSERT( queue1.top() == set1 );
        TS_ASSERT( queue1.topPriority() == 8 );

        queue1.setPriority( 0, 4 );
        TS_ASSERT( queue1.top() == set1 );
        TS_ASSERT( queue1.topPriority() == 4 );
        TS_ASSERT_THROWS( queue1.setPriority( 10, 4 ) ,gum::NotFound );

        queue1.setPriorityByVal( set2, 2 );
        TS_ASSERT( queue1.top() == set2 );
        TS_ASSERT( queue1.topPriority() == 2 );
        TS_GUM_ASSERT_THROWS_NOTHING( queue1.setPriorityByVal( set6, 4 ) );

        TS_ASSERT_THROWS( queue1.priorityByVal( set6 ), gum::NotFound );
        TS_ASSERT( queue1.priorityByVal( set2 ) == 2 );

        queue1.resize( 10 );
        TS_ASSERT( queue1.capacity() == 10 );

        TS_ASSERT( queue1.uniquenessPolicy() == false );
        queue1.setUniquenessPolicy( true );
        TS_ASSERT( queue1.uniquenessPolicy() == true );
        TS_ASSERT_THROWS( queue1.insert( 10, set1 ), gum::DuplicateElement );

        const gum::HashTable< gum::Set<int>,std::vector<gum::Size> >&
        vals = queue1.allValues();
        TS_ASSERT( vals.size() == 4 );

        queue1.clear();
        queue1.setUniquenessPolicy( false );
        queue1.insert( 8,  set1 );
        queue1.insert( 10, set2 );
        queue1.insert( 2,  set3 );
        queue1.insert( 23, set4 );
        queue1.insert( 24, set5 );
        queue1.insert( 10, set1 );

        gum::PriorityQueue< gum::Set<int> > queue2 = queue1;
        queue2 = queue1;
        queue2.eraseTop();
        queue2 = queue1;

        TS_ASSERT( queue2.size() == 6 );
        TS_ASSERT( queue2.empty() == false );
        TS_ASSERT( queue2.contains( set1 ) == true );
        TS_ASSERT( queue2.contains( set6 ) == false );
        queue2.setUniquenessPolicy( true );
        queue2 = queue1;
        TS_ASSERT( queue2.size() == 6 );
        TS_ASSERT( queue2.empty() == false );
        TS_ASSERT( queue2.contains( set1 ) == true );
        TS_ASSERT( queue2.contains( set6 ) == false );
        TS_ASSERT( queue2.uniquenessPolicy() == false );

        queue2.eraseTop();
        TS_ASSERT( queue2.topPriority() == 8 );
        queue2.erase( 0 );
        TS_ASSERT( queue2.topPriority() == 10 );
        queue2.erase( 3 );
        TS_ASSERT( queue2.size() == 3 );
        queue2.eraseByVal( set1 );
        TS_ASSERT( queue2.size() == 2 );

        const gum::Set<int>& str3 = queue2[0];
        const gum::Set<int>& str4 = queue2[1];
        TS_ASSERT( str3 == set2 );
        TS_ASSERT( str4 == set4 );
      }

  };

} /* namespace gum_tests */

