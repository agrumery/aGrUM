############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
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
    self.assertEqual(mg.mixedUnorientedPath(1, 4), [])
    self.assertEqual(mg.mixedOrientedPath(1, 4), [])
    mg.addArc(0, 1)
    mg.addEdge(1, 2)
    mg.addArc(3, 2)
    self.assertEqual(mg.mixedUnorientedPath(0, 3), [0, 1, 2, 3])
    self.assertEqual(mg.mixedOrientedPath(0, 3), [])
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
  def testAdjancecyMatrix(self):
    mg = gum.MixedGraph()
    mg.addNodes(4)
    mg.addArc(0, 1)
    mg.addArc(1, 2)
    mg.addEdge(2, 3)
    self.assertTrue(np.array_equal(mg.adjacencyMatrix(), np.array([[0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1], [0, 0, 1, 0]])))
    
    mg = gum.DAG()
    mg.addNodes(4)
    mg.addArc(0, 1)
    mg.addArc(1, 2)
    mg.addArc(2, 3)
    self.assertTrue(np.array_equal(mg.adjacencyMatrix(), np.array([[0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1], [0, 0, 0, 0]])))
    
    mg = gum.UndiGraph()
    mg.addNodes(4)
    mg.addEdge(0, 1)
    mg.addEdge(1, 2)
    mg.addEdge(2, 3)
    self.assertTrue(np.array_equal(mg.adjacencyMatrix(), np.array([[0, 1, 0, 0], [1, 0, 1, 0], [0, 1, 0, 1], [0, 0, 1, 0]])))

ts = unittest.TestSuite()
addTests(ts, TestGraph)
