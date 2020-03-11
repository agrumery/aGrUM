
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN (@LIP6) et Christophe GONZALES (@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/tools/graphs/parts/nodeGraphPart.h>

namespace gum_tests {

  class NodeGraphPartTestSuite: public CxxTest::TestSuite {
    public:
    void testConstructor() {
      TS_GUM_ASSERT_THROWS_NOTHING(gum::NodeGraphPart ngp);
    }

    void testInsertion() {
      gum::NodeGraphPart ngp;
      TS_ASSERT_EQUALS(ngp.size(), (gum::Size)0);
      TS_ASSERT(ngp.empty());

      gum::NodeId firstId = ngp.addNode();
      TS_ASSERT(!ngp.empty());
      TS_ASSERT_EQUALS(ngp.size(), (gum::Size)1);
      TS_ASSERT_EQUALS(firstId, (gum::NodeId)0);

      ngp.addNode();
      TS_ASSERT_EQUALS(ngp.size(), (gum::Size)2);

      ngp.addNode();
      TS_ASSERT_EQUALS(ngp.size(), (gum::Size)3);

      gum::NodeId next = ngp.nextNodeId();
      gum::NodeId next2 = ngp.addNode();
      TS_ASSERT_EQUALS(next, next2);

      TS_GUM_ASSERT_THROWS_NOTHING(ngp.addNodeWithId(next2 + 1));
      TS_ASSERT_THROWS(ngp.addNodeWithId(next2 + 1), gum::DuplicateElement);
    }

    void testSuppression() {
      gum::NodeGraphPart ngp;
      ngp.addNode();
      ngp.addNode();
      gum::NodeId id3 = ngp.addNode();
      ngp.addNode();

      ngp.eraseNode(id3);
      TS_GUM_ASSERT_THROWS_NOTHING(ngp.eraseNode(id3));
      TS_ASSERT_EQUALS(ngp.size(), (gum::Size)3);

      TS_GUM_ASSERT_THROWS_NOTHING(ngp.addNodeWithId(id3));
      TS_ASSERT_THROWS(ngp.addNodeWithId(id3), gum::DuplicateElement);
      TS_ASSERT_EQUALS(ngp.size(), (gum::Size)4);

      ngp.clear();
      TS_ASSERT_EQUALS(ngp.size(), (gum::Size)0);
    }

    void testAdd2() {
      gum::NodeGraphPart ngp;
      ngp.addNodeWithId(gum::NodeId(3));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size)3);
      ngp.addNodeWithId(gum::NodeId(2));
      ngp.addNodeWithId(gum::NodeId(1));
      ngp.addNodeWithId(gum::NodeId(0));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size)0);

      gum::NodeGraphPart ngp2;
      ngp2.addNodeWithId(gum::NodeId(0));
      ngp2.addNodeWithId(gum::NodeId(1));
      ngp2.addNodeWithId(gum::NodeId(2));
      ngp2.addNodeWithId(gum::NodeId(3));

      TS_ASSERT(ngp == ngp2);
    }

    void testCopy() {
      gum::NodeGraphPart ngp;
      ngp.addNode();
      ngp.addNode();
      __ForTestCopy(ngp);
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size)0);
      gum::NodeId id3 = ngp.addNode();
      gum::NodeId id4 = ngp.addNode();
      ngp.eraseNode(id3);
      __ForTestCopy(ngp);
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size)1);
      ngp.eraseNode(id4);
      __ForTestCopy(ngp);
      TS_ASSERT_EQUALS(ngp.__sizeHoles(),
                       (gum::Size)0);   // 2 last hole has vanished
    }

    void testInsertionForcee() {
      gum::NodeGraphPart ngp;
      gum::NodeId        a = 1;
      gum::NodeId        b = 2;
      gum::NodeId        c = 3;
      gum::NodeId        d = 4;
      gum::NodeId        e = 5;
      gum::NodeId        f = 6;
      gum::NodeId        g = 7;

      ngp.addNodeWithId(c);
      TS_ASSERT(ngp.__inHoles(a));
      TS_ASSERT(ngp.__inHoles(b));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(3)));
      TS_ASSERT_EQUALS(ngp.bound(), c + 1);

      ngp.addNodeWithId(a);
      TS_ASSERT(ngp.__inHoles(b));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(2)));
      TS_ASSERT_EQUALS(ngp.bound(), c + 1);

      ngp.addNodeWithId(f);
      TS_ASSERT(ngp.__inHoles(b));
      TS_ASSERT(ngp.__inHoles(d));
      TS_ASSERT(ngp.__inHoles(e));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(4)));
      TS_ASSERT_EQUALS(ngp.bound(), f + 1);

      ngp.addNodeWithId(e);
      TS_ASSERT(ngp.__inHoles(b));
      TS_ASSERT(ngp.__inHoles(d));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(3)));
      TS_ASSERT_EQUALS(ngp.bound(), f + 1);

      ngp.addNodeWithId(b);
      TS_ASSERT(ngp.__inHoles(d));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(2)));
      TS_ASSERT_EQUALS(ngp.bound(), f + 1);

      ngp.addNodeWithId(d);
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(1)));
      TS_ASSERT_EQUALS(ngp.bound(), f + 1);

      ngp.addNodeWithId(g);
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(1)));
      TS_ASSERT_EQUALS(ngp.bound(), g + 1);

      ngp.addNodeWithId(gum::NodeId(0));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(0)));
      TS_ASSERT_EQUALS(ngp.bound(), g + 1);

      TS_ASSERT_THROWS(ngp.addNodeWithId(f), gum::DuplicateElement);
    }

    void testGarbageCollecting() {
      gum::NodeGraphPart ngp;
      gum::NodeId        node = 6;

      TS_ASSERT_EQUALS(ngp.bound(), (gum::NodeId)(0));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(0)));
      TS_ASSERT_EQUALS(ngp.nextNodeId(), (gum::Size(0)));
      ngp.addNodeWithId(node);
      TS_ASSERT_EQUALS(ngp.bound(), (gum::NodeId)(node + 1));
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(node)));
      TS_ASSERT(ngp.nextNodeId() < node);   // we fill one of the holes
      ngp.eraseNode(node);
      TS_ASSERT_EQUALS(ngp.__sizeHoles(), (gum::Size(0)));
      TS_ASSERT_EQUALS(ngp.nextNodeId(), (gum::Size(0)));
      TS_ASSERT_EQUALS(ngp.bound(), (gum::NodeId)(0));

      // do we fill all the holes?
      gum::NodeGraphPart ngp2;
      ngp2.addNodeWithId(node);

      for (gum::Size i = 1; i < node; i++) {
        TS_ASSERT_EQUALS(ngp2.__sizeHoles(), (gum::Size(node) + 1 - i));
        TS_ASSERT(ngp2.addNode() < node);
      }

      TS_ASSERT_EQUALS(ngp2.__sizeHoles(), gum::Size(1));

      TS_ASSERT_EQUALS(ngp2.nextNodeId(), gum::NodeId(node - 1));

      ngp2.addNode();

      TS_ASSERT_EQUALS(ngp2.__sizeHoles(), gum::Size(0));
      TS_ASSERT_EQUALS(ngp2.nextNodeId(), gum::NodeId(node + 1));
    }

    void testUnsafeIterator() {
      gum::NodeGraphPart ngp;

      for (gum::NodeId i = 0; i < 20; ++i) {
        ngp.addNodeWithId(i);
      }

      for (gum::NodeId i = 0; i < 20; ++i) {
        if (i % 3 == 0) { ngp.eraseNode(i); }
      }

      gum::NodeGraphPartIteratorSafe safe_iter =
         ngp.beginSafe();   // safe iterator needed here

      for (gum::NodeGraphPartIterator iter = ngp.begin(); iter != ngp.end();
           ++iter, ++safe_iter) {
        TS_ASSERT_EQUALS(*iter, *safe_iter);
      }

      gum::Size nb = 0, nb2 = 0;

      for (auto x: ngp) {
        ++nb;
        nb2 += x;
      }

      TS_ASSERT_EQUALS(nb, (gum::Size)13);
    }

    void testBigNodeGrapPart() {
      TS_GUM_ASSERT_THROWS_NOTHING(__testBigNodeGrapPart());
    }

    void testIteratorEnd() {
      gum::NodeGraphPart nodeset;
      nodeset.addNode();
      gum::Size cpt = 0;

      for (gum::NodeGraphPartIteratorSafe iter =
              nodeset.beginSafe();   // safe iterator needed here
           iter != nodeset.endSafe();
           ++iter) {
        if (cpt == 0) {
          nodeset.eraseNode(*iter);
          cpt++;
        } else {
          // If false : infinite loop spotted
          TS_ASSERT(false);
          break;
        }
      }
    }

    void testIteratorEraseNode() {
      gum::NodeGraphPart nodeset;
      const gum::Size    max_cpt = 100;

      for (gum::NodeId i = 0; i < max_cpt; ++i) {
        nodeset.addNode();
      }

      gum::Size cpt = 0;

      for (gum::NodeGraphPartIteratorSafe iter =
              nodeset.beginSafe();   // safe iterator needed here
           iter != nodeset.endSafe();
           ++iter, ++cpt) {
        TS_GUM_ASSERT_THROWS_NOTHING(nodeset.eraseNode(*iter));

        if (cpt > max_cpt) {
          // If false : infinite loop spotted
          TS_ASSERT(false);
          break;
        }
      }

      TS_ASSERT_EQUALS(cpt, max_cpt);
    }

    void testIteratorAddNodes() {
      gum::NodeGraphPart nodeset;

      auto v = nodeset.addNodes(100);
      for (gum::NodeId i = 0; i < 100; i++)
        TS_ASSERT_EQUALS(v[i], i)

      for (int i = 0; i < 5; i++)
        nodeset.eraseNode(2 + i * 19);

      nodeset.addNodes(5);

      TS_ASSERT_EQUALS(nodeset.size(), gum::Size(100));

      gum::NodeId i = 0;
      for (auto n: nodeset.nodes())
        TS_ASSERT_EQUALS(n, i++);

      gum::NodeGraphPart nodeset2;
      nodeset2.addNodes(10);
      gum::NodeGraphPart futureIds;

      nodeset2.eraseNode(1);
      futureIds.addNodeWithId(1);
      nodeset2.eraseNode(3);
      futureIds.addNodeWithId(3);
      nodeset2.eraseNode(5);
      futureIds.addNodeWithId(5);

      auto v2 = nodeset2.addNodes(4);
      futureIds.addNodeWithId(10);   // the 4th added node as 10 for id

      for (auto n: v2) {
        TS_GUM_ASSERT_THROWS_NOTHING(futureIds.eraseNode(n));
      }
      TS_ASSERT(futureIds.empty());
    }

    private:
