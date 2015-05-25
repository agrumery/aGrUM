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
#include <chrono>
#include <random>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/utils_random.h>
#include <agrum/graphs/DAGCycleDetector.h>

namespace gum_tests {

  class DAGCycleDetectorTestSuite : public CxxTest::TestSuite {
    private:
    std::default_random_engine generator{gum::randomGeneratorSeed()};

    gum::DAG __createDAG(unsigned int nb_nodes, unsigned int nb_arcs) {
      gum::DAG dag;
      for (unsigned int i = 0; i < nb_nodes; ++i) {
        dag.addNode(i);
      }
      std::uniform_int_distribution<int> distribution(0, nb_nodes - 1);
      while (nb_arcs) {
        gum::NodeId id1 = distribution(generator);
        gum::NodeId id2 = distribution(generator);
        if ((id1 != id2) && (!dag.existsArc(id1, id2))) {
          try {
            dag.addArc(id1, id2);
            --nb_arcs;
          } catch (gum::Exception &) {
          }
        }
      }

      return dag;
    }

    void __createChanges(const gum::DAG &g,
                         std::vector<gum::DAGCycleDetector::Change> &changes,
                         std::vector<gum::DAGCycleDetector::Change> &del_add_changes,
                         unsigned int length) {
      std::uniform_int_distribution<int> distrib_type(0, 2);
      std::uniform_int_distribution<int> distrib_node(0, g.size() - 1);
      std::uniform_int_distribution<int> distrib_arc(0, g.size() * g.size());

      changes.clear();
      del_add_changes.clear();

      gum::DiGraph gg = g;

      for (; length; --length) {
        unsigned int chgt_type = distrib_type(generator);

        switch (chgt_type) {
          case 0: { // deletions
            float nb_arcs = gg.sizeArcs();
            if (!nb_arcs) {
              ++length;
              break;
            }
            nb_arcs /= g.size() * g.size();
            unsigned int nb_del_arc = distrib_arc(generator) * nb_arcs;
            if (nb_del_arc >= gg.sizeArcs())
              nb_del_arc = gg.sizeArcs() - 1;
            for (auto iter = gg.arcs().begin(); iter != gg.arcs().end();
                 ++iter, --nb_del_arc) {
              if (!nb_del_arc) {
                changes.push_back(
                    gum::DAGCycleDetector::ArcDel(iter->tail(), iter->head()));
                gg.eraseArc(gum::Arc(iter->tail(), iter->head()));
                break;
              }
            }
          } break;

          case 1: { // additions
            unsigned int node1, node2;
            while (true) {
              node1 = distrib_node(generator);
              node2 = distrib_node(generator);
              if ((node1 != node2) && !gg.existsArc(node1, node2) &&
                  !gg.existsArc(node2, node1)) {
                changes.push_back(gum::DAGCycleDetector::ArcAdd(node1, node2));
                gg.addArc(node1, node2);
                break;
              }
            }
          } break;

          case 2: { // reversal
            float nb_arcs = gg.sizeArcs();
            if (!nb_arcs) {
              ++length;
              break;
            }
            nb_arcs /= g.size() * g.size();
            unsigned int nb_del_arc = distrib_arc(generator) * nb_arcs;
            if (nb_del_arc >= gg.sizeArcs())
              nb_del_arc = gg.sizeArcs() - 1;
            for (auto iter = gg.arcs().begin(); iter != gg.arcs().end();
                 ++iter, --nb_del_arc) {
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

      gum::HashTable<gum::Arc, unsigned int> deletions(changes.size());
      gum::HashTable<gum::Arc, unsigned int> additions(changes.size());
      for (const auto &modif : changes) {
        gum::Arc arc(modif.tail(), modif.head());

        switch (modif.type()) {
          case gum::DAGCycleDetector::ChangeType::ARC_DELETION:
            if (deletions.exists(arc))
              ++deletions[arc];
            else
              deletions.insert(arc, 1);
            break;

          case gum::DAGCycleDetector::ChangeType::ARC_ADDITION:
            if (additions.exists(arc))
              ++additions[arc];
            else
              additions.insert(arc, 1);
            break;

          case gum::DAGCycleDetector::ChangeType::ARC_REVERSAL:
            if (deletions.exists(arc))
              ++deletions[arc];
            else
              deletions.insert(arc, 1);
            arc = gum::Arc(modif.head(), modif.tail());
            if (additions.exists(arc))
              ++additions[arc];
            else
              additions.insert(arc, 1);
            break;

          default:
            GUM_ERROR(gum::OperationNotAllowed, "undefined change type");
        }
      }

      for (auto iter = additions.beginSafe(); // safe iterator needed here
           iter != additions.endSafe(); ++iter) {
        if (deletions.exists(iter.key())) {
          unsigned int &nb_del = deletions[iter.key()];
          unsigned int &nb_add = iter.val();
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
    void testSmallGraph() {
      gum::DAG g;
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

      std::vector<gum::DAGCycleDetector::Change> changes;
      TS_ASSERT(detector.hasCycleFromModifications(changes) == false);

      changes.push_back(gum::DAGCycleDetector::ArcAdd(id5, id1));
      TS_ASSERT(detector.hasCycleFromModifications(changes) == true);

      changes.clear();
      changes.push_back(gum::DAGCycleDetector::ArcAdd(id2, id1));
      changes.push_back(gum::DAGCycleDetector::ArcAdd(id5, id1));
      changes.push_back(gum::DAGCycleDetector::ArcReverse(id4, id5));
      changes.push_back(gum::DAGCycleDetector::ArcDel(id1, id3));
      TS_ASSERT(detector.hasCycleFromModifications(changes) == false);

      changes.clear();
      changes.push_back(gum::DAGCycleDetector::ArcAdd(id2, id1));
      changes.push_back(gum::DAGCycleDetector::ArcAdd(id5, id1));
      changes.push_back(gum::DAGCycleDetector::ArcDel(id1, id3));
      changes.push_back(gum::DAGCycleDetector::ArcReverse(id4, id5));
      TS_ASSERT(detector.hasCycleFromModifications(changes) == false);
    }

    void testG1() {
      gum::DAG g;
      for (unsigned int i = 0; i < 10; ++i) {
        g.addNode(i);
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

      std::vector<gum::DAGCycleDetector::Change> changes;
      changes.push_back(gum::DAGCycleDetector::ArcReverse(2, 9));
      changes.push_back(gum::DAGCycleDetector::ArcReverse(6, 9));
      changes.push_back(gum::DAGCycleDetector::ArcReverse(3, 0));
      changes.push_back(gum::DAGCycleDetector::ArcAdd(4, 3));

      TS_ASSERT(detector.hasCycleFromModifications(changes) == true);
    }

    void testRandom() {
      gum::DAGCycleDetector detector;
      std::vector<gum::DAGCycleDetector::Change> changes;
      std::vector<gum::DAGCycleDetector::Change> del_add_changes;
      std::uniform_int_distribution<int> distrib_length(1, 10);

      for (unsigned int i = 15; i < 25; ++i) {
        gum::DAG g = __createDAG(20, i);
        detector.setDAG(g);

        changes.clear();
        TS_ASSERT(detector.hasCycleFromModifications(changes) == false);

        for (unsigned int j = 0; j < 20; ++j) {
          unsigned int length = distrib_length(generator);
          __createChanges(g, changes, del_add_changes, length);

          bool hasCycle = false;
          {
            gum::DiGraph gg = g;
            for (auto &chgt : del_add_changes) {
              switch (chgt.type()) {
                case gum::DAGCycleDetector::ChangeType::ARC_DELETION:
                  gg.eraseArc(gum::Arc(chgt.tail(), chgt.head()));
                  break;

                case gum::DAGCycleDetector::ChangeType::ARC_ADDITION:
                  gg.addArc(chgt.tail(), chgt.head());
                  break;

                default:
                  GUM_ERROR(gum::NotFound, "del_add_changes");
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
            } catch (gum::InvalidDirectedCycle &) {
              hasCycle = true;
            }
          }

          TS_ASSERT(detector.hasCycleFromModifications(changes) == hasCycle);
        }
      }
    }

    void testModifications() {
      gum::DAGCycleDetector detector1, detector2;
      std::vector<gum::DAGCycleDetector::Change> changes;
      std::vector<gum::DAGCycleDetector::Change> del_add_changes;

      for (unsigned int i = 7; i < 15; ++i) {
        gum::DAG g = __createDAG(10, i);
        detector1.setDAG(g);
        detector2.setDAG(g);

        TS_ASSERT(detector1 == detector2);

        for (unsigned int j = 0; j < 30; ++j) {
          __createChanges(g, changes, del_add_changes, 1);
          TS_ASSERT(changes.size() == 1);
          if (detector1.hasCycleFromModifications(changes)) {
            --j;
            continue;
          }

          for (auto &chgt : changes) {
            switch (chgt.type()) {
              case gum::DAGCycleDetector::ChangeType::ARC_DELETION:
                g.eraseArc(gum::Arc(chgt.tail(), chgt.head()));
                detector1.eraseArc(chgt.tail(), chgt.head());
                break;

              case gum::DAGCycleDetector::ChangeType::ARC_ADDITION:
                g.addArc(chgt.tail(), chgt.head());
                detector1.addArc(chgt.tail(), chgt.head());
                break;

              case gum::DAGCycleDetector::ChangeType::ARC_REVERSAL:
                g.eraseArc(gum::Arc(chgt.tail(), chgt.head()));
                g.addArc(chgt.head(), chgt.tail());
                detector1.reverseArc(chgt.tail(), chgt.head());
                break;

              default:
                GUM_ERROR(gum::NotFound, "del_add_changes");
            }
            detector2.setDAG(g);
          }

          TS_ASSERT(detector1 == detector2);
        }
      }
    }

    void testModifications2() {
      gum::DAGCycleDetector detector1, detector2;
      std::vector<gum::DAGCycleDetector::Change> changes;
      std::vector<gum::DAGCycleDetector::Change> del_add_changes;

      for (unsigned int i = 7; i < 15; ++i) {
        gum::DAG g = __createDAG(10, i);
        detector1.setDAG(g);
        detector2.setDAG(g);

        TS_ASSERT(detector1 == detector2);

        for (unsigned int j = 0; j < 30; ++j) {
          __createChanges(g, changes, del_add_changes, 1);
          TS_ASSERT(changes.size() == 1);

          for (auto &chgt : changes) {
            switch (chgt.type()) {
              case gum::DAGCycleDetector::ChangeType::ARC_DELETION:
                TS_ASSERT(detector1.hasCycleFromDeletion(chgt.tail(), chgt.head()) ==
                          detector2.hasCycleFromModifications(changes));
                break;

              case gum::DAGCycleDetector::ChangeType::ARC_ADDITION:
                TS_ASSERT(detector1.hasCycleFromAddition(chgt.tail(), chgt.head()) ==
                          detector2.hasCycleFromModifications(changes));
                break;

              case gum::DAGCycleDetector::ChangeType::ARC_REVERSAL:
                TS_ASSERT(detector1.hasCycleFromReversal(chgt.tail(), chgt.head()) ==
                          detector2.hasCycleFromModifications(changes));
                break;

              default:
                GUM_ERROR(gum::NotFound, "del_add_changes");
            }
          }
        }
      }
    }
  };

} // namespace gum_tests
