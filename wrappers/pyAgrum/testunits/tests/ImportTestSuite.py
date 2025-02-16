############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

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
