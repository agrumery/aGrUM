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

import gc
import unittest
import warnings

import pyagrum as gum
import pyagrum.lib.image as gimg

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


def _cairosvgAvailable() -> bool:
  try:
    import cairosvg  # noqa: F401
  except (ImportError, OSError):  # OSError: cairosvg without the cairo library
    return False
  return True


class TestExportAsArray(pyAgrumTestCase):
  def _assertNoTemporaryFileIsLeftOpen(self, export):
    bn = gum.fastBN("A->B->C")
    with warnings.catch_warnings(record=True) as w:
      warnings.simplefilter("always", ResourceWarning)
      img = export(bn)  # without file name: an image as a numpy array
      gc.collect()
    self.assertEqual(img.ndim, 3)
    self.assertEqual([str(x.message) for x in w if issubclass(x.category, ResourceWarning)], [])

  def testExportLeavesNoTemporaryFileOpen(self):
    self._assertNoTemporaryFileIsLeftOpen(gimg.export)

  @unittest.skipUnless(_cairosvgAvailable(), "exportInference needs cairosvg")
  def testExportInferenceLeavesNoTemporaryFileOpen(self):
    self._assertNoTemporaryFileIsLeftOpen(gimg.exportInference)


ts = unittest.TestSuite()
addTests(ts, TestExportAsArray)
