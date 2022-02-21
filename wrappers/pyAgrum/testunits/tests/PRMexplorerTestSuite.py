# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
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

import pyAgrum as gum
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
