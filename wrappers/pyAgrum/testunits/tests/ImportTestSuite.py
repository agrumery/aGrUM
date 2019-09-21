# -*- encoding: UTF-8 -*-

import unittest

from pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestImport(pyAgrumTestCase):
  def testImport(self):
#    try:
#      import pyAgrum
#      import pyAgrum.lib.bn2graph
#      import pyAgrum.lib.bn2roc
#      import pyAgrum.lib.bn2scores
#      import pyAgrum.lib.bn_vs_bn
#      import pyAgrum.lib.dynamicBN
#      import pyAgrum.lib.ipython
#      import pyAgrum.lib.notebook
#    except Exception as e:
#      print(e)
#      self.assertFalse("Import error : "+e.message)
    pass
      
ts = unittest.TestSuite()
addTests(ts, TestImport)