/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/learning/database/DBTranslator4LabelizedVariable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/aprioris/aprioriNoApriori2.h>

namespace gum_tests {

  class AprioriNoAprioriTestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      // create the translator set
      gum::LabelizedVariable var1("X1", "", 0);
      var1.addLabel("0");
      var1.addLabel("1");
      var1.addLabel("2");
      
      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("0");
      var2.addLabel("1");
      var2.addLabel("2");
      var2.addLabel("3");
      
      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string > miss;
        gum::learning::DBTranslator4LabelizedVariable<> translator1 (var1,miss);
        gum::learning::DBTranslator4LabelizedVariable<> translator2 (var2,miss);
        std::vector<std::string> names {"A","B","C","D","E","F"};
        
        for ( std::size_t i = std::size_t(0); i< names.size(); ++i ) {
          //translator.setName ( name );
          if ( i % 2 == 0 ) {
            trans_set.insertTranslator ( translator1, i );
          }
          else {
            trans_set.insertTranslator ( translator2, i );
          }
        }
      }

      gum::learning::DatabaseTable<> database(trans_set);


      gum::learning::AprioriNoApriori2<> apriori ( database );
      TS_ASSERT ( apriori.weight () == 0.0 );
      apriori.setWeight ( 4.0 );
      TS_ASSERT ( apriori.weight () == 0.0 );

      TS_ASSERT ( apriori.getType() == "NoApriori" );
      TS_ASSERT ( apriori.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori.isOfType("Smoothing") );

      gum::NodeId node0 = 0;
      gum::NodeId node1 = 1;
      gum::NodeId node3 = 3;
      gum::NodeId node4 = 4;
      gum::NodeId node5 = 5;
      std::vector<gum::NodeId> cond_empty;
      std::vector<gum::NodeId> cond1 { node3, node5, node4};

      gum::learning::IdSet2<> idset1 (node0,cond_empty);           // #3,#0
      gum::learning::IdSet2<> idset2 (node0,node1,cond_empty,true);// #12,#0
      gum::learning::IdSet2<> idset3 (node1,node0,cond1,true,true);// #576,#48

      std::vector<double> vect = apriori.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(3) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(12) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(48) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      
      gum::learning::AprioriNoApriori2<> apriori2 ( apriori );
      TS_ASSERT ( apriori2.weight () == 0.0 );
      apriori2.setWeight ( 4.0 );
      TS_ASSERT ( apriori2.weight () == 0.0 );

      TS_ASSERT ( apriori2.getType() == "NoApriori" );
      TS_ASSERT ( apriori2.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori2.isOfType("Smoothing") );
      
      vect = apriori2.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(3) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori2.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori2.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(12) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori2.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori2.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori2.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(48) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }

      
      gum::learning::AprioriNoApriori2<> apriori3 ( std::move(apriori2) );
      TS_ASSERT ( apriori3.weight () == 0.0 );
      apriori3.setWeight ( 4.0 );
      TS_ASSERT ( apriori3.weight () == 0.0 );

      TS_ASSERT ( apriori3.getType() == "NoApriori" );
      TS_ASSERT ( apriori3.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori3.isOfType("Smoothing") );
      
      vect = apriori3.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(3) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori3.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori3.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(12) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori3.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori3.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori3.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(48) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }

      
      gum::learning::AprioriNoApriori2<>* apriori4 = apriori.clone ();
      TS_ASSERT ( apriori4->weight () == 0.0 );
      apriori4->setWeight ( 4.0 );
      TS_ASSERT ( apriori4->weight () == 0.0 );

      TS_ASSERT ( apriori4->getType() == "NoApriori" );
      TS_ASSERT ( apriori4->isOfType("NoApriori") );
      TS_ASSERT ( ! apriori4->isOfType("Smoothing") );
      
      vect = apriori4->getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(3) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori4->getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori4->getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(12) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori4->getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori4->getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori4->getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(48) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }

      delete apriori4;

      gum::learning::DatabaseTable<> database2;
      gum::learning::AprioriNoApriori2<> apriori5 ( database2 );
      apriori5 = apriori;
      TS_ASSERT ( apriori5.weight () == 0.0 );
      apriori5.setWeight ( 4.0 );
      TS_ASSERT ( apriori5.weight () == 0.0 );

      TS_ASSERT ( apriori5.getType() == "NoApriori" );
      TS_ASSERT ( apriori5.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori5.isOfType("Smoothing") );
      
      vect = apriori5.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(3) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori5.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(12) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori5.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(48) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }

      apriori5 = std::move ( apriori );
      TS_ASSERT ( apriori5.weight () == 0.0 );
      apriori5.setWeight ( 4.0 );
      TS_ASSERT ( apriori5.weight () == 0.0 );

      TS_ASSERT ( apriori5.getType() == "NoApriori" );
      TS_ASSERT ( apriori5.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori5.isOfType("Smoothing") );
      
      vect = apriori5.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(3) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori5.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(12) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori5.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(48) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
    }


    void test2 () {
      // create the translator set
      gum::LabelizedVariable var1("X1", "", 0);
      var1.addLabel("0");
      var1.addLabel("1");
      var1.addLabel("2");
      
      gum::LabelizedVariable var2("X2", "", 0);
      var2.addLabel("0");
      var2.addLabel("1");
      var2.addLabel("2");
      var2.addLabel("3");
      
      gum::learning::DBTranslatorSet<> trans_set;
      {
        const std::vector< std::string > miss;
        gum::learning::DBTranslator4LabelizedVariable<> translator1 (var1,miss);
        gum::learning::DBTranslator4LabelizedVariable<> translator2 (var2,miss);
        std::vector<std::string> names {"A","B","C","D","E","F"};
        
        for ( std::size_t i = std::size_t(0); i< names.size(); ++i ) {
          //translator.setName ( name );
          if ( i % 2 == 0 ) {
            trans_set.insertTranslator ( translator1, i );
          }
          else {
            trans_set.insertTranslator ( translator2, i );
          }
        }
      }

      gum::learning::DatabaseTable<> database(trans_set);

      gum::NodeId node0 = 0;
      gum::NodeId node1 = 1;
      gum::NodeId node2 = 2;
      gum::NodeId node3 = 3;
      gum::NodeId node4 = 4;
      gum::NodeId node5 = 5;
      gum::Bijection<gum::NodeId,std::size_t> nodeId2columns;
      nodeId2columns.insert ( node0, std::size_t(1) );
      nodeId2columns.insert ( node1, std::size_t(3) );
      nodeId2columns.insert ( node2, std::size_t(0) );
      nodeId2columns.insert ( node3, std::size_t(2) );
      nodeId2columns.insert ( node4, std::size_t(4) );
      nodeId2columns.insert ( node5, std::size_t(5) );

      
      gum::learning::AprioriNoApriori2<> apriori ( database, nodeId2columns );
      TS_ASSERT ( apriori.weight () == 0.0 );
      apriori.setWeight ( 4.0 );
      TS_ASSERT ( apriori.weight () == 0.0 );

      TS_ASSERT ( apriori.getType() == "NoApriori" );
      TS_ASSERT ( apriori.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori.isOfType("Smoothing") );

      std::vector<gum::NodeId> cond_empty;
      std::vector<gum::NodeId> cond1 { node3, node5, node4};

      gum::learning::IdSet2<> idset1 (node0,cond_empty);           // #4,#0
      gum::learning::IdSet2<> idset2 (node0,node1,cond_empty,true);// #16,#0
      gum::learning::IdSet2<> idset3 (node1,node0,cond1,true,true);// #576,#36

      std::vector<double> vect = apriori.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(4) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(16) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(36) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }

      
      gum::learning::AprioriNoApriori2<> apriori2 ( apriori );
      TS_ASSERT ( apriori2.weight () == 0.0 );
      apriori2.setWeight ( 4.0 );
      TS_ASSERT ( apriori2.weight () == 0.0 );

      TS_ASSERT ( apriori2.getType() == "NoApriori" );
      TS_ASSERT ( apriori2.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori2.isOfType("Smoothing") );

      vect = apriori2.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(4) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori2.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori2.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(16) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori2.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori2.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori2.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(36) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }

      gum::learning::AprioriNoApriori2<> apriori3 ( std::move(apriori2) );
      TS_ASSERT ( apriori3.weight () == 0.0 );
      apriori3.setWeight ( 4.0 );
      TS_ASSERT ( apriori3.weight () == 0.0 );

      TS_ASSERT ( apriori3.getType() == "NoApriori" );
      TS_ASSERT ( apriori3.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori3.isOfType("Smoothing") );
      
      vect = apriori3.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(4) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori3.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori3.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(16) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori3.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori3.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori3.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(36) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }

      
      gum::learning::AprioriNoApriori2<>* apriori4 = apriori.clone ();
      TS_ASSERT ( apriori4->weight () == 0.0 );
      apriori4->setWeight ( 4.0 );
      TS_ASSERT ( apriori4->weight () == 0.0 );

      TS_ASSERT ( apriori4->getType() == "NoApriori" );
      TS_ASSERT ( apriori4->isOfType("NoApriori") );
      TS_ASSERT ( ! apriori4->isOfType("Smoothing") );
      
      vect = apriori4->getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(4) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori4->getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori4->getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(16) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori4->getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori4->getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori4->getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(36) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }

      delete apriori4;

      gum::learning::DatabaseTable<> database2;
      gum::learning::AprioriNoApriori2<> apriori5 ( database2 );
      apriori5 = apriori;
      TS_ASSERT ( apriori5.weight () == 0.0 );
      apriori5.setWeight ( 4.0 );
      TS_ASSERT ( apriori5.weight () == 0.0 );

      TS_ASSERT ( apriori5.getType() == "NoApriori" );
      TS_ASSERT ( apriori5.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori5.isOfType("Smoothing") );
      
      vect = apriori5.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(4) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori5.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(16) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori5.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(36) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }

      apriori5 = std::move ( apriori );
      TS_ASSERT ( apriori5.weight () == 0.0 );
      apriori5.setWeight ( 4.0 );
      TS_ASSERT ( apriori5.weight () == 0.0 );

      TS_ASSERT ( apriori5.getType() == "NoApriori" );
      TS_ASSERT ( apriori5.isOfType("NoApriori") );
      TS_ASSERT ( ! apriori5.isOfType("Smoothing") );
      
      vect = apriori5.getAllApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(4) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset1 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori5.getAllApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(16) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset2 );
      TS_ASSERT ( vect.size () == std::size_t(0) );
     
      vect = apriori5.getAllApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(576) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      vect = apriori5.getConditioningApriori( idset3 );
      TS_ASSERT ( vect.size () == std::size_t(36) );
      for ( const auto val : vect ) {
        TS_ASSERT ( val == 0.0 );
      }
      
    }

  };
  
} /* namespace gum_tests */
