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

#include <cxxtest/TestSuite.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/io/BIFIO.h>


#define GET_PATH_STR(x) ("../../../src/testunits/resources/" #x)

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//         \ / /            4 -> 5
//          5_/             2 -> 4
//                          2 -> 5

class BIFReaderTestSuite: public CxxTest::TestSuite {
  public:
    void testConstuctor() {
      gum::BIFReader *reader=NULL;
      TS_GUM_ASSERT_THROWS_NOTHING( reader = new gum::BIFReader() );
      TS_GUM_ASSERT_THROWS_NOTHING( delete reader );
    }

    void testRead_file1() {
      std::string file = GET_PATH_STR( BIFReader_file1.txt );
      gum::BIFReader reader;
      gum::BayesNet<float> *net=NULL;
      TS_GUM_ASSERT_THROWS_NOTHING( net = reader.read( file ) );

      TS_ASSERT( net != NULL );

      if ( net != NULL ) {
        TS_ASSERT( net->empty() );
        delete net;
      }
    }

    void testRead_file2() {
      std::string file = GET_PATH_STR( BIFReader_file2.txt );
      gum::BIFReader reader;
      gum::BayesNet<float> *net=NULL;
      TS_GUM_ASSERT_THROWS_NOTHING( net = reader.read( file ) );

      TS_ASSERT( net != NULL );

      if ( net != NULL ) {
        TS_ASSERT_EQUALS( net->size(), ( gum::Size )2 );
        gum::NodeId node_1=0, node_2=0;

        for ( gum::NodeSet::iterator iter = net->dag().beginNodes();iter != net->dag().endNodes(); ++iter ) {
          if ( net->variable( *iter ).name() == "1" )
            node_1 = *iter;
          else
            node_2 = *iter;
        }

        const gum::DiscreteVariable &var_1 = net->variable( node_1 );

        TS_ASSERT_EQUALS( var_1.name(), "1" );
        TS_ASSERT_EQUALS( var_1.domainSize(), ( gum::Size )2 );
        const gum::Potential<float> &proba_1 = net->cpt( node_1 );
        TS_ASSERT_EQUALS( proba_1.domainSize(), ( gum::Size )2 );
        gum::Instantiation inst_1( proba_1 );
        inst_1.setFirst();
        TS_ASSERT( abs(( proba_1[inst_1] - 0.2 ) ) < 0.001 );
        inst_1.setLast();
        TS_ASSERT( abs(( proba_1[inst_1] - 0.8 ) ) < 0.001 );

        const gum::DiscreteVariable &var_2 = net->variable( node_2 );
        TS_ASSERT_EQUALS( var_2.name(), "2" );
        TS_ASSERT_EQUALS( var_2.domainSize(), ( gum::Size )2 );
        const gum::Potential<float> &proba_2 = net->cpt( node_2 );
        TS_ASSERT_EQUALS( proba_2.domainSize(), ( gum::Size )2 );
        gum::Instantiation inst_2( proba_2 );
        inst_2.setFirst();
        TS_ASSERT( abs(( proba_2[inst_2] - 0.3 ) ) < 0.001 );
        inst_2.setLast();
        TS_ASSERT( abs(( proba_2[inst_2] - 0.7 ) ) < 0.001 );

        delete net;
      }
    }

