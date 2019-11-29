# -*- encoding: UTF-8 -*-
import unittest

try:
  import matplotlib
  matplotlibFound=True
except ImportError:
  matplotlibFound=False

try:
  import IPython
  ipythonFound=True
except ImportError:
  ipythonFound=False



from pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestImport(pyAgrumTestCase):
  def testImport(self):
    try:
      import pyAgrum
      import pyAgrum.lib.bn2scores
      import pyAgrum.lib.bn_vs_bn
      
      if matplotlibFound:
        if ipythonFound:
          import pyAgrum.lib.ipython
          import pyAgrum.lib.notebook

        import pyAgrum.lib.bn2graph
        import pyAgrum.lib.bn2roc
        import pyAgrum.lib.dynamicBN

    except Exception as e:
      self.assertFalse("Import error : "+str(e))
    pass
      
ts = unittest.TestSuite()
addTests(ts, TestImport)