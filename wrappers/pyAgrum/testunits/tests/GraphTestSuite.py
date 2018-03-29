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


ts = unittest.TestSuite()
addTests(ts, TestGraph)
