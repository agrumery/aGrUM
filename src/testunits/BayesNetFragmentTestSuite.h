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
// all variables are binary except 5 and 6 (ternary)

namespace gum_tests {

  class BayesNetFragmentTestSuite: public CxxTest::TestSuite {

    private:
      void fillTopo ( gum::BayesNet<float>& bn, gum::List<gum::NodeId>& idList ) {
        try {
          auto var1 = gum::LabelizedVariable ( "v1", "1", 2 );
          auto var2 = gum::LabelizedVariable ( "v2", "2", 2 );
          auto var3 = gum::LabelizedVariable ( "v3", "3", 2 );
          auto var4 = gum::LabelizedVariable ( "v4", "4", 2 );
          auto var5 = gum::LabelizedVariable ( "v5", "(gum::Size) 3", 3 );
          auto var6 = gum::LabelizedVariable ( "v6", "(gum::Size) 3", 3 );

          bn.add ( var1 );
          bn.add ( var2 );
          bn.add ( var3 );
          bn.add ( var4 );
          bn.add ( var5 );
          bn.add ( var6 ) ;

          bn.addArc ( bn.idFromName ( "v1" ), bn.idFromName ( "v3" ) );
          bn.addArc ( bn.idFromName ( "v1" ), bn.idFromName ( "v4" ) );
          bn.addArc ( bn.idFromName ( "v2" ), bn.idFromName ( "v4" ) );
          bn.addArc ( bn.idFromName ( "v2" ), bn.idFromName ( "v5" ) );
          bn.addArc ( bn.idFromName ( "v3" ), bn.idFromName ( "v5" ) );
          bn.addArc ( bn.idFromName ( "v3" ), bn.idFromName ( "v6" ) );
          bn.addArc ( bn.idFromName ( "v4" ), bn.idFromName ( "v5" ) );

        } catch ( gum::Exception& e ) {
          std::cerr << std::endl << e.content() << std::endl;
          throw ;
        }
      }

      void fill ( gum::BayesNet<float>& bn ) {
        gum::List<gum::NodeId> idList;
        fillTopo ( bn, idList );
        bn.generateCPTs();
      }

    public:
      void testCreation() {
        gum::BayesNet<float> bn;
        fill ( bn );

        gum::BayesNetFragment<float> frag ( bn );
      }

