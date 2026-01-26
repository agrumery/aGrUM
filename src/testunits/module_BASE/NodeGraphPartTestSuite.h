/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once


#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/parts/nodeGraphPart.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  NodeGraphPart
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct NodeGraphPartTestSuite {
    public:
    static void testConstructor() { GUM_CHECK_ASSERT_THROWS_NOTHING(gum::NodeGraphPart ngp); }

    static void testInsertion() {
      gum::NodeGraphPart ngp;
      CHECK((ngp.size()) == (static_cast< gum::Size >(0)));
      CHECK(ngp.empty());

      gum::NodeId firstId = ngp.addNode();
      CHECK(!ngp.empty());
      CHECK((ngp.size()) == (static_cast< gum::Size >(1)));
      CHECK((firstId) == ((gum::NodeId)0));

      ngp.addNode();
      CHECK((ngp.size()) == (static_cast< gum::Size >(2)));

      ngp.addNode();
      CHECK((ngp.size()) == (static_cast< gum::Size >(3)));

      gum::NodeId next  = ngp.nextNodeId();
      gum::NodeId next2 = ngp.addNode();
      CHECK((next) == (next2));

      GUM_CHECK_ASSERT_THROWS_NOTHING(ngp.addNodeWithId(next2 + 1));
      CHECK_THROWS_AS(ngp.addNodeWithId(next2 + 1), const gum::DuplicateElement&);
    }

    static void testSuppression() {
      gum::NodeGraphPart ngp;
      ngp.addNode();
      ngp.addNode();
      gum::NodeId id3 = ngp.addNode();
      ngp.addNode();

      ngp.eraseNode(id3);
      GUM_CHECK_ASSERT_THROWS_NOTHING(ngp.eraseNode(id3));
      CHECK((ngp.size()) == (static_cast< gum::Size >(3)));

      GUM_CHECK_ASSERT_THROWS_NOTHING(ngp.addNodeWithId(id3));
      CHECK_THROWS_AS(ngp.addNodeWithId(id3), const gum::DuplicateElement&);
      CHECK((ngp.size()) == (static_cast< gum::Size >(4)));

      ngp.clear();
      CHECK((ngp.size()) == (static_cast< gum::Size >(0)));
    }

    static void testAdd2() {
      gum::NodeGraphPart ngp;
      ngp.addNodeWithId(gum::NodeId(3));
      CHECK((ngp._sizeHoles_()) == (static_cast< gum::Size >(3)));
      ngp.addNodeWithId(gum::NodeId(2));
      ngp.addNodeWithId(gum::NodeId(1));
      ngp.addNodeWithId(gum::NodeId(0));
      CHECK((ngp._sizeHoles_()) == (static_cast< gum::Size >(0)));

      gum::NodeGraphPart ngp2;
      ngp2.addNodeWithId(gum::NodeId(0));
      ngp2.addNodeWithId(gum::NodeId(1));
      ngp2.addNodeWithId(gum::NodeId(2));
      ngp2.addNodeWithId(gum::NodeId(3));

      CHECK((ngp) == (ngp2));
    }

    static void testCopy() {
      gum::NodeGraphPart ngp;
      ngp.addNode();
      ngp.addNode();
      _ForTestCopy_(ngp);
      CHECK((ngp._sizeHoles_()) == (static_cast< gum::Size >(0)));
      gum::NodeId id3 = ngp.addNode();
      gum::NodeId id4 = ngp.addNode();
      ngp.eraseNode(id3);
      _ForTestCopy_(ngp);
      CHECK((ngp._sizeHoles_()) == (static_cast< gum::Size >(1)));
      ngp.eraseNode(id4);
      _ForTestCopy_(ngp);
      CHECK((ngp._sizeHoles_()) == (static_cast< gum::Size >(0)));   // 2 last hole has vanished
    }

    static void testInsertionForcee() {
      gum::NodeGraphPart ngp;
      gum::NodeId        a = 1;
      gum::NodeId        b = 2;
      gum::NodeId        c = 3;
      gum::NodeId        d = 4;
      gum::NodeId        e = 5;
      gum::NodeId        f = 6;
      gum::NodeId        g = 7;

      ngp.addNodeWithId(c);
      CHECK(ngp._inHoles_(a));
      CHECK(ngp._inHoles_(b));
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(3))));
      CHECK((ngp.bound()) == (c + 1));

      ngp.addNodeWithId(a);
      CHECK(ngp._inHoles_(b));
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(2))));
      CHECK((ngp.bound()) == (c + 1));

      ngp.addNodeWithId(f);
      CHECK(ngp._inHoles_(b));
      CHECK(ngp._inHoles_(d));
      CHECK(ngp._inHoles_(e));
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(4))));
      CHECK((ngp.bound()) == (f + 1));

      ngp.addNodeWithId(e);
      CHECK(ngp._inHoles_(b));
      CHECK(ngp._inHoles_(d));
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(3))));
      CHECK((ngp.bound()) == (f + 1));

      ngp.addNodeWithId(b);
      CHECK(ngp._inHoles_(d));
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(2))));
      CHECK((ngp.bound()) == (f + 1));

      ngp.addNodeWithId(d);
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(1))));
      CHECK((ngp.bound()) == (f + 1));

      ngp.addNodeWithId(g);
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(1))));
      CHECK((ngp.bound()) == (g + 1));

      ngp.addNodeWithId(gum::NodeId(0));
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(0))));
      CHECK((ngp.bound()) == (g + 1));

      CHECK_THROWS_AS(ngp.addNodeWithId(f), const gum::DuplicateElement&);
    }

    static void testGarbageCollecting() {
      gum::NodeGraphPart ngp;
      gum::NodeId        node = 6;

      CHECK((ngp.bound()) == ((gum::NodeId)(0)));
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(0))));
      CHECK((ngp.nextNodeId()) == ((static_cast< gum::Size >(0))));
      ngp.addNodeWithId(node);
      CHECK((ngp.bound()) == ((gum::NodeId)(node + 1)));
      CHECK((ngp._sizeHoles_()) == ((gum::Size(node))));
      CHECK(ngp.nextNodeId() < node);   // we fill one of the holes
      ngp.eraseNode(node);
      CHECK((ngp._sizeHoles_()) == ((static_cast< gum::Size >(0))));
      CHECK((ngp.nextNodeId()) == ((static_cast< gum::Size >(0))));
      CHECK((ngp.bound()) == ((gum::NodeId)(0)));

      // do we fill all the holes?
      gum::NodeGraphPart ngp2;
      ngp2.addNodeWithId(node);

      for (gum::Size i = 1; i < node; i++) {
        CHECK((ngp2._sizeHoles_()) == ((gum::Size(node) + 1 - i)));
        CHECK(ngp2.addNode() < node);
      }

      CHECK((ngp2._sizeHoles_()) == (static_cast< gum::Size >(1)));

      CHECK((ngp2.nextNodeId()) == (gum::NodeId(node - 1)));

      ngp2.addNode();

      CHECK((ngp2._sizeHoles_()) == (static_cast< gum::Size >(0)));
      CHECK((ngp2.nextNodeId()) == (gum::NodeId(node + 1)));
    }

    static void testUnsafeIterator() {
      gum::NodeGraphPart ngp;

      for (gum::NodeId i = 0; i < 20; ++i) {
        ngp.addNodeWithId(i);
      }

      for (gum::NodeId i = 0; i < 20; ++i) {
        if (i % 3 == 0) { ngp.eraseNode(i); }
      }

      gum::NodeGraphPartIteratorSafe safe_iter = ngp.beginSafe();   // safe iterator needed here

      for (gum::NodeGraphPartIterator iter = ngp.begin(); iter != ngp.end(); ++iter, ++safe_iter) {
        CHECK((*iter) == (*safe_iter));
      }

      gum::Size nb = 0, nb2 = 0;

      for (auto x: ngp) {
        ++nb;
        nb2 += x;
      }

      CHECK((nb) == (static_cast< gum::Size >(13)));
      CHECK(nb2 > gum::Size(0));
    }

    static void testBigNodeGraphPart() {
      GUM_CHECK_ASSERT_THROWS_NOTHING(_privateTestBigNodeGraphPart_());
    }

    static void testIteratorEnd() {
      gum::NodeGraphPart nodeset;
      nodeset.addNode();
      gum::Size cpt = 0;

      for (gum::NodeGraphPartIteratorSafe iter = nodeset.beginSafe();   // safe iterator needed here
           iter != nodeset.endSafe();
           ++iter) {
        if (cpt == 0) {
          nodeset.eraseNode(*iter);
          cpt++;
        } else {
          // If false : infinite loop spotted
          CHECK(false);
          break;
        }
      }
    }

    static void testIteratorEraseNode() {
      gum::NodeGraphPart nodeset;
      const gum::Size    max_cpt = 100;

      for (gum::NodeId i = 0; i < max_cpt; ++i) {
        nodeset.addNode();
      }

      gum::Size cpt = 0;

      for (gum::NodeGraphPartIteratorSafe iter = nodeset.beginSafe();   // safe iterator needed here
           iter != nodeset.endSafe();
           ++iter, ++cpt) {
        GUM_CHECK_ASSERT_THROWS_NOTHING(nodeset.eraseNode(*iter));

        if (cpt > max_cpt) {
          // If false : infinite loop spotted
          CHECK(false);
          break;
        }
      }

      CHECK((cpt) == (max_cpt));
    }

    static void testIteratorAddNodes() {
      gum::NodeGraphPart nodeset;

      auto v = nodeset.addNodes(100);
      for (gum::NodeId i = 0; i < 100; i++)
        CHECK((v[i]) == (i));

      for (int i = 0; i < 5; i++)
        nodeset.eraseNode(2 + i * 19);

      nodeset.addNodes(5);

      CHECK((nodeset.size()) == (static_cast< gum::Size >(100)));

      gum::NodeId i = 0;
      for (auto n: nodeset.nodes())
        CHECK((n) == (i++));

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
        GUM_CHECK_ASSERT_THROWS_NOTHING(futureIds.eraseNode(n));
      }
      CHECK(futureIds.empty());
    }

    private:
    static void _privateTestBigNodeGraphPart_() {
      {
        gum::NodeGraphPart ngp;
        // direct

        for (gum::NodeId node = 1; node < _NBR_PROFILING_NODES_; node++) {
          ngp.addNode();
        }

        for (gum::NodeId node = 1; node < _NBR_PROFILING_NODES_; node++) {
          ngp.eraseNode(node);
        }
      }

      {
        gum::NodeGraphPart ngp;
        // reverse

        for (gum::NodeId node = 1; node < _NBR_PROFILING_NODES_; node++) {
          ngp.addNode();
        }

        for (gum::NodeId node = 1; node < _NBR_PROFILING_NODES_; node++) {
          ngp.eraseNode(_NBR_PROFILING_NODES_ - node);
        }
      }

      {
        gum::NodeGraphPart ngp;

        // direct with id

        for (gum::NodeId node = 1; node < _NBR_PROFILING_NODES_; node++) {
          ngp.addNodeWithId(node);
        }

        for (gum::NodeId node = 1; node < _NBR_PROFILING_NODES_; node++) {
          ngp.eraseNode(node);
        }
      }

      {
        gum::NodeGraphPart ngp;

        // reverse with id

        for (gum::NodeId node = 1; node < _NBR_PROFILING_NODES_; node++) {
          ngp.addNodeWithId(_NBR_PROFILING_NODES_ - node);
        }

        for (gum::NodeId node = 1; node < _NBR_PROFILING_NODES_; node++) {
          ngp.eraseNode(10000 - node);
        }
      }
    }

    static void _ForTestCopy_(gum::NodeGraphPart& ngp) {
      gum::NodeGraphPart ngp2(ngp);
      CHECK((ngp.toString()) == (ngp2.toString()));

      gum::NodeGraphPart ngp3 = ngp;
      CHECK((ngp.toString()) == (ngp3.toString()));

      gum::NodeGraphPart ngp4;
      CHECK(ngp4.empty());
      ngp4 = ngp;
      CHECK((ngp.toString()) == (ngp3.toString()));
    }

    static constexpr const int _NBR_PROFILING_NODES_ = 50000;
  };

  GUM_TEST_ACTIF(Constructor)
  GUM_TEST_ACTIF(Insertion)
  GUM_TEST_ACTIF(Suppression)
  GUM_TEST_ACTIF(Add2)
  GUM_TEST_ACTIF(Copy)
  GUM_TEST_ACTIF(InsertionForcee)
  GUM_TEST_ACTIF(GarbageCollecting)
  GUM_TEST_ACTIF(UnsafeIterator)
  GUM_TEST_ACTIF(BigNodeGraphPart)
  GUM_TEST_ACTIF(IteratorEnd)
  GUM_TEST_ACTIF(IteratorEraseNode)
  GUM_TEST_ACTIF(IteratorAddNodes)
}   // namespace gum_tests
