
/***************************************************************************
 *   Copyright (C) 2017 by Pierre-Henri WUILLEMIN and Christophe GONZALES   *
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


/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   (gumSize) 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.   *
 ***************************************************************************/
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/ID/generator/influenceDiagramGenerator.h>
#include <agrum/ID/inference/influenceDiagramInference.h>
#include <agrum/ID/influenceDiagram.h>
#include <agrum/ID/io/BIFXML/BIFXMLIDWriter.h>
#include <agrum/graphs/graphElements.h>
#include <agrum/variables/discreteVariable.h>
#include <agrum/variables/labelizedVariable.h>

// The graph used for the tests:
//           D1
//           |   D1 -> C1
//           C1
//          /  \  C1 -> C2 & C1 -> U1
//        C2   U1
//       /  \ /  C2 -> D3 & C2 -> D2 & D2 -> U1
//      D3  D2
//      |    |  D3 -> C3 & D2 -> C4
//     C3    C4
//       \  /  C3 -> C5 & C4 -> C5
//        C5   D4
//         \  /  C5 -> U2 & D4 -> U2
//          U2

namespace gum_tests {

  class InfluenceDiagramInferenceTestSuite : public CxxTest::TestSuite {
    private:
    void fillTopoOilWildcater(gum::InfluenceDiagram< double >& id,
                              gum::List< gum::NodeId >&        idList) {
      try {
        idList.insert(id.addDecisionNode(*TestVar));             // 0
        idList.insert(id.addDecisionNode(*DrillVar));            // 1
        idList.insert(id.addChanceNode(*ResultTestVar));         // 2
        idList.insert(id.addChanceNode(*OilAmountVar));          // 3
        idList.insert(id.addUtilityNode(*TestUtilityVar));       // 4
        idList.insert(id.addUtilityNode(*DrillingUtilityVar));   // 5

        id.addArc(idList[0], idList[4]);
        id.addArc(idList[0], idList[2]);
        id.addArc(idList[0], idList[1]);
        id.addArc(idList[1], idList[5]);
        id.addArc(idList[2], idList[1]);
        id.addArc(idList[3], idList[2]);
        id.addArc(idList[3], idList[5]);

      } catch (gum::Exception& e) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void populateOilWildcater(gum::InfluenceDiagram< double >& id,
                              gum::List< gum::NodeId >&        idList) {
      fillTopoOilWildcater(id, idList);

      try {
        id.cpt(idList[3]).populate({0.5f, 0.3f, 0.2f});
        id.cpt(idList[2]).populate(   // clang-format off
                              {0.6f, 0.3f, 0.1f, 0,
                               0  , 0  , 0  , 1,
                               0.3f, 0.4f, 0.3f, 0,
                               0  , 0  , 0  , 1,
                               0.1f, 0.4f, 0.5f, 0,
                               0  , 0  , 0  , 1}); //clang-format on
        id.utility( idList[4] ).populate({-10, 0});
        id.utility( idList[5] ).populate({-70, 0, 50, 0, 200, 0});
      } catch ( gum::Exception& e ) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    void fillTopoDecAsia( gum::InfluenceDiagram<double>& id,
                          gum::List<gum::NodeId>& idList ) {
      try {
        idList.insert( id.addDecisionNode( *HospitalizeVar ) );            // 0
        idList.insert( id.addDecisionNode( *TakeXRayVar ) );               // 1
        idList.insert( id.addChanceNode( *SmokingVar ) );                  // 2
        idList.insert( id.addChanceNode( *BronchitisVar ) );               // 3
        idList.insert( id.addChanceNode( *LungCancerVar ) );               // 4
        idList.insert( id.addChanceNode( *EitherVar ) );                   // 5
        idList.insert( id.addChanceNode( *DyspnoeaVar ) );                 // 6
        idList.insert( id.addChanceNode( *PositiveXRayVar ) );             // 7
        idList.insert( id.addChanceNode( *TubercolisisVar ) );             // 8
        idList.insert( id.addChanceNode( *VisitAsiaVar ) );                // 9
        idList.insert( id.addUtilityNode( *HospitalizationUtilityVar ) );  // 10
        idList.insert( id.addUtilityNode( *TakingXRayUtilityVar ) );       // 11

        id.addArc( idList[0], idList[10] );
        id.addArc( idList[0], idList[1] );

        id.addArc( idList[1], idList[7] );
        id.addArc( idList[1], idList[11] );

        id.addArc( idList[2], idList[3] );
        id.addArc( idList[2], idList[4] );

        id.addArc( idList[3], idList[6] );

        id.addArc( idList[4], idList[10] );
        id.addArc( idList[4], idList[5] );

        id.addArc( idList[5], idList[6] );
        id.addArc( idList[5], idList[7] );

        id.addArc( idList[8], idList[10] );
        id.addArc( idList[8], idList[5] );
        id.addArc( idList[8], idList[11] );

        id.addArc( idList[9], idList[8] );

      } catch ( gum::Exception& e ) {
        std::cerr << std::endl << e.errorCallStack() << std::endl;
        throw;
      }
    }

    void populateDecAsia( gum::InfluenceDiagram<double>& id,
                          gum::List<gum::NodeId>& idList ) {
      fillTopoDecAsia( id, idList );

      try {
        id.cpt( idList[9] ).populate({0.01f, 0.99f});
        id.cpt( idList[4] ).populate({0.1f, 0.9f, 0.01f, 0.99f});
        id.cpt( idList[3] ).populate({0.6f, 0.4f, 0.3f, 0.7f});
        id.cpt( idList[2] ).populate({0.5f, 0.5f});
        id.cpt( idList[6] ).populate({0.9f, 0.1f, 0.7f, 0.3f, 0.8f, 0.2f, 0.1f, 0.9f});
        id.cpt( idList[8] ).populate({0.05f, 0.95f, 0.01f, 0.99f});
        id.cpt( idList[5] ).populate({1, 0, 1, 0, 1, 0, 0, 1});
        id.cpt( idList[7] ).populate({0.98f, 0.02f, 0.5f, 0.5f, 0.05f, 0.95f, 0.5f, 0.5f});

        id.utility( idList[10] ).populate({180, 2, 120, 4, 160, 0, 15, 40});
        id.utility( idList[11] ).populate({0, 10, 1, 10});
      } catch ( gum::Exception& e ) {
        std::cerr << std::endl << e.errorContent() << std::endl;
        throw;
      }
    }

    public:
    // OilWildcater test variable
    gum::LabelizedVariable *TestVar, *DrillVar;
    gum::LabelizedVariable *ResultTestVar, *OilAmountVar;
    gum::LabelizedVariable *TestUtilityVar, *DrillingUtilityVar;

    // Dec-Asia test variable
    gum::LabelizedVariable *HospitalizeVar, *TakeXRayVar;
    gum::LabelizedVariable *SmokingVar, *BronchitisVar, *LungCancerVar,
        *EitherVar, *DyspnoeaVar, *PositiveXRayVar, *TubercolisisVar,
        *VisitAsiaVar;
    gum::LabelizedVariable *HospitalizationUtilityVar, *TakingXRayUtilityVar;

    void setUp() {
      TestVar = new gum::LabelizedVariable( "T", "Test", 2 );
      TestVar->eraseLabels();
      TestVar->addLabel( "Yes" );
      TestVar->addLabel( "No" );
      DrillVar = new gum::LabelizedVariable( "D", "Drill", 2 );
      DrillVar->eraseLabels();
      DrillVar->addLabel( "Yes" );
      DrillVar->addLabel( "No" );
      ResultTestVar = new gum::LabelizedVariable( "R", "Result of test", 4 );
      ResultTestVar->eraseLabels();
      ResultTestVar->addLabel( "NoS" );
      ResultTestVar->addLabel( "OpS" );
      ResultTestVar->addLabel( "ClS" );
      ResultTestVar->addLabel( "NoR" );
      OilAmountVar = new gum::LabelizedVariable( "O", "Oil Amount", 3 );
      OilAmountVar->eraseLabels();
      OilAmountVar->addLabel( "Dry" );
      OilAmountVar->addLabel( "Wet" );
      OilAmountVar->addLabel( "Soak" );
      TestUtilityVar =
          new gum::LabelizedVariable( "Ut", "Utility of Testing", 1 );
      DrillingUtilityVar =
          new gum::LabelizedVariable( "Ud", "Utility of Drilling", 1 );

      HospitalizeVar = new gum::LabelizedVariable( "H", "Hospitalize?", 2 );
      HospitalizeVar->eraseLabels();
      HospitalizeVar->addLabel( "Yes" );
      HospitalizeVar->addLabel( "No" );
      TakeXRayVar = new gum::LabelizedVariable( "Ta", "Take X-Ray?", 2 );
      TakeXRayVar->eraseLabels();
      TakeXRayVar->addLabel( "Yes" );
      TakeXRayVar->addLabel( "No" );
      SmokingVar = new gum::LabelizedVariable( "S", "Smoking?", 2 );
      SmokingVar->eraseLabels();
      SmokingVar->addLabel( "Yes" );
      SmokingVar->addLabel( "No" );
      BronchitisVar = new gum::LabelizedVariable( "B", "Bronchitis?", 2 );
      BronchitisVar->eraseLabels();
      BronchitisVar->addLabel( "Yes" );
      BronchitisVar->addLabel( "No" );
      LungCancerVar = new gum::LabelizedVariable( "L", "Lung Cancer?", 2 );
      LungCancerVar->eraseLabels();
      LungCancerVar->addLabel( "Yes" );
      LungCancerVar->addLabel( "No" );
      EitherVar = new gum::LabelizedVariable(
          "E", "Either tuberculosis or lung cancer?", 2 );
      EitherVar->eraseLabels();
      EitherVar->addLabel( "Yes" );
      EitherVar->addLabel( "No" );
      DyspnoeaVar = new gum::LabelizedVariable( "D", "Dyspnoea?", 2 );
      DyspnoeaVar->eraseLabels();
      DyspnoeaVar->addLabel( "Yes" );
      DyspnoeaVar->addLabel( "No" );
      PositiveXRayVar = new gum::LabelizedVariable( "P", "Positive X-Ray?", 2 );
      PositiveXRayVar->eraseLabels();
      PositiveXRayVar->addLabel( "Yes" );
      PositiveXRayVar->addLabel( "No" );
      TubercolisisVar = new gum::LabelizedVariable( "Tu", "Tuberculosis?", 2 );
      TubercolisisVar->eraseLabels();
      TubercolisisVar->addLabel( "Yes" );
      TubercolisisVar->addLabel( "No" );
      VisitAsiaVar = new gum::LabelizedVariable( "A", "Visit to Asia?", 2 );
      VisitAsiaVar->eraseLabels();
      VisitAsiaVar->addLabel( "Yes" );
      VisitAsiaVar->addLabel( "No" );
      HospitalizationUtilityVar =
          new gum::LabelizedVariable( "Uh", "Utility of Hospitalization", 1 );
      TakingXRayUtilityVar =
          new gum::LabelizedVariable( "Ut", "Utility of Taking XRay", 1 );
    }

    void tearDown() {
      delete TestVar;
      delete DrillVar;
      delete ResultTestVar;
      delete OilAmountVar;
      delete TestUtilityVar;
      delete DrillingUtilityVar;

      delete HospitalizeVar;
      delete TakeXRayVar;
      delete SmokingVar;
      delete BronchitisVar;
      delete LungCancerVar;
      delete EitherVar;
      delete DyspnoeaVar;
      delete PositiveXRayVar;
      delete TubercolisisVar;
      delete VisitAsiaVar;
      delete HospitalizationUtilityVar;
      delete TakingXRayUtilityVar;
    }

    void testConstructor() {
      gum::InfluenceDiagram<double>* topology = nullptr;
      gum::List<gum::NodeId> idList;
      TS_GUM_ASSERT_THROWS_NOTHING( topology =
                                        new gum::InfluenceDiagram<double>() );
      TS_GUM_ASSERT_THROWS_NOTHING( populateDecAsia( *topology, idList ) );

      gum::InfluenceDiagramInference<double>* dIDI = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          dIDI = new gum::InfluenceDiagramInference<double>( *topology ) );
      TS_GUM_ASSERT_THROWS_NOTHING( delete dIDI );
      delete topology;
    }

    void testStrongJunctionTree() {
      gum::InfluenceDiagram<double> id;
      gum::List<gum::NodeId> idList;

      gum::LabelizedVariable dVar1( "decisionVar1", "D1", 2 );
      gum::LabelizedVariable dVar2( "decisionVar2", "D2", 2 );
      gum::LabelizedVariable dVar3( "decisionVar3", "D3", 2 );
      gum::LabelizedVariable dVar4( "decisionVar4", "D4", 2 );
      gum::LabelizedVariable cVar1( "A", "a", 2 );
      gum::LabelizedVariable cVar2( "B", "b", 2 );
      gum::LabelizedVariable cVar3( "C", "c", 2 );
      gum::LabelizedVariable cVar4( "D", "d", 2 );
      gum::LabelizedVariable cVar5( "E", "e", 2 );
      gum::LabelizedVariable cVar6( "F", "f", 2 );
      gum::LabelizedVariable cVar7( "G", "g", 2 );
      gum::LabelizedVariable cVar8( "H", "h", 2 );
      gum::LabelizedVariable cVar9( "I", "i", 2 );
      gum::LabelizedVariable cVar10( "J", "j", 2 );
      gum::LabelizedVariable cVar11( "K", "k", 2 );
      gum::LabelizedVariable cVar12( "L", "l", 2 );
      gum::LabelizedVariable uVar1( "utilityVar1", "U1", 1 );
      gum::LabelizedVariable uVar2( "utilityVar2", "U2", 1 );
      gum::LabelizedVariable uVar3( "utilityVar3", "U3", 1 );
      gum::LabelizedVariable uVar4( "utilityVar4", "U4", 1 );

      idList.insert( id.addDecisionNode( dVar1 ) );  // 0
      idList.insert( id.addDecisionNode( dVar2 ) );  // 1
      idList.insert( id.addDecisionNode( dVar3 ) );  // 2
      idList.insert( id.addDecisionNode( dVar4 ) );  // 3
      idList.insert( id.addChanceNode( cVar1 ) );    // 4
      idList.insert( id.addChanceNode( cVar2 ) );    // 5
      idList.insert( id.addChanceNode( cVar3 ) );    // 6
      idList.insert( id.addChanceNode( cVar4 ) );    // 7
      idList.insert( id.addChanceNode( cVar5 ) );    // 8
      idList.insert( id.addChanceNode( cVar6 ) );    // 9
      idList.insert( id.addChanceNode( cVar7 ) );    // 10
      idList.insert( id.addChanceNode( cVar8 ) );    // 11
      idList.insert( id.addChanceNode( cVar9 ) );    // 12
      idList.insert( id.addChanceNode( cVar10 ) );   // 13
      idList.insert( id.addChanceNode( cVar11 ) );   // 14
      idList.insert( id.addChanceNode( cVar12 ) );   // 15
      idList.insert( id.addUtilityNode( uVar1 ) );   // 16
      idList.insert( id.addUtilityNode( uVar2 ) );   // 17
      idList.insert( id.addUtilityNode( uVar3 ) );   // 18
      idList.insert( id.addUtilityNode( uVar4 ) );   // 19

      id.addArc( idList[4], idList[6] );
      id.addArc( idList[5], idList[6] );
      id.addArc( idList[5], idList[7] );
      id.addArc( idList[5], idList[0] );
      id.addArc( idList[0], idList[7] );
      id.addArc( idList[0], idList[16] );
      id.addArc( idList[6], idList[8] );
      id.addArc( idList[7], idList[8] );
      id.addArc( idList[7], idList[9] );
      id.addArc( idList[8], idList[10] );
      id.addArc( idList[8], idList[1] );
      id.addArc( idList[9], idList[1] );
      id.addArc( idList[9], idList[11] );
      id.addArc( idList[10], idList[3] );
      id.addArc( idList[10], idList[12] );
      id.addArc( idList[1], idList[12] );
      id.addArc( idList[1], idList[2] );
      id.addArc( idList[3], idList[15] );
      id.addArc( idList[12], idList[15] );
      id.addArc( idList[11], idList[13] );
      id.addArc( idList[11], idList[14] );
      id.addArc( idList[2], idList[14] );
      id.addArc( idList[2], idList[17] );
      id.addArc( idList[2], idList[3] );
      id.addArc( idList[13], idList[18] );
      id.addArc( idList[14], idList[18] );
      id.addArc( idList[15], idList[19] );

      gum::NullStream devnull;
      gum::InfluenceDiagramInference<double> dIDI( id );
      dIDI.displayStrongJunctionTree( devnull );
    }

    void testInferenceWithOilWildCater() {
      gum::InfluenceDiagram<double>* topology = nullptr;
      gum::List<gum::NodeId> idList;
      topology = new gum::InfluenceDiagram<double>();
      populateOilWildcater( *topology, idList );

      gum::InfluenceDiagramInference<double>* dIDI = nullptr;
      dIDI = new gum::InfluenceDiagramInference<double>( *topology );

      TS_GUM_ASSERT_THROWS_NOTHING( dIDI->makeInference() );

      delete dIDI;
      delete topology;
    }

    void testInferenceWithDecAsia() {
      gum::InfluenceDiagram<double>* topology = nullptr;
      gum::List<gum::NodeId> idList;
      topology = new gum::InfluenceDiagram<double>();
      populateDecAsia( *topology, idList );
      gum::NullStream devnull;

      gum::InfluenceDiagramInference<double>* dIDI = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          dIDI = new gum::InfluenceDiagramInference<double>( *topology ) );

      TS_ASSERT_THROWS( dIDI->getMEU(), gum::OperationNotAllowed );
      TS_ASSERT_THROWS( dIDI->getBestDecisionChoice( idList[0] ),
                        gum::OperationNotAllowed );
      TS_ASSERT_THROWS( dIDI->displayResult(), gum::OperationNotAllowed );
      TS_GUM_ASSERT_THROWS_NOTHING( dIDI->makeInference() );
      TS_GUM_ASSERT_THROWS_NOTHING( dIDI->getMEU() );
      TS_GUM_ASSERT_THROWS_NOTHING( dIDI->getBestDecisionChoice( idList[0] ) );
      TS_ASSERT_THROWS( dIDI->getBestDecisionChoice( idList[2] ),
                        gum::InvalidNode );
      TS_GUM_ASSERT_THROWS_NOTHING( dIDI->displayResult() );

      TS_GUM_ASSERT_THROWS_NOTHING( dIDI->makeInference() );
      TS_GUM_ASSERT_THROWS_NOTHING( dIDI->displayResult() );

      delete dIDI;
      delete topology;
    }

