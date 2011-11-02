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

#include <cxxtest/AgrumTestSuite.h>
#include <agrum/BN/generator/MCBayesNetGenerator.h>
#include <agrum/BN/generator/simpleCPTGenerator.h>
#include <agrum/core/set.h>
#include <agrum/core/exceptions.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>

namespace gum {

namespace tests {

class MCBayesNetGeneratorTestSuite: public CxxTest::TestSuite {
public:

    void setUp() {
    }

    void tearDown() {
    }

    void testCreationDeletion_1() {
        gum::MCBayesNetGenerator<double>* gen = NULL;

        TS_GUM_ASSERT_THROWS_NOTHING( gen = new gum::MCBayesNetGenerator<double>(10) );
        TS_GUM_ASSERT_THROWS_NOTHING( delete gen );
    }

    void testCreationDeletion_2() {
        gum::MCBayesNetGenerator<double>* gen = NULL;

        gum::SimpleCPTGenerator* cptGen = new gum::SimpleCPTGenerator();
        TS_GUM_ASSERT_THROWS_NOTHING( gen = new gum::MCBayesNetGenerator<double>( cptGen ,10) );
        TS_GUM_ASSERT_THROWS_NOTHING( delete gen );
    }


    void testGenerationDouble_1() {
        gum::MCBayesNetGenerator<double> gen(500);
        gum::BayesNet<double>* bn = 0;

        TS_GUM_ASSERT_THROWS_NOTHING( bn = gen.generateBN(100,30,30 ) );

        if ( bn != 0 ) delete bn;
    }

    void testGenerationDouble_2() {
        gum::MCBayesNetGenerator<double> gen(500);

        gum::BayesNet<double>* bn = gen.generateBN( 10,30,30 );
        // Test for cicuits
        std::vector<gum::NodeId> stack;
        gum::Set<gum::NodeId> passed;
        const gum::DAG& dag = bn->dag();

        for ( gum::DAG::NodeIterator iter = dag.beginNodes(); iter != dag.endNodes(); ++iter ) {
            TS_ASSERT_THROWS( dag.directedPath( *iter, *iter ), gum::NotFound );
        }

        if ( bn != 0 ) delete bn;
    }
    
    void testGenerationDouble_4() {
        gum::MCBayesNetGenerator<double> gen(500);

        gum::BayesNet<double>* bn = gen.generateBN( 100,30,30 );
        // Test for inference
        gum::LazyPropagation<double> lazyInf( *bn );
        TS_GUM_ASSERT_THROWS_NOTHING( lazyInf.makeInference() );

        if ( bn != 0 ) delete bn;
    }
    
    void testGenerationDouble_3() {std::cout << 6 << std::endl;
        //     while(1){
        gum::MCBayesNetGenerator<double> gen(500);

        gum::BayesNet<double>* bn = gen.generateBN( 100,30,30 );
        // Test for inference
        //  gum::ShaferShenoyInference<double> ssInf( *bn );
        //  TS_GUM_ASSERT_THROWS_NOTHING( ssInf.makeInference() );

        if ( bn != 0 ) delete bn;
        //   }
    }



};

}
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;
