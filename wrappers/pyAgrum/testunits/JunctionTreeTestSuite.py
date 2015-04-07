# -*- encoding: UTF-8 -*-
import pyAgrum as gum 

import unittest
from pyAgrumTestSuite import pyAgrumTestCase


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
        self.bn.cpt(self.s)[:] = [ [0.5, 0.5],
                                   [0.9, 0.1] ]
        self.bn.cpt(self.r)[:] = [ [0.8, 0.2],
                                   [0.2, 0.8] ]
        self.bn.cpt(self.w)[0,0,:] = [1, 0]
        self.bn.cpt(self.w)[0,1,:] = [0.1, 0.9]
        self.bn.cpt(self.w)[1,0,:] = [0.1, 0.9]
        self.bn.cpt(self.w)[1,1,:] = [0.01, 0.99]
        
        self.ie = gum.LazyPropagation(self.bn)
        self.jt=self.ie.junctionTree()

class TopologyFeature(JunctionTreeTestCase):

    def testSimple(self):
        self.assertEqual(len(self.jt.ids()),2)
        self.assertEqual(len(self.jt.edges()),1)
        self.assertEqual(self.jt.clique(0).union(self.jt.clique(1)),set([0,1,2,3]))
        self.assertTrue(self.jt.clique(0).intersection(self.jt.clique(1))==set([1,2])
                        or
                        self.jt.clique(0).intersection(self.jt.clique(1))==set([0,3])
                        )

    def testUnorientedGraph(self):
      self.assertEqual(self.jt.neighbours(0),{1})
      self.assertEqual(self.jt.neighbours(1),{0})
      
ts = unittest.TestSuite()
ts.addTest(TopologyFeature('testSimple'))
