############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest
from collections.abc import Callable

import numpy as np

import pyagrum as gum

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class TestGraph(pyAgrumTestCase):
  def _testAddNodes(self, g):
    self.assertEqual(g.addNodes(6), set(range(6)))
    g.eraseNode(1)
    g.eraseNode(2)
    g.eraseNode(4)
    self.assertEqual(g.nodes(), {0, 3, 5})
    self.assertEqual(g.addNodes(4), {1, 2, 4, 6})
    self.assertEqual(g.nodes(), set(range(7)))

  def testAddNodes(self):
    self._testAddNodes(gum.DiGraph())
    self._testAddNodes(gum.UndiGraph())
    self._testAddNodes(gum.MixedGraph())
    self._testAddNodes(gum.DAG())
    self._testAddNodes(gum.PDAG())
    self._testAddNodes(gum.PAG())

  def testConstructorFromUG(self):
    ug = gum.UndiGraph()

    ug.addNodes(4)

    ug.addEdge(0, 2)
    ug.addEdge(1, 2)
    ug.addEdge(2, 3)

    mixed_graph = gum.MixedGraph()

    mixed_graph.addNodes(4)

    mixed_graph.addEdge(0, 2)
    mixed_graph.addEdge(1, 2)
    mixed_graph.addEdge(2, 3)

    mg = gum.MixedGraph(ug)

    self.assertEqual(mixed_graph, mg)

  def testPath(self):
    mg = gum.MixedGraph()
    mg.addNodes(5)
    self.assertIsNone(mg.mixedUnorientedPath(1, 4))
    self.assertIsNone(mg.mixedOrientedPath(1, 4))
    mg.addArc(0, 1)
    mg.addEdge(1, 2)
    mg.addArc(3, 2)
    self.assertEqual(mg.mixedUnorientedPath(0, 3), [0, 1, 2, 3])
    self.assertIsNone(mg.mixedOrientedPath(0, 3))
    self.assertEqual(mg.mixedOrientedPath(0, 2), [0, 1, 2])

  def testConstructorFromDG(self):
    dg = gum.DiGraph()

    dg.addNodes(4)

    dg.addArc(0, 2)
    dg.addArc(1, 2)
    dg.addArc(2, 3)

    mixed_graph = gum.MixedGraph()

    mixed_graph.addNodes(4)

    mixed_graph.addArc(0, 2)
    mixed_graph.addArc(1, 2)
    mixed_graph.addArc(2, 3)

    mg = gum.MixedGraph(dg)

    self.assertEqual(mixed_graph, mg)

  def testCopyConstructor(self):
    mixed_graph = gum.MixedGraph()

    mixed_graph.addNodes(4)

    mixed_graph.addArc(0, 2)
    mixed_graph.addEdge(1, 2)
    mixed_graph.addArc(2, 3)

    self.assertEqual(mixed_graph.parents(2), {0})
    self.assertEqual(mixed_graph.children(2), {3})
    self.assertEqual(mixed_graph.neighbours(2), {1})
    self.assertEqual(mixed_graph.boundary(2), {0, 1, 3})

    copy = mixed_graph
    self.assertEqual(mixed_graph, copy)

    copy = gum.MixedGraph(mixed_graph)
    self.assertEqual(mixed_graph, copy)

  def testNonRegressionAddEdge(self):
    ug = gum.UndiGraph()
    ug.addNodes(4)
    with self.assertRaises(gum.InvalidNode):
      ug.addEdge(1, 6)
    with self.assertRaises(gum.InvalidNode):
      ug.addEdge(7, 0)
    with self.assertRaises(gum.InvalidNode):
      ug.addEdge(6, 7)

  def testMissingNodeBeforeMonoCycle(self):
    # every graph class reports a missing node before a mono-cycle
    for cls in [gum.DiGraph, gum.DAG, gum.MixedGraph, gum.PDAG, gum.ChainGraph]:
      g = cls()
      g.addNodes(2)
      with self.assertRaises(gum.InvalidNode):
        g.addArc(7, 7)
    for cls in [gum.UndiGraph, gum.MixedGraph, gum.PDAG, gum.ChainGraph]:
      g = cls()
      g.addNodes(2)
      with self.assertRaises(gum.InvalidNode):
        g.addEdge(7, 7)
    for cls in [gum.DAG, gum.PDAG, gum.ChainGraph]:
      g = cls()
      g.addNodes(2)
      with self.assertRaises(gum.InvalidDirectedCycle):
        g.addArc(1, 1)

  def _testConnectedComponents(
    self,
    g: gum.Graph,
    add_arc: Callable[[gum.Graph, int, int], None] | None = None,
    add_edge: Callable[[gum.Graph, int, int], None] | None = None,
  ):
    """
    Helper: tests connectedComponents / connectedComponentsList / connectedComponentsCount
    on any graph supporting both arc-like or edge-like connections.
    add_arc(g, u, v)  -- callable that adds a directed or undirected arc/edge
    add_edge(g, u, v) -- callable that adds an undirected edge (or same as add_arc)
    """
    if add_arc is None:

      def add_arc(g: gum.Graph, u: int, v: int) -> None:
        if hasattr(g, "addArc"):
          g.addArc(u, v)

    if add_edge is None:

      def add_edge(g: gum.Graph, u: int, v: int) -> None:
        if hasattr(g, "addEdge"):
          g.addEdge(u, v)

    # Empty graph: no nodes → no components
    self.assertEqual(g.connectedComponents(), {})
    self.assertEqual(g.connectedComponentsList(), {})
    self.assertEqual(g.connectedComponentsCount(), 0)

    # Single node: one component
    g.addNodeWithId(0)
    cc = g.connectedComponents()
    self.assertEqual(set(cc.keys()), {0})
    self.assertEqual(len(set(cc.values())), 1)
    self.assertEqual(g.connectedComponentsCount(), 1)
    ccl = g.connectedComponentsList()
    self.assertEqual(len(ccl), 1)
    self.assertEqual(set(list(ccl.values())[0]), {0})

    # Three isolated nodes: three components
    g.addNodeWithId(1)
    g.addNodeWithId(2)
    self.assertEqual(g.connectedComponentsCount(), 3)
    cc = g.connectedComponents()
    self.assertEqual(set(cc.keys()), {0, 1, 2})
    # all three roots are distinct
    self.assertEqual(len(set(cc.values())), 3)

    # Connect 0 and 1: two components remain
    add_edge(g, 0, 1)
    self.assertEqual(g.connectedComponentsCount(), 2)
    cc = g.connectedComponents()
    self.assertEqual(cc[0], cc[1])  # 0 and 1 share a root
    self.assertNotEqual(cc[0], cc[2])

    # Connect 1 and 2: one component
    add_edge(g, 1, 2)
    self.assertEqual(g.connectedComponentsCount(), 1)
    cc = g.connectedComponents()
    self.assertEqual(cc[0], cc[1])
    self.assertEqual(cc[1], cc[2])

    # connectedComponentsList: one root, all three nodes in its set
    ccl = g.connectedComponentsList()
    self.assertEqual(len(ccl), 1)
    self.assertEqual(set(list(ccl.values())[0]), {0, 1, 2})

  def testConnectedComponentsUndiGraph(self):
    self._testConnectedComponents(
      gum.UndiGraph(),
      add_edge=lambda g, u, v: g.addEdge(u, v),
    )

  def testConnectedComponentsDiGraph(self):
    # DiGraph uses weak connectivity (follows arcs in both directions)
    self._testConnectedComponents(
      gum.DiGraph(),
      add_edge=lambda g, u, v: g.addArc(u, v),
    )

  def testDAGFromDiGraph(self):
    digraph = gum.fastDiGraph("A->B->C;D")
    dag = gum.DAG(digraph)
    self.assertIsInstance(dag, gum.DAG)
    self.assertEqual(dag.nodes(), digraph.nodes())
    self.assertEqual(dag.arcs(), digraph.arcs())
    self.assertEqual({dag.nameFromId(n) for n in dag.nodes()}, {"A", "B", "C", "D"})
    with self.assertRaises(gum.InvalidDirectedCycle):
      dag.addArc(2, 0)
    with self.assertRaises(gum.InvalidDirectedCycle):
      gum.DAG(gum.fastDiGraph("A->B->C->A"))
    # a mixed graph: its arcs only
    self.assertEqual(gum.DAG(gum.fastPDAG("A->B-C")).arcs(), {(0, 1)})
    # still a copy of a DAG
    self.assertEqual(gum.DAG(dag), dag)

  def testConnectedComponentsDAG(self):
    self._testConnectedComponents(
      gum.DAG(),
      add_edge=lambda g, u, v: g.addArc(u, v),
    )

  def testConnectedComponentsMixedGraph(self):
    # MixedGraph: test with undirected edges
    self._testConnectedComponents(
      gum.MixedGraph(),
      add_edge=lambda g, u, v: g.addEdge(u, v),
    )
    # MixedGraph: test also with arcs
    mg = gum.MixedGraph()
    mg.addNodeWithId(0)
    mg.addNodeWithId(1)
    mg.addNodeWithId(2)
    self.assertEqual(mg.connectedComponentsCount(), 3)
    mg.addArc(0, 1)
    self.assertEqual(mg.connectedComponentsCount(), 2)
    mg.addEdge(1, 2)
    self.assertEqual(mg.connectedComponentsCount(), 1)
    # Mixed: arc 0→1 and edge 1--2: all in one component
    ccl = mg.connectedComponentsList()
    self.assertEqual(len(ccl), 1)
    self.assertEqual(set(list(ccl.values())[0]), {0, 1, 2})

  def testConnectedComponentsPDAG(self):
    pg = gum.PDAG()
    pg.addNodeWithId(0)
    pg.addNodeWithId(1)
    pg.addNodeWithId(2)
    self.assertEqual(pg.connectedComponentsCount(), 3)
    pg.addArc(0, 1)
    self.assertEqual(pg.connectedComponentsCount(), 2)
    pg.addEdge(1, 2)
    self.assertEqual(pg.connectedComponentsCount(), 1)
    cc = pg.connectedComponents()
    self.assertEqual(cc[0], cc[1])
    self.assertEqual(cc[1], cc[2])
    ccl = pg.connectedComponentsList()
    self.assertEqual(len(ccl), 1)
    self.assertEqual(set(list(ccl.values())[0]), {0, 1, 2})

  def testPDAGAllowsPartiallyDirectedCycles(self):
    pg = gum.PDAG()
    pg.addNodes(3)
    pg.addEdge(0, 1)
    pg.addEdge(1, 2)
    pg.addArc(2, 0)  # 2->0-1-2 is a partially directed cycle: allowed in a PDAG
    self.assertEqual(pg.sizeArcs(), 1)
    self.assertEqual(pg.sizeEdges(), 2)

  def testPDAGForbidsDirectedCyclesAndDoubleConnections(self):
    pg = gum.PDAG()
    pg.addNodes(3)
    pg.addArc(0, 1)
    pg.addArc(1, 2)
    with self.assertRaises(gum.InvalidDirectedCycle):
      pg.addArc(2, 0)
    with self.assertRaises(gum.InvalidEdge):
      pg.addEdge(0, 1)
    pg.addEdge(0, 2)
    with self.assertRaises(gum.InvalidArc):
      pg.addArc(2, 0)

    mg = gum.MixedGraph()
    mg.addNodes(2)
    mg.addArc(0, 1)
    mg.addArc(1, 0)
    with self.assertRaises(gum.InvalidDirectedCycle):
      gum.PDAG(mg)

  def testChainGraph(self):
    cg = gum.ChainGraph()
    cg.addNodes(3)
    cg.addEdge(0, 1)
    cg.addEdge(1, 2)
    with self.assertRaises(gum.InvalidPartiallyDirectedCycle):
      cg.addArc(2, 0)
    self.assertIsInstance(cg, gum.PDAG)
    self.assertIsInstance(cg, gum.MixedGraph)

    # constructors
    pg = gum.PDAG()
    pg.addNodes(3)
    pg.addEdge(0, 1)
    pg.addEdge(1, 2)
    self.assertEqual(gum.ChainGraph(pg).sizeEdges(), 2)
    pg.addArc(2, 0)
    with self.assertRaises(gum.InvalidPartiallyDirectedCycle):
      gum.ChainGraph(pg)
    self.assertEqual(gum.ChainGraph(gum.fastDAG("A->B->C")).sizeArcs(), 2)
    self.assertEqual(gum.ChainGraph(gum.fastUndiGraph("A-B-C")).sizeEdges(), 2)
    self.assertEqual(gum.ChainGraph(gum.fastMixedGraph("A->B-C")).sizeArcs(), 1)

    # fast syntax and pickling
    fcg = gum.fastChainGraph("A->B-C;D->C")
    self.assertIsInstance(fcg, gum.ChainGraph)
    self.assertEqual((fcg.sizeArcs(), fcg.sizeEdges()), (2, 1))
    with self.assertRaises(gum.InvalidPartiallyDirectedCycle):
      gum.fastChainGraph("A->B-C-A")
    import pickle

    copy = pickle.loads(pickle.dumps(fcg))
    self.assertIsInstance(copy, gum.ChainGraph)
    self.assertEqual(copy.arcs(), fcg.arcs())
    self.assertEqual(copy.edges(), fcg.edges())

  def testChainGraphSeparation(self):
    # P1->X--Y<-P2 (0=P1, 1=X, 2=Y, 3=P2): the parents of the chain component {X,Y} are married
    cg = gum.ChainGraph()
    cg.addNodes(4)
    cg.addArc(0, 1)
    cg.addEdge(1, 2)
    cg.addArc(3, 2)
    moral = cg.moralGraph()
    self.assertTrue(moral.existsEdge(0, 3))
    self.assertFalse(moral.existsEdge(1, 3))
    self.assertEqual(moral.sizeEdges(), 4)
    self.assertEqual(cg.moralizedAncestralGraph({1}).size(), 4)
    self.assertTrue(cg.cSeparation(0, 3))
    self.assertFalse(cg.cSeparation(0, 3, {1}))
    self.assertFalse(cg.cSeparation({0}, {3}, {1, 2}))

  def testTopologicalOrderOfACyclicGraph(self):
    g = gum.DiGraph()
    g.addNodes(4)
    g.addArc(0, 1)
    g.addArc(2, 3)
    g.addArc(3, 2)  # a cycle separated from the node without parent
    with self.assertRaises(gum.InvalidDirectedCycle):
      g.topologicalOrder()

  def testCPDAG(self):
    dag = gum.fastDAG("A->B->C;D->C")
    cpdag = gum.CPDAG.fromDAG(dag)
    self.assertEqual(cpdag.arcs(), {(1, 2), (3, 2)})
    self.assertEqual(cpdag.edges(), {(0, 1)})
    self.assertTrue(cpdag.contains(dag))
    self.assertTrue(cpdag.contains(cpdag.consistentExtension()))
    self.assertEqual(cpdag.nameFromId(3), "D")
    self.assertEqual(cpdag.idFromName("C"), 2)
    self.assertIsNone(cpdag.idFromName("Z"))
    self.assertEqual(cpdag.graph().nameFromId(0), "A")
    self.assertEqual(cpdag.consistentExtension().nameFromId(1), "B")
    # the compelled arcs are colored, unless the caller gives a color
    self.assertIn(f'1 -> 2 [color="{gum.config["dot", "compelledArcColor"]}"]', cpdag.toDot())
    options = gum.DotOptions()
    options.setArcColor(1, 2, "green")
    self.assertIn('1 -> 2 [color="green"]', cpdag.toDot(options))
    gum.config.push()
    try:
      gum.config["dot", "compelledArcColor"] = "blue"
      self.assertIn('3 -> 2 [color="blue"]', cpdag.toDot())
    finally:
      gum.config.pop()
    self.assertFalse(cpdag.contains(gum.fastDAG("A->B<-C;D->C")))
    self.assertIsInstance(cpdag.graph(), gum.ChainGraph)
    self.assertEqual(set(cpdag), {0, 1, 2, 3})

    # d-separation of the class
    self.assertTrue(cpdag.dSeparation(0, 3))
    self.assertTrue(cpdag.dSeparation(0, 3, None))
    self.assertFalse(cpdag.dSeparation(0, 3, 2))
    self.assertTrue(cpdag.dSeparation({0}, {2}, {1}))

    # checked conversion
    self.assertEqual(gum.CPDAG.fromPDAG(gum.PDAG(cpdag.graph())), cpdag)
    with self.assertRaises(gum.InvalidArgument):
      gum.CPDAG.fromPDAG(gum.fastPDAG("A->B-C"))

  def testMPDAG(self):
    cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C"))
    self.assertEqual(cpdag.sizeEdges(), 2)
    mpdag = gum.MPDAG.fromCPDAG(cpdag, [(0, 1)])
    self.assertEqual(mpdag.arcs(), {(0, 1), (1, 2)})
    self.assertEqual(gum.MPDAG.fromCPDAG(cpdag, [("A", "B")]), mpdag)  # nodes given by their names
    self.assertEqual(gum.MPDAG.fromCPDAG(cpdag, [("A", 1)]), mpdag)
    with self.assertRaises(gum.NotFound):
      gum.MPDAG.fromCPDAG(cpdag, [("A", "Z")])
    self.assertEqual(mpdag.cpdag(), cpdag)
    self.assertEqual(mpdag.nameFromId(2), "C")
    self.assertEqual(mpdag.idFromName("B"), 1)
    self.assertEqual(mpdag.cpdag().nameFromId(0), "A")
    self.assertTrue(mpdag.dSeparation(0, 2, 1))
    self.assertFalse(mpdag.dSeparation(0, 2))
    self.assertIsInstance(mpdag.graph(), gum.PDAG)
    with self.assertRaises(gum.InvalidArgument):
      gum.MPDAG.fromCPDAG(cpdag, [(0, 1), (2, 1)])  # a new v-structure
    with self.assertRaises(gum.InvalidArc):
      gum.MPDAG.fromCPDAG(cpdag, [(0, 2)])
    with self.assertRaises(gum.InvalidArc):
      gum.MPDAG.fromCPDAG(cpdag, [(-1, 0)])  # not a node

    pdag = gum.fastPDAG("A->B-C")
    self.assertEqual(gum.MPDAG.meekClosure(pdag).arcs(), {(0, 1), (1, 2)})
    with self.assertRaises(gum.InvalidArgument):
      gum.MPDAG.fromPDAG(pdag)  # not closed under the Meek rules
    with self.assertRaises(gum.InvalidArgument):
      gum.MPDAG.meekClosure(gum.fastPDAG("A-B-C-D-A"))  # not extendable

  def testReadAccessOfTheImmutableGraphs(self):
    cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B<-C;D"))
    mpdag = gum.MPDAG.meekClosure(gum.fastPDAG("A->B-C;D"))
    mag = gum.MAG.fromMarkedGraph(gum.fastMarkedGraph("A-B->C;D<->E"))
    for g in (cpdag, mpdag, mag):
      a, d = g.idFromName("A"), g.idFromName("D")
      self.assertFalse(g.empty())
      self.assertTrue(g.hasName(a))
      self.assertEqual(g.connectedComponentsCount(), 2)
      self.assertEqual(len(g.connectedComponentsList()), 2)
      self.assertNotEqual(g.connectedComponents()[a], g.connectedComponents()[d])
      skeleton = g.skeleton()
      self.assertIsInstance(skeleton, gum.UndiGraph)
      self.assertEqual(skeleton.sizeEdges(), g.sizeEdges() + (g.sizeArcs() if hasattr(g, "sizeArcs") else 0))
      self.assertEqual(skeleton.nameFromId(a), "A")
      matrix = g.adjacencyMatrix()
      self.assertEqual(int(matrix.sum()), 2 * skeleton.sizeEdges() if g is mag else g.sizeArcs() + 2 * g.sizeEdges())
    self.assertTrue(gum.CPDAG.fromDAG(gum.DAG()).empty())
    # MAG: neighbours are the undirected edges only, adjacents any edge
    b, c = mag.idFromName("B"), mag.idFromName("C")
    self.assertEqual(mag.neighbours(b), {mag.idFromName("A")})
    self.assertEqual(mag.adjacents(b), {mag.idFromName("A"), c})
    self.assertEqual(mag.neighbours(c), set())
    self.assertFalse(hasattr(mag, "undirectedNeighbours"))

  def testPDAGDeprecatedLWFMethods(self):
    if not hasattr(gum.PDAG, "cSeparation"):
      self.skipTest("the deprecated LWF methods of PDAG have been removed")
    pg = gum.PDAG()
    pg.addNodes(4)
    pg.addArc(0, 1)
    pg.addEdge(1, 2)
    pg.addArc(3, 2)
    cg = gum.ChainGraph(pg)
    with self.assertWarns(DeprecationWarning):
      self.assertEqual(pg.moralGraph().edges(), cg.moralGraph().edges())
    with self.assertWarns(DeprecationWarning):
      self.assertEqual(pg.moralizedAncestralGraph({1}).edges(), cg.moralizedAncestralGraph({1}).edges())
    with self.assertWarns(DeprecationWarning):
      self.assertEqual(pg.cSeparation(0, 3, {1}), cg.cSeparation(0, 3, {1}))
    with self.assertWarns(DeprecationWarning):
      self.assertEqual(pg.cSeparation(0, 3), cg.cSeparation(0, 3))
    self.assertTrue(cg.hasMixedReallyOrientedPath(0, 2))
    self.assertFalse(cg.hasMixedReallyOrientedPath(2, 1))
    with self.assertWarns(DeprecationWarning):
      self.assertTrue(pg.hasMixedReallyOrientedPath(0, 2))

    # a PDAG which is not a chain graph (partially directed cycle 0->1-2-0): computed as in C++
    nc = gum.PDAG()
    nc.addNodes(3)
    nc.addArc(0, 1)
    nc.addEdge(1, 2)
    nc.addEdge(2, 0)
    with self.assertWarns(DeprecationWarning):
      self.assertEqual(nc.moralGraph().size(), 3)
    with self.assertWarns(DeprecationWarning):
      self.assertEqual(nc.moralizedAncestralGraph({1}).size(), 3)
    with self.assertWarns(DeprecationWarning):
      self.assertFalse(nc.cSeparation(0, 1))

  def testAdjancecyMatrix(self):
    mg = gum.MixedGraph()
    mg.addNodes(4)
    mg.addArc(0, 1)
    mg.addArc(1, 2)
    mg.addEdge(2, 3)
    self.assertTrue(
      np.array_equal(
        mg.adjacencyMatrix(),
        np.array([[0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1], [0, 0, 1, 0]]),
      )
    )

    mg = gum.DAG()
    mg.addNodes(4)
    mg.addArc(0, 1)
    mg.addArc(1, 2)
    mg.addArc(2, 3)
    self.assertTrue(
      np.array_equal(
        mg.adjacencyMatrix(),
        np.array([[0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1], [0, 0, 0, 0]]),
      )
    )

    mg = gum.UndiGraph()
    mg.addNodes(4)
    mg.addEdge(0, 1)
    mg.addEdge(1, 2)
    mg.addEdge(2, 3)
    self.assertTrue(
      np.array_equal(
        mg.adjacencyMatrix(),
        np.array([[0, 1, 0, 0], [1, 0, 1, 0], [0, 1, 0, 1], [0, 0, 1, 0]]),
      )
    )

  def testNodeNames(self):
    for g in (gum.DiGraph(), gum.UndiGraph(), gum.DAG(), gum.MixedGraph()):
      g.addNodes(3)

      # defaults
      self.assertFalse(g.hasName(0))
      self.assertEqual(g.nameFromId(0), "0")
      self.assertIsNone(g.idFromName("foo"))

      # setName / nameFromId / hasName / idFromName
      g.setName(0, "alpha")
      self.assertTrue(g.hasName(0))
      self.assertEqual(g.nameFromId(0), "alpha")
      self.assertEqual(g.idFromName("alpha"), 0)

      g.setName(2, "gamma")
      self.assertEqual(g.idFromName("gamma"), 2)

      # duplicate name on different node → error
      with self.assertRaises(gum.DuplicateElement):
        g.setName(1, "alpha")

      # same name same node → no error
      g.setName(0, "alpha")
      self.assertEqual(g.nameFromId(0), "alpha")

      # rename
      g.setName(0, "alpha2")
      self.assertEqual(g.nameFromId(0), "alpha2")
      self.assertIsNone(g.idFromName("alpha"))
      self.assertEqual(g.idFromName("alpha2"), 0)

      # eraseNode removes name
      g.eraseNode(2)
      self.assertFalse(g.hasName(2))
      self.assertIsNone(g.idFromName("gamma"))

      # clear resets names
      g.clear()
      self.assertFalse(g.hasName(0))

  def testToDotWithNames(self):
    g = gum.DiGraph()
    g.addNodes(3)
    g.setName(0, "alpha")
    g.setName(2, 'ga"mm\\a')
    g.addArc(0, 1)
    g.addArc(1, 2)

    dot = g.toDot()
    self.assertIn('0 [label="(0) alpha"]', dot)
    self.assertIn('2 [label="(2) ga\\"mm\\\\a"]', dot)
    self.assertNotIn("1 [label=", dot)
    self.assertIn("0 -> 1", dot)
    self.assertIn("1 -> 2", dot)

    u = gum.UndiGraph()
    u.addNodes(3)
    u.setName(1, "beta")
    u.addEdge(0, 1)
    dot = u.toDot()
    self.assertIn('1 [label="(1) beta"]', dot)
    self.assertNotIn("0 [label=", dot)

  def testToDotLookFollowsTheConfiguration(self):
    g = gum.DiGraph()
    g.addNodes(2)
    g.addArc(0, 1)

    gum.config.push()
    try:
      gum.config.reset()
      dot = g.toDot()
      self.assertTrue(dot.startswith("digraph {\n"))
      self.assertIn('bgcolor="transparent"', dot)
      self.assertIn('fillcolor="#404040", fontcolor="white", color="#4A4A4A"', dot)
      self.assertIn('edge [color="#4A4A4A"]', dot)

      # the dark theme only changes the color of the arcs
      gum.config["dot", "arcColor"] = "#AAAAAA"
      self.assertIn('edge [color="#AAAAAA"]', g.toDot())

      gum.config["dot", "graphSize"] = "12!"
      gum.config["dot", "nodeBgColor"] = "burlywood"
      dot = g.toDot()
      self.assertIn('size="12!"', dot)
      self.assertIn('fillcolor="burlywood", fontcolor="black"', dot)

      # an undirected graph is drawn as a graph
      u = gum.UndiGraph()
      u.addNodes(2)
      u.addEdge(0, 1)
      self.assertTrue(u.toDot().startswith("graph {\n"))
      self.assertIn("0 -- 1;", u.toDot())
    finally:
      gum.config.pop()

  def testMarkedGraph(self):
    g = gum.fastMarkedGraph("A o-> B;B<->C;C-D;D->E")
    a, b, c, d, e = (g.idFromName(n) for n in "ABCDE")
    self.assertIsInstance(g.markAt(a, b), gum.EdgeMark)
    self.assertEqual(g.markAt(b, a), gum.EdgeMark.Circle)
    self.assertEqual(g.markAt(a, b), gum.EdgeMark.Arrowhead)
    self.assertEqual(g.markAt(a, b), gum.EdgeMark_Arrowhead)  # the former constants still work
    self.assertEqual(g.spouses(b), {c})
    self.assertEqual(g.undirectedNeighbours(c), {d})
    self.assertEqual(g.parents(e), {d})
    self.assertEqual(g.children(d), {e})
    self.assertEqual(g.parents(b), set())  # o-> is not a definite arc
    self.assertEqual(g.neighbours(b), {a, c})

    # addEdge on an existing edge updates its marks
    g.addEdge(a, b, gum.EdgeMark.Tail, gum.EdgeMark.Arrowhead)
    self.assertTrue(g.isDefinitelyDirected(a, b))
    self.assertEqual(g.sizeEdges(), 4)
    with self.assertRaises(gum.NotFound):
      g.markAt(a, e)

    self.assertEqual(g.skeleton().sizeEdges(), 4)
    self.assertEqual(g.connectedComponentsCount(), 1)
    self.assertIn("A", g.toDot())
    self.assertIn("-->", str(g))

  def testMarkedGraphRejectsInvalidMarks(self):
    g = gum.fastMarkedGraph("A o-> B")
    a, b = g.idFromName("A"), g.idFromName("B")
    for bad in (3, 7, -1):
      with self.assertRaises(ValueError):
        g.setMarkAt(a, b, bad)
      with self.assertRaises(ValueError):
        g.addEdge(a, b, gum.EdgeMark.Tail, bad)
      with self.assertRaises(ValueError):
        g.reorientAllWith(bad)
    self.assertEqual(g.markAt(a, b), gum.EdgeMark.Arrowhead)
    self.assertEqual(g.markAt(b, a), gum.EdgeMark.Circle)
    g.setMarkAt(b, a, 1)  # a plain int of a valid mark is accepted
    self.assertEqual(g.markAt(b, a), gum.EdgeMark.Tail)

  def testReprOfChainGraphAndPAG(self):
    self.assertIn("pyagrum.ChainGraph@", repr(gum.ChainGraph(gum.fastPDAG("A->B;B-C"))))
    self.assertIn("pyagrum.PAG@", repr(gum.fastPAG("A o-> B")))
    self.assertIn("pyagrum.MarkedGraph@", repr(gum.fastMarkedGraph("A o-> B")))

  def testAdjacencyMatrixWithSparseIds(self):
    # a node is indexed by its position among the sorted ids (an erased node left a hole)
    g = gum.DiGraph()
    g.addNodes(4)
    g.eraseNode(0)
    g.addArc(1, 3)
    m = g.adjacencyMatrix()
    self.assertEqual(m.shape, (3, 3))
    self.assertEqual(m.tolist(), [[0, 0, 1], [0, 0, 0], [0, 0, 0]])
    u = gum.UndiGraph()
    u.addNodes(3)
    u.eraseNode(0)
    u.addEdge(1, 2)
    self.assertEqual(u.adjacencyMatrix().tolist(), [[0, 1], [1, 0]])

  def testIteratorRaisingInAnArgument(self):
    # the exception of the iteration is raised (it gave a partial set, then a SystemError)
    def nodes():
      yield 0
      raise KeyError("in the iteration")

    dag = gum.fastDAG("A->B->C")
    with self.assertRaises(KeyError):
      dag.dSeparation(nodes(), 2)

  def testFastPAG(self):
    p = gum.fastPAG("0 o-o 1;1 o-> 2;2<->3")
    self.assertIsInstance(p, gum.PAG)
    self.assertIsInstance(p, gum.MarkedGraph)
    self.assertTrue(p.isCircle(0, 1) and p.isCircle(1, 0))
    self.assertTrue(p.isArrowhead(1, 2) and p.isCircle(2, 1))
    self.assertTrue(p.isBidirected(2, 3))
    self.assertEqual(p.addNodes(2), {4, 5})
    # an edge repeated with the same marks is accepted, with different marks it is rejected
    self.assertEqual(gum.fastPAG("0 o-> 1;1 <-o 0").sizeEdges(), 1)
    with self.assertRaises(gum.InvalidEdge):
      gum.fastPAG("0 o-> 1;0<->1")

  def testFastGraphsRefuseMalformedNamesAndLargeIds(self):
    for desc in ("A>-B", "A B->C", "A--B", "A->"):
      with self.assertRaises(gum.InvalidArgument):
        gum.fastMixedGraph(desc)
    with self.assertRaises(gum.InvalidArgument):
      gum.fastMarkedGraph("A o -B")
    # an arc in an undirected graph is not a syntax error
    with self.assertRaises(gum.InvalidArc):
      gum.fastUndiGraph("A->B")
    with self.assertRaises(gum.InvalidArgument):
      gum.fastDiGraph("0->2000000000")
    with self.assertRaises(gum.InvalidArgument):
      gum.fastPAG("0->2000000000")

  def testQuotedNames(self):
    g = gum.fastGraph("'Visit Asia'->X;'l''eau'->X")
    self.assertIsInstance(g, gum.DAG)
    self.assertEqual({g.nameFromId(n) for n in g.nodes()}, {"Visit Asia", "X", "l'eau"})
    # a '-' inside a quoted name is not an edge
    self.assertIsInstance(gum.fastGraph("'a-b'->'c-d'"), gum.DAG)
    self.assertIsInstance(gum.fastGraph("'a-b'-'c-d'"), gum.UndiGraph)
    p = gum.fastPAG("'A'o->'B'")
    self.assertEqual(p.markAt(p.idFromName("A"), p.idFromName("B")), gum.EdgeMark.Arrowhead)
    self.assertEqual(p.markAt(p.idFromName("B"), p.idFromName("A")), gum.EdgeMark.Circle)

  def testFastGraphDispatcher(self):
    self.assertIsInstance(gum.fastGraph(""), gum.UndiGraph)
    self.assertIsInstance(gum.fastGraph("A-B"), gum.UndiGraph)
    self.assertIsInstance(gum.fastGraph("A->B"), gum.DAG)
    self.assertIsInstance(gum.fastGraph("A->B->A"), gum.DiGraph)
    self.assertIsInstance(gum.fastGraph("A->B;B-C"), gum.PDAG)
    self.assertIsInstance(gum.fastGraph("A->B->C->A;C-D"), gum.MixedGraph)
    self.assertIsInstance(gum.fastGraph("A o-> B"), gum.MarkedGraph)
    self.assertIsInstance(gum.fastGraph("A<->B"), gum.MarkedGraph)
    for desc in ("A--B", "A->", "A B->C"):
      with self.assertRaises(gum.InvalidArgument):
        gum.fastGraph(desc)

  def testCheckFastSyntax(self):
    check = gum.checkFastSyntax("A->B->C")
    self.assertTrue(check)
    self.assertEqual(
      check.validSyntaxes(),
      [
        gum.FastSyntax.DiGraph,
        gum.FastSyntax.MixedGraph,
        gum.FastSyntax.MarkedGraph,
        gum.FastSyntax.BayesNet,
        gum.FastSyntax.InfluenceDiagram,
      ],
    )
    self.assertIsInstance(check.validSyntaxes()[0], gum.FastSyntax)
    self.assertTrue(check.isValid(gum.FastSyntax.BayesNet))
    self.assertFalse(check.isValid(gum.FastSyntax.UndiGraph))
    self.assertEqual(check.error(gum.FastSyntax.DiGraph), "")
    self.assertNotEqual(check.error(gum.FastSyntax.UndiGraph), "")
    self.assertEqual(check.description(), "A->B->C")
    self.assertIn("BayesNet", repr(check))
    self.assertEqual(gum.checkFastSyntax("A--B--C").validSyntaxes(), [gum.FastSyntax.MarkovRandomField])
    self.assertFalse(gum.checkFastSyntax("'A->B"))
    with self.assertRaises(gum.InvalidArgument):
      gum.checkFastSyntax("A->B", "2")
    self.assertEqual(gum.quoteFastName("X-ray"), "'X-ray'")
    self.assertTrue(gum.fastNameNeedsQuotes("X-ray"))
    self.assertFalse(gum.fastNameNeedsQuotes("Visit Asia"))
    with self.assertRaises(gum.OutOfBounds):
      check.isValid(42)
    # pyagrum.FastSyntax follows the C++ enumeration
    cpp = {name.removeprefix("FastSyntax_") for name in dir(gum.pyagrumcpp) if name.startswith("FastSyntax_")}
    self.assertEqual(cpp, {syntax.name for syntax in gum.FastSyntax})

  def testCheckFastSyntaxAgreesWithTheModels(self):
    # valid for a model = accepted by its fast function, with the same exception
    self.assertFalse(gum.checkFastSyntax("A->B;A->B").isValid(gum.FastSyntax.BayesNet))
    with self.assertRaises(gum.DuplicateElement):
      gum.fastBN("A->B;A->B")
    with self.assertRaises(gum.DuplicateElement):
      gum.fastID("A->B;A->B")
    for desc in ("A--B;B--A", "A;A", "A;--", "A---B"):
      with self.subTest(desc=desc):
        self.assertFalse(gum.checkFastSyntax(desc).isValid(gum.FastSyntax.MarkovRandomField))
        with self.assertRaises(gum.InvalidArgument):
          gum.fastMRF(desc)
    mrf = gum.fastMRF("'-B'--A")
    self.assertEqual({mrf.variable(n).name() for n in mrf.nodes()}, {"-B", "A"})
    with self.assertRaises(gum.InvalidArgument):
      gum.fastVariable("+[0,1,2]")

  def testMAG(self):
    dag = gum.fastDAG("A->X<-T->Y<-B")
    t = dag.idFromName("T")
    mag = gum.MAG.fromDAG(dag, {t})  # T latent
    x, y = mag.idFromName("X"), mag.idFromName("Y")
    self.assertEqual(mag.size(), 4)
    self.assertTrue(mag.graph().isBidirected(x, y))
    self.assertIsInstance(mag.graph(), gum.MarkedGraph)
    self.assertEqual(mag.markAt(x, y), gum.EdgeMark.Arrowhead)
    self.assertEqual(mag.spouses(x), {y})
    self.assertEqual(mag.parents(x), {mag.idFromName("A")})
    # the m-separation of the MAG is the d-separation of the DAG
    self.assertFalse(mag.mSeparation(x, y))
    self.assertTrue(mag.mSeparation(mag.idFromName("A"), mag.idFromName("B")))
    self.assertTrue(
      mag.mSeparation(mag.idFromName("A"), y, x)
      == dag.dSeparation(dag.idFromName("A"), dag.idFromName("Y"), {dag.idFromName("X")})
    )

    selected = gum.MAG.fromDAG(dag, set(), {t})  # T selected
    self.assertEqual(selected.sizeEdges(), 2)
    with self.assertRaises(gum.InvalidArgument):
      gum.MAG.fromDAG(dag, {t}, {t})

    rep = gum.MAG.fromPAG(gum.fastPAG("A o-> B;C o-> B;B -o D"))
    self.assertTrue(rep.graph().isDefinitelyDirected(rep.idFromName("B"), rep.idFromName("D")))
    with self.assertRaises(gum.InvalidArgument):
      gum.MAG.fromMarkedGraph(gum.fastMarkedGraph("A->B->C;A<->C"))
    self.assertIn("<->", str(mag))
    self.assertEqual(sorted(mag), sorted(mag.nodes()))

  def testGumFormatAndPickling(self):
    import pickle

    dag = gum.fastDAG("A->B;C")
    magDAG = gum.fastDAG("A->X<-T->Y;Z")
    graphs = [
      dag,
      gum.fastDiGraph("A->B->A;C"),
      gum.fastUndiGraph("A-B;C"),
      gum.fastMixedGraph("A->B-C;D"),
      gum.fastPDAG("A->B-C;D"),
      gum.fastChainGraph("A->B;B-C;D"),
      gum.fastMarkedGraph("A o-> B;B<->C;D"),
      gum.fastPAG("A o-> B;B<->C;D"),
      gum.CPDAG.fromDAG(dag),
      gum.MPDAG.meekClosure(gum.PDAG(dag)),
      gum.MAG.fromDAG(magDAG, {magDAG.idFromName("T")}),
    ]
    for g in graphs:
      for copy in (pickle.loads(pickle.dumps(g)), gum.graphFromGUMString(g.saveGUMstring())):
        self.assertIs(type(copy), type(g))
        self.assertEqual(copy, g)
        self.assertEqual(sorted(copy.nodes()), sorted(g.nodes()))  # isolated nodes included
        for n in g.nodes():
          self.assertEqual(copy.nameFromId(n), g.nameFromId(n))

    eg = gum.EssentialGraph(gum.fastBN("a->b;c"))
    eg2 = pickle.loads(pickle.dumps(eg))
    self.assertEqual(eg2.arcs(), eg.arcs())
    self.assertEqual(eg2.nameFromId(2), "c")

    import os
    import tempfile

    with tempfile.TemporaryDirectory() as tmp:
      for ext in (".jgum", ".bgum"):
        filename = os.path.join(tmp, "graph" + ext)
        gum.saveGraph(graphs[7], filename)
        read = gum.loadGraph(filename)
        self.assertIsInstance(read, gum.PAG)
        self.assertEqual(read, graphs[7])
      with self.assertRaises(gum.InvalidArgument):  # nothing is written
        gum.saveGraph(dag, os.path.join(tmp, "graph.txt"))
    with self.assertRaises(gum.GumIOError):
      gum.graphFromGUMString("not a graph")
    with self.assertRaises(gum.GumIOError):  # a DAG with a cycle is a malformed jgum
      gum.graphFromGUMString('{"type": "DAG", "nodes": [0, 1], "arcs": [[0, 1], [1, 0]]}')

    # the former pickles (a dict of arcs and edges) are still read, with a deprecation warning
    former = gum.DAG.__new__(gum.DAG)
    with self.assertWarns(DeprecationWarning):
      former.__setstate__({"arcs": {(0, 1), (1, 2)}})
    self.assertEqual(former.arcs(), {(0, 1), (1, 2)})

  def testPAGBasic(self):
    p = gum.PAG()
    for i in range(4):
      p.addNodeWithId(i)
    self.assertEqual(p.size(), 4)
    self.assertEqual(p.sizeEdges(), 0)

    p.addEdge(0, 1, gum.EdgeMark_Tail, gum.EdgeMark_Arrowhead)
    p.addEdge(1, 2, gum.EdgeMark_Circle, gum.EdgeMark_Circle)
    p.addEdge(2, 3, gum.EdgeMark_Arrowhead, gum.EdgeMark_Arrowhead)
    self.assertEqual(p.sizeEdges(), 3)

    self.assertTrue(p.isDefinitelyDirected(0, 1))
    self.assertFalse(p.isDefinitelyDirected(1, 0))
    self.assertTrue(p.isCircle(1, 2))
    self.assertTrue(p.isCircle(2, 1))
    self.assertTrue(p.isBidirected(2, 3))
    self.assertFalse(p.isTail(0, 1))
    self.assertTrue(p.isTail(1, 0))

  def testPAGMarkAt(self):
    p = gum.PAG()
    p.addNodeWithId(0)
    p.addNodeWithId(1)
    p.addEdge(0, 1, gum.EdgeMark_Tail, gum.EdgeMark_Arrowhead)
    self.assertEqual(p.markAt(0, 1), gum.EdgeMark_Arrowhead)
    self.assertEqual(p.markAt(1, 0), gum.EdgeMark_Tail)

    p.setMarkAt(0, 1, gum.EdgeMark_Circle)
    self.assertEqual(p.markAt(0, 1), gum.EdgeMark_Circle)

  def testPAGReorient(self):
    p = gum.PAG()
    p.addNodeWithId(0)
    p.addNodeWithId(1)
    p.addNodeWithId(2)
    p.addEdge(0, 1, gum.EdgeMark_Tail, gum.EdgeMark_Arrowhead)
    p.addEdge(1, 2, gum.EdgeMark_Circle, gum.EdgeMark_Circle)
    p.reorientAllWith(gum.EdgeMark_Circle)
    for e in p.edges():
      self.assertEqual(p.markAt(e[0], e[1]), gum.EdgeMark_Circle)
      self.assertEqual(p.markAt(e[1], e[0]), gum.EdgeMark_Circle)

  def testPAGToDot(self):
    p = gum.PAG()
    p.addNodeWithId(0)
    p.addNodeWithId(1)
    p.addEdge(0, 1, gum.EdgeMark_Tail, gum.EdgeMark_Arrowhead)
    dot = p.toDot()
    self.assertIn("digraph", dot)
    self.assertIn("0", dot)
    self.assertIn("1", dot)

  def testPAGIter(self):
    p = gum.PAG()
    for i in range(5):
      p.addNodeWithId(i)
    self.assertEqual(set(p), {0, 1, 2, 3, 4})

  def testPAGConnectedComponents(self):
    p = gum.PAG()
    for i in range(4):
      p.addNodeWithId(i)
    p.addEdge(0, 1)
    p.addEdge(2, 3)
    self.assertEqual(p.connectedComponentsCount(), 2)

  def testGraphSaveGUMFormatGivenByTheExtension(self):
    import os
    import tempfile

    g = gum.fastDAG("A->B->C")
    with tempfile.TemporaryDirectory() as tmp:
      for ext in (".jgum", ".bgum"):
        filename = os.path.join(tmp, "graph" + ext)
        g.saveGUM(filename)  # no binary: the format of the extension
        self.assertEqual(gum.loadGraph(filename), g)
      # binary for another extension
      g.saveGUM(os.path.join(tmp, "graph.gum"))
      with open(os.path.join(tmp, "graph.gum"), "rb") as f:
        self.assertNotEqual(f.read(1), b"{")
      with self.assertRaises(gum.InvalidArgument):
        g.saveGUM(os.path.join(tmp, "graph.jgum"), binary=True)
      with self.assertRaises(gum.InvalidArgument):
        g.saveGUM(os.path.join(tmp, "graph.bgum"), binary=False)
      with self.assertRaises(gum.InvalidArgument):
        gum.loadGraph(os.path.join(tmp, "graph.txt"))

  def testSaveGraphAndLoadGraphGivenByTheExtension(self):
    import os
    import tempfile

    self.assertEqual(gum.availableGraphExts(), "jgum|bgum|pkl")
    cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B<-C;B->D"))
    cliques = gum.LazyPropagation(gum.fastBN("A->B->C<-D;C->E;B->E")).junctionTree()
    with tempfile.TemporaryDirectory() as tmp:
      for ext in ("jgum", "bgum", "pkl"):
        filename = os.path.join(tmp, "graph." + ext)
        gum.saveGraph(cpdag, filename)
        read = gum.loadGraph(filename)
        self.assertIsInstance(read, gum.CPDAG)
        self.assertEqual(read, cpdag)
        self.assertEqual(read.nameFromId(3), "D")
      # a clique graph can only be pickled
      filename = os.path.join(tmp, "cliques.pkl")
      gum.saveGraph(cliques, filename)
      read = gum.loadGraph(filename)
      self.assertEqual(read.edges(), cliques.edges())
      for n in cliques.nodes():
        self.assertEqual(read.clique(n), cliques.clique(n))
      with self.assertRaises(NotImplementedError):
        gum.saveGraph(cliques, os.path.join(tmp, "cliques.jgum"))
      # an unknown extension: the known ones are given
      for io in (lambda f: gum.saveGraph(cpdag, f), gum.loadGraph):
        with self.assertRaisesRegex(gum.InvalidArgument, "jgum\\|bgum\\|pkl"):
          io(os.path.join(tmp, "graph.txt"))
        # the extension is the one of the file name, not of a directory
        with self.assertRaisesRegex(gum.InvalidArgument, "extension '' unknown"):
          io(os.path.join(tmp, "d.v2", "graph"))

  def testEdgeMarkIsTheEnumerationOfAgrum(self):
    self.assertEqual(int(gum.EdgeMark.Circle), gum.EdgeMark_Circle)
    self.assertEqual(int(gum.EdgeMark.Tail), gum.EdgeMark_Tail)
    self.assertEqual(int(gum.EdgeMark.Arrowhead), gum.EdgeMark_Arrowhead)
    self.assertFalse(hasattr(gum, "IntEnum"))

  def testPAGToMixedGraphKeepsTheNames(self):
    # transitory: to be removed with PAG.toMixedGraph (deprecated after 3.3.2)
    if not hasattr(gum.PAG, "toMixedGraph"):
      self.skipTest("the deprecated PAG.toMixedGraph has been removed")
    pag = gum.fastPAG("A o-> B;B<->C")
    with self.assertWarns(DeprecationWarning):
      mg = pag.toMixedGraph()
    self.assertEqual({mg.nameFromId(n) for n in mg.nodes()}, {"A", "B", "C"})
    self.assertTrue(mg.existsArc(pag.idFromName("B"), pag.idFromName("C")))
    self.assertTrue(mg.existsArc(pag.idFromName("C"), pag.idFromName("B")))

  def testCliqueGraphPickling(self):
    import os
    import pickle
    import tempfile

    bn = gum.fastBN("A->B->C<-D;C->E;B->E")
    jt = gum.LazyPropagation(bn).junctionTree()
    copy = pickle.loads(pickle.dumps(jt))
    self.assertEqual(set(copy.nodes()), set(jt.nodes()))
    self.assertEqual(copy.edges(), jt.edges())
    for n in jt.nodes():
      self.assertEqual(copy.clique(n), jt.clique(n))
    for x, y in jt.edges():
      self.assertEqual(copy.separator(x, y), jt.separator(x, y))

    # the cliques are not in the GUM format
    with self.assertRaises(NotImplementedError):
      jt.saveGUMstring()
    with tempfile.TemporaryDirectory() as tmp:
      with self.assertRaises(NotImplementedError):
        jt.saveGUM(os.path.join(tmp, "jt.jgum"))
      with self.assertRaises(NotImplementedError):
        gum.saveGraph(jt, os.path.join(tmp, "jt.bgum"))
      self.assertEqual(os.listdir(tmp), [])

  def testMeekRulesPropagateToDAGWithDomainSizes(self):
    mg = gum.MixedGraph()
    for _ in range(3):
      mg.addNode()
    mg.addEdge(0, 1)
    mg.addEdge(1, 2)
    meek = gum.MeekRules()
    dag = meek.propagateToDAG(mg, {0: 2, 1: 2, 2: 10})
    self.assertEqual(dag.sizeArcs(), 2)
    # no v-structure added: the propagated graph has a consistent extension
    self.assertLessEqual(len(dag.parents(1)), 1)
    self.assertEqual(meek.propagateToDAG(mg).sizeArcs(), 2)

    with self.assertRaises(gum.InvalidArgument):
      meek.propagateToDAG(mg, {0: 2, 1: 2})
    with self.assertRaises(gum.InvalidArgument):
      meek.propagateToDAG(mg, {0: 2, 1: 0, 2: 2})
    with self.assertRaises(TypeError):
      meek.propagateToDAG(mg, [2, 2, 2])

  def testNamesFromIds(self):
    dag = gum.fastDAG("A->B->C;D->C")
    unnamed = dag.addNode()
    self.assertEqual(dag.namesFromIds(2), "C")
    self.assertEqual(dag.namesFromIds({0, 1}), {"A", "B"})
    self.assertEqual(dag.namesFromIds([2, 0, 2]), ["C", "A", "C"])  # a list keeps its order
    self.assertEqual(dag.namesFromIds((3, 1)), ("D", "B"))
    self.assertEqual(dag.namesFromIds(frozenset({0})), frozenset({"A"}))
    self.assertEqual(dag.namesFromIds(set()), set())
    # nested containers, keys of dicts included
    self.assertEqual(dag.namesFromIds(dag.arcs()), {("A", "B"), ("B", "C"), ("D", "C")})
    self.assertEqual(dag.namesFromIds({0: [{1}, (2, 3)], (1, 2): 3}), {"A": [{"B"}, ("C", "D")], ("B", "C"): "D"})
    self.assertEqual(dag.namesFromIds(unnamed), str(unnamed))  # a node without name: its id as a string

    self.assertEqual(dag.idsFromNames("C"), 2)
    self.assertEqual(dag.idsFromNames(["C", "A"]), [2, 0])
    self.assertEqual(dag.idsFromNames({"A": ("B", {"C"})}), {0: (1, {2})})
    self.assertEqual(dag.idsFromNames(dag.namesFromIds(dag.arcs())), dag.arcs())

    for wrong in (99, -1, 10**30):
      with self.assertRaises(gum.NotFound):
        dag.namesFromIds(wrong)
    with self.assertRaises(gum.NotFound):
      dag.namesFromIds({0, 99})
    with self.assertRaises(gum.NotFound):
      dag.idsFromNames("Z")
    with self.assertRaises(gum.NotFound):
      dag.idsFromNames(str(unnamed))  # a node without name is not found by its id as a string
    for wrong in ("A", True, 1.5, iter([0]), [0, "A"]):
      with self.assertRaises(TypeError):
        dag.namesFromIds(wrong)
    for wrong in (0, ["A", 1], 1.5):
      with self.assertRaises(TypeError):
        dag.idsFromNames(wrong)

    # every class of graph
    graphs = [
      gum.fastDiGraph("A->B->A"),
      gum.fastUndiGraph("A-B"),
      gum.fastMixedGraph("A->B-C"),
      gum.fastPDAG("A->B-C"),
      gum.fastChainGraph("A->B-C"),
      gum.fastMarkedGraph("A o-> B"),
      gum.fastPAG("A o-> B"),
      gum.CPDAG.fromDAG(gum.fastDAG("A->B")),
      gum.MPDAG.fromCPDAG(gum.CPDAG.fromDAG(gum.fastDAG("A->B")), {("A", "B")}),
      gum.MAG.fromDAG(gum.fastDAG("A->B"), set()),
      gum.EssentialGraph(gum.fastBN("A->B")),
    ]
    for g in graphs:
      with self.subTest(graph=type(g).__name__):
        self.assertEqual(g.namesFromIds([0, 1]), ["A", "B"])
        self.assertEqual(g.idsFromNames(("B", "A")), (1, 0))
        with self.assertRaises(gum.NotFound):
          g.namesFromIds(7)


ts = unittest.TestSuite()
addTests(ts, TestGraph)
