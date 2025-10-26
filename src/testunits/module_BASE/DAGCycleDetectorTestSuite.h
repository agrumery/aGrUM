/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
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


#include <chrono>
#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphs/algorithms/DAGCycleDetector.h>

#include <agrum/base/core/utils_random.h>

namespace gum_tests {

  class GUM_TEST_SUITE(DAGCycleDetector) {
    gum::DAG _createDAG_(gum::Size nb_nodes, gum::Size nb_arcs) {
      gum::DAG dag;
      for (gum::Idx i = 0; i < nb_nodes; ++i) {
        dag.addNodeWithId(i);
      }
      std::uniform_int_distribution< int > distribution(0, int(nb_nodes) - 1);
      while (nb_arcs) {
        gum::NodeId id1 = gum::randomValue(nb_nodes);
        gum::NodeId id2 = gum::randomValue(nb_nodes);
        if ((id1 != id2) && (!dag.existsArc(id1, id2))) {
          try {
            dag.addArc(id1, id2);
            --nb_arcs;
          } catch (gum::Exception&) {}
        }
      }

      return dag;
    }   // namespace gum_tests

    void _createChanges_(const gum::DAG&                               g,
                         std::vector< gum::DAGCycleDetector::Change >& changes,
                         std::vector< gum::DAGCycleDetector::Change >& del_add_changes,
                         gum::Size                                     length) {
      std::uniform_int_distribution< int > distrib_node(0, int(g.size()) - 1);
      std::uniform_int_distribution< int > distrib_arc(0, int(g.size() * g.size()));

      changes.clear();
      del_add_changes.clear();

      gum::DiGraph gg = g;

      for (; length; --length) {
        gum::Idx chgt_type = gum::randomValue(2);

        switch (chgt_type) {
          case 0 : {   // deletions
            float nb_arcs = (float)gg.sizeArcs();
            if (!nb_arcs) {
              ++length;
              break;
            }
            nb_arcs /= g.size() * g.size();
            gum::Size nb_del_arc = (gum::Size)(gum::randomValue(g.size() * g.size()) * nb_arcs);
            if (nb_del_arc >= gg.sizeArcs()) nb_del_arc = gg.sizeArcs() - 1;
            for (auto iter = gg.arcs().begin(); iter != gg.arcs().end(); ++iter, --nb_del_arc) {
              if (!nb_del_arc) {
                changes.push_back(gum::DAGCycleDetector::ArcDel(iter->tail(), iter->head()));
                gg.eraseArc(gum::Arc(iter->tail(), iter->head()));
                break;
              }
            }
          } break;

          case 1 : {   // additions
            gum::NodeId node1, node2;
            while (true) {
              node1 = gum::randomValue(g.size());
              node2 = gum::randomValue(g.size());
              if ((node1 != node2) && !gg.existsArc(node1, node2) && !gg.existsArc(node2, node1)) {
                changes.push_back(gum::DAGCycleDetector::ArcAdd(node1, node2));
                gg.addArc(node1, node2);
                break;
              }
            }
          } break;

          case 2 : {   // reversal
            float nb_arcs = float(gg.sizeArcs());
            if (!nb_arcs) {
              ++length;
              break;
            }
            nb_arcs /= g.size() * g.size();
            gum::Size nb_del_arc = gum::Size(gum::randomValue(g.size() * g.size()) * nb_arcs);
            if (nb_del_arc >= gg.sizeArcs()) nb_del_arc = gg.sizeArcs() - 1;
            for (auto iter = gg.arcs().begin(); iter != gg.arcs().end(); ++iter, --nb_del_arc) {
              if (!nb_del_arc) {
                gum::NodeId tail = iter->tail(), head = iter->head();
                changes.push_back(gum::DAGCycleDetector::ArcReverse(tail, head));
                gg.eraseArc(gum::Arc(tail, head));
                gg.addArc(head, tail);
                break;
              }
            }
          } break;
        }
      }

      gum::HashTable< gum::Arc, gum::Idx > deletions(gum::Size(changes.size()));
      gum::HashTable< gum::Arc, gum::Idx > additions(gum::Size(changes.size()));
      for (const auto& modif: changes) {
        gum::Arc arc(modif.tail(), modif.head());

        switch (modif.type()) {
          case gum::DAGCycleDetector::ChangeType::ARC_DELETION :
            if (deletions.exists(arc)) ++deletions[arc];
            else deletions.insert(arc, 1);
            break;

          case gum::DAGCycleDetector::ChangeType::ARC_ADDITION :
            if (additions.exists(arc)) ++additions[arc];
            else additions.insert(arc, 1);
            break;

          case gum::DAGCycleDetector::ChangeType::ARC_REVERSAL :
            if (deletions.exists(arc)) ++deletions[arc];
            else deletions.insert(arc, 1);
            arc = gum::Arc(modif.head(), modif.tail());
            if (additions.exists(arc)) ++additions[arc];
            else additions.insert(arc, 1);
            break;

          default : GUM_ERROR(gum::OperationNotAllowed, "undefined change type")
        }
      }

      for (auto iter = additions.beginSafe();   // safe iterator needed here
           iter != additions.endSafe();
           ++iter) {
        if (deletions.exists(iter.key())) {
          gum::Size& nb_del = deletions[iter.key()];
          gum::Size& nb_add = iter.val();
          if (nb_del > nb_add) {
            additions.erase(iter);
            nb_del -= nb_add;
          } else if (nb_del < nb_add) {
            deletions.erase(iter.key());
            nb_add -= nb_del;
          } else {
            deletions.erase(iter.key());
            additions.erase(iter);
          }
        }
      }

      for (auto iter = deletions.cbegin(); iter != deletions.cend(); ++iter) {
        del_add_changes.push_back(
            gum::DAGCycleDetector::ArcDel(iter.key().tail(), iter.key().head()));
      }
      for (auto iter = additions.cbegin(); iter != additions.cend(); ++iter) {
        del_add_changes.push_back(
            gum::DAGCycleDetector::ArcAdd(iter.key().tail(), iter.key().head()));
      }
    }

