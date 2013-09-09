/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
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

#include <iostream>

#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"

#include <agrum/variables/labelizedVariable.h>
#include <agrum/BN/BayesNetFragment.h>

// The graph used for the tests:
//          1   2_          1 -> 3
//         / \ / /          1 -> 4
//        3   4 /           3 -> 5
//       / \ / /            4 -> 5
//      6   5_/             2 -> 4
//                          2 -> 5
//                          3 -> 6
namespace gum_tests {

  class BayesNetFragmentTestSuite: public CxxTest::TestSuite {

    private:
      void fillTopo ( gum::BayesNet<float>& bn, gum::List<gum::NodeId>& idList ) {
        try {
          auto var1 = new gum::LabelizedVariable ( "var1", "1", 2 );
          auto var2 = new gum::LabelizedVariable ( "var2", "2", 2 );
          auto var3 = new gum::LabelizedVariable ( "var3", "3", 2 );
          auto var4 = new gum::LabelizedVariable ( "var4", "4", 2 );
          auto var5 = new gum::LabelizedVariable ( "var5", "(gum::Size) 3", 3 );
          auto var6 = new gum::LabelizedVariable ( "var6", "(gum::Size) 3", 3 );

          idList.insert ( bn.add ( *var1 ) );
          idList.insert ( bn.add ( *var2 ) );
          idList.insert ( bn.add ( *var3 ) );
          idList.insert ( bn.add ( *var4 ) );
          idList.insert ( bn.add ( *var5 ) );
          idList.insert ( bn.add ( *var6 ) );

          bn.addArc ( idList[0], idList[2] );
          bn.addArc ( idList[2], idList[4] );
          bn.addArc ( idList[1], idList[3] );
          bn.addArc ( idList[0], idList[3] );
          bn.addArc ( idList[3], idList[4] );
          bn.addArc ( idList[1], idList[4] );
          bn.addArc ( idList[2], idList[5] );

        } catch ( gum::Exception& e ) {
          std::cerr << std::endl << e.content() << std::endl;
          throw ;
        }
      }

      void fill ( gum::BayesNet<float>& bn ) {
        gum::List<gum::NodeId> idList;
        fillTopo ( bn, idList );

        try {

          bn.cpt ( idList[0] ).fillWith ( std::vector<float> {
            0.2, 0.8
          } );

          bn.cpt ( idList[1] ).fillWith ( std::vector<float> {
            0.3, 0.7
          } );

          bn.cpt ( idList[2] ).fillWith ( std::vector<float> {
            0.1, 0.9,
            0.9, 0.1
          } );

          bn.cpt ( idList[3] ).fillWith ( std::vector<float> {
            0.4, 0.6,
            0.5, 0.5,
            0.5, 0.5,
            1.0, 0.0
          } );

          bn.cpt ( idList[4] ).fillWith ( std::vector<float>  {
            0.3, 0.6, 0.1,
            0.5, 0.5, 0.0,
            0.5, 0.5, 0.0,
            1.0, 0.0, 0.0,
            0.4, 0.6, 0.0,
            0.5, 0.5, 0.0,
            0.5, 0.5, 0.0,
            0.0, 0.0, 1.0
          } );

          bn.cpt ( idList[5] ).fillWith ( std::vector<float> {
            0.4, 0.5,0.1,
            0.1,0.4, 0.5
          } );

        } catch ( gum::Exception& e ) {
          std::cerr << std::endl << e.content() << std::endl;
          throw ;
        }
      }

    public:
      void testCreation() {
        gum::BayesNet<float> bn;
        fill ( bn );

        gum::BayesNetFragment<float> frag ( bn );
      }

      void testImportNode() {
        gum::BayesNet<float> bn;
        fill ( bn );

        gum::BayesNetFragment<float> frag ( bn );
        gum::Sequence<gum::NodeId> top=bn.topologicalOrder();

        // import a node
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 0 );
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.importNode ( top.atPos ( 0 ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 1 );

        // import twice the same node
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.importNode ( top.atPos ( 0 ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 1 );

        // import a non-existing node
        TS_ASSERT_EQUALS ( top.exists ( ( gum::NodeId ) 100 ),false );
        TS_ASSERT_THROWS ( frag.importNode ( ( gum::NodeId ) 100 ),gum::NotFound );
      }

      void testIBayetNetMethodsWithoutLocalCPTs() {
        gum::BayesNet<float> bn;
        fill ( bn );

        gum::BayesNetFragment<float> frag ( bn );

        TS_GUM_ASSERT_THROWS_NOTHING ( frag.importNode ( bn.idFromName ( "var1" ) ) );
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.importNode ( bn.idFromName ( "var6" ) ) );

        TS_ASSERT_EQUALS ( frag.dag().sizeArcs(),gum::Size ( 0 ) );
        TS_ASSERT_EQUALS ( frag.dag().sizeNodes(),gum::Size ( 2 ) );
        TS_ASSERT_EQUALS ( frag.size(),gum::Size ( 2 ) );
        TS_ASSERT_EQUALS ( frag.dim(),gum::Size ( ( 3-1 ) + ( 2-1 ) ) );
        TS_ASSERT_EQUALS ( pow ( 10,frag.log10DomainSize() ),2*3 );
      }
  };
} // gum_tests
