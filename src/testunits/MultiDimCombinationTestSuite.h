/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
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


#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include <cxxtest/AgrumTestSuite.h>

#include <agrum/config.h>

#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/multiDimCombinationDefault.h>

namespace gum_tests {


  class MultiDimCombinationTestSuite: public CxxTest::TestSuite {
    private:
      // ==========================================================================
      /// initialize randomly a table
      // ==========================================================================
      void randomInitP( Potential<float>& t ) {
        Instantiation i( t );
        srand( time( NULL ) );

        for( i.setFirst(); ! i.end(); ++i )
          t.set( i, ( int )( ( ( float ) rand() / RAND_MAX ) * 100000 ) );
      }


      // the function used to combine two tables
      static Potential<float>* addPotential( const Potential<float>& t1,
                                             const Potential<float>& t2 ) {
        return new Potential<float> ( t1 + t2 );
      }


      // the function used to combine two tables
      static Potential<float>* multPotential( const Potential<float>& t1,
                                              const Potential<float>& t2 ) {
        return new Potential<float> ( t1 * t2 );
      }


    public:
      void test_op_multidimArray() {
        std::vector<LabelizedVariable*> vars( 10 );

        for( unsigned int i = 0; i < 10; ++i ) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable( s, s, 4 );
        }

        Potential<float> t1, t2, t3;

        t1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] );
        t2 << *( vars[0] ) << *( vars[1] ) << *( vars[5] );
        t3 << *( vars[6] ) << *( vars[4] ) << *( vars[3] );

        randomInitP( t1 );
        randomInitP( t2 );
        randomInitP( t3 );

        Potential<float>* t4, *t5, *t6;
        t4 = new Potential<float> ( t1 + t2 );
        t5 = new Potential<float> ( t3 + *t4 );

        Set<const Potential<float>*> set;
        set << &t1 << &t2 << &t3;

        MultiDimCombinationDefault<float,Potential> xxx( addPotential );
        t6 = xxx.combine( set );
        TS_ASSERT( t6 );
        TS_ASSERT( *t6 == *t5 );

        delete t4;
        delete t5;
        delete t6;

        TS_ASSERT( xxx.nbOperations( set ) == 16640 );
        std::pair<long,long> yyy = xxx.memoryUsage( set );
        TS_ASSERT( yyy.first == 16640 );
        TS_ASSERT( yyy.second == 16384 );

        t4 = new Potential<float> ( t1 * t2 );
        t5 = new Potential<float> ( t3 * ( *t4 ) );
        xxx.setCombineFunction( multPotential );
        t6 = xxx.combine( set );
        TS_ASSERT( t6 );
        TS_ASSERT( *t6 == *t5 );

        delete t4;
        delete t5;
        delete t6;

        for( unsigned int i = 0; i < vars.size(); ++i )
          delete vars[i];
      }


  };

} /* namespace gum_tests */

