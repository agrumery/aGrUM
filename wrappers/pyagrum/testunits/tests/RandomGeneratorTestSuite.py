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

import pyagrum as gum

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class RandomGeneratorTestCase(pyAgrumTestCase):
  # each test changing the seed is run in a RandomGeneratorGuard: the next tests are driven by the displayed seed

  def testInitSeed0(self):
    with gum.RandomGeneratorGuard():
      gum.initRandom(0)
      bn1 = gum.fastBN("A->B->C")
      gum.initRandom(0)
      bn2 = gum.fastBN("A->B->C")
    self.assertNotEqual(bn1.cpt("B").min(), bn2.cpt("B").min())  # may fail but highly unlikely

  def testInitSeed1(self):
    with gum.RandomGeneratorGuard():
      gum.initRandom(1)
      bn1 = gum.fastBN("A->B->C")
      gum.initRandom(1)
      bn2 = gum.fastBN("A->B->C")
    self.assertEqual(bn1.cpt("B").min(), bn2.cpt("B").min())

  def testInitSeed42(self):
    with gum.RandomGeneratorGuard():
      gum.initRandom(42)
      bn1 = gum.fastBN("A->B->C")
      gum.initRandom(42)
      bn2 = gum.fastBN("A->B->C")
    self.assertEqual(bn1.cpt("B").min(), bn2.cpt("B").min())

  def testGuardRestoresTheStream(self):
    seed = gum.randomGeneratorSeed()
    with gum.RandomGeneratorGuard():
      expected = [gum.randomProba() for _ in range(5)]
    guard = gum.RandomGeneratorGuard()
    gum.initRandom(42)
    _ = [gum.randomProba() for _ in range(10)]
    self.assertEqual(gum.randomGeneratorSeed(), 42)
    self.assertFalse(guard.isRestored())
    guard.restore()
    self.assertTrue(guard.isRestored())
    self.assertEqual(gum.randomGeneratorSeed(), seed)
    self.assertEqual([gum.randomProba() for _ in range(5)], expected)
    # only the first call restores: a second call does not go back before the 5 draws
    guard.restore()
    self.assertNotEqual(gum.randomProba(), expected[0])

  def testGuardRestoresOnException(self):
    seed = gum.randomGeneratorSeed()
    with gum.RandomGeneratorGuard():
      expected = gum.randomProba()
    with self.assertRaises(ValueError), gum.RandomGeneratorGuard() as guard:
      gum.initRandom(7)
      raise ValueError("leaving the block")  # not swallowed by the guard
    self.assertTrue(guard.isRestored())
    self.assertEqual(gum.randomGeneratorSeed(), seed)
    self.assertEqual(gum.randomProba(), expected)


ts = unittest.TestSuite()
addTests(ts, RandomGeneratorTestCase)
