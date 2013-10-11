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

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/bijection.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/graphs/graphElements.h>

namespace gum_tests {

  class BijectionTestSuite: public CxxTest::TestSuite {
    public:

      void test_constructors() {
        gum::Bijection<int, int> bijection;

        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 1, 2 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 3, 4 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 5, 6 ) );

        TS_ASSERT_THROWS( bijection.insert( 5, 6 ), gum::DuplicateElement );
        TS_ASSERT_THROWS( bijection.insert( 5, 7 ), gum::DuplicateElement );
        TS_ASSERT_THROWS( bijection.insert( 7, 6 ), gum::DuplicateElement );

        gum::Bijection<int, int> bijection2 = bijection;
        TS_ASSERT( bijection2.size() == 3 );

        gum::Bijection<int, int>* bijection3 = new gum::Bijection<int, int>;
        bijection3->insert( 1, 2 );
        bijection3->insert( 3, 3 );

        gum::Bijection<int, int> bijection4 = bijection;
        bijection4 = *bijection3;
        delete bijection3;
        TS_ASSERT( bijection4.first( 2 ) == 1 );
        TS_ASSERT( bijection4.first( 3 ) == 3 );
        TS_ASSERT( bijection4.second( 1 ) == 2 );
        TS_ASSERT( bijection4.second( 3 ) == 3 );
      }

      void testAccess() {
        gum::Bijection<int, int> bijection;

        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 1, 2 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 3, 4 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 5, 6 ) );

        TS_ASSERT( bijection.first( 2 ) == 1 );
        TS_ASSERT( bijection.first( 4 ) == 3 );
        TS_ASSERT( bijection.first( 6 ) == 5 );
        TS_ASSERT( bijection.second( 1 ) == 2 );
        TS_ASSERT( bijection.second( 3 ) == 4 );
        TS_ASSERT( bijection.second( 5 ) == 6 );
        TS_ASSERT( bijection.existsFirst( 1 ) );
        TS_ASSERT( bijection.existsFirst( 3 ) );
        TS_ASSERT( bijection.existsFirst( 5 ) );
        TS_ASSERT( bijection.existsSecond( 2 ) );
        TS_ASSERT( bijection.existsSecond( 4 ) );
        TS_ASSERT( bijection.existsSecond( 6 ) );
        TS_ASSERT( bijection.size() == 3 );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.eraseFirst( 1 ) );
        TS_ASSERT( ! bijection.existsSecond( 2 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.eraseFirst( 3 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.eraseSecond( 6 ) );
        TS_ASSERT( bijection.empty() );

        bijection.insert( 1, 2 );
        bijection.insert( 3, 4 );
        bijection.insert( 5, 6 );

        TS_ASSERT( ! bijection.empty() );

        bijection.clear();
        TS_ASSERT( bijection.empty() );
      }

      void test_ph() {
        gum::Bijection<int, int> carre;
        carre.insert( 1, 1 );
        carre.insert( 2, 4 );
        carre.insert( 3, 9 );

        try {
          carre.insert( 4, 1 );
        } catch ( gum::DuplicateElement& ) { }

        TS_ASSERT( ! carre.existsFirst( 4 ) );
      }

      void testResize() {
        gum::Bijection<int, int> bijection( 2 );

        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 1, 2 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 3, 4 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 5, 6 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 6, 3 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 7, 5 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 8, 9 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 9, 7 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( 2, 1 ) );

        bijection.resize( 2 );

        TS_ASSERT( bijection.capacity() != 2 );
      }


      void testIterators() {
        gum::Bijection<int, int> bijection;

        bijection.insert( 1, 2 );
        bijection.insert( 3, 4 );
        bijection.insert( 5, 6 );
        bijection.insert( 6, 3 );
        bijection.insert( 7, 5 );
        bijection.insert( 8, 9 );
        bijection.insert( 9, 7 );
        bijection.insert( 2, 1 );

        gum::Bijection<int, int>::iterator iter1 = bijection.begin();
        gum::Bijection<int, int>::iterator iter2 = bijection.end();

        unsigned int nb = 0;

        for ( gum::Bijection<int, int>::iterator iter = bijection.begin();
              iter != bijection.end(); ++iter, ++nb ) { }

        TS_ASSERT( nb == 8 );

        bijection.eraseFirst( 1 );
        bijection.eraseFirst( 5 );
        bijection.eraseFirst( 6 );
        bijection.eraseFirst( 9 );

        nb = 0;

        for ( ; iter1 != iter2; ++iter1, ++nb ) { }

        TS_ASSERT( nb == 4 );

        nb = 0;
        gum::Bijection<int, int>::iterator iter = iter2;

        for ( iter = bijection.begin(); iter != iter2; ++iter, ++nb ) { }

        TS_ASSERT( nb == 4 );

        iter = bijection.begin();
        nb = iter.first();
        nb = iter.second();

      }

      void test_constructorsStar() {
        gum::Bijection<int*, int*> bijection;

        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )1, ( int* )2 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )3, ( int* )4 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )5, ( int* )6 ) );

        TS_ASSERT_THROWS( bijection.insert( ( int* )5, ( int* )6 ), gum::DuplicateElement );
        TS_ASSERT_THROWS( bijection.insert( ( int* )5, ( int* )7 ), gum::DuplicateElement );
        TS_ASSERT_THROWS( bijection.insert( ( int* )7, ( int* )6 ), gum::DuplicateElement );

        gum::Bijection<int*, int*> bijection2 = bijection;
        TS_ASSERT( bijection2.size() == 3 );

        gum::Bijection<int*, int*>* bijection3 = new gum::Bijection<int*, int*>;
        bijection3->insert( ( int* )1, ( int* )2 );
        bijection3->insert( ( int* )3, ( int* )3 );

        gum::Bijection<int*, int*> bijection4 = bijection;
        bijection4 = *bijection3;
        delete bijection3;
        TS_ASSERT( bijection4.first( ( int* )2 ) == ( int* )1 );
        TS_ASSERT( bijection4.first( ( int* )3 ) == ( int* )3 );
        TS_ASSERT( bijection4.second( ( int* )1 ) == ( int* )2 );
        TS_ASSERT( bijection4.second( ( int* )3 ) == ( int* )3 );
      }

      void testAccessStar() {
        gum::Bijection<int*, int*> bijection;

        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )1, ( int* )2 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )3, ( int* )4 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )5, ( int* )6 ) );

        TS_ASSERT( bijection.first( ( int* )2 ) == ( int* )1 );
        TS_ASSERT( bijection.first( ( int* )4 ) == ( int* )3 );
        TS_ASSERT( bijection.first( ( int* )6 ) == ( int* )5 );
        TS_ASSERT( bijection.second( ( int* )1 ) == ( int* )2 );
        TS_ASSERT( bijection.second( ( int* )3 ) == ( int* )4 );
        TS_ASSERT( bijection.second( ( int* )5 ) == ( int* )6 );
        TS_ASSERT( bijection.existsFirst( ( int* )1 ) );
        TS_ASSERT( bijection.existsFirst( ( int* )3 ) );
        TS_ASSERT( bijection.existsFirst( ( int* )5 ) );
        TS_ASSERT( bijection.existsSecond( ( int* )2 ) );
        TS_ASSERT( bijection.existsSecond( ( int* )4 ) );
        TS_ASSERT( bijection.existsSecond( ( int* )6 ) );
        TS_ASSERT( bijection.size() == 3 );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.eraseFirst( ( int* )1 ) );
        TS_ASSERT( ! bijection.existsSecond( ( int* )2 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.eraseFirst( ( int* )3 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.eraseSecond( ( int* )6 ) );
        TS_ASSERT( bijection.empty() );

        bijection.insert( ( int* )1, ( int* )2 );
        bijection.insert( ( int* )3, ( int* )4 );
        bijection.insert( ( int* )5, ( int* )6 );

        TS_ASSERT( ! bijection.empty() );

        bijection.clear();
        TS_ASSERT( bijection.empty() );
      }

      void test_phStar() {
        gum::Bijection<int*, int*> carre;
        carre.insert( ( int* ) 1, ( int* )1 );
        carre.insert( ( int* ) 2, ( int* )4 );
        carre.insert( ( int* ) 3, ( int* )9 );

        try {
          carre.insert( ( int* )4, ( int* )1 );
        } catch ( gum::DuplicateElement& ) { }

        TS_ASSERT( ! carre.existsFirst( ( int* )4 ) );
      }

      void testResizeStar() {
        gum::Bijection<int*, int*> bijection( 2 );

        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )1, ( int* )2 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )3, ( int* )4 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )5, ( int* )6 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )6, ( int* )3 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )7, ( int* )5 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )8, ( int* )9 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )9, ( int* )7 ) );
        TS_GUM_ASSERT_THROWS_NOTHING( bijection.insert( ( int* )2, ( int* )1 ) );

        bijection.resize( 2 );

        TS_ASSERT( bijection.capacity() != 2 );
      }


      void testIteratorsStar() {
        gum::Bijection<int*, int*> bijection;

        bijection.insert( ( int* )1, ( int* )2 );
        bijection.insert( ( int* )3, ( int* )4 );
        bijection.insert( ( int* )5, ( int* )6 );
        bijection.insert( ( int* )6, ( int* )3 );
        bijection.insert( ( int* )7, ( int* )5 );
        bijection.insert( ( int* )8, ( int* )9 );
        bijection.insert( ( int* )9, ( int* )7 );
        bijection.insert( ( int* )2, ( int* )1 );

        gum::Bijection<int*, int*>::iterator iter1 = bijection.begin();
        gum::Bijection<int*, int*>::iterator iter2 = bijection.end();

        unsigned int nb = 0;

        for ( gum::Bijection<int*, int*>::iterator iter = bijection.begin();
              iter != bijection.end(); ++iter, ++nb ) { }

        TS_ASSERT( nb == 8 );

        bijection.eraseFirst( ( int* )1 );
        bijection.eraseFirst( ( int* )5 );
        bijection.eraseFirst( ( int* )6 );
        bijection.eraseFirst( ( int* )9 );

        nb = 0;

        for ( ; iter1 != iter2; ++iter1, ++nb ) { }

        TS_ASSERT( nb == 4 );

        nb = 0;
        gum::Bijection<int*, int*>::iterator iter = iter2;

        for ( iter = bijection.begin(); iter != iter2; ++iter, ++nb ) { }

        TS_ASSERT( nb == 4 );

        iter = bijection.begin();
        iter.first();
        iter.second();

      }

      void testcopyConstructor() {
        gum::Bijection<gum::NodeId, const gum::LabelizedVariable*> b;
        b.insert( 1, new gum::LabelizedVariable( "A" ) );
        b.insert( 2, new gum::LabelizedVariable( "B" ) );
        b.insert( 3, new gum::LabelizedVariable( "C" ) );
        b.insert( 4, new gum::LabelizedVariable( "D" ) );
        b.insert( 5, new gum::LabelizedVariable( "E" ) );

        gum::Bijection<gum::NodeId, const gum::LabelizedVariable*> copy( b );

        for ( gum::Bijection<gum::NodeId, const gum::LabelizedVariable*>::iterator iter = b.begin();
              iter != b.end(); ++iter ) {
          delete iter.second();
        }
      }

  };

}
