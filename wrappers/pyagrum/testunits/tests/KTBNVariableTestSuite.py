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

import pyagrum.ktbn as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

AT = gum.KTBN.ATEMPORAL


def _flatCPT(cpt):
  inst = gum.Instantiation(cpt)
  vals = []
  inst.setFirst()
  while not inst.end():
    vals.append(cpt[inst])
    inst.inc()
  return vals


class KTBNVariableTestCase(pyAgrumTestCase):
  def testAddVariables(self):
    m = gum.KTBN(3)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("Y", "", 3), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)

    self.assertEqual(m.nbTemporalVars(), 2)
    self.assertEqual(m.nbAtemporalVars(), 1)
    self.assertEqual(m.size(), 2 * 3 + 1)

    self.assertIn("X", m.temporalVarNames())
    self.assertIn("C", m.atemporalVarNames())
    self.assertTrue(m.exists("Y"))
    self.assertFalse(m.exists("Z"))

    self.assertEqual(m.timeSlice(m.variable("X", 1)), 1)
    self.assertEqual(m.baseName(m.variable("X", 1)), "X")
    self.assertEqual(m.timeSlice(m.variable("C")), AT)

    self.assertEqual(m.variable("X", 0).domainSize(), 2)
    self.assertEqual(m.variable("Y", 2).domainSize(), 3)

    foreign = gum.LabelizedVariable("FOREIGN", "", 2)
    with self.assertRaises(gum.NotFound):
      m.timeSlice(foreign)
    with self.assertRaises(gum.NotFound):
      m.baseName(foreign)

  def testAddVariableOverloads(self):
    m = gum.KTBN(2)
    m.addTemporal(gum.LabelizedVariable("X", "", 3))
    m.addAtemporal(gum.LabelizedVariable("C", "", 2))

    self.assertIn("X", m.temporalVarNames())
    self.assertIn("C", m.atemporalVarNames())
    self.assertEqual(m.nbTemporalVars(), 1)
    self.assertEqual(m.nbAtemporalVars(), 1)
    self.assertEqual(m.size(), 2 * 1 + 1)
    self.assertEqual(m.variable("X", 0).domainSize(), 3)
    self.assertEqual(m.variable("C").domainSize(), 2)

  def testAddVariableErrors(self):
    m = gum.KTBN(3)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)

    with self.assertRaises(gum.DuplicateLabel):
      m.add(gum.LabelizedVariable("X", "", 2), True)

    # names not matching "[number]" decode as plain bases
    m.add(gum.LabelizedVariable("W2", "", 2), True)
    self.assertIn("W2", m.temporalVarNames())
    self.assertEqual(m.baseName(m.variable("W2", 1)), "W2")
    m.add(gum.LabelizedVariable("A[B]", "", 2), True)
    self.assertIn("A[B]", m.temporalVarNames())

    # atemporal "X[0]" conflicts with temporal "X" at slice 0
    with self.assertRaises(gum.InvalidArgument):
      m.add(gum.LabelizedVariable("X[0]", "", 2), False)

    # a bracket-named atemporal variable is allowed when no temporal process
    # shares its decoded base
    m.add(gum.LabelizedVariable("Z[1]", "", 2), False)
    self.assertIn("Z[1]", m.atemporalVarNames())
    # ... but adding temporal "Z" now collides with the existing "Z[1]" node
    with self.assertRaises(gum.InvalidArgument):
      m.add(gum.LabelizedVariable("Z", "", 2), True)

    # temporal and atemporal may NOT share the same base name
    with self.assertRaises(gum.DuplicateLabel):
      m.add(gum.LabelizedVariable("X", "", 2), False)
    self.assertNotIn("X", m.atemporalVarNames())
    self.assertIn("X", m.temporalVarNames())

    with self.assertRaises(gum.OutOfBounds):
      m.variable("X", 5)
    with self.assertRaises(gum.OperationNotAllowed):
      m.variable("X", AT)
    with self.assertRaises(gum.OperationNotAllowed):
      m.variable("C", 0)
    with self.assertRaises(gum.NotFound):
      m.variable("Z", 0)

  def testTemporalBracketBaseName(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X[0]", "", 2), True)

    self.assertIn("X[0]", m.temporalVarNames())
    self.assertEqual(m.nbTemporalVars(), 1)
    self.assertEqual(m.size(), 2)  # slice nodes "X[0][0]" and "X[0][1]"

    m.variable("X[0]", 0)
    m.variable("X[0]", 1)

    self.assertEqual(m.baseName(m.variable("X[0]", 1)), "X[0]")
    self.assertEqual(m.timeSlice(m.variable("X[0]", 1)), 1)

  def testFastDescription(self):
    m = gum.KTBN(2)
    m.add("Z[3]", True)  # temporal Z, 3 modalities
    m.add("C[4]", False)  # atemporal C, 4 modalities

    self.assertIn("Z", m.temporalVarNames())
    self.assertIn("C", m.atemporalVarNames())
    self.assertEqual(m.variable("Z", 0).domainSize(), 3)
    self.assertEqual(m.variable("Z", 1).domainSize(), 3)
    self.assertEqual(m.variable("C").domainSize(), 4)

    m2 = gum.KTBN(2)
    m2.addTemporal("X[3]")
    m2.addAtemporal("C{yes|no}")
    m2.addTemporal("R")  # default 2 modalities

    self.assertIn("X", m2.temporalVarNames())
    self.assertIn("C", m2.atemporalVarNames())
    self.assertIn("R", m2.temporalVarNames())
    self.assertEqual(m2.variable("X", 0).domainSize(), 3)
    self.assertEqual(m2.variable("C").domainSize(), 2)
    self.assertEqual(m2.variable("R", 1).domainSize(), 2)

  def testErase(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("Y", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.addArc("X", 0, "X", 1)
    m.addArc("C", AT, "X", 1)

    m.erase("Y")
    self.assertNotIn("Y", m.temporalVarNames())
    self.assertEqual(m.nbTemporalVars(), 1)
    self.assertEqual(m.size(), 3)  # X[0], X[1], C

    m.add(gum.LabelizedVariable("Z", "", 2), True)
    m.erase("Z")
    self.assertNotIn("Z", m.temporalVarNames())

    m.erase("C")
    self.assertNotIn("C", m.atemporalVarNames())
    self.assertEqual(m.nbAtemporalVars(), 0)
    self.assertEqual(m.sizeArcs(), 1)  # C->X[1] removed with C; X[0]->X[1] remains

    with self.assertRaises(gum.NotFound):
      m.erase("W")

  def testChangeVariableName(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.addArc("X", 0, "X", 1)
    m.addArc("C", AT, "X", 1)

    x1values = [0.9, 0.1, 0.2, 0.8, 0.6, 0.4, 0.3, 0.7]
    m.cpt("X", 1).fillWith(x1values)

    m.changeVariableName("X", "W")
    self.assertEqual(_flatCPT(m.cpt("W", 1)), x1values)
    self.assertIn("W", m.temporalVarNames())
    self.assertNotIn("X", m.temporalVarNames())
    self.assertFalse(m.exists("X"))
    m.variable("W", 0)
    m.variable("W", 1)
    self.assertTrue(m.existsArc("W", 0, "W", 1))
    self.assertTrue(m.existsArc("C", AT, "W", 1))

    m.changeVariableName("W", "V")
    self.assertIn("V", m.temporalVarNames())
    self.assertNotIn("W", m.temporalVarNames())

    m.changeVariableName("C", "D")
    self.assertIn("D", m.atemporalVarNames())
    self.assertNotIn("C", m.atemporalVarNames())
    self.assertTrue(m.existsArc("D", AT, "V", 1))

    m.changeVariableName("D", "D")  # no-op: rename to same name

    with self.assertRaises(gum.InvalidArgument):
      m.changeVariableName("D", "V[0]")  # atemporal -> bracket notation forbidden
    with self.assertRaises(gum.DuplicateLabel):
      m.changeVariableName("V", "D")  # temporal/atemporal may not share a name

    m.add(gum.LabelizedVariable("P", "", 2), True)
    with self.assertRaises(gum.DuplicateLabel):
      m.changeVariableName("P", "D")
    with self.assertRaises(gum.InvalidArgument):
      m.changeVariableName("P", "")
    with self.assertRaises(gum.NotFound):
      m.changeVariableName("Z", "W")

  def testParentsChildren(self):
    m = gum.KTBN(3)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("Y", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.addArc("X", 0, "X", 2)
    m.addArc("X", 1, "X", 2)
    m.addArc("C", AT, "X", 2)
    m.addArc("X", 2, "Y", 2)

    pX2 = m.parents("X", 2)
    self.assertEqual(len(pX2), 3)  # X@0, X@1, C
    self.assertEqual(m.parents("X", 0), ())

    cX2 = m.children("X", 2)
    self.assertEqual(len(cX2), 1)
    self.assertEqual(cX2[0], ("Y", 2))

    cC = m.children("C")
    self.assertEqual(len(cC), 1)
    self.assertEqual(cC[0][0], "X")

  def testBracketNotationVariable(self):
    m = gum.KTBN(3)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 4), False)
    m.add(gum.LabelizedVariable("Y[0]", "", 3), False)  # bracket-named atemporal

    self.assertEqual(m.variable("Y[0]").domainSize(), 3)
    self.assertEqual(m.variable("X[1]").domainSize(), 2)
    self.assertEqual(m.variable("C").domainSize(), 4)

    with self.assertRaises(gum.NotFound):
      m.variable("Z[0]")


ts = unittest.TestSuite()
addTests(ts, KTBNVariableTestCase)
