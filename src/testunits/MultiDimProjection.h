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
#include <agrum/core/exceptions.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/potential.h>



namespace gum {


#define GUM_OPERATOR_PATTERN_ALLOWED

#define GUM_PROJECTION_CREATION(result_varSequence,neutral_element) \
  MultiDimArray<T>* result = new MultiDimArray<T>; \
  result->beginMultipleChanges (); \
  for ( Sequence<const DiscreteVariable *>::const_iterator iter = \
          result_varSeq.begin(); iter != result_varSeq.end(); ++iter ) { \
    *result << **iter; \
  } \
  result->endMultipleChanges (); \
  result->fill ( neutral_element );


#define GUM_PROJECTION_OPERATION(table_offset,result_offset) \
  result->unsafeSet \
    ( result_offset, \
      GUM_MULTI_DIM_OPERATOR( result->unsafeGet ( result_offset ), \
                              table->unsafeGet ( table_offset ) ) );
 


#define GUM_MULTI_DIM_OPERATOR_NAME glurps
#define GUM_MULTI_DIM_OPERATOR(x,y) (x > y ? x : y)
#include <agrum/multidim/patterns/projectionPattern4MultiDimArray.h>
#undef GUM_MULTI_DIM_OPERATOR_NAME
#undef GUM_MULTI_DIM_OPERATOR



  namespace tests {

    class MultiDimProjectionTestSuite: public CxxTest::TestSuite {
    private:
      // ==========================================================================
      /// initialize randomly a table
      // ==========================================================================
      void randomInit ( MultiDimArray<float>* t ) {
        Instantiation i (t);
        srand ( time ( NULL) );
        for ( i.setFirst(); ! i.end(); ++i )
          t->set (i, (int) ( ( (float) rand() / RAND_MAX ) * 100000 ) );
      }

      // the function used to combine two tables
      static Potential<float>* addPotential ( const Potential<float>& t1,
                                              const Potential<float>& t2 ) {
        return new Potential<float> (t1 + t2);
      }


      // the function used to combine two tables
      static Potential<float>* multPotential ( const Potential<float>& t1,
                                               const Potential<float>& t2 ) {
        return new Potential<float> (t1 * t2);
      }

    
      /** @brief the function used to extract from a TABLE a decorator that will be
       * used in __combined_size */
      static const Sequence<const DiscreteVariable *>*
      extractVars ( const Potential<float>& p) {
        return &( p.variablesSequence () );
      }


      // projection of a table over a set
      MultiDimArray<float>* proj ( const MultiDimArray<float>& table,
                                   const Set<const DiscreteVariable *>& set,
                                   float neutral_elt ) {
        MultiDimArray<float>* result = new MultiDimArray<float>;
        result->beginMultipleChanges ();
        for ( Set<const DiscreteVariable *>::const_iterator iter =
                set.begin(); iter != set.end(); ++iter ) {
          *result << **iter;
        }
        result->endMultipleChanges ();
        result->fill ( neutral_elt );

        Instantiation inst ( table );
        for ( inst.setFirst (); ! inst.end(); ++inst ) {
          float xxx = result->get (inst );
          float yyy = table[inst];
          result->set ( inst, xxx > yyy ? xxx : yyy );
        }

        return result;
      }

    
    public:
      void test1 () {
        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 4);
        }
        
        MultiDimArray<float> t1;
        t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3])
           << *(vars[4]) << *(vars[5]) << *(vars[6]) << *(vars[7])
           << *(vars[8]) << *(vars[9]);
        randomInit ( &t1 );
        Set<const DiscreteVariable *> proj_set;
        proj_set.insert ( vars[2] );
        proj_set.insert ( vars[3] );
        proj_set.insert ( vars[6] );
        proj_set.insert ( vars[7] );
        proj_set.insert ( vars[4] );
        proj_set.insert ( vars[5] );
        proj_set.insert ( vars[8] );
        
        MultiDimArray<float>* t2 =
          glurps ( &t1, t1.variablesSequence (), proj_set, 0.0f );

        MultiDimArray<float>* t3 = proj ( t1, proj_set, 0.0f );
        TS_ASSERT ( *t2 == *t3 );
        
      }

    };


  } /* namespace tests */

} /* namespace gum */

