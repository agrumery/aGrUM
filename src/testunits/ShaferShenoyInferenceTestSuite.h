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
#include <sstream>
#include <cstdlib>

#include <cxxtest/AgrumTestSuite.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/generator/BayesNetGenerator.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

class ShaferShenoyInferenceBNTestSuite: public CxxTest::TestSuite {
  public:
    gum::BayesNet<float> *bn;
    gum::Id i1, i2, i3, i4, i5;

    void setUp() {
      bn = new gum::BayesNet<float>();

      gum::LabelizedVariable n1( "1", "", 2 ), n2( "2", "", 2 ),  n3( "3", "" , 2 );
      gum::LabelizedVariable n4( "4", "", 2 ), n5( "5", "", 3 );

      i1 = bn->addVariable( n1 );
      i2 = bn->addVariable( n2 );
      i3 = bn->addVariable( n3 );
      i4 = bn->addVariable( n4 );
      i5 = bn->addVariable( n5 );

      bn->insertArc( i1, i3 );
      bn->insertArc( i1, i4 );
      bn->insertArc( i3, i5 );
      bn->insertArc( i4, i5 );
      bn->insertArc( i2, i4 );
      bn->insertArc( i2, i5 );
    }

    void tearDown() {
      delete bn;
    }

    void testFill() {
      const gum::Potential<float>& p1 = bn->cpt( i1 );
      TS_ASSERT( p1.nbrDim() == 1 );

      {
        // FILLING PARAMS
        const float t[2] = {0.2, 0.8};
        int n = 2;const std::vector<float> v( t, t + n );
        p1.fillWith( v );
      }

      const gum::Potential<float>& p2 = bn->cpt( i2 );
      TS_ASSERT( p2.nbrDim() == 1 );

      {
        // FILLING PARAMS
        const float t[2] = {0.3, 0.7};
        int n = 2;const std::vector<float> v( t, t + n );
        p2.fillWith( v );
      }

      const gum::Potential<float>& p3 = bn->cpt( i3 );
      TS_ASSERT( p3.nbrDim() == 2 );
      {
        // FILLING PARAMS
        const float t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;const std::vector<float> v( t, t + n );
        p3.fillWith( v );

        // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
        gum::Potential<float> p( new gum::MultiDimArray<float>() );
        p << bn->variable( i1 );
        p.marginalize( p3 );

        for ( gum::Instantiation j( p );! j.end();++j ) TS_ASSERT_DELTA( p[j], 1.0 ,1e-5 );
      }

      const gum::Potential<float>& p4 = bn->cpt( i4 );
      TS_ASSERT( p4.nbrDim() == 3 );
      {
        // FILLING PARAMS
        const float t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int n = 8;const std::vector<float> v( t, t + n );
        p4.fillWith( v );

        // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
        gum::Potential<float> p( new gum::MultiDimArray<float>() );
        p << bn->variable( i1 ) << bn->variable( i2 ) ;
        p.marginalize( p4 );

        for ( gum::Instantiation j( p );! j.end();++j ) TS_ASSERT_DELTA( p[j], 1.0 ,1e-5 );
      }

      const gum::Potential<float>& p5 = bn->cpt( i5 );
      TS_ASSERT( p5.nbrDim() == 4 );
      {
        // FILLING PARAMS
        const float t[24] = {0.3, 0.6, 0.1, 0.5, 0.5,0.0, 0.5, 0.5,0.0, 1.0, 0.0, 0.0,
                             0.4, 0.6, 0.0,0.5, 0.5,0.0, 0.5, 0.5,0.0, 0.0, 0.0,1.0
                            };
        int n = 24;const std::vector<float> v( t, t + n );
        p5.fillWith( v );

        // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
        gum::Potential<float> p( new gum::MultiDimArray<float>() );
        p << bn->variable( i4 ) << bn->variable( i2 ) << bn->variable( i3 );
        p.marginalize( p5 );

        for ( gum::Instantiation j( p ); ! j.end(); ++j ) {
          TS_ASSERT_DELTA( p[j], 1.0 ,1e-5 );
        }
      }
    }

    // Testing when there is no evidence
    void testShaferShenoyInf_1() {
      try {
        fill( *bn );
        // Testing the inference
        gum::ShaferShenoyInference<float> inf( *bn );
        inf.makeInference();
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
        std::cerr << std::endl << e.getContent() << std::endl;
        throw e;
      }
    }

    void testShaferShenoyInf_2() {
      fill( *bn );
      gum::ShaferShenoyInference<float> inf( *bn );

      try {
        // Testing the inference
        inf.makeInference();
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i1 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i2 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        std::cerr << e.getContent() << std::endl;
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i3 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i4 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i5 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }
    }

