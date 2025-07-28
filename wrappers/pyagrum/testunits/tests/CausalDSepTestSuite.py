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
from typing import Set, List

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests
import pyagrum.causal as csl


class TestDSep(pyAgrumTestCase):
  #  A   B
  #   \ / \
  # H  C   D
  #  \ |   |
  #    E   |
  #     \ /
  #      F
  #      |
  #      G

  def setUp(self):
    self.bn = gum.fastBN("A->C->E->F->G;F<-D<-B->C;H->E")
    self.iA, self.iB, self.iC, self.iD, self.iE, self.iF, self.iG, self.iH = [self.bn.idFromName(s) for s in "ABCDEFGH"]
    self.tous = {self.iA, self.iB, self.iC, self.iD, self.iE, self.iF, self.iG, self.iH}

  def test_minimalCondSet4A(self):
    r = self.bn.minimalCondSet(self.iA, self.tous)
    self.assertEqual(r, {self.iA})

    r = self.bn.minimalCondSet(self.iA, self.tous - {self.iA})
    self.assertEqual(r, {self.iB, self.iC})

    r = self.bn.minimalCondSet(self.iA, {self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iE, self.iF})

    r = self.bn.minimalCondSet(self.iA, {self.iB, self.iC, self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iB, self.iC})

    r = self.bn.minimalCondSet(self.iA, {self.iC, self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iC, self.iE, self.iF})

  def test_minimalCondSet4C(self):
    r = self.bn.minimalCondSet(self.iC, self.tous)
    self.assertEqual(r, {self.iC})

    r = self.bn.minimalCondSet(self.iC, self.tous - {self.iC})
    self.assertEqual(r, {self.iA, self.iB, self.iH, self.iE})

    r = self.bn.minimalCondSet(self.iC, {self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iE, self.iF})

    r = self.bn.minimalCondSet(self.iC, {self.iB, self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iE, self.iB})

    r = self.bn.minimalCondSet(self.iC, {self.iC, self.iE, self.iF, self.iG})
    self.assertEqual(r, {self.iC})

  def test_minimaCondSetForJoint(self):
    r = self.bn.minimalCondSet({self.iA, self.iB}, self.tous)
    self.assertEqual(r, {self.iA, self.iB})
    r = self.bn.minimalCondSet({self.iA, self.iB}, self.tous - {self.iA, self.iB})
    self.assertEqual(r, {self.iC, self.iD})
    r = self.bn.minimalCondSet({self.iC, self.iE}, self.tous - {self.iC, self.iE})
    self.assertEqual(r, self.tous - {self.iC, self.iE, self.iG})


