/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
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

#include <agrum/variables/labelizedVariable.h>
#include <agrum/variables/discretizedVariable.h>

#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/potential.h>

namespace gum_tests {

  class PotentialTestSuite : public CxxTest::TestSuite {
    public:
    void setUp() {}

    void tearDown() {}

    void testCreation() {
      gum::Potential<double> p( new gum::MultiDimArray<double>() );
      TS_ASSERT( p.empty() );

      gum::LabelizedVariable a( "a", "first var", 2 ),
          b( "b", "second var", 4 ), c( "c", "third var", 5 );
      TS_GUM_ASSERT_THROWS_NOTHING( p << a << b << c );
    }

    void testNormalisation() {

      gum::Potential<double> p( new gum::MultiDimArray<double>() );

      gum::LabelizedVariable a( "a", "first var", 2 ),
          b( "b", "second var", 4 ), c( "c", "third var", 5 );

      p << a << b << c;

      gum::Size taille = p.domainSize();

      try {
        gum::Instantiation i( &p );

        p.fill( (double)0 );

        int cpt = 1;
        for ( i.setFirst(); !i.end(); ++i )
          p.set( i, (double)( cpt++ ) );

        p.normalize();

        i.setLast();
        TS_ASSERT_EQUALS( p[i], 2.0 / ( taille + 1 ) );

        i.setFirst();
        TS_ASSERT_EQUALS( p[i], 2.0 / ( taille * ( taille + 1 ) ) );

      } catch ( gum::Exception e ) {
        GUM_SHOWERROR( e );
      }
    }

    void testDomainSizeChanges() {
      gum::Potential<double> p1( new gum::MultiDimArray<double>() );
      gum::LabelizedVariable var1( "var1", "first var", 2 ),
          var2( "var2", "second var", 2 ), var3( "var3", "third var", 2 );
      p1 << var1 << var2 << var3;

      TS_ASSERT_EQUALS(
          p1.domainSize(),
          ( var1.domainSize() * var2.domainSize() * var3.domainSize() ) );
    }

    void testAddAnyNumber() {
      gum::Potential<double> proba( new gum::MultiDimArray<double>() );
      gum::LabelizedVariable a( "a", "first var", 2 ),
          b( "b", "second var", 4 ), c( "c", "third var", 5 );
      proba << a << b << c;

      gum::Instantiation i( proba );

      for ( i.setFirst(); !i.end(); ++i )
        TS_GUM_ASSERT_THROWS_NOTHING( proba.set( i, (double)-1 ) );
    }

    void testCopyProba() {
      gum::Potential<double> m( new gum::MultiDimArray<double>() );
      gum::LabelizedVariable a( "a", "first var", 2 ),
          b( "b", "second var", 4 ), c( "c", "third var", 5 );

      m << a << b << c;
      gum::Instantiation i( m );

      gum::Size cpt = 0;

      for ( i.setFirst(); !i.end(); ++i ) {
        m.set( i, cpt );
        cpt++;
      }

      gum::Potential<double> mm( new gum::MultiDimArray<double>() );

      gum::LabelizedVariable x( "x", "first var", 2 ),
          y( "y", "second var", 4 ), z( "z", "third var", 5 );
      mm << x << z;
      TS_ASSERT_THROWS_ANYTHING( mm.copyFrom( m ) );
      mm << y;
      TS_GUM_ASSERT_THROWS_NOTHING( mm.copyFrom( m ) );

      gum::Instantiation j( mm );
      i.chgVal( a, 0 ).chgVal( b, 2 ).chgVal( c, 2 );
      j.chgVal( x, 0 ).chgVal( y, 2 ).chgVal( z, 0 );
      TS_ASSERT_EQUALS( m[i], mm[j] );
    }

    void testRegressionCopy() {
      gum::LabelizedVariable a( "a", "first var", 2 ),
          b( "b", "second var", 4 ), c( "c", "third var", 5 );

      {
        gum::Potential<double> P1, P2;
        P1 << a << b << c;
        P2 << a << c;
        TS_GUM_ASSERT_THROWS_NOTHING( P2.copy( P1 ) );
      }

      {
        gum::Potential<double> P1, P2;
        P1 << a << b << c;
        P2 << a << c;
        TS_GUM_ASSERT_THROWS_NOTHING( P1.copy( P2 ) );
      }

      {
        gum::Potential<double> P1, P2;
        P1 << b;
        P2 << a << c;
        TS_GUM_ASSERT_THROWS_NOTHING( P1.copy( P2 ) );
      }
    }

