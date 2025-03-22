############################################################################
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class PRMexplorerTestCase(pyAgrumTestCase):
  def testO3PRMLoad(self):
    prm = gum.PRMexplorer()
    prm.load(
      self.agrumSrcDir('o3prm/Asia.o3prm'))  # verbose=False : don't want to see the warnings

  def testO3PRMLoadErrs(self):
    try:
      prm = gum.PRMexplorer()
      prm.load(self.agrumSrcDir('o3prm/DoesNotExist.o3prm'))
      self.assertTrue(False)
    except:
      self.assertTrue(True)

    try:
      prm = gum.PRMexplorer()
      prm.load(self.agrumSrcDir('o3prm/AsiaWithError.o3prm'), '', False)
      self.assertTrue(False)
    except:
      self.assertTrue(True)

  def testO3PRMClasses(self):
    prm = gum.PRMexplorer()
    prm.load(self.agrumSrcDir('o3prm/Asia.o3prm'))
    self.assertEqual(str(prm.classes()), "['Asia']")

  def testO3PRMAttributes(self):
    prm = gum.PRMexplorer()
    prm.load(self.agrumSrcDir('o3prm/Asia.o3prm'))

    witness = [('boolean', 'tuberculosis', ['visitToAsia']), ('boolean', 'visitToAsia', []), ('boolean', 'smoking', []),
               ('boolean', 'dyspnea', ['bronchitis', 'tubOrCancer']), ('boolean', 'lungCancer', ['smoking']),
               ('boolean', 'tubOrCancer', ['lungCancer', 'tuberculosis']), ('boolean', 'bronchitis', ['smoking']),
               ('boolean', 'positiveXRay', ['tubOrCancer'])]
    attrs = prm.classAttributes('Asia')
    self.assertEqual(len(attrs), len(witness))
    wit_nom = set([nom for (t, nom, par) in witness])
    for t, n, p in attrs:
      self.assertTrue(n in wit_nom)
      self.assertTrue(t == 'boolean')


ts = unittest.TestSuite()
addTests(ts, PRMexplorerTestCase)
