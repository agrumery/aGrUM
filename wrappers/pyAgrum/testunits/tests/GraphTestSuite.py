# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
# (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestGraph(pyAgrumTestCase):
  def _testAddNodes(self,g):
    self.assertEqual(g.addNodes(6), set(range(6)))
    g.eraseNode(1)
    g.eraseNode(2)
    g.eraseNode(4)
    self.assertEqual(g.nodes(), set([0, 3, 5]))
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

    ug.addEdge(0,2)
    ug.addEdge(1,2)
    ug.addEdge(2,3)

    mixed_graph = gum.MixedGraph()

    mixed_graph.addNodes(4)

    mixed_graph.addEdge(0,2)
    mixed_graph.addEdge(1,2)
    mixed_graph.addEdge(2,3)

    mg = gum.MixedGraph(ug)

    self.assertEqual(mixed_graph, mg)

  def testPath(self):
    mg=gum.MixedGraph()
    mg.addNodes(5)
    self.assertEqual(mg.mixedUnorientedPath(1,4),[])
    self.assertEqual(mg.mixedOrientedPath(1,4),[])
    mg.addArc(0,1)
    mg.addEdge(1,2)
    mg.addArc(3,2)
    self.assertEqual(mg.mixedUnorientedPath(0,3),[0,1,2,3])
    self.assertEqual(mg.mixedOrientedPath(0,3),[])
    self.assertEqual(mg.mixedOrientedPath(0,2),[0,1,2])

  def testConstructorFromDG(self):
    dg = gum.DiGraph()

    dg.addNodes(4)

    dg.addArc(0,2)
    dg.addArc(1,2)
    dg.addArc(2,3)

    mixed_graph = gum.MixedGraph()

    mixed_graph.addNodes(4)

    mixed_graph.addArc(0,2)
    mixed_graph.addArc(1,2)
    mixed_graph.addArc(2,3)

    mg = gum.MixedGraph(dg)

    self.assertEqual(mixed_graph, mg)

  def testCopyConstructor(self):
    mixed_graph = gum.MixedGraph()

    mixed_graph.addNodes(4)

    mixed_graph.addArc(0,2)
    mixed_graph.addEdge(1,2)
    mixed_graph.addArc(2,3)

    self.assertEquals(mixed_graph.parents(2),{0})
    self.assertEquals(mixed_graph.children(2),{3})
    self.assertEquals(mixed_graph.neighbours(2),{1})
    self.assertEquals(mixed_graph.adjacents(2),{0,1,3})

    copy = mixed_graph
    self.assertEqual(mixed_graph, copy)

    copy = gum.MixedGraph(mixed_graph)
    self.assertEqual(mixed_graph, copy)

  def testNonRegressionAddEdge(self):
    ug=gum.UndiGraph()
    ug.addNodes(4)
    with self.assertRaises(gum.InvalidNode):
      ug.addEdge(1,6)
    with self.assertRaises(gum.InvalidNode):
      ug.addEdge(7,0)
    with self.assertRaises(gum.InvalidNode):
      ug.addEdge(6,7)


ts = unittest.TestSuite()
addTests(ts, TestGraph)
