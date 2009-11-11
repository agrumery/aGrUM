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
#include <agrum/multidim/potential.h>

namespace gum {

  namespace tests {
    
    class MultiDimOperatorsTestSuite: public CxxTest::TestSuite {
    public:
      void test_op_multidimArray () {
        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 4);
        }

        MultiDimArray<float> t1, t2, t3, *t4, *t5, *t6, *t7;
        t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
        t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
        t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);
        randomInit ( &t1 );
        randomInit ( &t2 );
        randomInit ( &t3 );

        t4 = add2MultiDimArrays ( &t1, &t2 );
        t5 = add_test_arrays ( &t1, &t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;
        t4 = add2MultiDimArrays ( &t3, &t2 );
        t5 = add_test_arrays ( &t3, &t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        t4 = add2MultiDimArrays ( &t2, &t3 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;

        t4 = subtract2MultiDimArrays ( &t1, &t2 );
        t5 = sub_test_arrays ( &t1, &t2 );
        TS_ASSERT (*t4 == *t5);
        t6 = subtract2MultiDimArrays ( &t2, &t1 );
        t7 = sub_test_arrays ( &t2, &t1 );
        TS_ASSERT (*t6 == *t7);
        TS_ASSERT (*t5 != *t7);
        TS_ASSERT (*t6 != *t4);
        delete t4;
        delete t5;
        delete t6;
        delete t7;

        t4 = multiply2MultiDimArrays ( &t1, &t2 );
        t5 = mult_test_arrays ( &t1, &t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;
        t4 = multiply2MultiDimArrays ( &t3, &t2 );
        t5 = mult_test_arrays ( &t3, &t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        t4 = multiply2MultiDimArrays ( &t2, &t3 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;

        t4 = divide2MultiDimArrays ( &t1, &t2 );
        t5 = div_test_arrays ( &t1, &t2 );
        TS_ASSERT (*t4 == *t5);
        t6 = divide2MultiDimArrays ( &t2, &t1 );
        t7 = div_test_arrays ( &t2, &t1 );
        TS_ASSERT (*t6 == *t7);
        TS_ASSERT (*t5 != *t7);
        TS_ASSERT (*t6 != *t4);
        delete t4;
        delete t5;
        delete t6;
        delete t7;

        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];
      }
      
      void test_op_multidimImplantationOfMultiDimArray () {
        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 4);
        }

        MultiDimArray<float> tt1, tt2, tt3;
        MultiDimImplementation<float> *t4, *t5, *t6, *t7;
        tt1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
        tt2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
        tt3 << *(vars[6]) << *(vars[4]) << *(vars[3]);
        randomInit ( &tt1 );
        randomInit ( &tt2 );
        randomInit ( &tt3 );
        MultiDimImplementation<float>& t1 = tt1;
        MultiDimImplementation<float>& t2 = tt2;
        MultiDimImplementation<float>& t3 = tt3;

        t4 = add2MultiDimArrays ( &t1, &t2 );
        t5 = add_test_arrays ( &tt1, &tt2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;
        t4 = add2MultiDimArrays ( &t3, &t2 );
        t5 = add_test_arrays ( &tt3, &tt2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        t4 = add2MultiDimArrays ( &t2, &t3 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;

        t4 = subtract2MultiDimArrays ( &t1, &t2 );
        t5 = sub_test_arrays ( &tt1, &tt2 );
        TS_ASSERT (*t4 == *t5);
        t6 = subtract2MultiDimArrays ( &t2, &t1 );
        t7 = sub_test_arrays ( &tt2, &tt1 );
        TS_ASSERT (*t6 == *t7);
        TS_ASSERT (*t5 != *t7);
        TS_ASSERT (*t6 != *t4);
        delete t4;
        delete t5;
        delete t6;
        delete t7;

        t4 = multiply2MultiDimArrays ( &t1, &t2 );
        t5 = mult_test_arrays ( &tt1, &tt2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;
        t4 = multiply2MultiDimArrays ( &t3, &t2 );
        t5 = mult_test_arrays ( &tt3, &tt2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        t4 = multiply2MultiDimArrays ( &t2, &t3 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;

        t4 = divide2MultiDimArrays ( &t1, &t2 );
        t5 = div_test_arrays ( &tt1, &tt2 );
        TS_ASSERT (*t4 == *t5);
        t6 = divide2MultiDimArrays ( &t2, &t1 );
        t7 = div_test_arrays ( &tt2, &tt1 );
        TS_ASSERT (*t6 == *t7);
        TS_ASSERT (*t5 != *t7);
        TS_ASSERT (*t6 != *t4);
        delete t4;
        delete t5;
        delete t6;
        delete t7;

        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];
      }

      void test_operators_init () {
        operators4MultiDimInit<float> ();
        operators4MultiDimInit<float> ();
        operators4MultiDimInit<float> ();

        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 4);
        }

        MultiDimArray<float> t1, t2, t3;
        MultiDimImplementation<float> *t4, *t5;
        t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
        t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
        t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);
        randomInit ( &t1 );
        randomInit ( &t2 );
        randomInit ( &t3 );

        t4 = t1 + t2;
        t5 = add_test_arrays ( &t1, &t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;
        t4 = t3 + t2;
        t5 = add_test_arrays ( &t3, &t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        t4 = t2 + t3;
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;

        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];
      }

      void test_potentials () {
        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 4);
        }

        Potential<double> t1, t2, t3;
        t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
        t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
        t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);

        randomInitP ( t1 );
        randomInitP ( t2 );
        randomInitP ( t3 );

        MultiDimDecorator<double>* t4, *t6;
        Potential<double> *t5, *t7;

        t4 = t1 + t2;
        t5 = add_test_potentials ( t1, t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;
        t4 = t3 + t2;
        t5 = add_test_potentials ( t3, t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        t4 = t2 + t3;
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;
 
        t4 = t1 - t2;
        t5 = sub_test_potentials ( t1, t2 );
        TS_ASSERT (*t4 == *t5);
        t6 = t2 - t1;
        t7 = sub_test_potentials ( t2, t1 );
        TS_ASSERT (*t6 == *t7);
        TS_ASSERT (*t5 != *t7);
        TS_ASSERT (*t6 != *t4);
        delete t4;
        delete t5;
        delete t6;
        delete t7;

        t4 = t1 * t2;
        t5 = mult_test_potentials ( t1, t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;
        t4 = t3 * t2;
        t5 = mult_test_potentials ( t3, t2 );
        TS_ASSERT (*t4 == *t5);
        delete t4;
        t4 = t2 * t3;
        TS_ASSERT (*t4 == *t5);
        delete t4;
        delete t5;

        t4 = t1 / t2;
        t5 = div_test_potentials ( t1, t2 );
        TS_ASSERT (*t4 == *t5);
        t6 = t2 / t1;
        t7 = div_test_potentials ( t2, t1 );
        TS_ASSERT (*t6 == *t7);
        TS_ASSERT (*t5 != *t7);
        TS_ASSERT (*t6 != *t4);
        delete t4;
        delete t5;
        delete t6;
        delete t7;

        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];
      }

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

      // ==========================================================================
      /// initialize randomly a table
      // ==========================================================================
      void randomInitP ( Potential<double>& t ) {
        Instantiation i (t);
        srand ( time ( NULL) );
        for ( i.setFirst(); ! i.end(); ++i )
          t.set (i, (int) ( ( (double) rand() / RAND_MAX ) * 100000 ) );
      }

      
      // ==========================================================================
      // ==========================================================================
      MultiDimArray<float>* add_test_arrays ( const MultiDimArray<float>* t1,
                                              const MultiDimArray<float>* t2 ) {
        // creation of the resulting variable list
        Sequence<const DiscreteVariable *> seq = t1->variablesSequence();
        const Sequence<const DiscreteVariable *>& seq2 = t2->variablesSequence();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq2.begin(); iter != seq2.end(); ++iter ) {
          if ( ! seq.exists ( *iter ) )
            seq << *iter;
        }

        // creation of the resulting table
        MultiDimArray<float>* result = new MultiDimArray<float>;
        result->beginMultipleChanges ();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq.begin(); iter != seq.end(); ++iter ) {
          *result << **iter;
        }
        result->endMultipleChanges ();
        Instantiation inst (*result);

        // compute the addition
        for ( inst.setFirst(); ! inst.end(); ++inst ) {
          result->set ( inst, (*t1)[inst] + (*t2)[inst] );
        }
          
        return result;
      }

      
      // ==========================================================================
      // ==========================================================================
      MultiDimArray<float>* sub_test_arrays ( const MultiDimArray<float>* t1,
                                              const MultiDimArray<float>* t2 ) {
        // creation of the resulting variable list
        Sequence<const DiscreteVariable *> seq = t1->variablesSequence();
        const Sequence<const DiscreteVariable *>& seq2 = t2->variablesSequence();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq2.begin(); iter != seq2.end(); ++iter ) {
          if ( ! seq.exists ( *iter ) )
            seq << *iter;
        }

        // creation of the resulting table
        MultiDimArray<float>* result = new MultiDimArray<float>;
        result->beginMultipleChanges ();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq.begin(); iter != seq.end(); ++iter ) {
          *result << **iter;
        }
        result->endMultipleChanges ();
        Instantiation inst (*result);

        // compute the addition
        for ( inst.setFirst(); ! inst.end(); ++inst ) {
          result->set ( inst, (*t1)[inst] - (*t2)[inst] );
        }
          
        return result;
      }

