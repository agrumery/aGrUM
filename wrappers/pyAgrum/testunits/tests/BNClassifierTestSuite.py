# -*- encoding: UTF-8 -*-
import unittest

import numpy
import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class BNCLassifierTestCase(pyAgrumTestCase):
  def testStructuralHamming(self):
    pass


ts = unittest.TestSuite()
addTests(ts, BNCLassifierTestCase)
