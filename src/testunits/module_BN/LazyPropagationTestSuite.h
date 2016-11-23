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
#include <cmath>
#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/config.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIF/BIFReader.h>

#include <agrum/BN/inference/relevantPotentialsFinderType.h>
#include <agrum/BN/inference/ShaferShenoyInference.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/variableElimination.h>
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

  class LazyInferenceNewTestSuite : public CxxTest::TestSuite {
    public:
    gum::BayesNet<float>* bn;
    gum::NodeId i1, i2, i3, i4, i5;
    gum::Potential<float> *e_i1, *e_i4;

    float __epsilon{1e-6f};

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
      e_i1->fill( 0.0f );
      gum::Instantiation inst_1( *e_i1 );
      inst_1.chgVal( bn->variable( i1 ), 0 );
      e_i1->set( inst_1, 1.0f );

      e_i4 = new gum::Potential<float>();
      ( *e_i4 ) << bn->variable( i4 );
      e_i4->fill( 0.0f );
      gum::Instantiation inst_4( *e_i4 );
      inst_4.chgVal( bn->variable( i4 ), 1 );
      e_i4->set( inst_4, 1.0f );
    }

    void tearDown() {
      delete bn;
      delete e_i1;
      delete e_i4;
    }

    bool equalPotentials( const gum::Potential<float>& p1,
                          const gum::Potential<float>& p2 ) {
      gum::Instantiation i1( p1 );
      gum::Instantiation i2( p2 );

      for ( i1.setFirst(), i2.setFirst(); !i1.end(); i1.inc(), i2.inc() ) {
        if ( ( p1[i1] == 0 ) && ( std::fabs( p2[i2] ) > __epsilon ) )
          return false;
        if ( p1[i1] > p2[i2] ) {
          if ( std::fabs( ( p1[i1] - p2[i2] ) / p1[i1] ) > __epsilon )
            return false;
        } else {
          if ( std::fabs( ( p1[i1] - p2[i2] ) / p1[i2] ) > __epsilon )
            return false;
        }
      }

      return true;
    }

    // Testing when there is no evidence
    void testCreationAndInference() {
      fill( *bn );
      // Testing the inference
      gum::LazyPropagation<float>* inf = 0;
      TS_ASSERT_THROWS_NOTHING( inf = new gum::LazyPropagation<float>( bn ) );
      TS_ASSERT_THROWS_NOTHING( inf->makeInference() );

      if ( inf != 0 ) {
        TS_ASSERT_THROWS_NOTHING( delete inf );
      }
    }

    void testMarginal() {
      fill( *bn );
      gum::LazyPropagation<float> inf( bn );
      gum::ShaferShenoyInference<float> inf2( bn );

      TS_ASSERT_THROWS_NOTHING( inf.makeInference() );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i1 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i2 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i3 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i4 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i5 ) );

      inf.makeInference();
      inf2.makeInference();

      TS_ASSERT( equalPotentials( inf.posterior( i1 ), inf2.posterior( i1 ) ) );
      TS_ASSERT( equalPotentials( inf.posterior( i2 ), inf2.posterior( i2 ) ) );
      TS_ASSERT( equalPotentials( inf.posterior( i3 ), inf2.posterior( i3 ) ) );
      TS_ASSERT( equalPotentials( inf.posterior( i4 ), inf2.posterior( i4 ) ) );
      TS_ASSERT( equalPotentials( inf.posterior( i5 ), inf2.posterior( i5 ) ) );
    }

    void testMarginalWithEvidence() {
      fill( *bn );
      gum::List<const gum::Potential<float>*> e_list;
      e_list.insert( e_i1 );
      e_list.insert( e_i4 );

      gum::LazyPropagation<float> inf( bn );
      gum::ShaferShenoyInference<float> infX( bn );

      for ( auto pot : e_list ) {
        TS_ASSERT_THROWS_NOTHING( inf.addEvidence( *pot ) );
        TS_ASSERT_THROWS_NOTHING( infX.addEvidence( *pot ) );
      }
      
      TS_ASSERT_THROWS_NOTHING( inf.makeInference() );
      TS_ASSERT_THROWS_NOTHING( infX.makeInference() );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i1 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i2 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i3 ) );

      TS_ASSERT_THROWS_NOTHING( inf.posterior( i4 ) );
      TS_ASSERT_THROWS_NOTHING( inf.posterior( i5 ) );

      TS_ASSERT( equalPotentials( inf.posterior( i1 ), infX.posterior( i1 ) ) );
      TS_ASSERT( equalPotentials( inf.posterior( i2 ), infX.posterior( i2 ) ) );
      TS_ASSERT( equalPotentials( inf.posterior( i3 ), infX.posterior( i3 ) ) );
      TS_ASSERT( equalPotentials( inf.posterior( i4 ), infX.posterior( i4 ) ) );
      TS_ASSERT( equalPotentials( inf.posterior( i5 ), infX.posterior( i5 ) ) );

      gum::LazyPropagation<float> inf2( bn );
      gum::ShaferShenoyInference<float> inf2X( bn );

      TS_ASSERT_THROWS_NOTHING( inf2.addEvidence( i1, 0 ) );
      TS_ASSERT_THROWS_NOTHING( inf2.addEvidence( i4, 1 ) );
      TS_ASSERT_THROWS_NOTHING( inf2X.addEvidence( i1, 0 ) );
      TS_ASSERT_THROWS_NOTHING( inf2X.addEvidence( i4, 1 ) );

      TS_ASSERT_THROWS_NOTHING( inf2.makeInference() );
      TS_ASSERT_THROWS_NOTHING( inf2X.makeInference() );
      TS_ASSERT_THROWS_NOTHING( inf2.posterior( i1 ) );
      TS_ASSERT_THROWS_NOTHING( inf2.posterior( i2 ) );
      TS_ASSERT_THROWS_NOTHING( inf2.posterior( i3 ) );
      TS_ASSERT_THROWS_NOTHING( inf2.posterior( i4 ) );
      TS_ASSERT_THROWS_NOTHING( inf2.posterior( i5 ) );

      TS_ASSERT( inf.posterior( i1 ) == inf2.posterior( i1 ) );
      TS_ASSERT( inf.posterior( i2 ) == inf2.posterior( i2 ) );
      TS_ASSERT( inf.posterior( i3 ) == inf2.posterior( i3 ) );
      TS_ASSERT( inf.posterior( i4 ) == inf2.posterior( i4 ) );
      TS_ASSERT( inf.posterior( i5 ) == inf2.posterior( i5 ) );

      TS_ASSERT(
                equalPotentials( inf2.posterior( i1 ), inf2X.posterior( i1 ) ) );
      TS_ASSERT(
                equalPotentials( inf2.posterior( i2 ), inf2X.posterior( i2 ) ) );
      TS_ASSERT(
                equalPotentials( inf2.posterior( i3 ), inf2X.posterior( i3 ) ) );
      TS_ASSERT(
                equalPotentials( inf2.posterior( i4 ), inf2X.posterior( i4 ) ) );
      TS_ASSERT(
                equalPotentials( inf2.posterior( i5 ), inf2X.posterior( i5 ) ) );
    }

    // Testing when there is no evidence
    void testJoint() {
      fill( *bn );
      // Testing the inference
      gum::LazyPropagation<float> inf( bn );
      gum::NodeSet nodeset;
      nodeset.insert( 2 );
      nodeset.insert( 4 );
      inf.addJointTarget( nodeset );

      TS_ASSERT_THROWS_NOTHING( inf.jointPosterior( nodeset ) );
    }

    // Testing when there is no evidence
    void testJoint2() {
      fill( *bn );
      // Testing the inference
      gum::LazyPropagation<float> inf( bn );
      gum::NodeSet nodeset;
      nodeset.insert( 1 );
      nodeset.insert( 2 );
      nodeset.insert( 3 );
      nodeset.insert( 4 );
      inf.addJointTarget( nodeset );

      gum::NodeSet nodeset2;
      nodeset2.insert( 2 );
      nodeset2.insert( 4 );

      TS_ASSERT_THROWS_NOTHING( inf.jointPosterior( nodeset2 ) );
    }

    // testing information methods
    void testInformationMethods() {
      fill( *bn );

      gum::LazyPropagation<float> inf( bn );
      gum::NodeSet nodeset;
      nodeset.insert( 0 );
      nodeset.insert( 4 );
      inf.addJointTarget( nodeset );
      nodeset.clear();
      nodeset.insert( 2 );
      nodeset.insert( 4 );
      inf.addJointTarget( nodeset );
      inf.makeInference();

      TS_GUM_ASSERT_THROWS_NOTHING( inf.H( (gum::NodeId)2 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( inf.I( (gum::NodeId)2, (gum::NodeId)4 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( inf.I( (gum::NodeId)2, (gum::NodeId)2 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( inf.VI( (gum::NodeId)2, (gum::NodeId)4 ) );
      TS_GUM_ASSERT_THROWS_NOTHING( inf.I( (gum::NodeId)0, (gum::NodeId)4 ) );

      for ( const auto node : bn->dag() ) {
        for ( const auto par : bn->dag().parents ( node ) ) {
          TS_GUM_ASSERT_THROWS_NOTHING( inf.I( node, par ) );
          TS_GUM_ASSERT_THROWS_NOTHING( inf.I( par, node ) );
        }
      }

      //@TODO : test computations and not only good behaviour
    }

    void testEvidenceProbability() {
      fill( *bn );
      gum::LazyPropagation<float> inf( bn );
      inf.setRelevantPotentialsFinderType(
           gum::RelevantPotentialsFinderType::FIND_ALL );
      inf.makeInference();
      auto p     = inf.posterior( 0 );
      auto I     = gum::Instantiation( p );
      auto proba = p.get( I );

      inf.addEvidence( 0, 0 );
      inf.makeInference();
      auto proba2 = inf.evidenceProbability();

      TS_ASSERT_DELTA( proba, proba2, 1e-5 );
    }

    void testEvidenceProbabilityAsia() {
      std::string file = GET_RESSOURCES_PATH( "asia.bif" );
      gum::BayesNet<float> bn;
      gum::BIFReader<float> reader( &bn, file );
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );
      TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );

      auto id = bn.idFromName( "lung_cancer?" );

      gum::LazyPropagation<float> inf( &bn );
      inf.setRelevantPotentialsFinderType(
         gum::RelevantPotentialsFinderType::FIND_ALL );
      inf.makeInference();
      auto p     = inf.posterior( id );
      auto I     = gum::Instantiation( p );
      auto proba = p.get( I );

      inf.addEvidence( id, 0 );
      inf.makeInference();
      auto proba2 = inf.evidenceProbability();

      TS_ASSERT_DELTA( proba, proba2, 1e-5 );
    }

    void testAsia() {
      std::string file = GET_RESSOURCES_PATH( "asia.bif" );
      gum::BayesNet<float> bn;
      gum::BIFReader<float> reader( &bn, file );
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );
      TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );

      for ( auto node : bn.dag() ) {
        const auto& variable = bn.variable( node );
        gum::Potential<float> ev_pot;
        ev_pot << variable;
        ev_pot.fill( 0.0f );
        gum::List<const gum::Potential<float>*> evidences;
        evidences.insert( &ev_pot );

        gum::Instantiation inst( ev_pot );
        for ( inst.setFirst(); !inst.end(); ++inst ) {
          ev_pot.set( inst, 1.0f );
          gum::LazyPropagation<float> inf1( &bn );
          gum::ShaferShenoyInference<float> inf2( &bn );
          for ( auto pot : evidences ) {
            TS_ASSERT_THROWS_NOTHING( inf1.addEvidence( *pot ) );
            TS_ASSERT_THROWS_NOTHING( inf2.addEvidence( *pot ) );
          }
          TS_ASSERT_THROWS_NOTHING( inf1.makeInference() );
          TS_ASSERT_THROWS_NOTHING( inf2.makeInference() );
          for ( auto node : bn.dag() ) {
            TS_ASSERT( equalPotentials( inf1.posterior( node ),
                                        inf2.posterior( node ) ) );
          }
          ev_pot.set( inst, (float)0 );
        }
      }
    }

    void testAlarm() {
      std::string file = GET_RESSOURCES_PATH( "alarm.bif" );
      gum::BayesNet<float> bn;
      gum::BIFReader<float> reader( &bn, file );
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );
      TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );

      gum::LazyPropagation<float> inf1( &bn );
      gum::VariableElimination<float> inf2( &bn );

      TS_ASSERT_THROWS_NOTHING( inf1.makeInference() );
      TS_ASSERT_THROWS_NOTHING( inf2.makeInference() );

      for ( auto node : bn.dag() ) {
        TS_ASSERT_THROWS_NOTHING( inf1.posterior( node ) );
        TS_ASSERT_THROWS_NOTHING( inf2.posterior( node ) );
        TS_ASSERT(
            equalPotentials( inf1.posterior( node ), inf2.posterior( node ) ) );
      }

      std::vector<gum::NodeId> ev_nodes{2, 6, 7, 10, 12, 14, 16};
      gum::List<const gum::Potential<float>*> evidences;
      for ( const auto node : ev_nodes ) {
        gum::Potential<float>* ev_pot = new gum::Potential<float>;
        ( *ev_pot ) << bn.variable( node );
        ev_pot->fill( 0.0f );
        gum::Instantiation inst( *ev_pot );
        if ( node <= 10 ) {
          inst.chgVal( bn.variable( node ), 0 );
          ev_pot->set( inst, 1.0f );
        } else {
          inst.chgVal( bn.variable( node ), 0 );
          ev_pot->set( inst, 0.4f );
          inst.chgVal( bn.variable( node ), 1 );
          ev_pot->set( inst, 0.6f );
        }
        evidences.insert( ev_pot );
      }

      gum::LazyPropagation<float> inf3( &bn );
      gum::ShaferShenoyInference<float> inf4( &bn );
      for ( auto pot : evidences ) {
        TS_ASSERT_THROWS_NOTHING( inf1.addEvidence( *pot ) );
        TS_ASSERT_THROWS_NOTHING( inf2.addEvidence( *pot ) );
        TS_ASSERT_THROWS_NOTHING( inf3.addEvidence( *pot ) );
        TS_ASSERT_THROWS_NOTHING( inf4.addEvidence( *pot ) );
      }

      TS_ASSERT_THROWS_NOTHING( inf1.makeInference() );
      TS_ASSERT_THROWS_NOTHING( inf2.makeInference() );
      TS_ASSERT_THROWS_NOTHING( inf3.makeInference() );
      TS_ASSERT_THROWS_NOTHING( inf4.makeInference() );

      for ( auto node : bn.dag() ) {
        TS_ASSERT_THROWS_NOTHING( inf1.posterior( node ) );
        TS_ASSERT_THROWS_NOTHING( inf2.posterior( node ) );
        TS_ASSERT_THROWS_NOTHING( inf3.posterior( node ) );
        TS_ASSERT_THROWS_NOTHING( inf4.posterior( node ) );
        TS_ASSERT(
            equalPotentials( inf1.posterior( node ), inf2.posterior( node ) ) );
        TS_ASSERT(
            equalPotentials( inf1.posterior( node ), inf3.posterior( node ) ) );
        TS_ASSERT(
            equalPotentials( inf1.posterior( node ), inf4.posterior( node ) ) );
      }

      gum::LazyPropagation<float> inf5( &bn );
      inf5.setRelevantPotentialsFinderType(
          gum::RelevantPotentialsFinderType::DSEP_BAYESBALL_NODES );
      for ( auto pot : evidences ) {
        TS_ASSERT_THROWS_NOTHING( inf5.addEvidence( *pot ) );
      }
      TS_ASSERT_THROWS_NOTHING( inf5.makeInference() );
      for ( auto node : bn.dag() ) {
        TS_ASSERT_THROWS_NOTHING( inf5.posterior( node ) );
        TS_ASSERT(
            equalPotentials( inf1.posterior( node ), inf5.posterior( node ) ) );
      }

      for ( auto pot : evidences )
        delete pot;
    }

    void testAsia2() {
      std::string file = GET_RESSOURCES_PATH( "asia3.bif" );
      gum::BayesNet<float> bn;
      gum::BIFReader<float> reader( &bn, file );
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );
      TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );

      for ( auto node : bn.dag() ) {
        const auto& variable = bn.variable( node );
        gum::Potential<float> ev_pot;
        ev_pot << variable;
        ev_pot.fill( 0.0f );

        gum::Instantiation inst( ev_pot );
        for ( inst.setFirst(); !inst.end(); ++inst ) {
          ev_pot.set( inst, 1.0f );

          for ( auto node2 : bn.dag() ) {
            if ( node2 > node ) {
              const auto& variable2 = bn.variable( node2 );
              gum::Potential<float> ev_pot2;
              ev_pot2 << variable2;
              ev_pot2.fill( 0.0f );

              gum::List<const gum::Potential<float>*> evidences;
              evidences.insert( &ev_pot );
              evidences.insert( &ev_pot2 );
              // std::cout << "hard ev: " << node << "  " << node2 << std::endl;

              gum::Instantiation inst2( ev_pot2 );
              for ( inst2.setFirst(); !inst2.end(); ++inst2 ) {
                ev_pot2.set( inst2, 1.0f );

                gum::LazyPropagation<float> inf1( &bn );
                gum::ShaferShenoyInference<float> inf2( &bn );
                for ( auto pot : evidences ) {
                  TS_ASSERT_THROWS_NOTHING( inf1.addEvidence( *pot ) );
                  TS_ASSERT_THROWS_NOTHING( inf2.addEvidence( *pot ) );
                }
                TS_ASSERT_THROWS_NOTHING( inf1.makeInference() );
                TS_ASSERT_THROWS_NOTHING( inf2.makeInference() );

                for ( auto xnode : bn.dag() ) {
                  // std::cout << "node : " << xnode << " : ";
                  // std::cout << "    " << inf1.posterior(xnode) << std::endl
                  //           << "    " << inf2.posterior(xnode) << std::endl;

                  TS_ASSERT( equalPotentials( inf1.posterior( xnode ),
                                              inf2.posterior( xnode ) ) );
                }
                ev_pot2.set( inst2, 0.0f );
              }
            }
          }

          ev_pot.set( inst, 0.0f );
        }
      }
    }

    void testAsia3() {
      std::string file = GET_RESSOURCES_PATH( "asia3.bif" );
      gum::BayesNet<float> bn;
      gum::BIFReader<float> reader( &bn, file );
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );
      TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );

      for ( auto node : bn.dag() ) {
        const auto& variable = bn.variable( node );
        gum::Potential<float> ev_pot;
        ev_pot << variable;
        ev_pot.fill( 0.0f );

        gum::Instantiation inst( ev_pot );
        for ( inst.setFirst(); !inst.end(); ++inst ) {
          ev_pot.set( inst, 1.0f );

          for ( auto node2 : bn.dag() ) {
            if ( node2 > node ) {
              const auto& variable2 = bn.variable( node2 );
              gum::Potential<float> ev_pot2;
              ev_pot2 << variable2;
              ev_pot2.fill( 0.0f );

              gum::List<const gum::Potential<float>*> evidences;
              evidences.insert( &ev_pot );
              evidences.insert( &ev_pot2 );
              // std::cout << "hard ev: " << node << "  " << node2 << std::endl;

              gum::Instantiation inst2( ev_pot2 );
              for ( inst2.setFirst(); !inst2.end(); ++inst2 ) {
                ev_pot2.set( inst2, 1.0f );

                gum::LazyPropagation<float> inf1( &bn );
                inf1.setRelevantPotentialsFinderType(
                    gum::RelevantPotentialsFinderType::
                        DSEP_BAYESBALL_NODES );
                gum::ShaferShenoyInference<float> inf2( &bn );
                for ( auto pot : evidences ) {
                  TS_ASSERT_THROWS_NOTHING( inf1.addEvidence( *pot ) );
                  TS_ASSERT_THROWS_NOTHING( inf2.addEvidence( *pot ) );
                }
                TS_ASSERT_THROWS_NOTHING( inf1.makeInference() );
                TS_ASSERT_THROWS_NOTHING( inf2.makeInference() );

                for ( auto xnode : bn.dag() ) {
                  // std::cout << "node : " << xnode << " : ";
                  // std::cout << "    " << inf1.posterior(xnode) << std::endl
                  //           << "    " << inf2.posterior(xnode) << std::endl;

                  TS_ASSERT( equalPotentials( inf1.posterior( xnode ),
                                              inf2.posterior( xnode ) ) );
                }
                ev_pot2.set( inst2, 0.0f );
              }
            }
          }

          ev_pot.set( inst, 0.0f );
        }
      }
    }

    void testAsia4() {
      std::string file = GET_RESSOURCES_PATH( "asia.bif" );
      gum::BayesNet<float> bn;
      gum::BIFReader<float> reader( &bn, file );
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );
      TS_ASSERT( nbrErr == 0 );
      TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );

      for ( auto node : bn.dag() ) {
        const auto& variable = bn.variable( node );
        gum::Potential<float> ev_pot;
        ev_pot << variable;
        ev_pot.fill( 0.0f );

        gum::Instantiation inst( ev_pot );
        int inst_index = 0;
        for ( inst.setFirst(); !inst.end(); ++inst, ++inst_index ) {
          ev_pot.set( inst, 1.0f );

          for ( auto node2 : bn.dag() ) {
            if ( node2 > node ) {
              const auto& variable2 = bn.variable( node2 );
              gum::Potential<float> ev_pot2;
              ev_pot2 << variable2;
              ev_pot2.fill( 0.0f );

              gum::List<const gum::Potential<float>*> evidences;
              evidences.insert( &ev_pot );
              evidences.insert( &ev_pot2 );
              // std::cout << "hard ev: " << node << "  " << node2 << std::endl;

              gum::Instantiation inst2( ev_pot2 );
              int inst2_index = 0;
              for ( inst2.setFirst(); !inst2.end(); ++inst2, ++inst2_index ) {
                ev_pot2.set( inst2, 1.0f );

                gum::LazyPropagation<float> inf1( &bn );
                inf1.setRelevantPotentialsFinderType(
                    gum::RelevantPotentialsFinderType::
                        DSEP_BAYESBALL_POTENTIALS );
                gum::LazyPropagation<float> inf2( &bn );
                for ( auto pot : evidences ) {
                  TS_ASSERT_THROWS_NOTHING( inf1.addEvidence( *pot ) );
                  TS_ASSERT_THROWS_NOTHING( inf2.addEvidence( *pot ) );
                }
                TS_ASSERT_THROWS_NOTHING( inf1.makeInference() );
                TS_ASSERT_THROWS_NOTHING( inf2.makeInference() );

                for ( auto xnode : bn.dag() ) {
                  // std::cout << "node : " << xnode << " : ";
                  // std::cout << "    " << inf1.posterior(xnode) << std::endl
                  //           << "    " << inf2.posterior(xnode) << std::endl;

                  try {
                    equalPotentials( inf1.posterior( xnode ),
                                     inf2.posterior( xnode ) );
                  } catch ( gum::IncompatibleEvidence& ) {
                    // check evidence incompatibility:
                    if ( node2 == gum::NodeId( 2 ) ) {
                      // node2 = tuberculos_or_cancer?, then node =
                      // tuberculosis?
                      TS_ASSERT( ( inst2_index == 1 ) && ( inst_index == 0 ) );
                    } else {  // node2 = lung_cancer? & node =
                              // tuberculos_or_cancer?
                      TS_ASSERT( ( inst2_index == 0 ) && ( inst_index == 1 ) );
                    }
                  }
                }
                ev_pot2.set( inst2, 0.0f );
              }
            }
          }

          ev_pot.set( inst, 0.0f );
        }
      }
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
      // gum::Instantiation inst(p);

      // for (inst.setFirst(); !inst.end(); ++inst)
      //{
      //  std::cerr << inst<<" : " <<p[inst] << std::endl;
      //}
      // std::cerr << std::endl;
    }
  };
}
