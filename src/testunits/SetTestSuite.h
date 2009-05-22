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

#include <iostream>
#include <string>

#include <cxxtest/TestSuite.h>

#include <agrum/core/list.h>
#include <agrum/core/set.h>

using namespace std;

class SetTestSuite: public CxxTest::TestSuite {
  public:

    void testConstructor() {
      gum::Set<int> *set=0;
      TS_GUM_ASSERT_THROWS_NOTHING( set = new gum::Set< int>() );
      TS_GUM_ASSERT_THROWS_NOTHING( delete set);
    }

    void testInsert() {
      gum::Set<string> set;
      TS_GUM_ASSERT_THROWS_NOTHING( set.insert( "a" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( set.insert( "b" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( set.insert( "c" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( set.insert( "d" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( set.insert( "e" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( set.insert( "f" ) );
    }

    void testEquality() {
      gum::Set<int> t1, t2, t3;
      fill( t1 );
      fill( t2 );
      fill( t3 );

      TS_ASSERT( t1 == t1 );
      TS_ASSERT( t2 == t2 );
      TS_ASSERT( t3 == t3 );

      TS_ASSERT( t1 == t2 );
      TS_ASSERT( t2 == t1 );
      TS_ASSERT( t1 == t3 );
      TS_ASSERT( t3 == t1 );
      TS_ASSERT( t2 == t3 );
      TS_ASSERT( t3 == t2 );

      t2.erase( 1 );
      t2.erase( 3 );
      t2.erase( 5 );

      t3.erase( 2 );
      t3.erase( 4 );
      t3.erase( 6 );

      TS_ASSERT( t1 != t2 );
      TS_ASSERT( t2 != t1 );
      TS_ASSERT( t1 != t3 );
      TS_ASSERT( t3 != t1 );
      TS_ASSERT( t2 != t3 );
      TS_ASSERT( t3 != t2 );
    }

    void testSize() {
      gum::Set<int> set;

      TS_ASSERT_EQUALS( set.size(), (gum::Size)0 );
      fill( set );
      TS_ASSERT_EQUALS( set.size(), (gum::Size)6 );
    }

    void testErase() {
      gum::Set<int> set;
      fill( set );

      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( 4 ) );
      TS_ASSERT_EQUALS( set.size(), (gum::Size)5 );
      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( 6 ) );
      TS_ASSERT_EQUALS( set.size(), (gum::Size)4 );
      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( 1 ) );
      TS_ASSERT_EQUALS( set.size(), (gum::Size)3 );
      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( 3 ) );
      TS_ASSERT_EQUALS( set.size(), (gum::Size)2 );
      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( 5 ) );
      TS_ASSERT_EQUALS( set.size(), (gum::Size)1 );

      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( 4 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( -23 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( 10000 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( 42 ) );

      TS_ASSERT_EQUALS( set.size(), (gum::Size)1 );
    }

    void testClear() {
      gum::Set<int> set;

      TS_ASSERT_EQUALS( set.size(), (gum::Size)0 );
      TS_GUM_ASSERT_THROWS_NOTHING( set.clear() );
      TS_ASSERT_EQUALS( set.size(), (gum::Size)0 );

      fill( set );
      TS_ASSERT_EQUALS( set.size(), (gum::Size)6 );
      TS_GUM_ASSERT_THROWS_NOTHING( set.clear() );
      TS_ASSERT_EQUALS( set.size(), (gum::Size)0 );
    }

    void testIsEmpty() {
      gum::Set<int> set;

      TS_ASSERT( set.empty() );
      fill( set );
      TS_ASSERT( ! set.empty() );
      TS_GUM_ASSERT_THROWS_NOTHING( set.clear() );
      TS_ASSERT( set.empty() );
    }

    void testExist() {
      gum::Set<int> set;

      TS_ASSERT( ! set.contains( 1 ) );
      TS_ASSERT( ! set.contains( 42 ) );

      fill( set );

      TS_ASSERT( set.contains( 1 ) );
      TS_ASSERT( set.contains( 2 ) );
      TS_ASSERT( set.contains( 3 ) );
      TS_ASSERT( set.contains( 4 ) );
      TS_ASSERT( set.contains( 5 ) );
      TS_ASSERT( set.contains( 6 ) );

      TS_GUM_ASSERT_THROWS_NOTHING( set.erase( 4 ) );

      TS_ASSERT( set.contains( 1 ) );
      TS_ASSERT( set.contains( 2 ) );
      TS_ASSERT( set.contains( 3 ) );
      TS_ASSERT( ! set.contains( 4 ) );
      TS_ASSERT( set.contains( 5 ) );
      TS_ASSERT( set.contains( 6 ) );
    }

    void testCopyOperator() {
      gum::Set<int> t1, t2, t3;
      fill( t1 );

      TS_GUM_ASSERT_THROWS_NOTHING( t2 = t1 );
      TS_GUM_ASSERT_THROWS_NOTHING( t3 = t2 );

      TS_ASSERT_EQUALS( t1, t2 );
      TS_ASSERT_EQUALS( t3, t2 );
      TS_ASSERT_EQUALS( t1, t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( t2.clear() );

      TS_ASSERT_DIFFERS( t1, t2 );
      TS_ASSERT_DIFFERS( t2, t3 );
      TS_ASSERT_EQUALS( t1, t3 );
    }

    void testIntersectionOperator() {
      gum::Set<int> t1, t2, t3, empty, inter;
      gum::Set<int> fakeInter;
      fill( t1 );
      fillEven( t2 );
      fillOdd( t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeInter = t1 * t2 );
      inter = getIntersection( fakeInter );
      TS_ASSERT_EQUALS( inter, t2 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeInter = t1 * t3 );
      inter = getIntersection( fakeInter );
      TS_ASSERT_EQUALS( inter, t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeInter = t1 * t1 );
      inter = getIntersection( fakeInter );
      TS_ASSERT_EQUALS( inter, t1 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeInter = t2 * t2 );
      inter = getIntersection( fakeInter );
      TS_ASSERT_EQUALS( inter, t2 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeInter = t3 * t3 );
      inter = getIntersection( fakeInter );
      TS_ASSERT_EQUALS( inter, t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeInter = t2 * t3 );
      inter = getIntersection( fakeInter );
      TS_ASSERT_EQUALS( inter, empty );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeInter = t3 * t2 );
      inter = getIntersection( fakeInter );
      TS_ASSERT_EQUALS( inter, empty );
    }

    void testUnionOperator() {
      gum::Set<int> t1, t2, t3, empty, unionSet;
      gum::Set<int> fakeUnion;
      fill( t1 );
      fillEven( t2 );
      fillOdd( t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeUnion = empty + t2 );
      unionSet= getUnion( fakeUnion );
      TS_ASSERT_EQUALS( unionSet, t2 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeUnion = empty + t3 );
      unionSet= getUnion( fakeUnion );
      TS_ASSERT_EQUALS( unionSet, t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeUnion = t1 + t1 );
      unionSet= getUnion( fakeUnion );
      TS_ASSERT_EQUALS( unionSet, t1 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeUnion = t2 + t2 );
      unionSet= getUnion( fakeUnion );
      TS_ASSERT_EQUALS( unionSet, t2 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeUnion = t3 + t3 );
      unionSet= getUnion( fakeUnion );
      TS_ASSERT_EQUALS( unionSet, t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeUnion = t2 + t3 );
      unionSet= getUnion( fakeUnion );
      TS_ASSERT_EQUALS( unionSet, t1 );

      TS_GUM_ASSERT_THROWS_NOTHING( fakeUnion = t3 + t2 );
      unionSet= getUnion( fakeUnion );
      TS_ASSERT_EQUALS( unionSet, t1 );
     }

    void testDisjunctionOperator() {
      gum::Set<int> t1, t2, t3, empty, disjunction;
      fill( t1 );
      fillEven( t2 );
      fillOdd( t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = t1 - t2 );
      TS_ASSERT_EQUALS( disjunction, t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = t1 - t3 );
      TS_ASSERT_EQUALS( disjunction, t2 );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = t1 - t1 );
      TS_ASSERT_EQUALS( disjunction, empty );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = t2 - t2 );
      TS_ASSERT_EQUALS( disjunction, empty );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = t3 - t3 );
      TS_ASSERT_EQUALS( disjunction, empty );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = t2 - t3 );
      TS_ASSERT_EQUALS( disjunction, t2 );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = t2 - empty );
      TS_ASSERT_EQUALS( disjunction, t2 );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = t3 - t2 );
      TS_ASSERT_EQUALS( disjunction, t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = t3 - empty );
      TS_ASSERT_EQUALS( disjunction, t3 );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = empty - t1 );
      TS_ASSERT_EQUALS( disjunction, empty );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = empty - t2 );
      TS_ASSERT_EQUALS( disjunction, empty );

