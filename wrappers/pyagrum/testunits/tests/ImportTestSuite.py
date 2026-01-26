############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest

from .pyAgrumTestSuite import pyAgrumTestCase, addTests
import importlib.util

# Check if the required libraries are installed
res = importlib.util.find_spec("sklearn")
sklearnFound = res is not None
res = importlib.util.find_spec("pandas")
pandasFound = res is not None
res = importlib.util.find_spec("matplotlib")
matplotlibFound = res is not None
res = importlib.util.find_spec("ipython")
ipythonFound = res is not None


class TestImport(pyAgrumTestCase):
  def testImport(self):
    try:
      if matplotlibFound and ipythonFound:
        pass
      else:
        self.log.warning("ipython and matplotlib are needed for modules lib.ipython and lib.notebook")

      if matplotlibFound:
        pass
      else:
        self.log.warning("matplotlib is needed for modules lib.ipython and lib.notebook")

      if sklearnFound:
        pass
      else:
        self.log.warning("sklearn is needed for modules lib.classifier")

    except Exception as e:
      self.assertFalse(False, "Import error : " + str(e))


ts = unittest.TestSuite()
addTests(ts, TestImport)
