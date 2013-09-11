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
#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"

#include <agrum/graphs/nodeGraphPart.h>

namespace gum_tests {

  class NodeGraphPartTestSuite: public CxxTest::TestSuite {

    public:

      void testConstructor() {
        TS_GUM_ASSERT_THROWS_NOTHING ( gum::NodeGraphPart ngp );
      }

      void testInsertion() {
        gum::NodeGraphPart ngp;
        TS_ASSERT_EQUALS ( ngp.size(), ( gum::Size ) 0 );
        TS_ASSERT ( ngp.empty() );

        gum::NodeId firstId = ngp.insertNode();
        TS_ASSERT ( ! ngp.empty() );
        TS_ASSERT_EQUALS ( ngp.size(), ( gum::Size ) 1 );
        TS_ASSERT_EQUALS ( firstId, ( gum::NodeId ) 0 );

        ngp.insertNode();
        TS_ASSERT_EQUALS ( ngp.size(), ( gum::Size ) 2 );

        ngp.insertNode();
        TS_ASSERT_EQUALS ( ngp.size(), ( gum::Size ) 3 );

        gum::NodeId next = ngp.nextNodeId();
        gum::NodeId next2 = ngp.insertNode();
        TS_ASSERT_EQUALS ( next, next2 );

        TS_GUM_ASSERT_THROWS_NOTHING ( ngp.insertNode ( next2 + 1 ) );
        TS_ASSERT_THROWS ( ngp.insertNode ( next2 + 1 ), gum::DuplicateElement );
      }

      void testSuppression() {
        gum::NodeGraphPart ngp;
        ngp.insertNode();
        ngp.insertNode();
        gum::NodeId id3 = ngp.insertNode();
        ngp.insertNode();

        ngp.eraseNode ( id3 );
        TS_GUM_ASSERT_THROWS_NOTHING ( ngp.eraseNode ( id3 ) );
        TS_ASSERT_EQUALS ( ngp.size(), ( gum::Size ) 3 );

        TS_GUM_ASSERT_THROWS_NOTHING ( ngp.insertNode ( id3 ) );
        TS_ASSERT_THROWS ( ngp.insertNode ( id3 ), gum::DuplicateElement );
        TS_ASSERT_EQUALS ( ngp.size(), ( gum::Size ) 4 );

        ngp.clear();
        TS_ASSERT_EQUALS ( ngp.size(), ( gum::Size ) 0 );
      }

