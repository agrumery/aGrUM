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

import pyagrum.prm as gum

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class PRMexplorerTestCase(pyAgrumTestCase):
  def testO3PRMLoad(self):
    prm = gum.PRMexplorer()
    prm.load(self.agrumSrcDir("o3prm/Asia.o3prm"))  # verbose=False : don't want to see the warnings

  def testO3PRMLoadErrs(self):
    with self.assertRaises(gum.FatalError):
      prm = gum.PRMexplorer()
      prm.load(self.agrumSrcDir("o3prm/DoesNotExist.o3prm"))

    with self.assertRaises(gum.FatalError):
      prm = gum.PRMexplorer()
      prm.load(self.agrumSrcDir("o3prm/AsiaWithError.o3prm"), "", False)

  def testO3PRMClasses(self):
    prm = gum.PRMexplorer()
    prm.load(self.agrumSrcDir("o3prm/Asia.o3prm"))
    self.assertEqual(str(prm.classes()), "['Asia']")

  def testO3PRMAttributes(self):
    prm = gum.PRMexplorer()
    prm.load(self.agrumSrcDir("o3prm/Asia.o3prm"))

    witness = [
      ("boolean", "tuberculosis", ["visitToAsia"]),
      ("boolean", "visitToAsia", []),
      ("boolean", "smoking", []),
      ("boolean", "dyspnea", ["bronchitis", "tubOrCancer"]),
      ("boolean", "lungCancer", ["smoking"]),
      ("boolean", "tubOrCancer", ["lungCancer", "tuberculosis"]),
      ("boolean", "bronchitis", ["smoking"]),
      ("boolean", "positiveXRay", ["tubOrCancer"]),
    ]
    attrs = prm.classAttributes("Asia")
    self.assertEqual(len(attrs), len(witness))
    wit_nom = set([nom for (t, nom, par) in witness])
    for t, n, p in attrs:
      self.assertTrue(n in wit_nom)
      self.assertEqual(t, "boolean")

  def _load(self, filename):
    prm = gum.PRMexplorer()
    prm.load(self.agrumSrcDir(f"o3prm/{filename}"), self.agrumSrcDir("o3prm"))
    return prm

  def testO3PRMTypes(self):
    # the file uses a deprecated syntax: warnings do not prevent the loading
    prm = self._load("types.o3prm")
    self.assertEqual(prm.getSuperType("t_ink"), "t_state")
    self.assertIsNone(prm.getSuperType("boolean"))
    self.assertEqual(set(prm.getDirectSubTypes("t_state")), {"t_ink", "t_degraded", "t_paper"})
    self.assertEqual(prm.getLabels("t_state"), ["OK", "NOK"])
    self.assertEqual(prm.getLabelMap("t_paper"), {"Ready": "OK", "Jammed": "NOK", "Empty": "NOK"})
    self.assertIsNone(prm.getLabelMap("boolean"))

  def testO3PRMAggregates(self):
    prm = self._load("aggregates.o3prm")
    aggregates = {a[1]: a for a in prm.classAggregates("HappyClass")}
    self.assertEqual(aggregates["has_red"], ("boolean", "has_red", "exists", "red", ["A.color"]))
    self.assertEqual(aggregates["infrared"][:4], ("t_color", "infrared", "min", None))
    self.assertEqual(set(aggregates["infrared"][4]), {"has_red", "has_blue", "has_green", "has_yellow"})

  def testO3PRMClassesAndInterfaces(self):
    prm = self._load("complexprinters_system.o3prm")
    self.assertEqual(set(prm.interfaces()), {"Printer", "Equipment"})
    self.assertEqual(prm.classImplements("SafeComputer"), ["Equipment"])
    self.assertEqual(prm.classImplements("PowerSupply"), [])
    self.assertIn(("Printer", "printers", True), prm.classReferences("SafeComputer"))
    self.assertEqual(set(prm.classParameters("ParamClass")), {"lambda", "t"})
    self.assertEqual(set(prm.getImplementations("Printer")), {"ColorPrinter", "BWPrinter"})
    self.assertEqual(prm.getDirectSubInterfaces("Equipment"), ["Printer"])
    self.assertIn(("Room", "room", False), prm.interReferences("Equipment"))
    self.assertEqual(prm.interAttributes("Equipment"), [("t_degraded", "equipState")])
    nodes, dot = prm.classDag("PowerSupply")
    self.assertEqual(set(nodes.values()), {"state"})
    self.assertIn("digraph", dot)
    systems = prm.getalltheSystems()
    self.assertEqual([name for name, _, _ in systems], ["aSys"])
    self.assertIn(("pow", "PowerSupply"), systems[0][1].values())


ts = unittest.TestSuite()
addTests(ts, PRMexplorerTestCase)