#define NBR_PROFILING_NODES 50000
    void __testBigNodeGrapPart() {
      {
        gum::NodeGraphPart ngp;
        // direct

        for (gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++) {
          ngp.addNode();
        }

        for (gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++) {
          ngp.eraseNode(node);
        }
      }

      {
        gum::NodeGraphPart ngp;
        // reverse

        for (gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++) {
          ngp.addNode();
        }

        for (gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++) {
          ngp.eraseNode(NBR_PROFILING_NODES - node);
        }
      }

      {
        gum::NodeGraphPart ngp;

        // direct with id

        for (gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++) {
          ngp.addNodeWithId(node);
        }

        for (gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++) {
          ngp.eraseNode(node);
        }
      }

      {
        gum::NodeGraphPart ngp;

        // reverse with id

        for (gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++) {
          ngp.addNodeWithId(NBR_PROFILING_NODES - node);
        }

        for (gum::NodeId node = 1; node < NBR_PROFILING_NODES; node++) {
          ngp.eraseNode(10000 - node);
        }
      }
    }

    void __ForTestCopy(gum::NodeGraphPart& ngp) {
      gum::NodeGraphPart ngp2(ngp);
      TS_ASSERT_EQUALS(ngp.toString(), ngp2.toString());

      gum::NodeGraphPart ngp3 = ngp;
      TS_ASSERT_EQUALS(ngp.toString(), ngp3.toString());

      gum::NodeGraphPart ngp4;
      TS_ASSERT(ngp4.empty());
      ngp4 = ngp;
      TS_ASSERT_EQUALS(ngp.toString(), ngp3.toString());
    }
  };
}   // namespace gum_tests