class TestBackDoors(pyAgrumTestCase):
  def getBackDoors(self, fbn: str, cause: int, effect: int, latent: Set[int] = None):
    bn = gum.fastBN(fbn)
    return [
      [bn.variable(i).name() for i in bd]
      for bd in csl.backdoor_generator(bn.dag(), bn.idFromName(cause), bn.idFromName(effect), latent)
    ]

  def hasBackDoor(self, fbn: str, cause: int, effect: int, latent: Set[int] = None):
    if self.verbose:
      self.log.warning(f"{cause} to {effect} ?".format(cause, effect))
    res = self.getBackDoors(fbn, cause, effect, latent)
    if len(res) == 0:
      if self.verbose:
        self.log.warning("error : no backdoor found")
        return False
    else:
      if self.verbose:
        self.log.warning("OK")
      return True

  def hasNoBackDoor(self, fbn: str, cause: int, effect: int, latent: Set[int] = None) -> object:
    if self.verbose:
      self.log.warning(f"{cause} to {effect} ?".format(cause, effect))
    res = self.getBackDoors(fbn, cause, effect, latent)
    if len(res) > 0:
      if self.verbose:
        self.log.warning(f"error : backdoors={res}")
      return False
    else:
      if self.verbose:
        self.log.warning("OK")
      return True

  def hasAllBackDoors(self, awaited: List[List[str]], fbn: str, cause: int, effect: int, latent: Set[int] = None):
    if self.verbose:
      self.log.warning(f"{cause} to {effect} ?".format(cause, effect))
    res = self.getBackDoors(fbn, cause, effect, latent)
    if self.verbose:
      self.log.warning(res)
    if len(res) != len(awaited):
      return False

    for bd in res:
      if bd not in awaited:
        self.log.warning(f"ERROR : {res}!={awaited}")
        return False
    return True

  def test_backdoors(self):
    self.verbose = False
    self.assertTrue(self.hasNoBackDoor("A->B->C", "A", "C"))
    self.assertTrue(self.hasBackDoor("A->B->C", "C", "A"))

    self.assertTrue(self.hasNoBackDoor("N0<-N1->N2;N0<-N3->N2;N0<-N4->N2;N2->N0;N1->N4", "N1", "N0"))
    self.assertTrue(
      self.hasAllBackDoors(
        [["N5"], ["N6"]],
        "N0[1,3]<-N1->N2[1,4];N0<-N3[0,3]->N2;N0<-N4[1,4]->N2;N2->N0;N1->N4;N1<-N5->N6->N0",
        "N1",
        "N0",
      )
    )

    self.assertTrue(
      self.hasNoBackDoor(
        "N0[1,3]<-N1->N2[1,4];N0<-N3[0,3]->N2;N0<-N4[1,4]->N2;N2->N0;N1->N4;N1<-N5->N6<-N0", "N1", "N0"
      )
    )
    self.assertTrue(self.hasBackDoor("Xi<-X3<-X1->X4<-X2->X5->Xj<-X6<-Xi<-X4->Xj", "Xi", "Xj"))

    self.assertTrue(
      self.hasAllBackDoors(
        [["X3", "X4"], ["X1", "X4"], ["X4", "X2"], ["X4", "X5"]],
        "Xi<-X3<-X1->X4<-X2->X5->Xj<-X6<-Xi<-X4->Xj",
        "Xi",
        "Xj",
      )
    )


class TestFrontDoors(pyAgrumTestCase):
  def getFrontDoors(self, fbn: str, cause: int, effect: int, latent: Set[int] = None):
    bn = gum.fastBN(fbn)
    return [
      [bn.variable(i).name() for i in bd]
      for bd in csl.frontdoor_generator(bn.dag(), bn.idFromName(cause), bn.idFromName(effect), latent)
    ]

  def hasFrontDoor(self, fbn: str, cause: int, effect: int, latent: Set[int] = None):
    if self.verbose:
      self.log.warning(f"{cause} to {effect} ?".format(cause, effect))
    res = self.getFrontDoors(fbn, cause, effect, latent)
    if len(res) == 0:
      if self.verbose:
        self.log.warning("error : no backdoor found")
        return False
    else:
      if self.verbose:
        self.log.warning("OK")
      return True

  def hasNoFrontDoor(self, fbn: str, cause: int, effect: int, latent: Set[int] = None) -> object:
    if self.verbose:
      self.log.warning(f"{cause} to {effect} ?".format(cause, effect))
    res = self.getFrontDoors(fbn, cause, effect, latent)
    if len(res) > 0:
      if self.verbose:
        self.log.warning(f"error : frontdoors={res}")
      return False
    else:
      if self.verbose:
        self.log.warning("OK")
      return True

  def hasAllFrontDoors(self, awaited: List[List[str]], fbn: str, cause: int, effect: int, latent: Set[int] = None):
    if self.verbose:
      self.log.warning(f"{cause} to {effect} ?".format(cause, effect))
    res = self.getFrontDoors(fbn, cause, effect, latent)
    if self.verbose:
      self.log.warning(res)
    if len(res) != len(awaited):
      return False

    for bd in res:
      if bd not in awaited:
        self.log.warning(f"ERROR : {res}!={awaited}")
        return False
    return True

  def testFrontDoorYann(self):
    self.verbose = False
    self.assertTrue(self.hasNoFrontDoor("B->C;D", "C", "B"))


ts = unittest.TestSuite()
addTests(ts, TestDSep)
addTests(ts, TestBackDoors)
addTests(ts, TestFrontDoors)
