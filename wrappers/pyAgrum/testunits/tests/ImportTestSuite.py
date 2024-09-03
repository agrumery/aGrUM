# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
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
import logging
import sys

from .pyAgrumTestSuite import pyAgrumTestCase, addTests

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

  matplotlibFound = True
except ImportError:
  matplotlibFound = False

try:
  import IPython

  ipythonFound = True
except ImportError:
  ipythonFound = False


class TestImport(pyAgrumTestCase):
  def testImport(self):
    try:
      import pyAgrum
      import pyAgrum.lib.bn2scores
      import pyAgrum.lib.bn_vs_bn

      if matplotlibFound and ipythonFound:
        import pyAgrum.lib.ipython
        import pyAgrum.lib.notebook
      else:
        self.log.warning(
          "ipython and matplotlib are needed for modules lib.ipython and lib.notebook")

      if matplotlibFound:
        import pyAgrum.lib.bn2graph
        import pyAgrum.lib.bn2roc
        import pyAgrum.lib.dynamicBN
      else:
        self.log.warning(
          "matplotlib is needed for modules lib.ipython and lib.notebook")

      if sklearnFound:
        import pyAgrum.lib.classifier
      else:
        self.log.warning(
          "sklearn is needed for modules lib.classifier")

    except Exception as e:
      self.assertFalse(False, "Import error : " + str(e))


ts = unittest.TestSuite()
addTests(ts, TestImport)
