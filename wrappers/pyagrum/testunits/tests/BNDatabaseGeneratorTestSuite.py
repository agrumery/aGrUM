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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
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

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class GenerateCSVTestCase(pyAgrumTestCase):
  def testSimpleUse(self):
    bn = gum.fastBN("A->B->C;A->D->C;D->E;")
    gum.generateSample(bn, 30, self.agrumSrcDir("genere.csv"))


class BNDatabaseGeneratorTestCase(pyAgrumTestCase):
  def testConstructor(self):
    bn = gum.fastBN("A->B->C;A->D->C;D->E;")
    _ = gum.BNDatabaseGenerator(bn)

  def testSetVarOrder(self):
    bn = gum.loadBN(self.agrumSrcDir("survey.bif"))
    dbgen = gum.BNDatabaseGenerator(bn)

    dbgen.setVarOrderFromCSV(self.agrumSrcDir("survey1.csv"))
    self.assertEqual(dbgen.varOrderNames(), ("E", "A", "O", "T", "R", "S"))

    dbgen.setVarOrder(["A", "E", "O", "R", "S", "T"])
    self.assertEqual(dbgen.varOrderNames(), ("A", "E", "O", "R", "S", "T"))

    with self.assertRaises(gum.FatalError):
      dbgen.setVarOrder(["A", "E", "O", "R", "A", "S", "T"])

    with self.assertRaises(gum.FatalError):
      dbgen.setVarOrder(["A", "O", "R", "S", "T"])

    with self.assertRaises(gum.NotFound):
      dbgen.setVarOrder(["A", "O", "R", "S", "T", "X"])

  def testDrawSamples(self):
    bn = gum.fastBN("A->B[4]->C;A->D->C;D->E[3];")
    dbgen = gum.BNDatabaseGenerator(bn)

    nbsample = 100
    nbsamples = [nbsample * i for i in [1, 100, 1000]]

    ns1, ns2, ns3 = nbsamples

    isOK = False
    for i in range(self.nbLoopForApproximatedTest):
      try:
        ll1, ll2, ll3 = [dbgen.drawSamples(n) for n in nbsamples]

        self.assertAlmostEqual(ns1 / ns2, ll1 / ll2, delta=0.1)
        self.assertAlmostEqual(ns3 / ns2, ll3 / ll2, delta=0.1)
        self.assertAlmostEqual(ns1 / ns3, ll1 / ll3, delta=0.1)
        isOK = True
        break
      except AssertionError:
        pass
    self.assertEqual(isOK, True, " Error in loop for Approximated tests")

    jointe = gum.Tensor().fillWith(1)
    for i in bn.nodes():
      jointe *= bn.cpt(i)
    entropy = jointe.entropy()

    self.assertAlmostEqual(entropy, -ll1 / ns1, delta=0.5)
    self.assertAlmostEqual(entropy, -ll2 / ns2, delta=0.2)
    self.assertAlmostEqual(entropy, -ll3 / ns3, delta=0.1)


ts = unittest.TestSuite()
addTests(ts, GenerateCSVTestCase)
addTests(ts, BNDatabaseGeneratorTestCase)
