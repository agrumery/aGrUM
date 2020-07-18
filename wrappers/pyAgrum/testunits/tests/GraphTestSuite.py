# -*- encoding: UTF-8 -*-
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

    mixed_graph.addEdge(0,2)
    mixed_graph.addEdge(1,2)
    mixed_graph.addEdge(2,3)

    copy = mixed_graph

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
