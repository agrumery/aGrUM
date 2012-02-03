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

#include <agrum/core/set.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/projections4MultiDim.h>
#include <agrum/multidim/multiDimProjection.h>


namespace gum {


  namespace tests {


    class MultiDimProjectionTestSuite: public CxxTest::TestSuite {
      private:
        // ==========================================================================
        /// initialize randomly a table
        // ==========================================================================
        void randomInit( MultiDimArray<float>* t ) {
          Instantiation i( t );
          srand( time( NULL ) );

          for ( i.setFirst(); ! i.end(); ++i )
            t->set( i, ( int )((( float ) rand() / RAND_MAX ) * 100000 ) );
        }

        // ==========================================================================
        /// initialize randomly a table
        // ==========================================================================
        void randomInitPointer( MultiDimArray<float*>* t ) {
          Instantiation i( t );
          srand( time( NULL ) );

          for ( i.setFirst(); ! i.end(); ++i )
            t->set( i, new float(( int )((( float )rand()/RAND_MAX ) * 100000 ) ) );
        }

        // ==========================================================================
        /// initialize randomly a table
        // ==========================================================================
        void randomInitP( Potential<float>& t ) {
          Instantiation i( t );
          srand( time( NULL ) );

          for ( i.setFirst(); ! i.end(); ++i )
            t.set( i, ( int )((( float ) rand() / RAND_MAX ) * 100000 ) );
        }


        // ==========================================================================
        /// initialize randomly a table
        // ==========================================================================
        void randomInitPPointer( Potential<float*>& t ) {
          Instantiation i( t );
          srand( time( NULL ) );

          for ( i.setFirst(); ! i.end(); ++i )
            t.set( i, new float(( int )((( float )rand()/RAND_MAX ) * 100000 ) ) );
        }


        template <typename T>
        void pointerDelete( MultiDimArray<T*>* t ) {
          if ( t->variablesSequence().size() ) {
            Instantiation i( t );

            for ( i.setFirst(); ! i.end(); ++i ) {
              delete t->get( i );
            }
          }

          delete t;
        }


        template <typename T>
        void pointerDelete( MultiDimImplementation<T*>* t ) {
          if ( t->variablesSequence().size() ) {
            Instantiation i( t );

            for ( i.setFirst(); ! i.end(); ++i ) {
              delete t->get( i );
            }
          }

          delete t;
        }


        template <typename T>
        void pointerDelete( Potential<T*>* t ) {
          if ( t->variablesSequence().size() ) {
            Instantiation i( t );

            for ( i.setFirst(); ! i.end(); ++i ) {
              delete t->get( i );
            }
          }

          delete t;
        }



        // ==========================================================================
        // ==========================================================================
        template <typename T>
        bool equal( const MultiDimImplementation<T*>& t1,
                    const MultiDimImplementation<T*>& t2 ) {
          if (( t1.nbrDim() == t2.nbrDim() ) &&
              ( t1.domainSize() == t2.domainSize() ) ) {
            typedef Sequence< const DiscreteVariable * >::const_iterator
            var_iterator;

            for ( var_iterator iter = t1.variablesSequence().begin();
                  iter != t1.variablesSequence().end(); ++iter ) {
              if ( ! t2.variablesSequence().exists( *iter ) ) {
                return false;
              }
            }

            Instantiation i( t1 );

            for ( i.setFirst(); ! i.end(); ++i ) {
              if ( *( t1.get( i ) ) != *( t2.get( i ) ) ) {
                return false;
              }
            }

            return true;
          } else {
            return false;
          }
        }