      // ==========================================================================
      // ==========================================================================
      MultiDimArray<float>* mult_test_arrays ( const MultiDimArray<float>* t1,
                                               const MultiDimArray<float>* t2 ) {
        // creation of the resulting variable list
        Sequence<const DiscreteVariable *> seq = t1->variablesSequence();
        const Sequence<const DiscreteVariable *>& seq2 = t2->variablesSequence();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq2.begin(); iter != seq2.end(); ++iter ) {
          if ( ! seq.exists ( *iter ) )
            seq << *iter;
        }

        // creation of the resulting table
        MultiDimArray<float>* result = new MultiDimArray<float>;
        result->beginMultipleChanges ();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq.begin(); iter != seq.end(); ++iter ) {
          *result << **iter;
        }
        result->endMultipleChanges ();
        Instantiation inst (*result);

        // compute the addition
        for ( inst.setFirst(); ! inst.end(); ++inst ) {
          result->set ( inst, (*t1)[inst] * (*t2)[inst] );
        }
          
        return result;
      }

      // ==========================================================================
      // ==========================================================================
      MultiDimArray<float>* div_test_arrays ( const MultiDimArray<float>* t1,
                                              const MultiDimArray<float>* t2 ) {
        // creation of the resulting variable list
        Sequence<const DiscreteVariable *> seq = t1->variablesSequence();
        const Sequence<const DiscreteVariable *>& seq2 = t2->variablesSequence();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq2.begin(); iter != seq2.end(); ++iter ) {
          if ( ! seq.exists ( *iter ) )
            seq << *iter;
        }

        // creation of the resulting table
        MultiDimArray<float>* result = new MultiDimArray<float>;
        result->beginMultipleChanges ();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq.begin(); iter != seq.end(); ++iter ) {
          *result << **iter;
        }
        result->endMultipleChanges ();
        Instantiation inst (*result);

        // compute the addition
        for ( inst.setFirst(); ! inst.end(); ++inst ) {
          result->set ( inst, (*t1)[inst] / (*t2)[inst] );
        }
          
        return result;
      }


      // ==========================================================================
      // ==========================================================================
      Potential<double>* add_test_potentials ( const Potential<double>& t1,
                                               const Potential<double>& t2 ) {
        // creation of the resulting variable list
        Sequence<const DiscreteVariable *> seq = t1.variablesSequence();
        const Sequence<const DiscreteVariable *>& seq2 = t2.variablesSequence();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq2.begin(); iter != seq2.end(); ++iter ) {
          if ( ! seq.exists ( *iter ) )
            seq << *iter;
        }

        // creation of the resulting table
        Potential<double>* result = new Potential<double>;
        result->beginMultipleChanges ();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq.begin(); iter != seq.end(); ++iter ) {
          *result << **iter;
        }
        result->endMultipleChanges ();
        Instantiation inst (*result);

        // compute the addition
        for ( inst.setFirst(); ! inst.end(); ++inst ) {
          result->set ( inst, t1[inst] + t2[inst] );
        }
          
        return result;
      }

      // ==========================================================================
      // ==========================================================================
      Potential<double>* sub_test_potentials ( const Potential<double>& t1,
                                               const Potential<double>& t2 ) {
        // creation of the resulting variable list
        Sequence<const DiscreteVariable *> seq = t1.variablesSequence();
        const Sequence<const DiscreteVariable *>& seq2 = t2.variablesSequence();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq2.begin(); iter != seq2.end(); ++iter ) {
          if ( ! seq.exists ( *iter ) )
            seq << *iter;
        }

        // creation of the resulting table
        Potential<double>* result = new Potential<double>;
        result->beginMultipleChanges ();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq.begin(); iter != seq.end(); ++iter ) {
          *result << **iter;
        }
        result->endMultipleChanges ();
        Instantiation inst (*result);

        // compute the addition
        for ( inst.setFirst(); ! inst.end(); ++inst ) {
          result->set ( inst, t1[inst] - t2[inst] );
        }
          
        return result;
      }

      // ==========================================================================
      // ==========================================================================
      Potential<double>* mult_test_potentials ( const Potential<double>& t1,
                                                const Potential<double>& t2 ) {
        // creation of the resulting variable list
        Sequence<const DiscreteVariable *> seq = t1.variablesSequence();
        const Sequence<const DiscreteVariable *>& seq2 = t2.variablesSequence();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq2.begin(); iter != seq2.end(); ++iter ) {
          if ( ! seq.exists ( *iter ) )
            seq << *iter;
        }

        // creation of the resulting table
        Potential<double>* result = new Potential<double>;
        result->beginMultipleChanges ();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq.begin(); iter != seq.end(); ++iter ) {
          *result << **iter;
        }
        result->endMultipleChanges ();
        Instantiation inst (*result);

        // compute the addition
        for ( inst.setFirst(); ! inst.end(); ++inst ) {
          result->set ( inst, t1[inst] * t2[inst] );
        }
          
        return result;
      }

      // ==========================================================================
      // ==========================================================================
      Potential<double>* div_test_potentials ( const Potential<double>& t1,
                                               const Potential<double>& t2 ) {
        // creation of the resulting variable list
        Sequence<const DiscreteVariable *> seq = t1.variablesSequence();
        const Sequence<const DiscreteVariable *>& seq2 = t2.variablesSequence();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq2.begin(); iter != seq2.end(); ++iter ) {
          if ( ! seq.exists ( *iter ) )
            seq << *iter;
        }

        // creation of the resulting table
        Potential<double>* result = new Potential<double>;
        result->beginMultipleChanges ();
        for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                seq.begin(); iter != seq.end(); ++iter ) {
          *result << **iter;
        }
        result->endMultipleChanges ();
        Instantiation inst (*result);

        // compute the addition
        for ( inst.setFirst(); ! inst.end(); ++inst ) {
          result->set ( inst, t1[inst] / t2[inst] );
        }
          
        return result;
      }
      
    };

  } /* namespace tests */

} /* namespace gum */
