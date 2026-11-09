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


#include <agrum/base/core/configuration/configuration.h>
#include <agrum/base/core/dot/dotOptions.h>
#include <agrum/base/graphs/CPDAG.h>
#include <agrum/base/graphs/MPDAG.h>

#include <agrum/base/core/utils_random.h>
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/randomGraphs.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  struct CPDAGTestSuite {};

  GUM_TEST(FromDAG) {
    // a->c<-b, c->d : all the arcs are compelled
    gum::DAG v;
    v.addNodes(4);
    v.addArc(0, 2);
    v.addArc(1, 2);
    v.addArc(2, 3);
    const auto cp = gum::CPDAG::fromDAG(v);
    CHECK_EQ(cp.sizeArcs(), gum::Size(3));
    CHECK(cp.contains(v));
    CHECK(cp.contains(cp.consistentExtension()));

    // a->b->c : all reversible, a<-b<-c is in the class, a->b<-c is not
    gum::DAG chain;
    chain.addNodes(3);
    chain.addArc(0, 1);
    chain.addArc(1, 2);
    const auto cc = gum::CPDAG::fromDAG(chain);
    CHECK_EQ(cc.sizeEdges(), gum::Size(2));
    gum::DAG reversed;
    reversed.addNodes(3);
    reversed.addArc(2, 1);
    reversed.addArc(1, 0);
    CHECK(cc.contains(reversed));
    gum::DAG collider;
    collider.addNodes(3);
    collider.addArc(0, 1);
    collider.addArc(2, 1);
    CHECK(!cc.contains(collider));
    // same numbers of nodes and arcs, other nodes
    gum::DAG otherNodes;
    otherNodes.addNodeWithId(0);
    otherNodes.addNodeWithId(1);
    otherNodes.addNodeWithId(5);
    otherNodes.addArc(0, 1);
    otherNodes.addArc(1, 5);
    CHECK(!cc.contains(otherNodes));
  }

  GUM_TEST(FromPDAG) {
    gum::DAG chain;
    chain.addNodes(3);
    chain.addArc(0, 1);
    chain.addArc(1, 2);
    const gum::PDAG valid(gum::CPDAG::fromDAG(chain).graph());
    CHECK(gum::CPDAG::fromPDAG(valid) == gum::CPDAG::fromDAG(chain));

    // not extendable
    gum::PDAG c4;
    c4.addNodes(4);
    for (gum::NodeId i = 0; i < 4; ++i)
      c4.addEdge(i, (i + 1) % 4);
    CHECK_THROWS_AS(gum::CPDAG::fromPDAG(c4), const gum::InvalidArgument&);

    // extendable but not completed: a->b-c (the CPDAG of a->b->c has no arc)
    gum::PDAG partial;
    partial.addNodes(3);
    partial.addArc(0, 1);
    partial.addEdge(1, 2);
    CHECK_THROWS_AS(gum::CPDAG::fromPDAG(partial), const gum::InvalidArgument&);
  }

  GUM_TEST(DSeparationIsTheDSeparationOfTheClass) {
    // d-separation of the kept extension == d-separation in every DAG of the class == c-separation
    // (LWF) of the chain graph
    for (int i = 0; i < 100; ++i) {
      const auto dag = randomDAG(4 + gum::randomValue(8), 0.3);
      const auto cp  = gum::CPDAG::fromDAG(dag);
      const auto ext = cp.consistentExtension();
      CHECK(cp.contains(ext));
      for (int k = 0; k < 10; ++k) {
        const gum::NodeId x = gum::randomValue(dag.size());
        const gum::NodeId y = gum::randomValue(dag.size());
        if (x == y) continue;
        const auto z   = randomSet(dag, x, y);
        const bool sep = dag.dSeparation(x, y, z);
        CHECK_EQ(cp.dSeparation(x, y, z), sep);
        CHECK_EQ(ext.dSeparation(x, y, z), sep);
        CHECK_EQ(cp.graph().cSeparation(x, y, z), sep);

        // the same for sets of nodes
        gum::NodeSet xs{x};
        for (const auto node: dag.nodes())
          if (node != x && node != y && !z.contains(node) && gum::randomProba() < 0.3)
            xs.insert(node);
        CHECK_EQ(cp.dSeparation(xs, gum::NodeSet{y}, z), dag.dSeparation(xs, gum::NodeSet{y}, z));
      }
    }
  }

  GUM_TEST(NamesAreKept) {
    gum::DAG dag;
    dag.addNodes(3);
    dag.setName(0, "A");
    dag.setName(1, "B");
    dag.setName(2, "C");
    dag.addArc(0, 1);
    dag.addArc(1, 2);
    const auto g = gum::CPDAG::fromDAG(dag);
    CHECK_EQ(g.nameFromId(1), "B");
    CHECK_EQ(g.idFromName("C"), std::optional< gum::NodeId >(2));
    CHECK(!g.idFromName("Z").has_value());
    CHECK_EQ(g.consistentExtension().nameFromId(0), "A");
  }

  GUM_TEST(ReadAccess) {
    // a->b<-c and an isolated d
    gum::DAG dag;
    dag.addNodes(4);
    dag.setName(0, "a");
    dag.addArc(0, 1);
    dag.addArc(2, 1);
    const auto cp = gum::CPDAG::fromDAG(dag);
    CHECK(!cp.empty());
    CHECK(gum::CPDAG::fromDAG(gum::DAG()).empty());
    CHECK(cp.hasName(0));
    CHECK(!cp.hasName(1));
    const auto cc = cp.connectedComponents();
    CHECK_EQ(cc[0], cc[1]);
    CHECK_EQ(cc[0], cc[2]);
    CHECK_NE(cc[0], cc[3]);
    const auto skeleton = cp.skeleton();
    CHECK_EQ(skeleton.size(), gum::Size(4));
    CHECK_EQ(skeleton.sizeEdges(), gum::Size(2));
    CHECK(skeleton.existsEdge(1, 2));
    CHECK_EQ(skeleton.nameFromId(0), "a");
    // the extension kept by fromPDAG
    const auto fromPDAG = gum::CPDAG::fromPDAG(cp.graph());
    CHECK(fromPDAG == cp);
    CHECK(cp.contains(fromPDAG.consistentExtension()));
  }

  GUM_TEST(CopyAndMove) {
    gum::DAG chain;
    chain.addNodes(3);
    chain.addArc(0, 1);
    chain.addArc(1, 2);
    const auto cp = gum::CPDAG::fromDAG(chain);
    gum::CPDAG copy(cp);
    CHECK(copy == cp);
    gum::CPDAG moved(std::move(copy));
    CHECK(moved == cp);
    gum::CPDAG assigned = gum::CPDAG::fromDAG(gum::DAG());
    assigned            = cp;
    CHECK(assigned == cp);
    gum::CPDAG moveAssigned = gum::CPDAG::fromDAG(gum::DAG());
    moveAssigned            = std::move(assigned);
    CHECK(moveAssigned == cp);
    CHECK(cp.contains(moveAssigned.consistentExtension()));
    CHECK(!cp.toDot().empty());
  }

  GUM_TEST(ToDot) {
    ConfigurationGuard guard;
    // a->c<-b (compelled), c-d is compelled by R1: c->d ; e-f reversible
    gum::DAG dag;
    dag.addNodes(6);
    dag.addArc(0, 2);
    dag.addArc(1, 2);
    dag.addArc(2, 3);
    dag.addArc(4, 5);
    const auto cp = gum::CPDAG::fromDAG(dag);

    const auto dot = cp.toDot();
    CHECK_NE(dot.find("  0 -> 2 [color=\"red\"];\n"), std::string::npos);
    CHECK_NE(dot.find("  2 -> 3 [color=\"red\"];\n"), std::string::npos);
    CHECK_NE(dot.find("  4 -> 5 [dir=\"none\"];\n"), std::string::npos);
    CHECK_NE(dot.find("color=\"#C9D6E3\""), std::string::npos);   // the chain component {4, 5}
    // rank=same in an anonymous subgraph inside the cluster (dot ignores it on the cluster)
    CHECK_NE(dot.find("    subgraph {\n      rank=\"same\";\n"), std::string::npos);

    // the configuration
    gum::config().set("dot", "compelledArcColor", "blue");
    gum::config().set("dot", "chainComponentBgColor", "#EEEEEE");
    const auto configured = cp.toDot();
    CHECK_NE(configured.find("  0 -> 2 [color=\"blue\"];\n"), std::string::npos);
    CHECK_NE(configured.find("color=\"#EEEEEE\""), std::string::npos);

    // the options of the caller win
    gum::dot::DotOptions options;
    options.setArcColor(0, 2, "green");
    const auto given = cp.toDot(options);
    CHECK_NE(given.find("  0 -> 2 [color=\"green\"];\n"), std::string::npos);
    CHECK_NE(given.find("  1 -> 2 [color=\"blue\"];\n"), std::string::npos);

    // the same colors for an MPDAG, but no cluster (an MPDAG is not always a chain graph)
    const auto mp    = gum::MPDAG::meekClosure(gum::PDAG(cp.graph()));
    const auto mpDot = mp.toDot();
    CHECK_NE(mpDot.find("  0 -> 2 [color=\"blue\"];\n"), std::string::npos);
    CHECK_NE(mpDot.find("  4 -> 5 [dir=\"none\"];\n"), std::string::npos);
    CHECK_EQ(mpDot.find("subgraph"), std::string::npos);
  }

}   // namespace gum_tests