    void testInferenceWithOilWildCaterAndEvidence() {
      gum::InfluenceDiagram<double>* topology =
          new gum::InfluenceDiagram<double>();
      gum::List<gum::NodeId> idList;
      populateOilWildcater( *topology, idList );

      gum::Potential<double>* evidence1 = new gum::Potential<double>();
      gum::Potential<double>* evidence2 = new gum::Potential<double>();

      gum::List<const gum::Potential<double>*> e_list;
      e_list.insert( evidence1 );
      e_list.insert( evidence2 );

      gum::InfluenceDiagramInference<double> inf( *topology );

      evidence1->add( topology->variable( idList[2] ) );
      evidence1->add( topology->variable( idList[3] ) );
      TS_ASSERT_THROWS( inf.insertEvidence( e_list ),
                        gum::OperationNotAllowed );
      evidence1->erase( topology->variable( idList[3] ) );
      evidence2->add( topology->variable( idList[3] ) );

      TS_GUM_ASSERT_THROWS_NOTHING( evidence1->populate( {0.2f, 0.3f, 0.1f, 0.4f} ) );
      evidence2->populate( {0.2f, 0.3f, 0.5f} );

      TS_GUM_ASSERT_THROWS_NOTHING( inf.insertEvidence( e_list ) );

      TS_GUM_ASSERT_THROWS_NOTHING( inf.makeInference() );

      TS_GUM_ASSERT_THROWS_NOTHING( inf.eraseEvidence( evidence1 ) );

      TS_GUM_ASSERT_THROWS_NOTHING( inf.eraseAllEvidence() );

      delete topology;
      delete evidence1;
      delete evidence2;
    }
  };
}
