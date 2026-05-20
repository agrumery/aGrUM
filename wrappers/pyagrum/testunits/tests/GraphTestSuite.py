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
import numpy as np

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestGraph(pyAgrumTestCase):
  def _testAddNodes(self, g):
    self.assertEqual(g.addNodes(6), set(range(6)))
    g.eraseNode(1)
    g.eraseNode(2)
    g.eraseNode(4)
    self.assertEqual(g.nodes(), {0, 3, 5})
    self.assertEqual(g.addNodes(4), set([1, 2, 4, 6]))
    self.assertEqual(g.nodes(), set(range(7)))

  def testAddNodes(self):
    self._testAddNodes(gum.DiGraph())
    self._testAddNodes(gum.UndiGraph())
    self._testAddNodes(gum.MixedGraph())
    self._testAddNodes(gum.DAG())

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

  def _testConnectedComponents(self, g, add_arc=None, add_edge=None):
    """
    Helper: tests connectedComponents / connectedComponentsList / connectedComponentsCount
    on any graph supporting both arc-like or edge-like connections.
    add_arc(g, u, v)  -- callable that adds a directed or undirected arc/edge
    add_edge(g, u, v) -- callable that adds an undirected edge (or same as add_arc)
    """
    if add_arc is None:
      add_arc = lambda g, u, v: g.addArc(u, v)
    if add_edge is None:
      add_edge = add_arc

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


ts = unittest.TestSuite()
addTests(ts, TestGraph)
