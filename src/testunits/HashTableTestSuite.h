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
#include <agrum/core/hashTable.h>

using namespace std;

class HashTableTestSuite: public CxxTest::TestSuite {
  public:

    void testConstructor() {
      gum::HashTable<int, int> *table=NULL;
      TS_GUM_ASSERT_THROWS_NOTHING( (table = new gum::HashTable<int, int>())  );
      TS_GUM_ASSERT_THROWS_NOTHING( delete table );
    }

    void testInsert() {
      gum::HashTable<int, string> table;
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 1, "a" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 2, "b" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 3, "c" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 4, "d" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 5, "e" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 6, "f" ) );
    }

    void testEquality() {
      gum::HashTable<int, string> t1, t2, t3;
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

    void testsize() {
      gum::HashTable<int, string> table;

      TS_ASSERT_EQUALS( table.size(), (gum::Size)0 );
      fill( table );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)6 );
    }

    void testErase() {
      gum::HashTable<int, string> table;
      fill( table );

      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 4 ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)5 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 6 ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)4 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 1 ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)3 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 3 ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)2 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 5 ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)1 );

      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 4 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( -23 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 10000 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 42 ) );

      TS_ASSERT_EQUALS( table.size(), (gum::Size)1 );
    }

    void testEraseIterator() {
      gum::HashTable<int, string> table;
      fill( table );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)6 );
      
      gum::HashTable<int, string>::iterator iter = table.begin();
      
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( iter ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)5 );

      ++iter; ++iter;
      
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( iter ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)4 );

      iter = table.rbegin();
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( iter ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)3 );

      iter = table.end();
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( iter ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)3 );

      iter = table.rend();
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( iter ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)3 );
    }

    void testEraseByVal() {
      gum::HashTable<int, string> table;
      fill( table );

      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseByVal( "a" ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)5 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseByVal( "d" ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)4 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseByVal( "e" ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)3 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseByVal( "b" ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)2 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseByVal( "f" ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)1 );

      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseByVal( "f" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseByVal( "foo" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseByVal( "bar" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseByVal( "42" ) );

      TS_ASSERT_EQUALS( table.size(), (gum::Size)1 );
    }

    void testEraseAllVal() {
      gum::HashTable<int, string> table;

      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 1, "Space, the final frontiere" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 2, "Space, the final frontiere" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 3, "Space, the final frontiere" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 4, "Space, the final frontiere" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 5, "Space, the final frontiere" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 6, "Space, the final frontiere" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 7, "Space, the final frontiere" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 8, "Space, the final frontiere" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 9, "Space, the final frontiere" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 10, "foo" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.insert( 11, "bar" ) );

      TS_ASSERT_EQUALS( table.size(), (gum::Size)11 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseAllVal( "Space, the final frontiere" ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)2 );

      TS_GUM_ASSERT_THROWS_NOTHING( table.eraseAllVal( "Space, the final frontiere" ) );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)2 );
    }

    void testClear() {
      gum::HashTable<int, string> table;

      TS_ASSERT_EQUALS( table.size(), (gum::Size)0 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.clear() );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)0 );

      fill( table );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)6 );
      TS_GUM_ASSERT_THROWS_NOTHING( table.clear() );
      TS_ASSERT_EQUALS( table.size(), (gum::Size)0 );
    }

    void testIsEmpty() {
      gum::HashTable<int, string> table;

      TS_ASSERT( table.empty() );
      fill( table );
      TS_ASSERT( ! table.empty() );
      TS_GUM_ASSERT_THROWS_NOTHING( table.clear() );
      TS_ASSERT( table.empty() );
    }

    void testExist() {
      gum::HashTable<int, string> table;

      TS_ASSERT( ! table.exists( 1 ) );
      TS_ASSERT( ! table.exists( 42 ) );

      fill( table );

      TS_ASSERT( table.exists( 1 ) );
      TS_ASSERT( table.exists( 2 ) );
      TS_ASSERT( table.exists( 3 ) );
      TS_ASSERT( table.exists( 4 ) );
      TS_ASSERT( table.exists( 5 ) );
      TS_ASSERT( table.exists( 6 ) );

      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 4 ) );

      TS_ASSERT( table.exists( 1 ) );
      TS_ASSERT( table.exists( 2 ) );
      TS_ASSERT( table.exists( 3 ) );
      TS_ASSERT( ! table.exists( 4 ) );
      TS_ASSERT( table.exists( 5 ) );
      TS_ASSERT( table.exists( 6 ) );
    }

    void testkey() {
      gum::HashTable<int, string> table;
      fill( table );

      /*
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(1));
      TS_ASSERT_EQUALS(val, "a");
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(2));
      TS_ASSERT_EQUALS(val, "b");
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(3));
      TS_ASSERT_EQUALS(val, "c");
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(4));
      TS_ASSERT_EQUALS(val, "d");
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(5));
      TS_ASSERT_EQUALS(val, "e");
      TS_GUM_ASSERT_THROWS_NOTHING(val = table.key(6));
      TS_ASSERT_EQUALS(val, "f");

      TS_ASSERT_THROWS_ANYTHING(table.key(42));
      TS_GUM_ASSERT_THROWS_NOTHING(table.erase(6));
      TS_ASSERT_THROWS_ANYTHING(table.key(6));
      */
    }

    void testkeyByVal() {
      gum::HashTable<int, string> table;
      fill( table );

      int key=0;

      TS_GUM_ASSERT_THROWS_NOTHING( key = table.keyByVal( "a" ) );
      TS_ASSERT_EQUALS( key, 1 );
      TS_GUM_ASSERT_THROWS_NOTHING( key = table.keyByVal( "b" ) );
      TS_ASSERT_EQUALS( key, 2 );
      TS_GUM_ASSERT_THROWS_NOTHING( key = table.keyByVal( "c" ) );
      TS_ASSERT_EQUALS( key, 3 );
      TS_GUM_ASSERT_THROWS_NOTHING( key = table.keyByVal( "d" ) );
      TS_ASSERT_EQUALS( key, 4 );
      TS_GUM_ASSERT_THROWS_NOTHING( key = table.keyByVal( "e" ) );
      TS_ASSERT_EQUALS( key, 5 );
      TS_GUM_ASSERT_THROWS_NOTHING( key = table.keyByVal( "f" ) );
      TS_ASSERT_EQUALS( key, 6 );

      TS_ASSERT_THROWS_ANYTHING( table.keyByVal( "foo" ) );
      TS_GUM_ASSERT_THROWS_NOTHING( table.erase( 6 ) );
      TS_ASSERT_THROWS_ANYTHING( table.keyByVal( "f" ) );
    }

    void testCopyOperator() {
      gum::HashTable<int, string> t1, t2, t3;
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

    void testGetOperator() {
      gum::HashTable<int, string> t1;
      fill( t1 );

      TS_ASSERT_EQUALS( t1[1], "a" );
      TS_ASSERT_EQUALS( t1[2], "b" );
      TS_ASSERT_EQUALS( t1[3], "c" );
      TS_ASSERT_EQUALS( t1[4], "d" );
      TS_ASSERT_EQUALS( t1[5], "e" );
      TS_ASSERT_EQUALS( t1[6], "f" );

      TS_ASSERT_THROWS_ANYTHING( t1[42] );
      TS_ASSERT_THROWS_ANYTHING( t1[-1] );
    }

    void testGetCopyOperator() {
      gum::HashTable<int, string> t1, t2;
      fill( t1 );

      TS_GUM_ASSERT_THROWS_NOTHING( t2.insert( 1, t1[1] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( t2.insert( 2, t1[2] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( t2.insert( 3, t1[3] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( t2.insert( 4, t1[4] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( t2.insert( 5, t1[5] ) );
      TS_GUM_ASSERT_THROWS_NOTHING( t2.insert( 6, t1[6] ) );

      TS_ASSERT_EQUALS( t1, t2 );

      TS_ASSERT_THROWS_ANYTHING( t2.insert( 4, t1[42] ) );
      TS_ASSERT_EQUALS( t2[4], t1[4] );
    }

    void testKeyUniqueness() {
      gum::HashTable<int, string> t1, t2;
      fill( t1 );
      fill( t2 );

      TS_ASSERT( t1.keyUniquenessPolicy() );
      TS_ASSERT( t2.keyUniquenessPolicy() );

      TS_GUM_ASSERT_THROWS_NOTHING( t2.setKeyUniquenessPolicy( false ) );
      TS_ASSERT( ! t2.keyUniquenessPolicy() );
      TS_ASSERT_EQUALS( t1, t2 );
    }

    void testResizePolicy() {
      gum::HashTable<int, string> t1, t2;
      fill( t1 );
      fill( t2 );

      TS_ASSERT( t1.resizePolicy() );
      TS_ASSERT( t2.resizePolicy() );

      TS_GUM_ASSERT_THROWS_NOTHING( t2.setResizePolicy( false ) );
      TS_ASSERT( ! t2.resizePolicy() );
      TS_ASSERT_EQUALS( t1, t2 );
    }

    void testSize() {
      gum::HashTable<int, int> t1, t2;

      TS_GUM_ASSERT_THROWS_NOTHING( t2.setResizePolicy( false ) );

      TS_ASSERT_EQUALS( t1.capacity(), (gum::Size)16 );
      TS_ASSERT_EQUALS( t2.capacity(), (gum::Size)16 );

      for ( int i = 0; i < 10000; i++ ) {
        TS_GUM_ASSERT_THROWS_NOTHING( t1.insert( i, i ) );
        TS_GUM_ASSERT_THROWS_NOTHING( t2.insert( i, i ) );
      }

      TS_ASSERT_DIFFERS( t1.capacity(), (gum::Size)16 );

      TS_ASSERT_EQUALS( t2.capacity(), (gum::Size)16 );
    }

    void testMap() {
      gum::HashTable<int, string> t1, map1, map2, map3, map4;
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
    }

    void testIterator_1() {
      gum::HashTable<int, string> t1;
      fill( t1 );

      gum::List<string> expected, obtained;

      for ( int i = 1; i < 7; i++ ) {
        expected.insert( t1[i] );
      }

      for ( gum::HashTable<int, string>::iterator iter = t1.begin();
            iter != t1.end();
            ++ iter ) {
        obtained.insert( *iter );
      }

      TS_ASSERT_EQUALS( expected.size(), obtained.size() );

      for ( gum::Size i = 0; i < obtained.size(); i++ ) {
        TS_ASSERT( expected.exists( obtained[i] ) );
      }
    }

    void testIterator_2() {
      gum::HashTable<int, string> t1;
      fill( t1 );

      gum::List<string> expected, obtained;

      for ( int i = 1; i < 7; i++ ) {
        expected.insert( t1[i] );
      }

      for ( gum::HashTable<int, string>::iterator iter = t1.rbegin();
            iter != t1.rend();
            -- iter ) {
        obtained.insert( *iter );
      }

      TS_ASSERT_EQUALS( expected.size(), obtained.size() );

      for ( gum::Size i = 0; i < obtained.size(); i++ ) {
        TS_ASSERT( expected.exists( obtained[i] ) );
      }
    }


  private:
    void fill( gum::HashTable<int, string>& table ) {
      table.insert( 1, "a" );
      table.insert( 2, "b" );
      table.insert( 3, "c" );
      table.insert( 4, "d" );
      table.insert( 5, "e" );
      table.insert( 6, "f" );
    }

    void fillEven( gum::HashTable<int, string>& table ) {
      table.insert( 2, "b" );
      table.insert( 4, "d" );
      table.insert( 6, "f" );
    }

    void fillOdd( gum::HashTable<int, string>& table ) {
      table.insert( 1, "a" );
      table.insert( 3, "c" );
      table.insert( 5, "e" );
    }

    gum::HashTable<int, string> getIntersection( gum::HashTable<int, bool> table ) {
      gum::HashTable<int, string> full;
      fill( full );
      gum::HashTable<int, string> inter;

      for ( int i = 1; i < 7; i++ ) {
        if ( table.exists( i ) && table[i] ) {
          inter.insert( i, full[i] );
        }
      }

      return inter;
    }

    gum::HashTable<int, string> getUnion( gum::HashTable<int, bool> table ) {
      gum::HashTable<int, string> full;
      fill( full );
      gum::HashTable<int, string> unionTable;

      for ( int i = 1; i < 7; i++ ) {
        if ( table.exists( i ) && table[i] ) {
          unionTable.insert( i, full[i] );
        }
      }

      return unionTable;
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