        // ==========================================================================
        // ==========================================================================
        template <typename T>
        bool equal( const Potential<T*>& t1,
                    const Potential<T*>& t2 ) {
          if (( t1.nbrDim() == t2.nbrDim() ) &&
              ( t1.domainSize() == t2.domainSize() ) ) {
            typedef Sequence< const DiscreteVariable * >::const_iterator
            var_iterator;

            for ( var_iterator iter = t1.variablesSequence().begin();
                  iter != t1.variablesSequence().end(); ++iter ) {
              if ( ! t2.variablesSequence().exists( *iter ) ) {
                return false;
              }
            }

            Instantiation i( t1 );

            for ( i.setFirst(); ! i.end(); ++i ) {
              if ( *( t1.get( i ) ) != *( t2.get( i ) ) ) {
                return false;
              }
            }

            return true;
          } else {
            return false;
          }
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


        // projection of a table over a set
        MultiDimArray<float>* proj( const MultiDimArray<float>& table,
                                    const Set<const DiscreteVariable *>& del_vars,
                                    float neutral_elt ) {
          MultiDimArray<float>* result = new MultiDimArray<float>;
          const Sequence<const DiscreteVariable *>& vars = table.variablesSequence();
          result->beginMultipleChanges();

          for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                  vars.begin(); iter != vars.end(); ++iter ) {
            if ( ! del_vars.exists( *iter ) ) {
              *result << **iter;
            }
          }

          result->endMultipleChanges();

          result->fill( neutral_elt );

          Instantiation inst( table );

          for ( inst.setFirst(); ! inst.end(); ++inst ) {
            float xxx = result->get( inst );
            float yyy = table[inst];
            result->set( inst, xxx > yyy ? xxx : yyy );
          }

          return result;
        }


        // projection of a table over a set
        MultiDimArray<float*>* proj( const MultiDimArray<float*>& table,
                                     const Set<const DiscreteVariable *>& del_vars,
                                     float neutral_elt ) {
          MultiDimArray<float*>* result = new MultiDimArray<float*>;
          const Sequence<const DiscreteVariable *>& vars = table.variablesSequence();
          result->beginMultipleChanges();

          for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                  vars.begin(); iter != vars.end(); ++iter ) {
            if ( ! del_vars.exists( *iter ) ) {
              *result << **iter;
            }
          }

          result->endMultipleChanges();

          Instantiation inst1( result );

          for ( inst1.setFirst(); ! inst1.end(); ++inst1 ) {
            result->set( inst1, new float( neutral_elt ) );
          }

          Instantiation inst2( table );

          for ( inst2.setFirst(); ! inst2.end(); ++inst2 ) {
            float* xxx = result->get( inst2 );
            float* yyy = table[inst2];

            if ( *xxx < *yyy ) *xxx = *yyy;
          }

          return result;
        }


        // projection of a table over a set
        Potential<float>* proj( const Potential<float>& table,
                                const Set<const DiscreteVariable *>& del_vars,
                                float neutral_elt ) {
          Potential<float>* result = new Potential<float>;
          const Sequence<const DiscreteVariable *>& vars = table.variablesSequence();
          result->beginMultipleChanges();

          for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                  vars.begin(); iter != vars.end(); ++iter ) {
            if ( ! del_vars.exists( *iter ) ) {
              *result << **iter;
            }
          }

          result->endMultipleChanges();

          result->fill( neutral_elt );

          Instantiation inst( table );

          for ( inst.setFirst(); ! inst.end(); ++inst ) {
            float xxx = result->get( inst );
            float yyy = table[inst];
            result->set( inst, xxx > yyy ? xxx : yyy );
          }

          return result;
        }