    void testRead_file3() {
      //std::cerr << std::endl << "In testRead_file3.";
      std::string file = GET_PATH_STR( BIFReader_file3.txt );
      gum::BIFReader reader;
      gum::BayesNet<float> *net=NULL;
      //TS_GUM_ASSERT_THROWS_NOTHING(net = reader.read(file));

      try {
        net = reader.read( file );
      } catch ( gum::Exception e ) {
        //std::cerr << std::endl << e.getContent();
      }

      TS_ASSERT( net != NULL );

      if ( net != NULL ) {
        gum::HashTable<std::string, gum::Id> idMap;

        for ( gum::NodeSetIterator iter = net->beginNodes(); iter != net->endNodes(); ++iter ) {
          idMap.insert( net->variable(*iter).name(), *iter);
        }

        const gum::DiscreteVariable &var_1 = net->variable( idMap["1"] );

        TS_ASSERT_EQUALS( var_1.name(), "1" );
        TS_ASSERT_EQUALS( var_1.domainSize(), ( gum::Size )2 );
        TS_ASSERT_EQUALS( var_1.label( 0 ), "0" );
        TS_ASSERT_EQUALS( var_1.label( 1 ), "1" );
        const gum::Potential<float> &proba_1 = net->cpt( idMap["1"] );
        TS_ASSERT_EQUALS( proba_1.domainSize(), ( gum::Size )2 );
        gum::Instantiation inst_1( proba_1 );
        inst_1.setFirst();
        TS_ASSERT( abs(( proba_1[inst_1] - 0.2 ) ) < 0.001 );
        inst_1.setLast();
        TS_ASSERT( abs(( proba_1[inst_1] - 0.8 ) ) < 0.001 );

        const gum::DiscreteVariable &var_2 = net->variable( idMap["2"] );
        TS_ASSERT_EQUALS( var_2.name(), "2" );
        TS_ASSERT_EQUALS( var_2.domainSize(), ( gum::Size )2 );
        TS_ASSERT_EQUALS( var_2.label( 0 ), "foo" );
        TS_ASSERT_EQUALS( var_2.label( 1 ), "bar" );
        const gum::Potential<float> &proba_2 = net->cpt( idMap["2"] );
        TS_ASSERT_EQUALS( proba_2.domainSize(), ( gum::Size )2 );
        gum::Instantiation inst_2( proba_2 );
        inst_2.setFirst();
        TS_ASSERT( abs(( proba_2[inst_2] - 0.3 ) ) < 0.001 );
        inst_2.setLast();
        TS_ASSERT( abs(( proba_2[inst_2] - 0.7 ) ) < 0.001 );

        const gum::DiscreteVariable &var_3 = net->variable( idMap["3"] );
        TS_ASSERT_EQUALS( var_3.name(), "3" );
        TS_ASSERT_EQUALS( var_3.domainSize(), ( gum::Size )2 );
        TS_ASSERT_EQUALS( var_3.label( 0 ), "0" );
        TS_ASSERT_EQUALS( var_3.label( 1 ), "1" );
        TS_ASSERT( net->dag().existsArc( idMap["1"], idMap["3"] ) );
        const gum::Potential<float> &proba_3 = net->cpt( idMap["3"] );
        TS_ASSERT_EQUALS( proba_3.domainSize(), ( gum::Size )4 );
        gum::Instantiation inst_3( proba_3 );
        inst_3.chgVal( var_1, 0 );
        inst_3.chgVal( var_3, 0 );
        TS_ASSERT( abs( proba_3[inst_3] - 0.1 ) < 0.001 );
        inst_3.chgVal( var_3, 1 );
        TS_ASSERT( abs( proba_3[inst_3] - 0.9 ) < 0.001 );
        inst_3.chgVal( var_1, 1 );
        inst_3.chgVal( var_3, 0 );
        TS_ASSERT( abs( proba_3[inst_3] - 0.9 ) < 0.001 );
        inst_3.chgVal( var_3, 1 );
        TS_ASSERT( abs( proba_3[inst_3] - 0.1 ) < 0.001 );

        const gum::DiscreteVariable &var_4 = net->variable( idMap["4"] );
        TS_ASSERT_EQUALS( var_4.name(), "4" );
        TS_ASSERT_EQUALS( var_4.domainSize(), ( gum::Size )2 );
        TS_ASSERT_EQUALS( var_4.label( 0 ), "0" );
        TS_ASSERT_EQUALS( var_4.label( 1 ), "1" );
        TS_ASSERT( net->dag().existsArc( idMap["1"], idMap["4"] ) );
        TS_ASSERT( net->dag().existsArc( idMap["2"], idMap["4"] ) );
        const gum::Potential<float> &proba_4 = net->cpt( idMap["4"] );
        TS_ASSERT_EQUALS( proba_4.domainSize(), ( gum::Size )8 );
        gum::Instantiation inst_4( proba_4 );
        inst_4.chgVal( var_1, 0 );
        inst_4.chgVal( var_2, 0 );
        inst_4.chgVal( var_4, 0 );
        TS_ASSERT( abs( proba_4[inst_4] - 0.4 ) < 0.001 );
        inst_4.chgVal( var_4, 1 );
        TS_ASSERT( abs( proba_4[inst_4] - 0.6 ) < 0.001 );
        inst_4.chgVal( var_1, 1 );
        inst_4.chgVal( var_2, 0 );
        inst_4.chgVal( var_4, 0 );
        TS_ASSERT( abs( proba_4[inst_4] - 0.5 ) < 0.001 );
        inst_4.chgVal( var_4, 1 );
        TS_ASSERT( abs( proba_4[inst_4] - 0.5 ) < 0.001 );
        inst_4.chgVal( var_1, 0 );
        inst_4.chgVal( var_2, 1 );
        inst_4.chgVal( var_4, 0 );
        TS_ASSERT( abs( proba_4[inst_4] - 0.5 ) < 0.001 );
        inst_4.chgVal( var_4, 1 );
        TS_ASSERT( abs( proba_4[inst_4] - 0.5 ) < 0.001 );
        inst_4.chgVal( var_1, 1 );
        inst_4.chgVal( var_2, 1 );
        inst_4.chgVal( var_4, 0 );
        TS_ASSERT( proba_4[inst_4] == 1 );
        inst_4.chgVal( var_4, 1 );
        TS_ASSERT( proba_4[inst_4] == 0 );

        const gum::DiscreteVariable &var_5 = net->variable( idMap["5"] );
        TS_ASSERT_EQUALS( var_5.name(), "5" );
        TS_ASSERT_EQUALS( var_5.domainSize(), ( gum::Size )3 );
        TS_ASSERT_EQUALS( var_5.label( 0 ), "space" );
        TS_ASSERT_EQUALS( var_5.label( 1 ), "final" );
        TS_ASSERT_EQUALS( var_5.label( 2 ), "frontiere" );
        TS_ASSERT( net->dag().existsArc( idMap["2"], idMap["5"] ) );
        TS_ASSERT( net->dag().existsArc( idMap["3"], idMap["5"] ) );
        const gum::Potential<float> &proba_5 = net->cpt( idMap["5"] );
        TS_ASSERT_EQUALS( proba_5.domainSize(), ( gum::Size )12 );
        gum::Instantiation inst_5( proba_5 );
        inst_5.chgVal( var_3, 0 );
        inst_5.chgVal( var_2, 0 );
        inst_5.chgVal( var_5, 0 );
        TS_ASSERT( abs( proba_5[inst_5] - 0.3 ) < 0.001 );
        inst_5.chgVal( var_5, 1 );
        TS_ASSERT( abs( proba_5[inst_5] - 0.6 ) < 0.001 );
        inst_5.chgVal( var_5, 2 );
        TS_ASSERT( abs( proba_5[inst_5] - 0.1 ) < 0.001 );
        inst_5.chgVal( var_2, 0 );
        inst_5.chgVal( var_3, 1 );
        inst_5.chgVal( var_5, 0 );
        TS_ASSERT( abs( proba_5[inst_5] - 0.5 ) < 0.001 );
        inst_5.chgVal( var_5, 1 );
        TS_ASSERT( abs( proba_5[inst_5] - 0.5 ) < 0.001 );
        inst_5.chgVal( var_5, 2 );
        TS_ASSERT( proba_5[inst_5] == 0 );
        inst_5.chgVal( var_2, 1 );
        inst_5.chgVal( var_3, 0 );
        inst_5.chgVal( var_5, 0 );
        TS_ASSERT( abs( proba_5[inst_5] - 0.4 ) < 0.001 );
        inst_5.chgVal( var_5, 1 );
        TS_ASSERT( abs( proba_5[inst_5] - 0.6 ) < 0.001 );
        inst_5.chgVal( var_5, 2 );
        TS_ASSERT( proba_5[inst_5] == 0 );
        inst_5.chgVal( var_2, 1 );
        inst_5.chgVal( var_3, 1 );
        inst_5.chgVal( var_5, 0 );
        TS_ASSERT( abs( proba_5[inst_5] - 0.5 ) < 0.001 );
        inst_5.chgVal( var_5, 1 );
        TS_ASSERT( abs( proba_5[inst_5] - 0.5 ) < 0.001 );
        inst_5.chgVal( var_5, 2 );
        TS_ASSERT( proba_5[inst_5] == 0 );

        const gum::DiscreteVariable &var_6 = net->variable( idMap["6"] );
        TS_ASSERT_EQUALS( var_6.name(), "6" );
        TS_ASSERT_EQUALS( var_6.domainSize(), ( gum::Size )2 );
        TS_ASSERT_EQUALS( var_6.label( 0 ), "0" );
        TS_ASSERT_EQUALS( var_6.label( 1 ), "1" );
        TS_ASSERT( net->dag().existsArc( idMap["1"], idMap["6"] ) );
        TS_ASSERT( net->dag().existsArc( idMap["5"], idMap["6"] ) );
        const gum::Potential<float> &proba_6 = net->cpt( idMap["6"] );
        TS_ASSERT_EQUALS( proba_6.domainSize(),( gum::Size ) 12 );
        gum::Instantiation inst_6( proba_6 );
        inst_6.chgVal( var_6, 0 );
        inst_6.chgVal( var_1, 0 );
        inst_6.chgVal( var_5, 0 );
        TS_ASSERT( abs( proba_6[inst_6] - 0.1 ) < 0.001 );
        inst_6.chgVal( var_5, 1 );
        TS_ASSERT( abs( proba_6[inst_6] - 0.2 ) < 0.001 );
        inst_6.chgVal( var_5, 2 );
        TS_ASSERT( abs( proba_6[inst_6] - 0.3 ) < 0.001 );
        inst_6.chgVal( var_1, 1 );
        inst_6.chgVal( var_5, 0 );
        TS_ASSERT( abs( proba_6[inst_6] - 0.4 ) < 0.001 );
        inst_6.chgVal( var_5, 1 );
        TS_ASSERT( abs( proba_6[inst_6] - 0.5 ) < 0.001 );
        inst_6.chgVal( var_5, 2 );
        TS_ASSERT( abs( proba_6[inst_6] - 0.6 ) < 0.001 );
        inst_6.chgVal( var_6, 1 );
        inst_6.chgVal( var_1, 0 );
        inst_6.chgVal( var_5, 0 );
        TS_ASSERT( abs( proba_6[inst_6] - 0.7 ) < 0.001 );
        inst_6.chgVal( var_5, 1 );
        TS_ASSERT( abs( proba_6[inst_6] - 0.8 ) < 0.001 );
        inst_6.chgVal( var_5, 2 );
        TS_ASSERT( abs( proba_6[inst_6] - 0.9 ) < 0.001 );
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

    void testAlarm() {
      std::cerr << std::endl;
      std::string file = GET_PATH_STR( alarm.bif );
      gum::BIFReader reader;
      gum::BayesNet<float> *net=NULL;

      try {
        net = reader.read( file );
      } catch ( gum::Exception e ) {
        TS_ASSERT( false );
      }

      TS_ASSERT( net != NULL );

      if ( net != NULL ) {
        gum::HashTable<std::string, gum::Id> idMap;

        for ( gum::NodeSetIterator iter = net->beginNodes(); iter != net->endNodes(); ++iter ) {
          idMap.insert( net->variable(*iter).name(), *iter);
        }

        try {
          // The node wich we'll test
          TS_ASSERT( idMap.exists( "HISTORY" ) );
          // It's parent
          TS_ASSERT( idMap.exists( "LVFAILURE" ) );

          const gum::DiscreteVariable &history = net->variable( idMap["HISTORY"] );
          TS_ASSERT_EQUALS( history.domainSize(), ( gum::Size )2 );
          TS_ASSERT_EQUALS( history.label( 0 ), "TRUE" );
          TS_ASSERT_EQUALS( history.label( 1 ), "FALSE" );
          TS_ASSERT( net->dag().existsArc( idMap["LVFAILURE"], idMap["HISTORY"] ) );
          const gum::Potential<float> &historyCPT = net->cpt( idMap["HISTORY"] );
          TS_ASSERT_EQUALS( historyCPT.domainSize(),( gum::Size ) 4 );
          TS_ASSERT( historyCPT.contains( net->variable( idMap["HISTORY"] ) ) );
          TS_ASSERT( historyCPT.contains( net->variable( idMap["LVFAILURE"] ) ) );
          gum::Instantiation historyInst( historyCPT );
          // (TRUE | TRUE)
          historyInst.chgVal( history, 0 );
          historyInst.chgVal( net->variable( idMap["LVFAILURE"] ), 0 );
          TS_ASSERT( abs( historyCPT[historyInst] - 0.9 ) < 0.0001 );
          // (FALSE | TRUE)
          historyInst.chgVal( history, 1 );
          historyInst.chgVal( net->variable( idMap["LVFAILURE"] ), 0 );
          TS_ASSERT( abs( historyCPT[historyInst] - 0.1 ) < 0.0001 );
          // (TRUE | FALSE)
          historyInst.chgVal( history, 0 );
          historyInst.chgVal( net->variable( idMap["LVFAILURE"] ), 1 );
          TS_ASSERT( abs( historyCPT[historyInst] - 0.01 ) < 0.0001 );
          // (FALSE | FALSE)
          historyInst.chgVal( history, 1 );
          historyInst.chgVal( net->variable( idMap["LVFAILURE"] ), 1 );
          TS_ASSERT( abs( historyCPT[historyInst] - 0.99 ) < 0.0001 );


          // The node wich we'll test
          TS_ASSERT( idMap.exists( "ERRLOWOUTPUT" ) );
          // It's Children
          TS_ASSERT( idMap.exists( "HRBP" ) );
          const gum::DiscreteVariable &errlowoutput = net->variable( idMap["ERRLOWOUTPUT"] );
          TS_ASSERT_EQUALS( errlowoutput.domainSize(), ( gum::Size )2 );
          TS_ASSERT_EQUALS( errlowoutput.label( 0 ), "TRUE" );
          TS_ASSERT_EQUALS( errlowoutput.label( 1 ), "FALSE" );
          TS_ASSERT( net->dag().existsArc( idMap["ERRLOWOUTPUT"], idMap["HRBP"] ) );
          const gum::Potential<float> &errlowoutputCPT = net->cpt( idMap["ERRLOWOUTPUT"] );
          TS_ASSERT_EQUALS( errlowoutputCPT.domainSize(),( gum::Size ) 2 );
          TS_ASSERT( errlowoutputCPT.contains( errlowoutput ) );
          gum::Instantiation errlowoutputInst( errlowoutputCPT );
          errlowoutputInst.chgVal( errlowoutput, 0 );
          TS_ASSERT( abs( errlowoutputCPT[errlowoutputInst] - 0.05 ) < 0.001 );
          errlowoutputInst.chgVal( errlowoutput, 1 );
          TS_ASSERT( abs( errlowoutputCPT[errlowoutputInst] - 0.95 ) < 0.001 );

        } catch ( gum::Exception &e ) {
          std::cerr << std::endl << e.getContent() << std::endl;
          TS_ASSERT( false );
        }
        delete net;
      }
    }

    void testBarley() {
      std::string file = GET_PATH_STR( Barley.bif );
      gum::BIFReader reader;
      gum::BayesNet<float> *net=NULL;

      try {
        TS_GUM_ASSERT_THROWS_NOTHING(net = reader.read( file ));
      } catch ( gum::SyntaxError ) {};

      if ( net ) delete net;
    }

    void testUnexisting() {
      std::string file = "Schmurtz";
      gum::BIFReader reader;
      gum::BayesNet<float> *net=NULL;

      try {
        net=reader.read( file );
      } catch ( gum::IOError ) {};

      if ( net ) delete net;
    }



  private:
    float abs( float d ) {
      if ( d < 0 )
        return ( d * ( float ) -1 );

      return d;
    }

};

