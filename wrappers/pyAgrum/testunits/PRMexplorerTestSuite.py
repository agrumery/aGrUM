# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class PRMexplorerTestCase(pyAgrumTestCase):
  def testO3PRMLoad(self):
    prm = gum.PRMexplorer()
    prm.load(
        self.agrumSrcDir('src/testunits/ressources/o3prm/Asia.o3prm'))  # verbose=False : don't want to see the warnings

  def testO3PRMLoadErrors(self):
    try:
      prm = gum.PRMexplorer()
      prm.load(self.agrumSrcDir('src/testunits/ressources/o3prm/DoesNotExist.o3prm'))
      self.assertTrue(False)
    except:
      self.assertTrue(True)

    try:
      prm = gum.PRMexplorer()
      prm.load(self.agrumSrcDir('src/testunits/ressources/o3prm/AsiaWithError.o3prm'), '', False)
      self.assertTrue(False)
    except:
      self.assertTrue(True)

  def testO3PRMClasses(self):
    prm = gum.PRMexplorer()
    prm.load(self.agrumSrcDir('src/testunits/ressources/o3prm/Asia.o3prm'))
    self.assertEqual(str(prm.classes()), "['Asia']")

  def testO3PRMAttributes(self):
    prm = gum.PRMexplorer()
    prm.load(self.agrumSrcDir('src/testunits/ressources/o3prm/Asia.o3prm'))

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
