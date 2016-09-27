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
#include <sstream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/generator/simpleBayesNetGenerator.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/variables/labelizedVariable.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class ShaferShenoyInferenceBNTestSuite : public CxxTest::TestSuite {
    public:
    gum::BayesNet<float>* bn;
    gum::NodeId i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet<float>();

      gum::LabelizedVariable n1( "1", "", 2 ), n2( "2", "", 2 ),
          n3( "3", "", 2 );
      gum::LabelizedVariable n4( "4", "", 2 ), n5( "5", "", 3 );

      i1 = bn->add( n1 );
      i2 = bn->add( n2 );
      i3 = bn->add( n3 );
      i4 = bn->add( n4 );
      i5 = bn->add( n5 );

      bn->addArc( i1, i3 );
      bn->addArc( i1, i4 );
      bn->addArc( i3, i5 );
      bn->addArc( i4, i5 );
      bn->addArc( i2, i4 );
      bn->addArc( i2, i5 );
    }

    void tearDown() { delete bn; }

    // Testing when there is no evidence
    void testCreationAndInference() {
      try {
        fill( *bn );
        // Testing the inference
        gum::ShaferShenoyInference<float>* inf = 0;
        TS_ASSERT_THROWS_NOTHING(
            inf = new gum::ShaferShenoyInference<float>( *bn ) );
        TS_ASSERT_THROWS_NOTHING( inf->makeInference() );

        if ( inf != 0 ) {
          TS_ASSERT_THROWS_NOTHING( delete inf );
        }
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void testMarginal() {
      fill( *bn );
      gum::ShaferShenoyInference<float> inf( *bn );

      TS_ASSERT_THROWS_NOTHING( inf.makeInference() );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i1 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i2 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i3 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i4 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i5 ) );
    }

    void testMarginalWithEvidence() {
      fill( *bn );
      gum::List<const gum::Potential<float>*> e_list;
      e_list.insert( &( bn->cpt( i1 ) ) );
      e_list.insert( &( bn->cpt( i2 ) ) );

      gum::ShaferShenoyInference<float> inf( *bn );

      TS_ASSERT_THROWS_NOTHING( inf.insertEvidence( e_list ) );

      TS_ASSERT_THROWS_NOTHING( inf.makeInference() );

      TS_ASSERT_THROWS_NOTHING( inf.posterior( i1 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i2 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i3 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i4 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i5 ) );
    }

    void testWithGenerator() {
      gum::Size density[] = {9, 18, 27, 36, 45};
      int trial_nb = 5;

      for ( int i = 0; i < trial_nb; ++i ) {
        gum::SimpleBayesNetGenerator<float> bnGen( 10, density[i] );

        gum::BayesNet<float>* bayesNet = new gum::BayesNet<float>();
        bnGen.generateBN( *bayesNet );

        gum::ShaferShenoyInference<float>* inf = nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING(
            inf = new gum::ShaferShenoyInference<float>( *bayesNet ) );

        TS_GUM_ASSERT_THROWS_NOTHING( if ( inf ) inf->makeInference() );

        TS_GUM_ASSERT_THROWS_NOTHING( if ( inf ) delete inf );

        delete bayesNet;
      }
    }

    private:
    // Builds a BN to tst the inference
    void fill( gum::BayesNet<float>& bn ) {
      bn.cpt( i1 ).fillWith( {0.2f, 0.8f} );
      bn.cpt( i2 ).fillWith( {0.3f, 0.7f} );
      bn.cpt( i3 ).fillWith( {0.1f, 0.9f, 0.9f, 0.1f} );
      bn.cpt( i4 ).fillWith(  // clang-format off
              {0.4f, 0.6f,
               0.5f, 0.5f,
               0.5f, 0.5f,
               1.0f, 0.0f} );  // clang-format
                                                               // on
      bn.cpt( i5 ).fillWith(  // clang-format off
              {0.3f, 0.6f, 0.1f,
               0.5f, 0.5f, 0.0f,
               0.5f, 0.5f, 0.0f,
               1.0f, 0.0f, 0.0f,
               0.4f, 0.6f, 0.0f,
               0.5f, 0.5f, 0.0f,
               0.5f, 0.5f, 0.0f,
               0.0f, 0.0f, 1.0f} );  // clang-format on
    }

    // Uncomment this to have some outputs.
    void printProba( const gum::Potential<float>& ) {
      // for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
