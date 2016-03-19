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
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/UAI/UAIReader.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

namespace gum_tests {

  class UAIReaderTestSuite : public CxxTest::TestSuite {
    public:
    void testConstuctor() {
      std::string file = GET_RESSOURCES_PATH( "BNUAIReader_file1.uai" );
      gum::BayesNet<float> net;

      gum::UAIReader<float>* reader = 0;
      TS_GUM_ASSERT_THROWS_NOTHING(
          reader = new gum::UAIReader<float>( &net, file ) );
      TS_GUM_ASSERT_THROWS_NOTHING( delete reader );
    }

    void testRead_file1() {
      std::string file = GET_RESSOURCES_PATH( "BNUAIReader_file1.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();

      TS_ASSERT( net != 0 );

      gum::UAIReader<float> reader( net, file );
      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
      reader.showElegantErrorsAndWarnings(std::cout);

      if ( net != 0 ) {
        TS_ASSERT( net->empty() );
        delete net;
      }
    }
    void /*test*/Read_file2_float() {
      std::string file = GET_RESSOURCES_PATH( "BNUAIReader_file2.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      int isOK = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( isOK = reader.proceed() );
      TS_ASSERT_EQUALS( isOK, 0 );

      TS_ASSERT( net != 0 );

      if ( net != 0 ) {
        TS_ASSERT( !net->empty() );
        TS_ASSERT_EQUALS( net->size(), (gum::Size)2 );
        gum::NodeId node_1 = 0, node_2 = 0;

        for ( const auto node : net->nodes() )
          if ( net->variable( node ).name() == "n1" )
            node_1 = node;
          else
            node_2 = node;

        const gum::DiscreteVariable& var_1 = net->variable( node_1 );

        TS_ASSERT_EQUALS( var_1.name(), "n1" );
        TS_ASSERT_EQUALS( var_1.domainSize(), (gum::Size)2 );

        const gum::Potential<float>& proba_1 = net->cpt( node_1 );
        TS_ASSERT_EQUALS( proba_1.domainSize(), (gum::Size)2 );

        gum::Instantiation inst_1( proba_1 );
        inst_1.setFirst();
        TS_ASSERT_DELTA( proba_1[inst_1], 0.2, 0.001 );
        inst_1.setLast();
        TS_ASSERT_DELTA( proba_1[inst_1], 0.8, 0.001 );

        const gum::DiscreteVariable& var_2 = net->variable( node_2 );
        TS_ASSERT_EQUALS( var_2.name(), "n2" );
        TS_ASSERT_EQUALS( var_2.domainSize(), (gum::Size)2 );

        const gum::Potential<float>& proba_2 = net->cpt( node_2 );
        TS_ASSERT_EQUALS( proba_2.domainSize(), (gum::Size)2 );

        gum::Instantiation inst_2( proba_2 );
        inst_2.setFirst();
        TS_ASSERT_DELTA( proba_2[inst_2], 0.3, 0.001 );
        inst_2.setLast();
        TS_ASSERT_DELTA( proba_2[inst_2], 0.7, 0.001 );
        delete net;
      }
    }

    void /*test*/Read_dog_double() {
      // from Charniak, Bayesian Networks Without Tears, AI Magazine, 1991
      std::string file = GET_RESSOURCES_PATH( "dog.uai" );
      gum::BayesNet<double>* net = new gum::BayesNet<double>();
      gum::UAIReader<double> reader( net, file );

      int isOK = false;
      TS_GUM_ASSERT_THROWS_NOTHING( isOK = reader.proceed() );
      TS_ASSERT_EQUALS( isOK, 0 );

      TS_ASSERT( net != 0 );

      if ( net != 0 ) {
        TS_ASSERT_EQUALS( net->size(), (gum::Size)5 );

        const gum::Potential<double>& proba =
            net->cpt( net->idFromName( "dog-out" ) );

        TS_ASSERT_EQUALS( proba.domainSize(), (gum::Size)8 );

        delete ( net );
      }
    }

    void /*test*/Read_file2_double() {
      std::string file = GET_RESSOURCES_PATH( "BNUAIReader_file2.uai" );
      gum::BayesNet<double>* net = new gum::BayesNet<double>();
      gum::UAIReader<double> reader( net, file );

      int isOK = false;
      TS_GUM_ASSERT_THROWS_NOTHING( isOK = reader.proceed() );
      TS_ASSERT_EQUALS( isOK, 0 );

      TS_ASSERT( net != 0 );

      if ( net != 0 ) {
        TS_ASSERT_EQUALS( net->size(), (gum::Size)2 );
        gum::NodeId node_1 = 0, node_2 = 0;

        for ( const auto node : net->nodes() )
          if ( net->variable( node ).name() == "n1" )
            node_1 = node;
          else
            node_2 = node;

        const gum::DiscreteVariable& var_1 = net->variable( node_1 );

        TS_ASSERT_EQUALS( var_1.name(), "n1" );

        TS_ASSERT_EQUALS( var_1.domainSize(), (gum::Size)2 );

        const gum::Potential<double>& proba_1 = net->cpt( node_1 );

        TS_ASSERT_EQUALS( proba_1.domainSize(), (gum::Size)2 );

        gum::Instantiation inst_1( proba_1 );

        inst_1.setFirst();

        TS_ASSERT_DELTA( proba_1[inst_1], 0.2, 0.001 );

        inst_1.setLast();

        TS_ASSERT_DELTA( proba_1[inst_1], 0.8, 0.001 );

        const gum::DiscreteVariable& var_2 = net->variable( node_2 );

        TS_ASSERT_EQUALS( var_2.name(), "n2" );

        TS_ASSERT_EQUALS( var_2.domainSize(), (gum::Size)2 );

        const gum::Potential<double>& proba_2 = net->cpt( node_2 );

        TS_ASSERT_EQUALS( proba_2.domainSize(), (gum::Size)2 );

        gum::Instantiation inst_2( proba_2 );

        inst_2.setFirst();

        TS_ASSERT_DELTA( proba_2[inst_2], 0.3, 0.001 );

        inst_2.setLast();

        TS_ASSERT_DELTA( proba_2[inst_2], 0.7, 0.001 );

        delete net;
      }
    }

    void /*test*/Read_file3() {
      std::string file = GET_RESSOURCES_PATH( "BNUAIReader_file3.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      int isOK = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( isOK = reader.proceed() );
      TS_ASSERT_EQUALS( isOK, 0 );
      TS_ASSERT( net != 0 );

      if ( net != 0 ) {
        gum::HashTable<std::string, gum::Id> idMap;

        for ( const auto node : net->nodes() )
          idMap.insert( net->variable( node ).name(), node );

        const gum::DiscreteVariable& var_1 = net->variable( idMap["n1"] );

        TS_ASSERT_EQUALS( var_1.name(), "n1" );

        TS_ASSERT_EQUALS( var_1.domainSize(), (gum::Size)2 );

        TS_ASSERT_EQUALS( var_1.label( 0 ), "0" );

        TS_ASSERT_EQUALS( var_1.label( 1 ), "1" );

        const gum::Potential<float>& proba_1 = net->cpt( idMap["n1"] );

        TS_ASSERT_EQUALS( proba_1.domainSize(), (gum::Size)2 );

        gum::Instantiation inst_1( proba_1 );

        inst_1.setFirst();

        TS_ASSERT_DELTA( proba_1[inst_1], 0.2, 0.001 );

        inst_1.setLast();

        TS_ASSERT_DELTA( proba_1[inst_1], 0.8, 0.001 );

        const gum::DiscreteVariable& var_2 = net->variable( idMap["n2"] );

        TS_ASSERT_EQUALS( var_2.name(), "n2" );

        TS_ASSERT_EQUALS( var_2.domainSize(), (gum::Size)2 );

        TS_ASSERT_EQUALS( var_2.label( 0 ), "foo" );

        TS_ASSERT_EQUALS( var_2.label( 1 ), "bar" );

        const gum::Potential<float>& proba_2 = net->cpt( idMap["n2"] );

        TS_ASSERT_EQUALS( proba_2.domainSize(), (gum::Size)2 );

        gum::Instantiation inst_2( proba_2 );

        inst_2.setFirst();

        TS_ASSERT_DELTA( proba_2[inst_2], 0.3, 0.001 );

        inst_2.setLast();

        TS_ASSERT_DELTA( proba_2[inst_2], 0.7, 0.001 );

        const gum::DiscreteVariable& var_3 = net->variable( idMap["n3"] );

        TS_ASSERT_EQUALS( var_3.name(), "n3" );

        TS_ASSERT_EQUALS( var_3.domainSize(), (gum::Size)2 );

        TS_ASSERT_EQUALS( var_3.label( 0 ), "0" );

        TS_ASSERT_EQUALS( var_3.label( 1 ), "1" );

        TS_ASSERT( net->dag().existsArc( idMap["n1"], idMap["n3"] ) );

        const gum::Potential<float>& proba_3 = net->cpt( idMap["n3"] );

        TS_ASSERT_EQUALS( proba_3.domainSize(), (gum::Size)4 );

        gum::Instantiation inst_3( proba_3 );

        inst_3.chgVal( var_1, 0 );

        inst_3.chgVal( var_3, 0 );

        TS_ASSERT_DELTA( proba_3[inst_3], 0.1, 0.001 );

        inst_3.chgVal( var_3, 1 );

        TS_ASSERT_DELTA( proba_3[inst_3], 0.9, 0.001 );

        inst_3.chgVal( var_1, 1 );

        inst_3.chgVal( var_3, 0 );

        TS_ASSERT_DELTA( proba_3[inst_3], 0.9, 0.001 );

        inst_3.chgVal( var_3, 1 );

        TS_ASSERT_DELTA( proba_3[inst_3], 0.1, 0.001 );

        const gum::DiscreteVariable& var_4 = net->variable( idMap["n4"] );

        TS_ASSERT_EQUALS( var_4.name(), "n4" );

        TS_ASSERT_EQUALS( var_4.domainSize(), (gum::Size)2 );

        TS_ASSERT_EQUALS( var_4.label( 0 ), "0" );

        TS_ASSERT_EQUALS( var_4.label( 1 ), "1" );

        TS_ASSERT( net->dag().existsArc( idMap["n1"], idMap["n4"] ) );

        TS_ASSERT( net->dag().existsArc( idMap["n2"], idMap["n4"] ) );

        const gum::Potential<float>& proba_4 = net->cpt( idMap["n4"] );

        TS_ASSERT_EQUALS( proba_4.domainSize(), (gum::Size)8 );

        gum::Instantiation inst_4( proba_4 );

        inst_4.chgVal( var_1, 0 );

        inst_4.chgVal( var_2, 0 );

        inst_4.chgVal( var_4, 0 );

        TS_ASSERT_DELTA( proba_4[inst_4], 0.4, 0.001 );

        inst_4.chgVal( var_4, 1 );

        TS_ASSERT_DELTA( proba_4[inst_4], 0.6, 0.001 );

        inst_4.chgVal( var_1, 1 );

        inst_4.chgVal( var_2, 0 );

        inst_4.chgVal( var_4, 0 );

        TS_ASSERT_DELTA( proba_4[inst_4], 0.5, 0.001 );

        inst_4.chgVal( var_4, 1 );

        TS_ASSERT_DELTA( proba_4[inst_4], 0.5, 0.001 );

        inst_4.chgVal( var_1, 0 );

        inst_4.chgVal( var_2, 1 );

        inst_4.chgVal( var_4, 0 );

        TS_ASSERT_DELTA( proba_4[inst_4], 0.5, 0.001 );

        inst_4.chgVal( var_4, 1 );

        TS_ASSERT_DELTA( proba_4[inst_4], 0.5, 0.001 );

        inst_4.chgVal( var_1, 1 );

        inst_4.chgVal( var_2, 1 );

        inst_4.chgVal( var_4, 0 );

        TS_ASSERT( proba_4[inst_4] == 1 );

        inst_4.chgVal( var_4, 1 );

        TS_ASSERT( proba_4[inst_4] == 0 );

        const gum::DiscreteVariable& var_5 = net->variable( idMap["n5"] );

        TS_ASSERT_EQUALS( var_5.name(), "n5" );

        TS_ASSERT_EQUALS( var_5.domainSize(), (gum::Size)3 );

        TS_ASSERT_EQUALS( var_5.label( 0 ), "space" );

        TS_ASSERT_EQUALS( var_5.label( 1 ), "final" );

        TS_ASSERT_EQUALS( var_5.label( 2 ), "frontiere" );

        TS_ASSERT( net->dag().existsArc( idMap["n2"], idMap["n5"] ) );

        TS_ASSERT( net->dag().existsArc( idMap["n3"], idMap["n5"] ) );

        const gum::Potential<float>& proba_5 = net->cpt( idMap["n5"] );

        TS_ASSERT_EQUALS( proba_5.domainSize(), (gum::Size)12 );

        gum::Instantiation inst_5( proba_5 );

        inst_5.chgVal( var_3, 0 );

        inst_5.chgVal( var_2, 0 );

        inst_5.chgVal( var_5, 0 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.3, 0.001 );

        inst_5.chgVal( var_5, 1 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.6, 0.001 );

        inst_5.chgVal( var_5, 2 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.1, 0.001 );

        inst_5.chgVal( var_2, 0 );

        inst_5.chgVal( var_3, 1 );

        inst_5.chgVal( var_5, 0 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.5, 0.001 );

        inst_5.chgVal( var_5, 1 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.5, 0.001 );

        inst_5.chgVal( var_5, 2 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.0, 0.001 );

        inst_5.chgVal( var_2, 1 );

        inst_5.chgVal( var_3, 0 );

        inst_5.chgVal( var_5, 0 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.4, 0.001 );

        inst_5.chgVal( var_5, 1 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.6, 0.001 );

        inst_5.chgVal( var_5, 2 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.0, 0.001 );

        inst_5.chgVal( var_2, 1 );

        inst_5.chgVal( var_3, 1 );

        inst_5.chgVal( var_5, 0 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.5, 0.001 );

        inst_5.chgVal( var_5, 1 );

        TS_ASSERT_DELTA( proba_5[inst_5], 0.5, 0.001 );

        inst_5.chgVal( var_5, 2 );

        TS_ASSERT( proba_5[inst_5] == 0 );

        const gum::DiscreteVariable& var_6 = net->variable( idMap["n6"] );

        TS_ASSERT_EQUALS( var_6.name(), "n6" );

        TS_ASSERT_EQUALS( var_6.domainSize(), (gum::Size)2 );

        TS_ASSERT_EQUALS( var_6.label( 0 ), "0" );

        TS_ASSERT_EQUALS( var_6.label( 1 ), "1" );

        TS_ASSERT( net->dag().existsArc( idMap["n1"], idMap["n6"] ) );

        TS_ASSERT( net->dag().existsArc( idMap["n5"], idMap["n6"] ) );

        const gum::Potential<float>& proba_6 = net->cpt( idMap["n6"] );

        TS_ASSERT_EQUALS( proba_6.domainSize(), (gum::Size)12 );

        gum::Instantiation inst_6( proba_6 );

        inst_6.chgVal( var_6, 0 );

        inst_6.chgVal( var_1, 0 );

        inst_6.chgVal( var_5, 0 );

        TS_ASSERT_DELTA( proba_6[inst_6], 0.1, 0.001 );

        inst_6.chgVal( var_5, 1 );

        TS_ASSERT_DELTA( proba_6[inst_6], 0.2, 0.001 );

        inst_6.chgVal( var_5, 2 );

        TS_ASSERT_DELTA( proba_6[inst_6], 0.3, 0.001 );

        inst_6.chgVal( var_1, 1 );

        inst_6.chgVal( var_5, 0 );

        TS_ASSERT_DELTA( proba_6[inst_6], 0.4, 0.001 );

        inst_6.chgVal( var_5, 1 );

        TS_ASSERT_DELTA( proba_6[inst_6], 0.5, 0.001 );

        inst_6.chgVal( var_5, 2 );

        TS_ASSERT_DELTA( proba_6[inst_6], 0.6, 0.001 );

        inst_6.chgVal( var_6, 1 );

        inst_6.chgVal( var_1, 0 );

        inst_6.chgVal( var_5, 0 );

        TS_ASSERT_DELTA( proba_6[inst_6], 0.7, 0.001 );

        inst_6.chgVal( var_5, 1 );

        TS_ASSERT_DELTA( proba_6[inst_6], 0.8, 0.001 );

        inst_6.chgVal( var_5, 2 );

        TS_ASSERT_DELTA( proba_6[inst_6], 0.9, 0.001 );

        inst_6.chgVal( var_1, 1 );

        inst_6.chgVal( var_5, 0 );

        TS_ASSERT( proba_6[inst_6] == 1 );

        inst_6.chgVal( var_5, 1 );

        TS_ASSERT( proba_6[inst_6] == 0 );

        inst_6.chgVal( var_5, 2 );

        TS_ASSERT( proba_6[inst_6] == 0 );

        delete net;
      }
    }

    void /*test*/Alarm() {
      std::string file = GET_RESSOURCES_PATH( "alarm.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      int isOK = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( isOK = reader.proceed() );
      TS_ASSERT_EQUALS( isOK, 0 );

      gum::HashTable<std::string, gum::Id> idMap;

      for ( const auto node : net->nodes() )
        idMap.insert( net->variable( node ).name(), node );

      // The node wich we'll test
      TS_ASSERT( idMap.exists( "HISTORY" ) );
      // It's parent
      TS_ASSERT( idMap.exists( "LVFAILURE" ) );

      if ( idMap.exists( "HISTORY" ) and idMap.exists( "LVFAILURE" ) ) {
        const gum::DiscreteVariable& history =
            net->variable( idMap["HISTORY"] );
        TS_ASSERT_EQUALS( history.domainSize(), (gum::Size)2 );
        TS_ASSERT_EQUALS( history.label( 0 ), "TRUE" );
        TS_ASSERT_EQUALS( history.label( 1 ), "FALSE" );
        TS_ASSERT(
            net->dag().existsArc( idMap["LVFAILURE"], idMap["HISTORY"] ) );

        const gum::Potential<float>& historyCPT = net->cpt( idMap["HISTORY"] );
        TS_ASSERT_EQUALS( historyCPT.domainSize(), (gum::Size)4 );
        TS_ASSERT( historyCPT.contains( net->variable( idMap["HISTORY"] ) ) );
        TS_ASSERT( historyCPT.contains( net->variable( idMap["LVFAILURE"] ) ) );

        gum::Instantiation historyInst( historyCPT );
        historyInst.chgVal( history, 0 );
        historyInst.chgVal( net->variable( idMap["LVFAILURE"] ), 0 );
        TS_ASSERT_DELTA( historyCPT[historyInst], 0.9, 0.0001 );
        historyInst.chgVal( history, 1 );
        historyInst.chgVal( net->variable( idMap["LVFAILURE"] ), 0 );
        TS_ASSERT_DELTA( historyCPT[historyInst], 0.1, 0.0001 );
        historyInst.chgVal( history, 0 );
        historyInst.chgVal( net->variable( idMap["LVFAILURE"] ), 1 );
        TS_ASSERT_DELTA( historyCPT[historyInst], 0.01, 0.0001 );
        historyInst.chgVal( history, 1 );
        historyInst.chgVal( net->variable( idMap["LVFAILURE"] ), 1 );
        TS_ASSERT_DELTA( historyCPT[historyInst], 0.99, 0.0001 );
      }

      // The node wich we'll test
      TS_ASSERT( idMap.exists( "ERRLOWOUTPUT" ) );
      // It's Children
      TS_ASSERT( idMap.exists( "HRBP" ) );

      if ( idMap.exists( "ERRLOWOUTPUT" ) and idMap.exists( "HRBP" ) ) {
        const gum::DiscreteVariable& errlowoutput =
            net->variable( idMap["ERRLOWOUTPUT"] );
        TS_ASSERT_EQUALS( errlowoutput.domainSize(), (gum::Size)2 );
        TS_ASSERT_EQUALS( errlowoutput.label( 0 ), "TRUE" );
        TS_ASSERT_EQUALS( errlowoutput.label( 1 ), "FALSE" );
        TS_ASSERT(
            net->dag().existsArc( idMap["ERRLOWOUTPUT"], idMap["HRBP"] ) );

        const gum::Potential<float>& errlowoutputCPT =
            net->cpt( idMap["ERRLOWOUTPUT"] );
        TS_ASSERT_EQUALS( errlowoutputCPT.domainSize(), (gum::Size)2 );
        TS_ASSERT( errlowoutputCPT.contains( errlowoutput ) );

        gum::Instantiation errlowoutputInst( errlowoutputCPT );
        errlowoutputInst.chgVal( errlowoutput, 0 );
        TS_ASSERT_DELTA( errlowoutputCPT[errlowoutputInst], 0.05, 0.001 );
        errlowoutputInst.chgVal( errlowoutput, 1 );
        TS_ASSERT_DELTA( errlowoutputCPT[errlowoutputInst], 0.95, 0.001 );
      }

      // The nide wich we'll test
      TS_ASSERT( idMap.exists( "LVEDVOLUME" ) );
      // It's parents
      TS_ASSERT( idMap.exists( "HYPOVOLEMIA" ) );
      TS_ASSERT( idMap.exists( "LVFAILURE" ) );

      if ( idMap.exists( "LVEDVOLUME" ) && idMap.exists( "HYPOVOLEMIA" ) &&
           idMap.exists( "LVFAILURE" ) ) {
        const gum::DiscreteVariable& lvedvolume =
            net->variable( idMap["LVEDVOLUME"] );
        const gum::DiscreteVariable& hypovolemia =
            net->variable( idMap["HYPOVOLEMIA"] );
        const gum::DiscreteVariable& lvfailure =
            net->variable( idMap["LVFAILURE"] );
        // checking label order
        TS_ASSERT_EQUALS( lvedvolume.label( 0 ), "LOW" );
        TS_ASSERT_EQUALS( lvedvolume.label( 1 ), "NORMAL" );
        TS_ASSERT_EQUALS( lvedvolume.label( 2 ), "HIGH" );
        TS_ASSERT_EQUALS( hypovolemia.label( 0 ), "TRUE" );
        TS_ASSERT_EQUALS( hypovolemia.label( 1 ), "FALSE" );
        TS_ASSERT_EQUALS( lvfailure.label( 0 ), "TRUE" );
        TS_ASSERT_EQUALS( lvfailure.label( 1 ), "FALSE" );

        const gum::Potential<float>& cpt = net->cpt( idMap["LVEDVOLUME"] );
        gum::Instantiation inst( cpt );

        gum::Instantiation var_inst;
        var_inst << lvedvolume;

        inst.chgVal( hypovolemia, 0 );
        inst.chgVal( lvfailure, 0 );
        float array_1[] = {0.95, 0.04, 0.01};
        size_t i = 0;

        for ( inst.setFirstIn( var_inst ); !inst.end();
              inst.incIn( var_inst ), ++i ) {
          TS_ASSERT_DELTA( cpt[inst], array_1[i], 0.001 );
        }

        inst.chgVal( hypovolemia, 1 );
        inst.chgVal( lvfailure, 0 );
        float array_2[] = {0.98, 0.01, 0.01};
        i = 0;

        for ( inst.setFirstIn( var_inst ); !inst.end();
              inst.incIn( var_inst ), ++i ) {
          TS_ASSERT_DELTA( cpt[inst], array_2[i], 0.001 );
        }

        inst.chgVal( hypovolemia, 0 );
        inst.chgVal( lvfailure, 1 );
        float array_3[] = {0.01, 0.09, 0.9};
        i = 0;

        for ( inst.setFirstIn( var_inst ); !inst.end();
              inst.incIn( var_inst ), ++i ) {
          TS_ASSERT_DELTA( cpt[inst], array_3[i], 0.001 );
        }

        inst.chgVal( hypovolemia, 1 );
        inst.chgVal( lvfailure, 1 );
        float array_4[] = {0.05, 0.9, 0.05};
        i = 0;

        for ( inst.setFirstIn( var_inst ); !inst.end();
              inst.incIn( var_inst ), ++i ) {
          TS_ASSERT_DELTA( cpt[inst], array_4[i], 0.001 );
        }
      }

      delete net;
    }

    void /*test*/Unexisting() {
      std::string file = "Schmurtz";
      gum::BayesNet<float>* net = new gum::BayesNet<float>();

      TS_GUM_ASSERT_THROWS_NOTHING(
          gum::UAIReader<float> reader( net, file ) );
      gum::UAIReader<float> reader( net, file );
      TS_ASSERT_THROWS( reader.proceed(), gum::IOError );

      if ( net ) delete net;
    }

    void Carpo() {  // Carpo is not correct as a xmlbif file
      std::string file = GET_RESSOURCES_PATH( "carpo.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );

      if ( net ) delete net;
    }

    void /*test*/Diabetes() {
      std::string file = GET_RESSOURCES_PATH( "Diabetes.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );

      if ( net ) delete net;
    }

    void /*test*/Hailfinder() {
      std::string file = GET_RESSOURCES_PATH( "hailfinder.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );

      if ( net ) delete net;
    }

    void /*test*/Insurance() {
      std::string file = GET_RESSOURCES_PATH( "insurance.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );

      if ( net ) delete net;
    }

    void /*test*/Link() {
      std::string file = GET_RESSOURCES_PATH( "Link.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );

      if ( net ) delete net;
    }

    void /*test*/Mildew() {
      std::string file = GET_RESSOURCES_PATH( "Mildew.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );

      if ( net ) delete net;
    }

    void /*test*/Munin1() {
      std::string file = GET_RESSOURCES_PATH( "Munin1.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );

      if ( net ) delete net;
    }

    void /*test*/Pigs() {
      std::string file = GET_RESSOURCES_PATH( "Pigs.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );

      if ( net ) delete net;
    }

    void /*test*/Water() {
      std::string file = GET_RESSOURCES_PATH( "Water.uai" );
      gum::BayesNet<float>* net = new gum::BayesNet<float>();
      gum::UAIReader<float> reader( net, file );

      TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );

      if ( net ) delete net;
    }
  };
}
