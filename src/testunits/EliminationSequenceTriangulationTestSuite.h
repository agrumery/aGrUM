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

#include <cxxtest/AgrumTestSuite.h>

#include <agrum/graphs/eliminationSequenceTriangulation.h>
#include <agrum/graphs/cliqueGraph.h>
#include <agrum/graphs/undiGraph.h>



// The graph used for the tests:
/* 
       D
      / \
     B   F-
    / \ / /
   A   E /
    \   /
     C-G
  Elimination seq : ABGEDCF

 */
class EliminationSequenceTriangulationTestSuite: public CxxTest::TestSuite {
public:

  gum::UndiGraph createGraph() {
    gum::UndiGraph markovGraph;
    gum::NodeId a = 0;
    gum::NodeId b = 1;
    gum::NodeId c = 2;
    gum::NodeId d = 3;
    gum::NodeId e = 4;
    gum::NodeId f = 5;
    gum::NodeId g = 6;
    markovGraph.insertNode(a);
    markovGraph.insertNode(b);
    markovGraph.insertNode(c);
    markovGraph.insertNode(d);
    markovGraph.insertNode(e);
    markovGraph.insertNode(f);
    markovGraph.insertNode(g);
    markovGraph.insertEdge( a,b);
    markovGraph.insertEdge( b,d);
    markovGraph.insertEdge( b,e);
    markovGraph.insertEdge( d,f);
    markovGraph.insertEdge( e,f);
    markovGraph.insertEdge( a,c);
    markovGraph.insertEdge( c,g);
    markovGraph.insertEdge( g,f);
    return markovGraph;
  }

  std::vector<gum::NodeId> createElimination() {
    std::vector<gum::NodeId> result;
    result.push_back(0);
    result.push_back(1);
    result.push_back(6);
    result.push_back(4);
    result.push_back(3);
    result.push_back(2);
    result.push_back(5);
    return result;
  }

  gum::EliminationSequenceTriangulation createTriangulation() {
    gum::UndiGraph graph = createGraph();
    std::vector< gum::NodeId> elim = createElimination();
    return gum::EliminationSequenceTriangulation(graph, elim);
  }
  
  void testFillIns() {
    gum::EliminationSequenceTriangulation tr = createTriangulation();
    TS_GUM_ASSERT_THROWS_NOTHING(tr.fillIns());
    const gum::EdgeSet& fillIns = tr.fillIns();
    TS_ASSERT(fillIns.size() == 5);
    TS_ASSERT(fillIns.contains(gum::Edge(1, 2))); // BC
    TS_ASSERT(fillIns.contains(gum::Edge(2, 3))); // CD
    TS_ASSERT(fillIns.contains(gum::Edge(3, 4))); // DE
    TS_ASSERT(fillIns.contains(gum::Edge(2, 4))); // CE
    TS_ASSERT(fillIns.contains(gum::Edge(2, 5))); // CF
  };

  void testTriangulated() {
    gum::EliminationSequenceTriangulation tr = createTriangulation();
    TS_GUM_ASSERT_THROWS_NOTHING(tr.triangulatedGraph());
    const gum::UndiGraph& triang = tr.triangulatedGraph();
    TS_ASSERT(triang.size() == 7);
    TS_ASSERT(triang.sizeEdges() == 5 + 8);
    // For more structural tests, see testFillIns
  }

  void testJunctionTree() {
    gum::EliminationSequenceTriangulation tr = createTriangulation();
    TS_GUM_ASSERT_THROWS_NOTHING(tr.junctionTree());
    const gum::CliqueGraph& junction = tr.junctionTree();
    //TS_ASSERT(junction.isJoinTree());

    TS_ASSERT(junction.size() == 4);
    TS_ASSERT(junction.sizeEdges() == 3);
    TS_ASSERT(junction.clique(tr.createdClique(0)).size() == 3);
    TS_ASSERT(junction.clique(tr.createdClique(1)).size() == 4);
    TS_ASSERT(junction.clique(tr.createdClique(2)).size() == 4);
    TS_ASSERT(junction.clique(tr.createdClique(3)).size() == 4);
    TS_ASSERT(junction.clique(tr.createdClique(4)).size() == 4);
    TS_ASSERT(junction.clique(tr.createdClique(5)).size() == 4);
    TS_ASSERT(junction.clique(tr.createdClique(6)).size() == 3);
    TS_ASSERT(junction.separator(tr.createdClique(0),
				 tr.createdClique(1)).size() == 2); // BC
    TS_ASSERT(junction.separator(tr.createdClique(1),
				 tr.createdClique(4)).size() == 3); // CED
    TS_ASSERT(junction.separator(tr.createdClique(4),
				 tr.createdClique(6)).size() == 2); // CF
  }

   

  private:

   

};