      TS_GUM_ASSERT_THROWS_NOTHING( disjunction = empty - t3 );
      TS_ASSERT_EQUALS( disjunction, empty );
    }

    void testMap() {
      /*
      gum::Set<int> t1, map1, map2, map3, map4;
      fill( t1 );

      TS_GUM_ASSERT_THROWS_NOTHING( map1 = t1.map( &mappingTestFunc_1 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( map2 = t1.map( &mappingTestFunc_2 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( map3 = t1.map( &mappingTestFunc_3 ) );
      string str = "Space, the final frontiere.";
      TS_GUM_ASSERT_THROWS_NOTHING( map4 = t1.map( str ) );

      for ( int i = 1; i < 7; i++ ) {
        TS_ASSERT_EQUALS( map1[i], t1[i] + ".foo" );
        TS_ASSERT_EQUALS( map2[i], t1[i] + ".bar" );
        TS_ASSERT_EQUALS( map3[i], t1[i] + ".42" );
        TS_ASSERT_EQUALS( map4[i], str );
      }
      */
    }

    void testIterator_1() {
      gum::Set<int> t1;
      fill( t1 );

      gum::Set<int> obtained;

      for ( gum::Set<int>::iterator iter = t1.begin();
            iter != t1.end();
            ++ iter ) {
        obtained.insert( *iter );
      }

      TS_ASSERT(t1==obtained);
    }


  private:
    void fill( gum::Set<int>& set ) {
      set.insert( 1 );
      set.insert( 2 );
      set.insert( 3 );
      set.insert( 4 );
      set.insert( 5 );
      set.insert( 6 );
    }

    void fillEven( gum::Set<int>& set ) {
      set.insert( 2 );
      set.insert( 4 );
      set.insert( 6 );
    }

    void fillOdd( gum::Set<int>& set ) {
      set.insert( 1 );
      set.insert( 3 );
      set.insert( 5 );
    }

    gum::Set<int> getIntersection( gum::Set<int> set ) {
      gum::Set<int> full;
      fill( full );
      gum::Set<int> inter;

      for ( int i = 1; i < 7; i++ ) {
        if ( set.contains( i )  ) {
          inter.insert( i);
        }
      }

      return inter;
    }

    gum::Set<int> getUnion( gum::Set<int> set ) {
      gum::Set<int> full;
      fill( full );
      gum::Set<int> unionSet;

      for ( int i = 1; i < 7; i++ ) {
        if ( set.contains( i ) ) {
          unionSet.insert( i );
        }
      }

      return unionSet;
    }

    static string mappingTestFunc_1( const string & s ) {
      return s + ".foo";
    }

    static string mappingTestFunc_2( string & s ) {
      return s + ".bar";
    }

    static string mappingTestFunc_3( string s ) {
      return s + ".42";
    }


};

