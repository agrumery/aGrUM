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
#include <iostream>
#include <string>
#include <vector>

#include <agrum/config.h>

#include <cxxtest/AgrumTestSuite.h>
#include <agrum/BN/generator/BayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/core/set.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace gum {

  namespace tests {

    class BayesNetGeneratorTestSuite: public CxxTest::TestSuite {
      public:

        void setUp() {
        }

        void tearDown() {
        }

        void testCreationDeletion_1() {
          gum::BayesNetGenerator* gen = NULL;

          TS_GUM_ASSERT_THROWS_NOTHING( gen = new gum::BayesNetGenerator() );
          TS_GUM_ASSERT_THROWS_NOTHING( delete gen );
        }

        void testCreationDeletion_2() {
          gum::BayesNetGenerator* gen = NULL;

          gum::SimpleCPTGenerator* cptGen = new gum::SimpleCPTGenerator();
          TS_GUM_ASSERT_THROWS_NOTHING( gen = new gum::BayesNetGenerator( cptGen ) );
          TS_GUM_ASSERT_THROWS_NOTHING( delete gen );
        }

        void testGenerationFloat_1() {
          gum::BayesNetGenerator gen;
          gum::BayesNet<float>* bn = 0;

          TS_GUM_ASSERT_THROWS_NOTHING( bn = gen.generateBNF( 10, 0.5 ) );

          if ( bn != 0 ) delete bn;
        }

        void testGenerationFloat_2() {
          gum::BayesNetGenerator gen;

          gum::BayesNet<float>* bn = gen.generateBNF( 10, 0.5 );
          // Test for cicuits
          std::vector<gum::NodeId> stack;
          gum::Set<gum::NodeId> passed;
          const gum::DAG& dag = bn->dag();

          for ( gum::DAG::NodeIterator iter = dag.beginNodes();
                iter != dag.endNodes();
                ++iter ) {
            TS_ASSERT_THROWS( dag.directedPath( *iter, *iter ), gum::NotFound );
          }

          if ( bn != 0 ) delete bn;
        }

        void testGenerationFloat_3() {
          gum::BayesNetGenerator gen;

          gum::BayesNet<float>* bn = gen.generateBNF( 10, 1.0 );
          // Test for inference
          gum::ShaferShenoyInference<float> ssInf( *bn );
          TS_GUM_ASSERT_THROWS_NOTHING( ssInf.makeInference() );

          if ( bn != 0 ) delete bn;
        }

        void testGenerationFloat_4() {
          gum::BayesNetGenerator gen;

          gum::BayesNet<float>* bn = gen.generateBNF( 10, 1.0 );
          // Test for inference
          gum::LazyPropagation<float> lazyInf( *bn );
          TS_GUM_ASSERT_THROWS_NOTHING( lazyInf.makeInference() );

          if ( bn != 0 ) delete bn;
        }

        void testGenerationFloat_5() {
          gum::BayesNetGenerator gen;
          gum::BayesNet<float>* bn = 0;

          TS_GUM_ASSERT_THROWS_NOTHING( bn = gen.generateBNF( 30, 0.1 ); gum::LazyPropagation<float> lazyInf( *bn );lazyInf.makeInference();delete bn );
          TS_GUM_ASSERT_THROWS_NOTHING( bn = gen.generateBNF( 30, 0.15 );gum::LazyPropagation<float> lazyInf( *bn );lazyInf.makeInference();delete bn );
          TS_GUM_ASSERT_THROWS_NOTHING( bn = gen.generateBNF( 30, 0.2 ); gum::LazyPropagation<float> lazyInf( *bn );lazyInf.makeInference();delete bn );
        }

        void testGenerationDouble_1() {
          gum::BayesNetGenerator gen;
          gum::BayesNet<double>* bn = 0;

          TS_GUM_ASSERT_THROWS_NOTHING( bn = gen.generateBND( 10, 1.0 ) );

          if ( bn != 0 ) delete bn;
        }

        void testGenerationDouble_2() {
          gum::BayesNetGenerator gen;

          gum::BayesNet<double>* bn = gen.generateBND( 10, 1.0 );
          // Test for cicuits
          std::vector<gum::NodeId> stack;
          gum::Set<gum::NodeId> passed;
          const gum::DAG& dag = bn->dag();

          for ( gum::DAG::NodeIterator iter = dag.beginNodes(); iter != dag.endNodes(); ++iter ) {
            TS_ASSERT_THROWS( dag.directedPath( *iter, *iter ), gum::NotFound );
          }

          if ( bn != 0 ) delete bn;
        }

        void testGenerationDouble_3() {
          gum::BayesNetGenerator gen;

          gum::BayesNet<double>* bn = gen.generateBND( 10, 1.0 );
          // Test for inference
          gum::ShaferShenoyInference<double> ssInf( *bn );
          TS_GUM_ASSERT_THROWS_NOTHING( ssInf.makeInference() );

          if ( bn != 0 ) delete bn;
        }

        void testGenerationDouble_4() {
          gum::BayesNetGenerator gen;

          gum::BayesNet<double>* bn = gen.generateBND( 10, 1.0 );
          // Test for inference
          gum::LazyPropagation<double> lazyInf( *bn );
          TS_GUM_ASSERT_THROWS_NOTHING( lazyInf.makeInference() );

          if ( bn != 0 ) delete bn;
        }

    };

  }
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;
