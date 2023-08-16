# (c) Copyright 2015-2023 by Pierre-Henri Wuillemin(@LIP6)
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
