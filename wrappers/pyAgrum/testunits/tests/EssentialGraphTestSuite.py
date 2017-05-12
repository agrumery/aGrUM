# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestEssentialGraph(pyAgrumTestCase):
  def testChain(self):
    bn = gum.fastBN("a->b->c")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 3)
    self.assertEqual(eg.sizeArcs(), 0)
    self.assertEqual(eg.sizeEdges(), 2)

  def testVstructure(self):
    bn = gum.fastBN("a->b;c->b")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 3)
    self.assertEqual(eg.sizeArcs(), 2)
    self.assertEqual(eg.sizeEdges(), 0)

  def testCaseD(self):
    bn = gum.fastBN("a->b;c1->b;c2->b;a->c1;a->c2")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 4)
    self.assertEqual(eg.sizeArcs(), 3)
    self.assertEqual(eg.sizeEdges(), 2)

  def testNotebook1(self):
    bn = gum.fastBN("A->B->C->D;E->B;F->G->D;F->H->I;E->J->K->I->M;K->L")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 13)
    self.assertEqual(eg.sizeArcs(), 8)
    self.assertEqual(eg.sizeEdges(), 5)

  def testNotebook2(self):
    bn = gum.fastBN("A->B;C->B;C->D;B->D;A->C")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 4)
    self.assertEqual(eg.sizeArcs(), 0)
    self.assertEqual(eg.sizeEdges(), 5)

  def testNotebook3(self):
    bn = gum.fastBN("Z->X->U;Y->X;Y->W")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 5)
    self.assertEqual(eg.sizeArcs(), 3)
    self.assertEqual(eg.sizeEdges(), 1)


ts = unittest.TestSuite()
addTests(ts, TestEssentialGraph)
