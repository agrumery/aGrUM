# -*- encoding: UTF-8 -*-
import unittest

from pyAgrumTestSuite import pyAgrumTestCase, addTests

try:
  import sklearn
  sklearnFound = True
except ImportError:
  sklearnFound = False

try:
  import pandas
  pandasFound = True
except ImportError:
  pandasFound = False
  
try:
  import matplotlib
  matplotlibFound=True
except ImportError:
  matplotlibFound=False

try:
  import IPython
  ipythonFound=True
except ImportError:
  ipythonFound = False
  
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

      if sklearnFound:
        import pyAgrum.lib.classifier

    except Exception as e:
      self.assertFalse("Import error : "+str(e))
    pass
      
ts = unittest.TestSuite()
addTests(ts, TestImport)