    public:
    GUM_ACTIVE_TEST(SmallGraph) {
      gum::DAG    g;
      gum::NodeId id1, id2, id3, id4, id5;
      id1 = g.addNode();
      id2 = g.addNode();
      id3 = g.addNode();
      id4 = g.addNode();
      id5 = g.addNode();

      g.addArc(id1, id3);
      g.addArc(id3, id5);
      g.addArc(id2, id4);
      g.addArc(id1, id4);
      g.addArc(id4, id5);
      g.addArc(id2, id5);

      gum::DAGCycleDetector detector;
      detector.setDAG(g);

      std::vector< gum::DAGCycleDetector::Change > changes;
      TS_ASSERT_EQUALS(detector.hasCycleFromModifications(changes), false)

      changes.push_back(gum::DAGCycleDetector::ArcAdd(id5, id1));
      TS_ASSERT_EQUALS(detector.hasCycleFromModifications(changes), true)

      changes.clear();
      changes.push_back(gum::DAGCycleDetector::ArcAdd(id2, id1));
      changes.push_back(gum::DAGCycleDetector::ArcAdd(id5, id1));
      changes.push_back(gum::DAGCycleDetector::ArcReverse(id4, id5));
      changes.push_back(gum::DAGCycleDetector::ArcDel(id1, id3));
      TS_ASSERT_EQUALS(detector.hasCycleFromModifications(changes), false)

      changes.clear();
      changes.push_back(gum::DAGCycleDetector::ArcAdd(id2, id1));
      changes.push_back(gum::DAGCycleDetector::ArcAdd(id5, id1));
      changes.push_back(gum::DAGCycleDetector::ArcDel(id1, id3));
      changes.push_back(gum::DAGCycleDetector::ArcReverse(id4, id5));
      TS_ASSERT_EQUALS(detector.hasCycleFromModifications(changes), false)
    }

    GUM_ACTIVE_TEST(G1) {
      gum::DAG g;
      for (gum::Idx i = 0; i < 10; ++i) {
        g.addNodeWithId(i);
      }
      g.addArc(3, 9);
      g.addArc(3, 5);
      g.addArc(3, 0);
      g.addArc(6, 9);
      g.addArc(8, 1);
      g.addArc(0, 6);
      g.addArc(5, 7);
      g.addArc(2, 4);
      g.addArc(4, 0);
      g.addArc(2, 9);
      g.addArc(2, 5);
      g.addArc(7, 6);
      g.addArc(1, 7);

      gum::DAGCycleDetector detector;
      detector.setDAG(g);

      std::vector< gum::DAGCycleDetector::Change > changes;
      changes.push_back(gum::DAGCycleDetector::ArcReverse(2, 9));
      changes.push_back(gum::DAGCycleDetector::ArcReverse(6, 9));
      changes.push_back(gum::DAGCycleDetector::ArcReverse(3, 0));
      changes.push_back(gum::DAGCycleDetector::ArcAdd(4, 3));

      TS_ASSERT_EQUALS(detector.hasCycleFromModifications(changes), true)
    }

