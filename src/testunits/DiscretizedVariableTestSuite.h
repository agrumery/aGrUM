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

#include <agrum/multidim/discretizedVariable.h>

class DiscretizedVariableTestSuite: public CxxTest::TestSuite {

  public:
    void testCreation() {
      TS_GUM_ASSERT_THROWS_NOTHING( gum::DiscretizedVariable<int> v( "var","a var" ) );
    }

    void testAddTicks() {
      gum::DiscretizedVariable<int> v( "var","a var" ) ;
      TS_ASSERT( v.empty() );
      TS_ASSERT_EQUALS( v.domainSize(),( unsigned int )0 );
      v.addTick( 1 );
      TS_ASSERT( v.empty() );
      TS_ASSERT_EQUALS( v.domainSize(),( unsigned int )0 );
      v.addTick( 3 );
      TS_ASSERT( v.empty() );
      TS_ASSERT_EQUALS( v.domainSize(),( unsigned int )1 );
      v.addTick( 2 );
      TS_ASSERT( ! v.empty() );
      TS_ASSERT_EQUALS( v.domainSize(),( unsigned int )2 );

      TS_ASSERT_THROWS( v.addTick( 2 ),gum::DefaultInLabel );

      TS_ASSERT_EQUALS( v.toString(),"var<[1;2[,[2;3]>" );
    }

    void testNormalLimits() {
      gum::DiscretizedVariable<unsigned int> v( "var","a var" ) ;
      v.addTick( 1 ).addTick( 5 ).addTick( 3 ) .addTick( 7 );

      TS_ASSERT_EQUALS( v.toString(),"var<[1;3[,[3;5[,[5;7]>" );

      unsigned int vv=( unsigned int )0;

      TS_ASSERT_THROWS( v[0],gum::OutOfBounds );
      TS_ASSERT_THROWS( v[0],gum::OutOfLowerBound );

      TS_GUM_ASSERT_THROWS_NOTHING( vv=v[1] );
      TS_ASSERT_EQUALS( vv,( unsigned int )0 );

      TS_GUM_ASSERT_THROWS_NOTHING( vv=v[2] );
      TS_ASSERT_EQUALS( vv,( unsigned int )0 );

      TS_GUM_ASSERT_THROWS_NOTHING( vv=v[3] );
      TS_ASSERT_EQUALS( vv,( unsigned int )1 );

      TS_GUM_ASSERT_THROWS_NOTHING( vv=v[4] );
      TS_ASSERT_EQUALS( vv,( unsigned int )1 );

      TS_GUM_ASSERT_THROWS_NOTHING( vv=v[5] );
      TS_ASSERT_EQUALS( vv,( unsigned int )2 );

      TS_GUM_ASSERT_THROWS_NOTHING( vv=v[6] );
      TS_ASSERT_EQUALS( vv,( unsigned int )2 );

      TS_GUM_ASSERT_THROWS_NOTHING( vv=v[7] );
      TS_ASSERT_EQUALS( vv,( unsigned int )2 );

      TS_ASSERT_THROWS( v[8],gum::OutOfBounds );
      TS_ASSERT_THROWS( v[8],gum::OutOfUpperBound );
    }

    void testOrderTicks() {
      for ( int i=1;i<7;i++ )
        for ( int j=1;j<7;j++ )
          for ( int k=1;k<7;k++ )
            for ( int l=1;l<7;l++ )
              for ( int m=1;m<7;m++ )
                for ( int n=1;n<7;n++ ) {
                  if (( i+j+k+l+m+n==21 ) && ( i*j*k*l*m*n==720 ) ) {
                    gum::DiscretizedVariable<int> d( "d","Discretized variable" );
                    d.addTick( i ).addTick( j ).addTick( k ).addTick( l ).addTick( m ).addTick( n );
                    TS_ASSERT_EQUALS( d.domainSize(),( gum::Size )5 );
                    TS_ASSERT_EQUALS( d.toString(),"d<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>" );
                  }
                }
    }

    void testFloatLimits() {
      gum::DiscretizedVariable<double> d( "d","Discretized variable" );;
      d.addTick( 3.1 ).addTick( 2.0 ).addTick( 4.0 );

      double vv;
      TS_GUM_ASSERT_THROWS_NOTHING( vv=d[2.5] );
      TS_ASSERT_THROWS( vv=d[0.5],gum::OutOfLowerBound );
      TS_ASSERT_THROWS( vv=d[4.5],gum::OutOfUpperBound );

      d.addTick( -std::numeric_limits<double>::infinity() );
      d.addTick( std::numeric_limits<double>::infinity() );

      TS_GUM_ASSERT_THROWS_NOTHING( vv=d[2.5] );
      TS_GUM_ASSERT_THROWS_NOTHING( vv=d[0.5] );
      TS_GUM_ASSERT_THROWS_NOTHING( vv=d[4.5] );
    }
};

