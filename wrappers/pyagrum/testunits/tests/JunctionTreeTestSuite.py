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

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class JunctionTreeTestCase(pyAgrumTestCase):
  def setUp(self):
    self.bn = gum.BayesNet()

    self.c, self.r = \
      [self.bn.add(gum.LabelizedVariable(name, name, 2))
       for name in 'c r'.split()]
    self.s, self.w = \
      [self.bn.add(gum.LabelizedVariable(name, name, 0).addLabel('no').addLabel('yes'))
       for name in 's w'.split()]

    for link in [(self.c, self.s), (self.c, self.r),
                 (self.s, self.w), (self.r, self.w)]:
      self.bn.addArc(*link)

    self.bn.cpt(self.c)[:] = [0.5, 0.5]
    self.bn.cpt(self.s)[:] = [[0.5, 0.5],
                              [0.9, 0.1]]
    self.bn.cpt(self.r)[:] = [[0.8, 0.2],
                              [0.2, 0.8]]
    self.bn.cpt(self.w)[0, 0, :] = [1, 0]
    self.bn.cpt(self.w)[0, 1, :] = [0.1, 0.9]
    self.bn.cpt(self.w)[1, 0, :] = [0.1, 0.9]
    self.bn.cpt(self.w)[1, 1, :] = [0.01, 0.99]

    self.ie = gum.LazyPropagation(self.bn)
    self.jt = self.ie.junctionTree()


class TopologyFeatureTestCase(JunctionTreeTestCase):
  def testSimple(self):
    self.assertEqual(len(self.jt.nodes()), 2)
    self.assertEqual(len(self.jt.edges()), 1)
    self.assertEqual(self.jt.clique(0).union(self.jt.clique(1)), set([0, 1, 2, 3]))
    self.assertTrue(self.jt.clique(0).intersection(self.jt.clique(1)) == set([1, 2])
                    or
                    self.jt.clique(0).intersection(self.jt.clique(1)) == set([0, 3])
                    )
    self.assertTrue(self.jt.separator(0, 1) == set([1, 2])
                    or
                    self.jt.separator(0, 1) == set([0, 3])
                    )

  def testUnorientedGraph(self):
    self.assertEqual(self.jt.neighbours(0), {1})
    self.assertEqual(self.jt.neighbours(1), {0})


class JunctionTreeGeneratorTestCase(pyAgrumTestCase):
  def testSimpleGraph(self):
    g = gum.UndiGraph();
    g.addNode()
    g.addNode()
    g.addEdge(0, 1)

    jtg = gum.JunctionTreeGenerator()
    jt = jtg.junctionTree(g)

    self.assertEqual(jt.size(), 1)
    self.assertEqual(jt.clique(0), {0, 1})

    jtg = gum.JunctionTreeGenerator()
    bjt = jtg.binaryJoinTree(g)

    self.assertEqual(bjt.size(), 1)
    self.assertEqual(bjt.clique(0), {0, 1})

  def testSimpleBN(self):
    bn = gum.BayesNet();
    c, r = [bn.add(gum.LabelizedVariable(name, name, 2)) for name in 'c r'.split()]
    bn.addArc(c, r)

    jtg = gum.JunctionTreeGenerator()
    jt = jtg.junctionTree(bn)

    self.assertEqual(jt.size(), 1)
    self.assertEqual(jt.clique(0), {0, 1})

    jtg = gum.JunctionTreeGenerator()
    bjt = jtg.binaryJoinTree(bn)

    self.assertEqual(bjt.size(), 1)
    self.assertEqual(bjt.clique(0), {0, 1})


ts = unittest.TestSuite()
addTests(ts, TopologyFeatureTestCase)
addTests(ts, JunctionTreeGeneratorTestCase)