      void testCopy() {
        gum::NodeGraphPart ngp;
        ngp.insertNode();
        ngp.insertNode();
        __ForTestCopy ( ngp );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ) 0 );
        gum::NodeId id3 = ngp.insertNode();
        gum::NodeId id4 = ngp.insertNode();
        ngp.eraseNode ( id3 );
        __ForTestCopy ( ngp );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ) 1 );
        ngp.eraseNode ( id4 );
        __ForTestCopy ( ngp );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ) 0 ); // 2 last hole has vanished
      }

      void testInsertionForcee() {
        gum::NodeGraphPart ngp;
        gum::NodeId a = 1;
        gum::NodeId b = 2;
        gum::NodeId c = 3;
        gum::NodeId d = 4;
        gum::NodeId e = 5;
        gum::NodeId f = 6;
        gum::NodeId g = 7;

        ngp.insertNode ( c );
        TS_ASSERT ( ngp.__inHoles ( a ) );
        TS_ASSERT ( ngp.__inHoles ( b ) );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 3 ) ) );
        TS_ASSERT_EQUALS ( ngp.bound(), c + 1 );

        ngp.insertNode ( a );
        TS_ASSERT ( ngp.__inHoles ( b ) );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 2 ) ) );
        TS_ASSERT_EQUALS ( ngp.bound(), c + 1 );

        ngp.insertNode ( f );
        TS_ASSERT ( ngp.__inHoles ( b ) );
        TS_ASSERT ( ngp.__inHoles ( d ) );
        TS_ASSERT ( ngp.__inHoles ( e ) );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 4 ) ) );
        TS_ASSERT_EQUALS ( ngp.bound(), f + 1 );

        ngp.insertNode ( e );
        TS_ASSERT ( ngp.__inHoles ( b ) );
        TS_ASSERT ( ngp.__inHoles ( d ) );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 3 ) ) );
        TS_ASSERT_EQUALS ( ngp.bound(), f + 1 );

        ngp.insertNode ( b );
        TS_ASSERT ( ngp.__inHoles ( d ) );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 2 ) ) );
        TS_ASSERT_EQUALS ( ngp.bound(), f + 1 );

        ngp.insertNode ( d );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 1 ) ) );
        TS_ASSERT_EQUALS ( ngp.bound(), f + 1 );

        ngp.insertNode ( g );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 1 ) ) );
        TS_ASSERT_EQUALS ( ngp.bound(), g + 1 );

        ngp.insertNode ( gum::NodeId ( 0 ) );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 0 ) ) );
        TS_ASSERT_EQUALS ( ngp.bound(), g + 1 );

        TS_ASSERT_THROWS ( ngp.insertNode ( f ), gum::DuplicateElement );
      }

      void testGarbageCollecting() {
        gum::NodeGraphPart ngp;
        gum::NodeId node = 6;

        TS_ASSERT_EQUALS ( ngp.bound(), ( gum::NodeId ) ( 0 ) );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 0 ) ) );
        TS_ASSERT_EQUALS ( ngp.nextNodeId(), ( gum::Size ( 0 ) ) );
        ngp.insertNode ( node );
        TS_ASSERT_EQUALS ( ngp.bound(), ( gum::NodeId ) ( node + 1 ) );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( node ) ) );
        TS_ASSERT ( ngp.nextNodeId() < node ); // we fill one of the holes
        ngp.eraseNode ( node );
        TS_ASSERT_EQUALS ( ngp.__sizeHoles(), ( gum::Size ( 0 ) ) );
        TS_ASSERT_EQUALS ( ngp.nextNodeId(), ( gum::Size ( 0 ) ) );
        TS_ASSERT_EQUALS ( ngp.bound(), ( gum::NodeId ) ( 0 ) );

        // do we fill all the holes?
        gum::NodeGraphPart ngp2;
        ngp2.insertNode ( node );

        for ( gum::Size i = 1; i < node; i++ ) {
          TS_ASSERT_EQUALS ( ngp2.__sizeHoles(), ( gum::Size ( node ) + 1 - i ) );
          TS_ASSERT ( ngp2.insertNode() < node );
        }

        TS_ASSERT_EQUALS ( ngp2.__sizeHoles(), gum::Size ( 1 ) );

        TS_ASSERT_EQUALS ( ngp2.nextNodeId(), gum::NodeId ( node - 1 ) );

        ngp2.insertNode();

        TS_ASSERT_EQUALS ( ngp2.__sizeHoles(), gum::Size ( 0 ) );
        TS_ASSERT_EQUALS ( ngp2.nextNodeId(), gum::NodeId ( node + 1 ) );
      }


      void testBigNodeGrapPart() {
        TS_GUM_ASSERT_THROWS_NOTHING ( __testBigNodeGrapPart() );
      }

      void testIteratorEnd() {
        gum::NodeGraphPart nodeset;
        nodeset.insertNode();
        unsigned int cpt = 0;

        //for ( gum::NodeGraphPartIterator iter = nodeset.beginNodes();iter != nodeset.endNodes(); ++iter ) {
        for ( const auto iter : nodeset ) {
          if ( cpt == 0 ) {
            nodeset.eraseNode ( iter );
            cpt++;
          } else {
            // If false : infinite loop spotted
            TS_ASSERT ( false );
            break;
          }
        }
      }

      void testIteratorEraseNode() {
        gum::NodeGraphPart nodeset;
        const unsigned int max_cpt = 100;

        for ( unsigned int i = 0; i < max_cpt; ++i ) {
          nodeset.insertNode();
        }

        unsigned int cpt = 0;

        //for ( gum::NodeGraphPartIterator iter = nodeset.beginNodes();iter != nodeset.endNodes(); ++iter, ++cpt ) {
        for ( const auto iter : nodeset ) {
          TS_GUM_ASSERT_THROWS_NOTHING ( nodeset.eraseNode ( iter ) );

          if ( cpt > max_cpt ) {
            // If false : infinite loop spotted
            TS_ASSERT ( false );
            break;
          }

          cpt++;
        }

        TS_ASSERT_EQUALS ( cpt, max_cpt );
      }


    private:
#define NBR_PROFILING_NODES 50000
      void __testBigNodeGrapPart() {
        {
          gum::NodeGraphPart ngp;
          // direct

          for ( gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++ ) {
            ngp.insertNode();
          }

          for ( gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++ ) {
            ngp.eraseNode ( node );
          }
        }

        {
          gum::NodeGraphPart ngp;
          //reverse

          for ( gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++ ) {
            ngp.insertNode();
          }

          for ( gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++ ) {
            ngp.eraseNode ( NBR_PROFILING_NODES - node );
          }
        }

        {
          gum::NodeGraphPart ngp;

          // direct with id

          for ( gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++ ) {
            ngp.insertNode ( node );
          }

          for ( gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++ ) {
            ngp.eraseNode ( node );
          }
        }

        {
          gum::NodeGraphPart ngp;

          // reverse with id

          for ( gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++ ) {
            ngp.insertNode ( NBR_PROFILING_NODES - node );
          }

          for ( gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++ ) {
            ngp.eraseNode ( 10000 - node );
          }
        }
      }


      void __ForTestCopy ( gum::NodeGraphPart& ngp ) {
        gum::NodeGraphPart ngp2 ( ngp );
        TS_ASSERT_EQUALS ( ngp.toString(), ngp2.toString() );

        gum::NodeGraphPart ngp3 = ngp;
        TS_ASSERT_EQUALS ( ngp.toString(), ngp3.toString() );

        gum::NodeGraphPart ngp4;
        TS_ASSERT ( ngp4.empty() );
        ngp4 = ngp;
        TS_ASSERT_EQUALS ( ngp.toString(), ngp3.toString() );
      }
  };

}