        // projection of a table over a set
        Potential<float*>* proj( const Potential<float*>& table,
                                 const Set<const DiscreteVariable *>& del_vars,
                                 float neutral_elt ) {
          Potential<float*>* result = new Potential<float*>;
          const Sequence<const DiscreteVariable *>& vars = table.variablesSequence();
          result->beginMultipleChanges();

          for ( Sequence<const DiscreteVariable *>::const_iterator iter =
                  vars.begin(); iter != vars.end(); ++iter ) {
            if ( ! del_vars.exists( *iter ) ) {
              *result << **iter;
            }
          }

          result->endMultipleChanges();

          Instantiation inst1( result );

          for ( inst1.setFirst(); ! inst1.end(); ++inst1 ) {
            result->set( inst1, new float( neutral_elt ) );
          }

          Instantiation inst2( table );

          for ( inst2.setFirst(); ! inst2.end(); ++inst2 ) {
            float* xxx = result->get( inst2 );
            float* yyy = table[inst2];

            if ( *xxx < *yyy ) *xxx = *yyy;
          }

          return result;
        }





      public:
        void test_MultiDimArray_time() {
          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 3 );
          }

          MultiDimArray<float> t1;

          t1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInit( &t1 );
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );

          MultiDimArray<float>* t2 = projectSumMultiDimArray( &t1, del_vars );
          TS_ASSERT( t2->variablesSequence().exists( vars[2] ) );
          TS_ASSERT( t2->variablesSequence().exists( vars[3] ) );
          TS_ASSERT( t2->variablesSequence().exists( vars[6] ) );
          TS_ASSERT( t2->variablesSequence().exists( vars[7] ) );
          TS_ASSERT( t2->variablesSequence().exists( vars[4] ) );
          TS_ASSERT( t2->variablesSequence().exists( vars[5] ) );
          TS_ASSERT( t2->variablesSequence().exists( vars[8] ) );
          TS_ASSERT( not t2->variablesSequence().exists( vars[0] ) );
          TS_ASSERT( not t2->variablesSequence().exists( vars[9] ) );
          TS_ASSERT( not t2->variablesSequence().exists( vars[1] ) );
          delete t2;


          t2 = projectMinMultiDimArray( &t1, proj_set );
          delete t2;

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          t2 = projectProductMultiDimArray( &t1, proj_set );
          delete t2;


          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];

        }


        void test_MultiDimArray() {
          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 3 );
          }

          MultiDimArray<float> t1;

          t1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInit( &t1 );
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );

          MultiDimArray<float>* t2 = projectMaxMultiDimArray( &t1, del_vars );
          MultiDimArray<float>* t3 = proj( t1, del_vars, 0.0f );
          TS_ASSERT( *t2 == *t3 );

          delete t2;
          delete t3;


          t2 = projectMaxMultiDimArray( &t1, proj_set );
          t3 = proj( t1, proj_set, 0.0f );
          TS_ASSERT( *t2 == *t3 );

          delete t2;
          delete t3;

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          t2 = projectMaxMultiDimArray( &t1, proj_set );
          delete t2;


          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];

        }


        void test_MultiDimImplementation() {
          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 3 );
          }

          MultiDimArray<float> tt1;

          tt1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInit( &tt1 );

          MultiDimImplementation<float>& t1 = tt1;
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );

          MultiDimImplementation<float>* t4 = projectMaxMultiDimArray( &t1,del_vars );
          MultiDimArray<float>* t3 = proj( tt1, del_vars, 0.0f );
          TS_ASSERT( *t4 == *t3 );

          delete t3;
          delete t4;

          t4 = projectMaxMultiDimArray( &t1, proj_set );
          t3 = proj( tt1, proj_set, 0.0f );
          TS_ASSERT( *t4 == *t3 );

          delete t4;
          delete t3;

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          t4 = projectMaxMultiDimArray( &t1, proj_set );
          delete t4;


          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];

        }


        void test_MultiDimArrayPointer() {
          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 3 );
          }

          MultiDimArray<float*>* t1 = new MultiDimArray<float*>;

          *t1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInitPointer( t1 );
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );

          MultiDimArray<float*>* t2 =
            projectMaxMultiDimArray4Pointers( t1, del_vars );
          MultiDimArray<float*>* t3 =
            proj( *t1, del_vars, 0.0f );


          TS_ASSERT( equal( *t2, *t3 ) );

          pointerDelete( t2 );
          pointerDelete( t3 );

          t2 = projectMaxMultiDimArray4Pointers( t1, proj_set );
          t3 = proj( *t1, proj_set, 0.0f );
          TS_ASSERT( equal( *t2, *t3 ) );

          pointerDelete( t2 );
          pointerDelete( t3 );

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          t2 = projectMaxMultiDimArray4Pointers( t1, proj_set );
          pointerDelete( t2 );

          pointerDelete( t1 );

          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];

        }


        void test_MultiDimImplementationPointer() {
          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 3 );
          }

          MultiDimArray<float*>* tt1 = new MultiDimArray<float*>;

          *tt1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInitPointer( tt1 );
          MultiDimImplementation<float*>* t1 = tt1;
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );

          MultiDimImplementation<float*>* t2 =
            projectMaxMultiDimArray4Pointers( t1, del_vars );
          MultiDimArray<float*>* t3 =
            proj( *tt1, del_vars, 0.0f );


          TS_ASSERT( equal( *t2, *t3 ) );

          pointerDelete( t2 );
          pointerDelete( t3 );

          t2 = projectMaxMultiDimArray4Pointers( t1, proj_set );
          t3 = proj( *tt1, proj_set, 0.0f );
          TS_ASSERT( equal( *t2, *t3 ) );

          pointerDelete( t2 );
          pointerDelete( t3 );

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          t2 = projectMaxMultiDimArray4Pointers( t1, proj_set );
          pointerDelete( t2 );

          pointerDelete( tt1 );

          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];

        }


        void test_projections_init() {
          projections4MultiDimInit<float> ();
          projections4MultiDimInit<float> ();
          projections4MultiDimInit<float> ();

          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 3 );
          }

          MultiDimArray<float> t1;

          t1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInit( &t1 );
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );

          MultiDimImplementation<float>* t2 = projectMax( t1, del_vars );
          MultiDimArray<float>* t3 = proj( t1, del_vars, 0.0f );
          TS_ASSERT( *t2 == *t3 );

          delete t2;
          delete t3;


          t2 = projectMax( t1, proj_set );
          t3 = proj( t1, proj_set, 0.0f );
          TS_ASSERT( *t2 == *t3 );

          delete t2;
          delete t3;

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          t2 = projectMax( t1, proj_set );
          delete t2;


          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];
        }


        void test_potentials() {
          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 3 );
          }

          Potential<float> t1;

          t1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInitP( t1 );
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );

          Potential<float>* t2 = new Potential<float> ( projectMax( t1, del_vars ) );
          Potential<float>* t3 = proj( t1, del_vars, 0.0f );
          TS_ASSERT( *t2 == *t3 );

          delete t2;
          delete t3;


          Potential<float>* t4 = new Potential<float> ( projectMax( t1, proj_set ) );
          t3 = proj( t1, proj_set, 0.0f );
          TS_ASSERT( *t4 == *t3 );

          delete t4;
          delete t3;

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          Potential<float>* t5 = new Potential<float> ( projectMax( t1, proj_set ) );
          delete t5;


          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];
        }


        void test_Pointer_init() {
          pointerProjections4MultiDimInit<float> ();
          pointerProjections4MultiDimInit<float> ();
          pointerProjections4MultiDimInit<float> ();

          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 3 );
          }

          MultiDimArray<float*>* t1 = new MultiDimArray<float*>;

          *t1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInitPointer( t1 );
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );

          MultiDimImplementation<float*>* t2 = projectMax( *t1, del_vars );
          MultiDimArray<float*>* t3 = proj( *t1, del_vars, 0.0f );


          TS_ASSERT( equal( *t2, *t3 ) );

          pointerDelete( t2 );
          pointerDelete( t3 );

          t2 = projectMax( *t1, proj_set );
          t3 = proj( *t1, proj_set, 0.0f );
          TS_ASSERT( equal( *t2, *t3 ) );

          pointerDelete( t2 );
          pointerDelete( t3 );

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          t2 = projectMax( *t1, proj_set );
          pointerDelete( t2 );

          pointerDelete( t1 );

          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];

        }


        void test_Pointer_potential() {
          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 4 );
          }

          Potential<float*>* t1 = new Potential<float*>;

          *t1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInitPPointer( *t1 );
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );

          Potential<float*>* t2 =
            new Potential<float*> ( projectMax( *t1, del_vars ) );
          Potential<float*>* t3 = proj( *t1, del_vars, 0.0f );
          TS_ASSERT( equal( *t2, *t3 ) );

          pointerDelete( t2 );
          pointerDelete( t3 );

          Potential<float*>* t4 =
            new Potential<float*> ( projectMax( *t1, proj_set ) );
          t3 = proj( *t1, proj_set, 0.0f );
          TS_ASSERT( equal( *t4, *t3 ) );

          pointerDelete( t4 );
          pointerDelete( t3 );

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          Potential<float*>* t5 =
            new Potential<float*> ( projectMax( *t1, proj_set ) );
          pointerDelete( t5 );

          pointerDelete( t1 );

          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];

        }


        static Potential<float>*
        myMax( const Potential<float>& table,
               const Set<const DiscreteVariable*>& del_vars ) {
          return new Potential<float> ( projectMax( table, del_vars ) );
        }


        void test_MultiDimProjection() {
          std::vector<LabelizedVariable*> vars( 10 );

          for ( unsigned int i = 0; i < 10; ++i ) {
            std::stringstream str;
            str << "x" << i;
            std::string s = str.str();
            vars[i] = new LabelizedVariable( s, s, 3 );
          }

          Potential<float> t1;

          t1 << *( vars[0] ) << *( vars[1] ) << *( vars[2] ) << *( vars[3] )
          << *( vars[4] ) << *( vars[5] ) << *( vars[6] ) << *( vars[7] )
          << *( vars[8] ) << *( vars[9] );
          randomInitP( t1 );
          Set<const DiscreteVariable *> proj_set;
          proj_set.insert( vars[2] );
          proj_set.insert( vars[3] );
          proj_set.insert( vars[6] );
          proj_set.insert( vars[7] );
          proj_set.insert( vars[4] );
          proj_set.insert( vars[5] );
          proj_set.insert( vars[8] );
          Set<const DiscreteVariable *> del_vars;
          del_vars.insert( vars[0] );
          del_vars.insert( vars[9] );
          del_vars.insert( vars[1] );
          MultiDimProjection<float,Potential> Proj( myMax );

          Potential<float>* t2 =
            new Potential<float> ( projectMax( t1, del_vars ) );
          Potential<float>* t3 =
            Proj.project( t1, del_vars );
          TS_ASSERT( *t2 == *t3 );

          delete t2;
          delete t3;


          t2 = new Potential<float> ( projectMax( t1, proj_set ) );
          t3 = Proj.project( t1, proj_set );
          TS_ASSERT( *t2 == *t3 );

          delete t2;
          delete t3;

          proj_set.insert( vars[0] );
          proj_set.insert( vars[9] );
          proj_set.insert( vars[1] );
          Potential<float>* t5 = Proj.project( t1, proj_set );
          delete t5;

          TS_ASSERT( Proj.nbOperations( t1, proj_set ) == 59049 );
          TS_ASSERT( Proj.nbOperations( t1.variablesSequence(),proj_set ) == 59049 );

          std::pair<long,long> yyy = Proj.memoryUsage( t1, del_vars );
          TS_ASSERT( yyy.first == 2187 );
          yyy = Proj.memoryUsage( t1.variablesSequence(), del_vars );

          for ( unsigned int i = 0; i < vars.size(); ++i )
            delete vars[i];

        }


    };


  } /* namespace tests */

} /* namespace gum */

