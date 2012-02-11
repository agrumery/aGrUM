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

#include <sstream>
#include <cxxtest/AgrumTestSuite.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/graphicalModels/inference/scheduleProject.h>

namespace gum_tests {


  class ScheduleOperationTestSuite: public CxxTest::TestSuite {
    public:
      void test_construct() {
        std::vector<LabelizedVariable*> vars( 10 );

        for( unsigned int i = 0; i < 10; ++i ) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable( s, s, 2 );
        }

        gum::Potential<float> pot1;
        pot1 << *( vars[0] ) << *( vars[2] ) << *( vars[3] ) << *( vars[4] );
        randomInit( pot1 );
        gum::ScheduleMultiDim<float> f1( pot1 );
        Set<const DiscreteVariable *> del_vars;
        del_vars << vars[0] << vars[3];


        ScheduleProject<float> real_myproj( f1, del_vars, projectMax );
        const ScheduleMultiDim<float>& res = real_myproj.result();
        ScheduleOperation<float>& myproj = real_myproj;

        Sequence<const ScheduleMultiDim<float>*> multidims = myproj.multiDimArgs();
        TS_ASSERT( multidims.size() == 1 );
        TS_ASSERT( *( multidims.atPos( 0 ) ) == f1 );

        std::stringstream s1;
        s1 << res.toString() << " = project ( " << f1.toString() << " , "
           << del_vars.toString() << " )";
        TS_ASSERT( s1.str() == myproj.toString() );

        ScheduleProject<float> real_myproj2 = real_myproj;
        ScheduleOperation<float>& myproj2 = real_myproj2;
        TS_ASSERT( real_myproj2.result().isAbstract() );
        TS_ASSERT( myproj2 == myproj );
        TS_ASSERT( !( myproj2 != myproj ) );

        myproj.execute();
        TS_ASSERT( ! res.isAbstract() );
        TS_ASSERT( ! real_myproj2.result().isAbstract() );
        Potential<float>* res2 = proj( pot1, del_vars, 0 );
        TS_ASSERT( *( res2->content() ) == res.multiDim() );

        delete res2;
        delete &( res.multiDim() );

        for( unsigned int i = 0; i < vars.size(); ++i )
          delete vars[i];

      }



    private:
      // ==========================================================================
      /// initialize randomly a table
      // ==========================================================================
      void randomInit( Potential<float>& t ) {
        Instantiation i( t );
        srand( time( NULL ) );

        for( i.setFirst(); ! i.end(); ++i )
          t.set( i, ( int )( ( ( float ) rand() / RAND_MAX ) * 100000 ) );
      }



      // projection of a table over a set
      Potential<float>* proj( const Potential<float>& table,
                              const Set<const DiscreteVariable *>& del_vars,
                              float neutral_elt ) {
        Potential<float>* result = new Potential<float>;
        const Sequence<const DiscreteVariable *>& vars =
          table.variablesSequence();
        result->beginMultipleChanges();

        for( Sequence<const DiscreteVariable *>::const_iterator iter =
               vars.begin(); iter != vars.end(); ++iter ) {
          if( ! del_vars.exists( *iter ) ) {
            *result << **iter;
          }
        }

        result->endMultipleChanges();
        result->fill( neutral_elt );

        Instantiation inst( table );

        for( inst.setFirst(); ! inst.end(); ++inst ) {
          float xxx = result->get( inst );
          float yyy = table[inst];
          result->set( inst, xxx > yyy ? xxx : yyy );
        }

        return result;
      }

  };


} /* namespace gum_tests */

