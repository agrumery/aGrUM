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

#include <agrum/variables/discretizedVariable.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/multidim/instantiation.h>
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
        m.set( i, double( cpt ) );
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

    void testEquality() {
      auto a = gum::LabelizedVariable( "a", "afoo", 3 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 3 );
      auto c = gum::LabelizedVariable( "c", "cfoo", 3 );

      gum::Potential<int> p, q, r, t, u;
      p << a << b;
      p.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );

      q << b << c;  // different dims
      q.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );

      r << a << b;  // same dims, same data
      r.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );

      t << a << b;  // same dims,different data
      t.fillWith( {1, 2, 3, 4, 0, 6, 7, 8, 9} );

      u << b << a;  // same dims, same data, different order
      u.fillWith( {1, 4, 7, 2, 5, 8, 3, 6, 9} );

      TS_ASSERT( p != q );
      TS_ASSERT( p == r );
      TS_ASSERT( p != t );
      TS_ASSERT( p == u );
    }

    void testOperators() {
      auto a = gum::LabelizedVariable( "a", "afoo" );
      auto b = gum::LabelizedVariable( "b", "bfoo" );
      auto c = gum::LabelizedVariable( "c", "cfoo" );

      gum::Potential<int> p1;
      p1 << a << b;
      p1.fillWith( {1, 2, 3, 4} );

      gum::Potential<int> p2;
      p2 << b << c;
      p2.fillWith( {5, 6, 7, 8} );

      auto p = p1;

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

      TS_ASSERT_EQUALS( ( ( p1 * p2 ) - ( p2 / p1 ) + p1 ).toString(),
                        "<b:0|a:0|c:0> :: 1 /"
                        "<b:1|a:0|c:0> :: 19 /"
                        "<b:0|a:1|c:0> :: 10 /"
                        "<b:1|a:1|c:0> :: 27 /"
                        "<b:0|a:0|c:1> :: 1 /"
                        "<b:1|a:0|c:1> :: 25 /"
                        "<b:0|a:1|c:1> :: 13 /"
                        "<b:1|a:1|c:1> :: 34" );

      p = p1 * p2;
      p -= ( p2 / p1 );
      p += p1;
      TS_ASSERT_EQUALS( p.toString(),
                        "<b:0|a:0|c:0> :: 1 /"
                        "<b:1|a:0|c:0> :: 19 /"
                        "<b:0|a:1|c:0> :: 10 /"
                        "<b:1|a:1|c:0> :: 27 /"
                        "<b:0|a:0|c:1> :: 1 /"
                        "<b:1|a:0|c:1> :: 25 /"
                        "<b:0|a:1|c:1> :: 13 /"
                        "<b:1|a:1|c:1> :: 34" );
    }

    void testMargOutFunctions() {
      auto a = gum::LabelizedVariable( "a", "afoo", 3 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 3 );
      auto c = gum::LabelizedVariable( "c", "cfoo", 3 );
      auto d = gum::LabelizedVariable( "d", "dfoo", 3 );

      gum::Potential<float> p;
      p << a << b;
      p.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );
      p.normalize();

      gum::Potential<float> q;
      q << c << d;
      q.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );
      q.normalize();

      TS_ASSERT( p != q );

      gum::Potential<float> r;
      r << c << d;
      r.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );
      TS_ASSERT( q != r );
      r.normalize();
      TS_ASSERT( q == r );

      auto joint = p * q;

      auto margAB = joint.margSumOut( {&c, &d} );
      TS_ASSERT( ( p == margAB ) );
      auto margCD = joint.margSumOut( {&b, &a} );
      TS_ASSERT( ( q == margCD ) );

      p.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );
      TS_ASSERT_EQUALS( p.margProdOut( {&a} ).toString(),
                        "<b:0> :: 6 /<b:1> :: 120 /<b:2> :: 504" );
      TS_ASSERT_EQUALS( p.margProdOut( {&b} ).toString(),
                        "<a:0> :: 28 /<a:1> :: 80 /<a:2> :: 162" );

      TS_ASSERT_EQUALS( p.margMaxOut( {&a} ).toString(),
                        "<b:0> :: 3 /<b:1> :: 6 /<b:2> :: 9" );
      TS_ASSERT_EQUALS( p.margMaxOut( {&b} ).toString(),
                        "<a:0> :: 7 /<a:1> :: 8 /<a:2> :: 9" );

      TS_ASSERT_EQUALS( p.margMinOut( {&a} ).toString(),
                        "<b:0> :: 1 /<b:1> :: 4 /<b:2> :: 7" );
      TS_ASSERT_EQUALS( p.margMinOut( {&b} ).toString(),
                        "<a:0> :: 1 /<a:1> :: 2 /<a:2> :: 3" );
    }

    void testMargInFunctions() {
      auto a = gum::LabelizedVariable( "a", "afoo", 3 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 3 );
      auto c = gum::LabelizedVariable( "c", "cfoo", 3 );
      auto d = gum::LabelizedVariable( "d", "dfoo", 3 );

      gum::Potential<float> p;
      p << a << b;
      p.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );

      gum::Potential<float> q;
      q << c << d;
      q.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );

      auto joint = p * q;

      TS_ASSERT(
          ( joint.margSumOut( {&c, &d} ) == joint.margSumIn( {&a, &b} ) ) );
      TS_ASSERT(
          ( joint.margSumOut( {&c, &d} ) == joint.margSumIn( {&b, &a} ) ) );

      TS_ASSERT(
          ( joint.margProdOut( {&c, &d} ) == joint.margProdIn( {&a, &b} ) ) );

      TS_ASSERT(
          ( joint.margMinOut( {&c, &d} ) == joint.margMinIn( {&a, &b} ) ) );

      TS_ASSERT(
          ( joint.margMaxOut( {&c, &d} ) == joint.margMaxIn( {&a, &b} ) ) );
    }

    void testAbsPotential() {
      auto a = gum::LabelizedVariable( "a", "afoo", 2 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 2 );

      gum::Potential<float> p;
      p << a << b;
      p.fillWith( {0, 1, 2, 3} );

      gum::Potential<float> q;
      q << a << b;
      q.fillWith( {0, 3, 0, 3} );

      TS_ASSERT_EQUALS( ( p - q ).abs().toString(),
                        "<a:0|b:0> :: 0 /<a:1|b:0> :: 2 /"
                        "<a:0|b:1> :: 2 /<a:1|b:1> :: 0" );
      TS_ASSERT_EQUALS( ( q - p ).abs().toString(),
                        "<a:0|b:0> :: 0 /<a:1|b:0> :: 2 /"
                        "<a:0|b:1> :: 2 /<a:1|b:1> :: 0" );
      TS_ASSERT_EQUALS( ( q - p ).abs().max(), 2 );
      TS_ASSERT_EQUALS( ( q - p ).abs().min(), 0 );
    }

    void testSqPotential() {
      auto a = gum::LabelizedVariable( "a", "afoo", 2 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 2 );

      gum::Potential<float> p;
      p << a << b;
      p.fillWith( {0, 1, 2, 3} );

      gum::Potential<float> q;
      q << b << a;
      q.fillWith( {0, 0, 3, 3} );

      TS_ASSERT_EQUALS( ( p - q ).sq().toString(),
                        "<a:0|b:0> :: 0 /<a:1|b:0> :: 4 /"
                        "<a:0|b:1> :: 4 /<a:1|b:1> :: 0" );
      TS_ASSERT_EQUALS( ( q - p ).sq().toString(),
                        "<b:0|a:0> :: 0 /<b:1|a:0> :: 4 /"
                        "<b:0|a:1> :: 4 /<b:1|a:1> :: 0" );
      TS_ASSERT_EQUALS( ( q - p ).sq().max(), 4 );
      TS_ASSERT_EQUALS( ( q - p ).sq().min(), 0 );
    }

    void testEntropyPotential() {
      auto a = gum::LabelizedVariable( "a", "afoo", 2 );
      gum::Potential<float> p;
      p.add( a );
      TS_ASSERT_EQUALS( p.fillWith( {0, 1} ).entropy(), 0.0 );
      TS_ASSERT_EQUALS( p.fillWith( {1, 0} ).entropy(), 0.0 );
      TS_ASSERT_EQUALS( p.fillWith( {0.5, 0.5} ).entropy(), 1.0 );
    }

    void testReorganizePotential() {

      auto a = gum::LabelizedVariable( "a", "afoo", 3 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 3 );
      auto c = gum::LabelizedVariable( "c", "cfoo", 3 );
      auto d = gum::LabelizedVariable( "d", "dfoo", 3 );

      gum::Potential<float> p;
      p << a << b;
      p.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );

      gum::Potential<float> q;
      q << c << d;
      q.fillWith( {4, 5, 6, 3, 2, 1, 4, 3, 2} );


      TS_ASSERT_THROWS_NOTHING( {
        auto joint1 = p * q;
        auto joint2 = q * p;
        TS_ASSERT_DIFFERS( joint1.toString(), joint2.toString() );
        auto joint3 = ( q * p ).reorganize( {&c, &d, &a, &b} );
        TS_ASSERT_EQUALS( joint1.toString(), joint3.toString() );
      } )

      TS_ASSERT_THROWS( p.reorganize( {&a, &b, &c} ), gum::InvalidArgument );
      TS_ASSERT_THROWS( p.reorganize( {&a, &d} ), gum::InvalidArgument );
      TS_ASSERT_THROWS( p.reorganize( {&a} ), gum::InvalidArgument );
    }

    void testPutFirst() {
      auto a = gum::LabelizedVariable( "a", "afoo", 3 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 3 );
      auto c = gum::LabelizedVariable( "b", "bfoo", 3 );

      gum::Potential<float> p;
      p << a << b;
      p.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );

      TS_ASSERT_DIFFERS( p.toString(), p.putFirst( &b ).toString() );
      TS_ASSERT_EQUALS( p.toString(),
                        p.putFirst( &b ).putFirst( &a ).toString() );
      TS_ASSERT_EQUALS( p.toString(), p.putFirst( &a ).toString() );

      TS_ASSERT_THROWS( p.putFirst( &c ), gum::InvalidArgument );
    }

    void testExtraction() {
      auto a = gum::LabelizedVariable( "a", "afoo", 3 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 3 );
      auto c = gum::LabelizedVariable( "c", "cfoo", 3 );

      gum::Potential<float> p;
      p << a << b;
      p.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );

      gum::Potential<float> q;
      q << c;
      q.fillWith( {1, 2, 3} );

      auto pot = q * p;

      gum::Instantiation I;
      I << c;
      I.chgVal( c, 0 );
      TS_ASSERT( pot.extract( I ) == p );
      I.chgVal( c, 2 );
      gum::Potential<float> r;
      r << a << b;
      r.fillWith( {3, 6, 9, 12, 15, 18, 21, 24, 27} );
      TS_ASSERT( pot.reorganize( {&b, &c, &a} ).extract( I ) == r );
    }
    void testOperatorEqual() {
      auto a = gum::LabelizedVariable( "a", "afoo", 3 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 3 );

      gum::Potential<float> p;
      {  // empty potentials are equal
        gum::Potential<float> q;
        TS_ASSERT_EQUALS( p, q );
      }
      p << a;
      p.fillWith( {1, 2, 3} );
      TS_ASSERT_EQUALS( p, p );

      {  // same potential
        gum::Potential<float> q;
        q << a;
        q.fillWith( {1, 2, 3} );
        TS_ASSERT_EQUALS( p, q );
      }

      {  // difference values
        gum::Potential<float> q;
        q << a;
        q.fillWith( {3, 6, 9} );
        TS_ASSERT_DIFFERS( p, q );
      }

      {  // same values, different variables
        gum::Potential<float> q;
        q << b;
        q.fillWith( {1, 2, 3} );
        TS_ASSERT_DIFFERS( p, q );
      }

      {  // different dimensions
        gum::Potential<float> q;
        q << b << a;
        q.fillWith( 1 );
        TS_ASSERT_DIFFERS( p, q );
      }
    }

    void testScale() {
      auto a = gum::LabelizedVariable( "a", "afoo", 3 );

      gum::Potential<float> p;
      p << a;
      p.fillWith( {1, 2, 3} );
      gum::Potential<float> p3;
      p3 << a;
      p3.fillWith( {3, 6, 9} );

      TS_GUM_ASSERT_THROWS_NOTHING( p.scale( 3.0 ) );

      TS_ASSERT( p.scale( 3.0 ) == p3 );
      TS_ASSERT_EQUALS( p.scale( 3.0 ), p3 );
    }

    void testNormalizeAsCPT() {
      auto a = gum::LabelizedVariable( "a", "afoo", 3 );
      auto b = gum::LabelizedVariable( "b", "bfoo", 3 );

      gum::Potential<float> p;
      p << a << b;
      p.fillWith( {1, 2, 3, 4, 5, 6, 7, 8, 9} );

      auto q = p / p.margSumOut( {&a} );
      p.normalizeAsCPT();
      TS_ASSERT_EQUALS( p, q );
      TS_ASSERT_EQUALS( q, p );

      gum::Potential<float> p2;
      TS_ASSERT_THROWS( p2.normalizeAsCPT(), gum::FatalError );
      p2 << a << b;
      p2.fill( 0.0f );
      TS_ASSERT_THROWS( p2.normalizeAsCPT(), gum::FatalError );

      gum::Potential<float> p3;
      p3 << a << b;
      p3.fillWith( {1, 2, 3, 0, 0, 0, 7, 8, 9} );
      TS_ASSERT_THROWS( p2.normalizeAsCPT(), gum::FatalError );

      gum::Potential<float> p4;
      p4 << a;
      p4.fillWith( {1, 3, 6} );
      p4.normalizeAsCPT();
      gum::Potential<float> witness;
      witness << a;
      witness.fillWith( {0.1f, 0.3f, 0.6f} );
      TS_ASSERT_EQUALS( p4, witness );
    }
  };
}