    void testOperators() {
      GUM_CHECKPOINT;
      auto a = gum::LabelizedVariable( "a", "afoo" );
      auto b = gum::LabelizedVariable( "b", "bfoo" );
      auto c = gum::LabelizedVariable( "c", "cfoo" );

      GUM_CHECKPOINT;
      gum::Potential<int> p1;
      p1 << a << b;
      p1.fillWith( {1, 2, 3, 4} );

      GUM_CHECKPOINT;
      gum::Potential<int> p2;
      p2 << b << c;
      p2.fillWith( {5, 6, 7, 8} );

      GUM_CHECKPOINT;
      gum::Potential<int> p = p1;

      // just checking memory allocation (what else ?)
      auto pA = p1 * p2;
      TS_ASSERT_EQUALS( pA.toString(),
                        "<b:0|c:0|a:0> :: 5 /"
                        "<b:1|c:0|a:0> :: 18 /"
                        "<b:0|c:1|a:0> :: 7 /"
                        "<b:1|c:1|a:0> :: 24 /"
                        "<b:0|c:0|a:1> :: 10 /"
                        "<b:1|c:0|a:1> :: 24 /"
                        "<b:0|c:1|a:1> :: 14 /"
                        "<b:1|c:1|a:1> :: 32" );

      GUM_CHECKPOINT;
      auto pB = p1 + p2;
      TS_ASSERT_EQUALS( pB.toString(),
                        "<b:0|c:0|a:0> :: 6 /"
                        "<b:1|c:0|a:0> :: 9 /"
                        "<b:0|c:1|a:0> :: 8 /"
                        "<b:1|c:1|a:0> :: 11 /"
                        "<b:0|c:0|a:1> :: 7 /"
                        "<b:1|c:0|a:1> :: 10 /"
                        "<b:0|c:1|a:1> :: 9 /"
                        "<b:1|c:1|a:1> :: 12" );

      GUM_CHECKPOINT;
      auto pC = p2 / p1;
      TS_ASSERT_EQUALS( pC.toString(),
                        "<b:0|a:0|c:0> :: 5 /"
                        "<b:1|a:0|c:0> :: 2 /"
                        "<b:0|a:1|c:0> :: 2 /"
                        "<b:1|a:1|c:0> :: 1 /"
                        "<b:0|a:0|c:1> :: 7 /"
                        "<b:1|a:0|c:1> :: 2 /"
                        "<b:0|a:1|c:1> :: 3 /"
                        "<b:1|a:1|c:1> :: 2" );

      GUM_CHECKPOINT;
      auto pD = p2 - p1;
      TS_ASSERT_EQUALS( pD.toString(),
                        "<b:0|a:0|c:0> :: 4 /"
                        "<b:1|a:0|c:0> :: 3 /"
                        "<b:0|a:1|c:0> :: 3 /"
                        "<b:1|a:1|c:0> :: 2 /"
                        "<b:0|a:0|c:1> :: 6 /"
                        "<b:1|a:0|c:1> :: 5 /"
                        "<b:0|a:1|c:1> :: 5 /"
                        "<b:1|a:1|c:1> :: 4" );

      GUM_CHECKPOINT;
      TS_ASSERT_EQUALS( ( ( p1 * p2 ) - ( p2 / p1 ) + p1 ).toString(),
                        "<b:0|a:0|c:0> :: 1 /"
                        "<b:1|a:0|c:0> :: 19 /"
                        "<b:0|a:1|c:0> :: 10 /"
                        "<b:1|a:1|c:0> :: 27 /"
                        "<b:0|a:0|c:1> :: 1 /"
                        "<b:1|a:0|c:1> :: 25 /"
                        "<b:0|a:1|c:1> :: 13 /"
                        "<b:1|a:1|c:1> :: 34" );
    }
  };
}
