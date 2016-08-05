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

#include <agrum/BN/io/BIF/BIFReader.h>
#include <agrum/BN/inference/VEWithBB.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class VEWithBBTestSuite : public CxxTest::TestSuite {
    public:
    gum::BayesNet<float>* bn;
    gum::Id i1, i2, i3, i4, i5;
    gum::Potential<float>* e_i1, *e_i4;

    void setUp() {
      bn = new gum::BayesNet<float>();

      gum::LabelizedVariable n1( "1", "", 2 ), n2( "2", "", 2 ),
          n3( "3", "", 2 ), n4( "4", "", 2 ), n5( "5", "", 3 );

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

    //// Testing when there is no evidence
    //void testMakeInference() {
    //  fill( *bn );
    //  // Testing the inference
    //  gum::VEWithBB<float>* inf = 0;
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      inf = new gum::VEWithBB<float>( *bn ) );

    //  if ( inf != 0 ) {
    //    TS_GUM_ASSERT_THROWS_NOTHING( inf->makeInference() );
    //    TS_GUM_ASSERT_THROWS_NOTHING( delete inf );
    //  }
    //}

    //void testVEWithBB() {
    //  fill( *bn );
    //  gum::VEWithBB<float>* inf = 0;
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      inf = new gum::VEWithBB<float>( *bn ) );

    //  if ( inf != 0 ) {
    //    TS_GUM_ASSERT_THROWS_NOTHING( inf->makeInference() );

    //    const gum::Potential<float>* posterior_1 = 0;
    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_1 = &( inf->posterior( i1 ) ) );

    //    if ( posterior_1 != 0 ) printProba( *posterior_1 );

    //    const gum::Potential<float>* posterior_2 = 0;

    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_2 = &( inf->posterior( i2 ) ) );

    //    if ( posterior_2 != 0 ) printProba( *posterior_2 );

    //    const gum::Potential<float>* posterior_3 = 0;

    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_3 = &( inf->posterior( i3 ) ) );

    //    if ( posterior_3 != 0 ) printProba( *posterior_3 );

    //    const gum::Potential<float>* posterior_4 = 0;

    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_4 = &( inf->posterior( i4 ) ) );

    //    if ( posterior_4 != 0 ) printProba( *posterior_4 );

    //    const gum::Potential<float>* posterior_5 = 0;

    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_5 = &( inf->posterior( i5 ) ) );

    //    if ( posterior_5 != 0 ) printProba( *posterior_5 );

    //    TS_GUM_ASSERT_THROWS_NOTHING( delete inf );
    //  }
    //}

    //void testShaferShenoyInf_3() {
    //  fill( *bn );
    //  gum::List<const gum::Potential<float>*> e_list;
    //  e_list.insert( e_i1 );
    //  e_list.insert( e_i4 );

    //  gum::VEWithBB<float>* inf = 0;
    //  TS_GUM_ASSERT_THROWS_NOTHING(
    //      inf = new gum::VEWithBB<float>( *bn ) );

    //  if ( inf != 0 ) {
    //    TS_GUM_ASSERT_THROWS_NOTHING( inf->insertEvidence( e_list ) );

    //    const gum::Potential<float>* posterior_1 = 0;
    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_1 = &( inf->posterior( i1 ) ) );

    //    if ( posterior_1 != 0 ) printProba( *posterior_1 );

    //    const gum::Potential<float>* posterior_2 = 0;

    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_2 = &( inf->posterior( i2 ) ) );

    //    if ( posterior_2 != 0 ) printProba( *posterior_2 );

    //    const gum::Potential<float>* posterior_3 = 0;

    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_3 = &( inf->posterior( i3 ) ) );

    //    if ( posterior_3 != 0 ) printProba( *posterior_3 );

    //    const gum::Potential<float>* posterior_4 = 0;

    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_4 = &( inf->posterior( i4 ) ) );

    //    if ( posterior_4 != 0 ) printProba( *posterior_4 );

    //    const gum::Potential<float>* posterior_5 = 0;

    //    TS_GUM_ASSERT_THROWS_NOTHING( posterior_5 = &( inf->posterior( i5 ) ) );

    //    if ( posterior_5 != 0 ) printProba( *posterior_5 );

    //    TS_GUM_ASSERT_THROWS_NOTHING( delete inf );
    //  }
    //}

    //void testAsiaWithHardEvidence() {
    //  // Arrange
    //  std::string file = GET_RESSOURCES_PATH( "asia.bif" );
    //  gum::BayesNet<double> bn;
    //  gum::BIFReader<double> reader( &bn, file );
    //  TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
    //  gum::ShaferShenoyInference<double> ss( bn );
    //  gum::VEWithBB<double> vebb( bn );
    //  gum::Potential<double> p_vebb, p_ss;
    //  auto e_id = bn.idFromName( "bronchitis?" );
    //  auto inf_list =
    //      std::vector<gum::BayesNetInference<double>*>{&ss, &vebb};
    //  for ( auto inf: inf_list ) {
    //    inf->addHardEvidence( e_id, 0 );
    //  }
    //  for ( auto var_id : bn.nodes() ) {
    //    // Act
    //    TS_GUM_ASSERT_THROWS_NOTHING( p_ss = ss.posterior( var_id ) );
    //    TS_GUM_ASSERT_THROWS_NOTHING( p_vebb = vebb.posterior( var_id ) );
    //    // Assert
    //    TS_ASSERT_EQUALS( p_vebb.domainSize(), p_ss.domainSize() );

    //    gum::Instantiation i_ve( p_vebb );
    //    for ( gum::Instantiation i( p_vebb ); not i.end(); i.inc() ) {
    //      TS_ASSERT_DELTA( p_vebb[i], p_ss[i], 1e-6 );
    //      if ( std::abs(p_vebb[i] - p_ss[i]) > 1e-6 ) {
    //        GUM_TRACE( bn.variable( var_id ).name() );
    //      }
    //    }
    //  }
    //}

    void testAsiaWithHardEvidence() {
      // Arrange
      std::string file = GET_RESSOURCES_PATH( "asia.bif" );
      gum::BayesNet<double> bn;
      gum::BIFReader<double> reader( &bn, file );
      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
      gum::ShaferShenoyInference<double> ss( bn );
      gum::VEWithBB<double> vebb( bn );
      gum::Potential<double> p_vebb, p_ss;
      auto e_id = bn.idFromName( "bronchitis?" );
      auto inf_list = std::vector<gum::BayesNetInference<double>*>{&ss, &vebb};
      for ( auto inf : inf_list ) {
        inf->addHardEvidence( e_id, 0 );
      }
      auto var_id = bn.idFromName( "tuberculos_or_cancer?" );
      // Act
      TS_GUM_ASSERT_THROWS_NOTHING( p_ss = ss.posterior( var_id ) );
      TS_GUM_ASSERT_THROWS_NOTHING( p_vebb = vebb.posterior( var_id ) );
      // Assert
      TS_ASSERT_EQUALS( p_vebb.domainSize(), p_ss.domainSize() );

      gum::Instantiation i_ve( p_vebb );
      for ( gum::Instantiation i( p_vebb ); not i.end(); i.inc() ) {
        TS_ASSERT_DELTA( p_vebb[i], p_ss[i], 1e-6 );
      }
    }

    private:
    // Builds a BN to test the inference
    void fill( gum::BayesNet<float>& bn ) {
      // FILLING PARAMS
      bn.cpt( i1 ).fillWith( {0.2, 0.8} );
      bn.cpt( i2 ).fillWith( {0.3, 0.7} );
      bn.cpt( i3 ).fillWith( {0.1, 0.9, 0.9, 0.1} );
      bn.cpt( i4 ).fillWith( {// clang-format off
                              0.4, 0.6,
                              0.5, 0.5,
                              0.5, 0.5,
                              1.0, 0.0}  // clang-format on
                             );
      bn.cpt( i5 ).fillWith( {// clang-format off
                0.3, 0.6, 0.1,
                0.5, 0.5, 0.0,
                0.5, 0.5, 0.0,
                1.0, 0.0, 0.0,
                0.4, 0.6, 0.0,
                0.5, 0.5, 0.0,
                0.5, 0.5, 0.0,
                0.0, 0.0, 1.0}  //clang-format on
       );
    }

    // Uncomment this to have some outputs.
    void printProba( const gum::Potential<float>& p ) {
      /*gum::Instantiation inst( p );
      for ( inst.setFirst(); !inst.end(); ++inst ) {
        std::cerr << inst << " : " << p[inst] << std::endl;
      }
      std::cerr << std::endl;*/
    }
  };
}
