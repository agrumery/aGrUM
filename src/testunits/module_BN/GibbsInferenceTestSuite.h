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

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/GibbsInference.h>

#include <agrum/core/approximations/approximationSchemeListener.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class aSimpleGibbsListener : public gum::ApproximationSchemeListener {
    private:
    int __nbr;
    std::string __mess;

    public:
    aSimpleGibbsListener( gum::ApproximationScheme& sch )
        : gum::ApproximationSchemeListener( sch )
        , __nbr( 0 )
        , __mess( "" ){};
    void whenProgress( const void* buffer, gum::Size a, double b, double c ) {
      __nbr++;
    }
    void whenStop( const void* buffer, std::string s ) { __mess = s; }

    int getNbr() { return __nbr; }
    std::string getMess() { return __mess; }
  };

  class GibbsInferenceTestSuite : public CxxTest::TestSuite {
    public:
    gum::BayesNet<float>* bn;
    gum::Id i1, i2, i3, i4, i5;
    gum::Potential<float>* e_i1, *e_i4;

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

      e_i1 = new gum::Potential<float>();
      ( *e_i1 ) << bn->variable( i1 );
      e_i1->fill( (float)0 );
      gum::Instantiation inst_1( *e_i1 );
      inst_1.chgVal( bn->variable( i1 ), 0 );
      e_i1->set( inst_1, (float)1 );

      e_i4 = new gum::Potential<float>();
      ( *e_i4 ) << bn->variable( i4 );
      e_i4->fill( (float)0 );
      gum::Instantiation inst_4( *e_i4 );
      inst_4.chgVal( bn->variable( i4 ), 1 );
      e_i4->set( inst_4, (float)1 );
    }

    void tearDown() {
      delete bn;
      delete e_i1;
      delete e_i4;
    }

    void testFill() {
      TS_ASSERT( bn->cpt( i1 ).nbrDim() == 1 );
      bn->cpt( i1 ).fillWith( {0.2f, 0.8f} );

      TS_ASSERT( bn->cpt( i2 ).nbrDim() == 1 );
      bn->cpt( i2 ).fillWith( {0.3f, 0.7f} );

      TS_ASSERT( bn->cpt( i3 ).nbrDim() == 2 );
      bn->cpt( i3 ).fillWith( {0.1f, 0.9f, 0.9f, 0.1f} );

      // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
      // gum::Set<const gum::DiscreteVariable*> del_vars;
      auto p3 = bn->cpt( i3 ).margSumOut( {&( bn->variable( i3 ) )} );
      for ( gum::Instantiation j( p3 ); !j.end(); ++j )
        TS_ASSERT_DELTA( p3.get( j ), 1.0f, 1e-5 );

      TS_ASSERT( bn->cpt( i4 ).nbrDim() == 3 );
      bn->cpt( i4 ).fillWith( {0.4f, 0.6f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f} );
      // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
      auto p4 = bn->cpt( i4 ).margSumOut( {&( bn->variable( i4 ) )} );
      for ( gum::Instantiation j( p4 ); !j.end(); ++j )
        TS_ASSERT_DELTA( p4.get( j ), 1.0f, 1e-5 );

      TS_ASSERT( bn->cpt( i5 ).nbrDim() == 4 );
      bn->cpt( i5 ).fillWith(  // clang-format off
                             {0.3f, 0.6f, 0.1f,
                              0.5f, 0.5f, 0.0f,
                              0.5f, 0.5f,0.0f,
                              1.0f, 0.0f, 0.0f,
                              0.4f, 0.6f, 0.0f,
                              0.5f, 0.5f, 0.0f,
                              0.5f, 0.5f, 0.0f,
                              0.0f, 0.0f, 1.0f}); //clang-format on

      // CHECKING IS FOR EACH INSTANCE OF PARENTS, WE HAVE A PROBA (SUM to 1)
      auto p5 = bn->cpt( i5 ).margSumOut( {&(bn->variable(i5))} );
      for ( gum::Instantiation j( p5 ); !j.end(); ++j ) {
        TS_ASSERT_DELTA( p5.get(j), 1.0f, 1e-5 );
      }
    }

    // Testing when there is no evidence
    void testGibbsInf_1() {
      //       try {
      //         fill( *bn );
      //         // Testing the inference
      //         gum::GibbsInference<float> inf( *bn );
      //         inf.setVerbosity( false );
      //         inf.makeInference();
      //       } catch ( gum::Exception e ) {
      //         TS_ASSERT( false );
      //         std::cerr << std::endl << e.content() << std::endl;
      //         throw e;
      //       }
    }

    void testGibbsInf_2() {
      fill( *bn );
      gum::GibbsInference<float> inf( *bn );
      inf.setVerbosity( false );

      try {
        // Testing the inference
        inf.makeInference();
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i1 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i2 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        std::cerr << e.errorContent() << std::endl;
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i3 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i4 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i5 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }
    }

    void testGibbsInf_3() {
      fill( *bn );
      gum::List<const gum::Potential<float>*> e_list;
      e_list.insert( e_i1 );
      e_list.insert( e_i4 );

      gum::GibbsInference<float> inf( *bn );
      inf.setVerbosity( false );

      try {
        inf.insertEvidence( e_list );
      } catch ( gum::Exception e ) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        TS_ASSERT( false );
      }

      try {
        // Testing the inference
        inf.makeInference();
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i1 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i2 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i3 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i4 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      try {
        const gum::Potential<float>& posterior = inf.posterior( i5 );
        printPotential( posterior );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }
    }

    void testGibbsInfListener() {
      fill( *bn );
      gum::List<const gum::Potential<float>*> e_list;
      e_list.insert( e_i1 );
      e_list.insert( e_i4 );

      gum::GibbsInference<float> inf( *bn );

      aSimpleGibbsListener agsl( inf );

      try {
        // Testing the inference
        inf.makeInference();
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      TS_ASSERT_EQUALS( agsl.getNbr() * inf.periodSize() + inf.burnIn(),
                        inf.nbrIterations() );
      TS_ASSERT_DIFFERS( agsl.getMess(), std::string( "" ) );
    }

    private:
    // Builds a BN to test the inference
    void fill( gum::BayesNet<float>& bn ) {
      bn.cpt( i1 ).fillWith( {0.2f, 0.8f} );
      bn.cpt( i2 ).fillWith( {0.3f, 0.7f} );
      bn.cpt( i3 ).fillWith( {0.1f, 0.9f, 0.9f, 0.1f} );
      bn.cpt( i4 ).fillWith(  // clang-format off
              {0.4f, 0.6f,
               0.5f, 0.5f,
               0.5f, 0.5f,
               1.0f, 0.0f} );  // clang-format on
      bn.cpt( i5 ).fillWith(  // clang-format off
              {0.3f, 0.6f, 0.1f,
               0.5f, 0.5f, 0.0f,
               0.5f, 0.5f, 0.0f,
               1.0f, 0.0f, 0.0f,
               0.4f, 0.6f, 0.0f,
               0.5f, 0.5f, 0.0f,
               0.5f, 0.5f, 0.0f,
               0.0f, 0.0f, 1.0f} );                                     // clang-format on
    }

    // Uncomment this to have some outputs.
    void printPotential( const gum::Potential<float>& ) {
      // gum::Instantiation inst(p);

      // for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}