    GUM_ACTIVE_TEST(Random) {
      gum::DAGCycleDetector                        detector;
      std::vector< gum::DAGCycleDetector::Change > changes;
      std::vector< gum::DAGCycleDetector::Change > del_add_changes;

      for (gum::Idx i = 15; i < 25; ++i) {
        gum::DAG g = _createDAG_(20, i);
        detector.setDAG(g);

        changes.clear();
        TS_ASSERT_EQUALS(detector.hasCycleFromModifications(changes), false)

        for (gum::Idx j = 0; j < 20; ++j) {
          gum::Size length = gum::randomValue(11);
          _createChanges_(g, changes, del_add_changes, length);

          bool hasCycle = false;
          {
            gum::DiGraph gg = g;
            for (auto& chgt: del_add_changes) {
              switch (chgt.type()) {
                case gum::DAGCycleDetector::ChangeType::ARC_DELETION :
                  gg.eraseArc(gum::Arc(chgt.tail(), chgt.head()));
                  break;

                case gum::DAGCycleDetector::ChangeType::ARC_ADDITION :
                  gg.addArc(chgt.tail(), chgt.head());
                  break;

                default : GUM_ERROR(gum::NotFound, "del_add_changes")
              }
            }

            try {
              gum::DAG dag = g;
              for (auto iter = g.arcs().begin(); iter != g.arcs().end(); ++iter) {
                dag.eraseArc(*iter);
              }
              for (auto iter = gg.arcs().begin(); iter != gg.arcs().end(); ++iter) {
                dag.addArc(iter->tail(), iter->head());
              }
            } catch (gum::InvalidDirectedCycle&) { hasCycle = true; }
          }

          TS_ASSERT_EQUALS(detector.hasCycleFromModifications(changes), hasCycle)
        }
      }
    }

    GUM_ACTIVE_TEST(Modifications) {
      gum::DAGCycleDetector                        detector1, detector2;
      std::vector< gum::DAGCycleDetector::Change > changes;
      std::vector< gum::DAGCycleDetector::Change > del_add_changes;

      for (gum::Idx i = 7; i < 15; ++i) {
        gum::DAG g = _createDAG_(10, i);
        detector1.setDAG(g);
        detector2.setDAG(g);

        TS_ASSERT_EQUALS(detector1, detector2)

        for (gum::Idx j = 0; j < 30; ++j) {
          _createChanges_(g, changes, del_add_changes, 1);
          TS_ASSERT_EQUALS(changes.size(), static_cast< gum::Size >(1))
          if (detector1.hasCycleFromModifications(changes)) {
            --j;
            continue;
          }

          for (auto& chgt: changes) {
            switch (chgt.type()) {
              case gum::DAGCycleDetector::ChangeType::ARC_DELETION :
                g.eraseArc(gum::Arc(chgt.tail(), chgt.head()));
                detector1.eraseArc(chgt.tail(), chgt.head());
                break;

              case gum::DAGCycleDetector::ChangeType::ARC_ADDITION :
                g.addArc(chgt.tail(), chgt.head());
                detector1.addArc(chgt.tail(), chgt.head());
                break;

              case gum::DAGCycleDetector::ChangeType::ARC_REVERSAL :
                g.eraseArc(gum::Arc(chgt.tail(), chgt.head()));
                g.addArc(chgt.head(), chgt.tail());
                detector1.reverseArc(chgt.tail(), chgt.head());
                break;

              default : GUM_ERROR(gum::NotFound, "del_add_changes")
            }
            detector2.setDAG(g);
          }

          TS_ASSERT_EQUALS(detector1, detector2)
        }
      }
    }

    GUM_ACTIVE_TEST(Modifications2) {
      gum::DAGCycleDetector                        detector1, detector2;
      std::vector< gum::DAGCycleDetector::Change > changes;
      std::vector< gum::DAGCycleDetector::Change > del_add_changes;

      for (gum::Idx i = 7; i < 15; ++i) {
        gum::DAG g = _createDAG_(10, i);
        detector1.setDAG(g);
        detector2.setDAG(g);

        TS_ASSERT_EQUALS(detector1, detector2)

        for (gum::Idx j = 0; j < 30; ++j) {
          _createChanges_(g, changes, del_add_changes, 1);
          TS_ASSERT_EQUALS(changes.size(), static_cast< gum::Size >(1))

          for (auto& chgt: changes) {
            switch (chgt.type()) {
              case gum::DAGCycleDetector::ChangeType::ARC_DELETION :
                TS_ASSERT_EQUALS(detector1.hasCycleFromDeletion(chgt.tail(), chgt.head()),
                                 detector2.hasCycleFromModifications(changes));
                break;

              case gum::DAGCycleDetector::ChangeType::ARC_ADDITION :
                TS_ASSERT_EQUALS(detector1.hasCycleFromAddition(chgt.tail(), chgt.head()),
                                 detector2.hasCycleFromModifications(changes));
                break;

              case gum::DAGCycleDetector::ChangeType::ARC_REVERSAL :
                TS_ASSERT_EQUALS(detector1.hasCycleFromReversal(chgt.tail(), chgt.head()),
                                 detector2.hasCycleFromModifications(changes));
                break;

              default : GUM_ERROR(gum::NotFound, "del_add_changes")
            }
          }
        }
      }
    }
  };

}   // namespace gum_tests