    void testShaferShenoyInf_3() {
      fill( *bn );
      gum::List<const gum::Potential<float>* > e_list;
      e_list.insert( &(bn->cpt(i1)) );
			e_list.insert( &(bn->cpt(i2)));

      gum::ShaferShenoyInference<float> inf( *bn );

      try {
        inf.insertEvidence( e_list );
      } catch ( gum::Exception e ) {
        std::cerr << std::endl << e.getContent() << std::endl;
        TS_ASSERT( false );
      }

      try {
        // Testing the inference
        inf.makeInference();
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i1 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i2 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i3 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i4 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& marginal = inf.marginal( i5 );
        printProba( marginal );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }
    }

    void testBlackSheep() {
      gum::BayesNetGenerator bnGen;
      float density[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
      bool error = false;

      int nbr_essai=10;

      for ( int i = 0; i < nbr_essai; ++i ) {
        gum::BayesNet<float>* bn = bnGen.generateBNF( 10, density[i] );
        gum::ShaferShenoyInference<float>* inf = NULL;
       try {
          //GUM_TRACE_VAR( i )
          inf = new gum::ShaferShenoyInference<float>(*bn);
        } catch ( std::bad_alloc ) {
          std::cout << bn->dag().toDot() << std::endl;
          error = true;
          break;
        }

        //try {
          inf->makeInference();
          //delete inf;
        /*} catch ( gum::Exception e ) {
          error = true;
          std::cout << bn->dag().toDot() << std::endl;
          break;
					}
					*/

				if (inf) delete inf;
        delete bn;
      }
    }

    // void tstWithBNGen_1() {
    //   gum::BayesNetGenerator bnGen;
    //   float density[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    //   for (int count = 0; count < 100; ++count) {
    //     bool error = false;
    //     while (!error) {
    //       for (int i = 0; i < 10; ++i) {
    //         gum::BayesNet<float> bn = bnGen.generateBNF(10, density[i]);
    //         gum::ShaferShenoyInference<float> inf(bn);
    //         TS_GUM_ASSERT_THROWS_NOTHING(inf.makeInference());
    //       }
    //     }
    //   }
    // }

    // void tstWithBNGen_2() {
    //   gum::BayesNetGenerator bnGen;
    //   float density[] = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0};
    //   for (int count = 0; count < 100; ++count) {
    //     for (int i = 0; i < 10; ++i) {
    //       gum::BayesNet<double> bn = bnGen.generateBND(10, density[i]);
    //       gum::ShaferShenoyInference<double> inf(bn);
    //       TS_GUM_ASSERT_THROWS_NOTHING(inf.makeInference());
    //     }
    //   }
    // }

  private:
    // Builds a BN to tst the inference
    void fill( gum::BayesNet<float> &bn ) {
      const gum::Potential<float>& p1 = bn.cpt( i1 );
      {
        // FILLING PARAMS
        const float t[2] = {0.2, 0.8};
        int n = 2;const std::vector<float> v( t, t + n );
        p1.fillWith( v );
      }

      const gum::Potential<float>& p2 = bn.cpt( i2 );
      {
        // FILLING PARAMS
        const float t[2] = {0.3, 0.7};
        int n = 2;const std::vector<float> v( t, t + n );
        p2.fillWith( v );
      }

      const gum::Potential<float>& p3 = bn.cpt( i3 );
      {
        // FILLING PARAMS
        const float t[4] = {0.1, 0.9, 0.9, 0.1};
        int n = 4;const std::vector<float> v( t, t + n );
        p3.fillWith( v );
      }

      const gum::Potential<float>& p4 = bn.cpt( i4 );
      {
        // FILLING PARAMS
        const float t[8] = {0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0};
        int n = 8;const std::vector<float> v( t, t + n );
        p4.fillWith( v );
      }

      const gum::Potential<float>& p5 = bn.cpt( i5 );
      {
        // FILLING PARAMS
        const float t[24] = {0.3, 0.6, 0.1, 0.5, 0.5,0.0, 0.5, 0.5,0.0, 1.0, 0.0, 0.0,
                             0.4, 0.6, 0.0,0.5, 0.5,0.0, 0.5, 0.5,0.0, 0.0, 0.0,1.0
                            };
        int n = 24;const std::vector<float> v( t, t + n );
        p5.fillWith( v );
        //printProba(p5);
      }
    }

    // Uncomment this to have some outputs.
    void printProba( const gum::Potential<float> & ) {
      //for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      //std::cerr << std::endl;
    }
};

