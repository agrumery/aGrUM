import sys
import os
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests
import pyAgrum.causal as csl

class TestDSep(pyAgrumTestCase):
  """
  A   B
   \ / \
 H  C   D
  \ |   |
    E   |
     \ /
      F
      |
      G
  """

  def setUp(self):
    self.bn = gum.fastBN("A->C->E->F->G;F<-D<-B->C;H->E")
    self.iA, self.iB, self.iC, self.iD, self.iE, self.iF, self.iG, self.iH = [self.bn.idFromName(s) for s in "ABCDEFGH"]
    self.tous = {self.iA, self.iB, self.iC, self.iD, self.iE, self.iF, self.iG, self.iH}

  def test_minimalCondSet4A(self):
    r = self.bn.minimalCondSet(self.iA, self.tous)
    self.assertEqual(r, {self.iA})

    r = self.bn.minimalCondSet(self.iA, self.tous - {self.iA})
    self.assertEqual(r, {self.iB, self.iC})

    r = self.bn.minimalCondSet(self.iA, {self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iE,self.iF})

    r = self.bn.minimalCondSet(self.iA, {self.iB, self.iC, self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iB, self.iC})

    r = self.bn.minimalCondSet(self.iA, {self.iC, self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iC, self.iE, self.iF})

  def test_minimalCondSet4C(self):
    r = self.bn.minimalCondSet(self.iC, self.tous)
    self.assertEqual(r, {self.iC})

    r = self.bn.minimalCondSet(self.iC, self.tous - {self.iC})
    self.assertEqual(r, {self.iA, self.iB, self.iH, self.iE})

    r = self.bn.minimalCondSet(self.iC, {self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iE, self.iF})

    r = self.bn.minimalCondSet(self.iC, {self.iB, self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iE, self.iB})

    r = self.bn.minimalCondSet(self.iC, {self.iC, self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iC})

  def test_minimaCondSetForJoint(self):
    r = self.bn.minimalCondSet({self.iA, self.iB}, self.tous)
    self.assertEqual(r, {self.iA, self.iB})
    r = self.bn.minimalCondSet({self.iA, self.iB}, self.tous - {self.iA, self.iB})
    self.assertEqual(r, {self.iC, self.iD})
    r = self.bn.minimalCondSet({self.iC, self.iE}, self.tous - {self.iC, self.iE})
    self.assertEqual(r, self.tous - {self.iC, self.iE, self.iG})

ts = unittest.TestSuite()
addTests(ts, TestDSep)