      void testInstallNodes() {
        gum::BayesNet<float> bn;
        fill ( bn );

        gum::BayesNetFragment<float> frag ( bn );

        // install a node
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 0 );
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installNode ( bn.idFromName ( "v1" ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 1 );

        // install twice the same node
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installNode ( bn.idFromName ( "v1" ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 1 );
        TS_ASSERT_EQUALS ( frag.sizeArcs(), ( gum::Size ) 0 );

        // install a non-existing node
        TS_ASSERT_EQUALS ( bn.dag().exists ( ( gum::NodeId ) 100 ),false );
        TS_ASSERT_THROWS ( frag.installNode ( ( gum::NodeId ) 100 ),gum::NotFound );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 1 );
        TS_ASSERT_EQUALS ( frag.sizeArcs(), ( gum::Size ) 0 );

        // install a second node (without arc)
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installNode ( bn.idFromName ( "v6" ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 2 );
        TS_ASSERT_EQUALS ( frag.sizeArcs(), ( gum::Size ) 0 );

        // install a third node (and 2 arcs)
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installNode ( bn.idFromName ( "v3" ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 3 );
        TS_ASSERT_EQUALS ( frag.sizeArcs(), ( gum::Size ) 2 );

        // install ascendants (nothing should happen)
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installAscendants ( bn.idFromName ( "v6" ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 3 );
        TS_ASSERT_EQUALS ( frag.sizeArcs(), ( gum::Size ) 2 );


        // install ascendants (nothing should happen)
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installAscendants ( bn.idFromName ( "v5" ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 6 );
        TS_ASSERT_EQUALS ( frag.sizeArcs(), ( gum::Size ) 7 );

        // another test for ascendants
        gum::BayesNetFragment<float> frag2 ( bn );
        TS_GUM_ASSERT_THROWS_NOTHING ( frag2.installAscendants ( bn.idFromName ( "v5" ) ) );
        TS_ASSERT_EQUALS ( frag2.size(), ( gum::Size ) 5 );
        TS_ASSERT_EQUALS ( frag2.sizeArcs(), ( gum::Size ) 6 );
      }

      void testUninstallNode() {
        gum::BayesNet<float> bn;
        fill ( bn );

        gum::BayesNetFragment<float> frag ( bn );

        // install ascendants (nothing should happen)
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installAscendants ( bn.idFromName ( "v6" ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 3 );
        TS_ASSERT_EQUALS ( frag.sizeArcs(), ( gum::Size ) 2 );

        //uninstall node 3 (in the middle)
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.uninstallNode ( bn.idFromName ( "v3" ) ) );
        TS_ASSERT_EQUALS ( frag.size(), ( gum::Size ) 2 );
        TS_ASSERT_EQUALS ( frag.sizeArcs(), ( gum::Size ) 0 );
      }

      void testIBayetNetMethodsWithoutLocalCPTs() {
        gum::BayesNet<float> bn;
        fill ( bn );

        gum::BayesNetFragment<float> frag ( bn );

        TS_ASSERT ( frag.empty() );
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installNode ( bn.idFromName ( "v1" ) ) );
        TS_ASSERT ( ! frag.empty() );
        
        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installNode ( bn.idFromName ( "v6" ) ) );

        TS_ASSERT_EQUALS ( frag.dag().sizeNodes(),gum::Size ( 2 ) );
        TS_ASSERT_EQUALS ( frag.dag().sizeArcs(),gum::Size ( 0 ) );
        TS_ASSERT_EQUALS ( frag.size(),gum::Size ( 2 ) );
        TS_ASSERT_EQUALS ( frag.dim(),gum::Size ( ( 3-1 ) + ( 2-1 ) ) );
        TS_ASSERT_EQUALS ( pow ( 10,frag.log10DomainSize() ),2*3 );

        TS_GUM_ASSERT_THROWS_NOTHING ( frag.installAscendants ( bn.idFromName ( "v6" ) ) );

        TS_ASSERT_EQUALS ( frag.dag().sizeNodes(),gum::Size ( 3 ) );
        TS_ASSERT_EQUALS ( frag.dag().sizeArcs(),gum::Size ( 2 ) );
        TS_ASSERT_EQUALS ( frag.size(),gum::Size ( 3 ) );
        TS_ASSERT_EQUALS ( frag.dim(),gum::Size (
                             ( 2* ( 3-1 ) ) +
                             ( 2* ( 2-1 ) ) +
                             ( 2-1 ) ) );
        TS_ASSERT_DELTA ( pow ( 10,frag.log10DomainSize() ),2*2*3 ,1e-5 );

        gum::Instantiation I;
        frag.completeInstantiation ( I );
        I.setFirst();
        TS_ASSERT_EQUALS ( I.toString(),"<v1:0|v3:0|v6:0>" );

        while ( ! I.end() ) {
          TS_ASSERT_DELTA ( frag.jointProbability ( I ),
                            bn.cpt ( bn.idFromName ( "v1" ) ) [I]*
                            bn.cpt ( bn.idFromName ( "v3" ) ) [I]*
                            bn.cpt ( bn.idFromName ( "v6" ) ) [I] ,
                            1e-5 );
          TS_ASSERT_DELTA ( frag.log2JointProbability ( I ),
                            log2 ( bn.cpt ( bn.idFromName ( "v1" ) ) [I]*
                                   bn.cpt ( bn.idFromName ( "v3" ) ) [I]*
                                   bn.cpt ( bn.idFromName ( "v6" ) ) [I] ),
                            1e-5 );
          ++I;
        }
      }
  };
} // gum_tests
