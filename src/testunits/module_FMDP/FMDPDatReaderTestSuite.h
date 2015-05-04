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

// =====================================================================
#include <iostream>
#include <string>
// =====================================================================
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>

namespace gum_tests {

  class FMDPDatReaderTestSuite : public CxxTest::TestSuite {
    public:
    void testConstuctor() {
      std::string file = GET_PATH_STR("FMDP/coffee/coffee.dat");

      gum::FactoredMarkovDecisionProcess<float> fmdp;
      gum::MultiDimDecisionDiagramFactory<float> ddf;

      gum::FMDPDatReader<float> *reader = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(
          reader = new gum::FMDPDatReader<float>(&fmdp, file, &ddf));

      TS_GUM_ASSERT_THROWS_NOTHING(delete reader);
    }

    void testRead_file1() {
      std::string file = GET_PATH_STR("FMDP/factory/factoryB.dat");

      gum::FactoredMarkovDecisionProcess<float> fmdp;
      gum::MultiDimDecisionDiagramFactory<float> ddf;

      gum::FMDPDatReader<float> reader(&fmdp, file, &ddf);

      reader.trace(false);

      int nbrErr = 0;

      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      reader.showElegantErrorsAndWarnings();
    }

    //~ void estRead_file2_float() {
    //~ std::string file = GET_PATH_STR( "FMDPDatReader_file2.dat" );
    //~
    //~ gum::BayesNet<float> *net = new gum::BayesNet<float>();
    //~
    //~ gum::FMDPDatReader<float> reader( net, file );
    //~
    //~
    //~
    //~ bool isOK = false;
    //~
    //~ TS_GUM_ASSERT_THROWS_NOTHING( isOK = reader.proceed() );
    //~
    //~ TS_ASSERT( isOK );TS_ASSERT_EQUALS( reader.warnings(), ( gum::Size )2 );
    //~ // 2 warnings
    //~ TS_ASSERT_EQUALS( reader.errors(), ( gum::Size ) 0 );
    //~
    //~
    //~ TS_ASSERT( net != 0 );
    //~
    //~
    //~ if ( net != 0 ) {
    //~ TS_ASSERT_EQUALS( net->size(), ( gum::Size ) 2 );
    //~
    //~ gum::NodeId node_1 = 0, node_2 = 0;
    //~
    //~
    //~ for ( gum::DAG::NodeIterator iter = net->dag().beginNodes();iter !=
    // net->dag().endNodes(); ++iter ) {
    //~ if ( net->variable( *iter ).name() == "n1" )
    //~ node_1 = *iter;
    //~ else
    //~ node_2 = *iter;
    //~ }
    //~
    //~ const gum::DiscreteVariable &var_1 = net->variable( node_1 );
    //~
    //~
    //~ TS_ASSERT_EQUALS( var_1.name(), "n1" );
    //~
    //~ TS_ASSERT_EQUALS( var_1.domainSize(), ( gum::Size ) 2 );
    //~
    //~ const gum::Potential<float> &proba_1 = net->cpt( node_1 );
    //~
    //~ TS_ASSERT_EQUALS( proba_1.domainSize(), ( gum::Size ) 2 );
    //~
    //~ gum::Instantiation inst_1( proba_1 );
    //~
    //~ inst_1.setFirst();
    //~
    //~ TS_ASSERT( abs(( proba_1[inst_1] - 0.2 ) ) < 0.001 );
    //~
    //~ inst_1.setLast();
    //~
    //~ TS_ASSERT( abs(( proba_1[inst_1] - 0.8 ) ) < 0.001 );
    //~
    //~
    //~ const gum::DiscreteVariable &var_2 = net->variable( node_2 );
    //~
    //~ TS_ASSERT_EQUALS( var_2.name(), "n2" );
    //~
    //~ TS_ASSERT_EQUALS( var_2.domainSize(), ( gum::Size ) 2 );
    //~
    //~ const gum::Potential<float> &proba_2 = net->cpt( node_2 );
    //~
    //~ TS_ASSERT_EQUALS( proba_2.domainSize(), ( gum::Size ) 2 );
    //~
    //~ gum::Instantiation inst_2( proba_2 );
    //~
    //~ inst_2.setFirst();
    //~
    //~ TS_ASSERT( abs(( proba_2[inst_2] - 0.3 ) ) < 0.001 );
    //~
    //~ inst_2.setLast();
    //~
    //~ TS_ASSERT( abs(( proba_2[inst_2] - 0.7 ) ) < 0.001 );
    //~
    //~
    //~ delete net;
    //~
    //~ }
    //~ }

    private:
    float abs(float d) {
      if (d < 0)
        return (d * (float)-1);

      return d;
    }
  };
}
