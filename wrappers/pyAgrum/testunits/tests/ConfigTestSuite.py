# -*- encoding: UTF-8 -*-

import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestConfig(pyAgrumTestCase):
  def testReadOnlyConfigurationStructure(self):
    with self.assertRaises(SyntaxError):
      gum.config["gogo", "gaga"] = 0
    with self.assertRaises(SyntaxError):
      gum.config["theme", "gaga"] = 0

ts = unittest.TestSuite()
addTests(ts, TestConfig)