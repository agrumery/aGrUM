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


class PythonBNListenerTestCase(pyAgrumTestCase):
  def setUp(self):
    self.bn = gum.BayesNet()
    self.buffer0 = ""
    self.buffer1 = ""
    self.var1 = gum.LabelizedVariable("a", "le noeud a", 2)
    self.var2 = gum.LabelizedVariable("b", "le noeud b", 2)
    self.var3 = gum.LabelizedVariable("c", "le noeud c", 3)

  def bufferize(self, str):
    self.buffer0 = str
    self.buffer1 += str


class TestMessagesSetting(PythonBNListenerTestCase):
  def testMessagesInsertion(self):
    self.bn.addStructureListener(
      whenNodeAdded=lambda i, s: self.bufferize(f"ajout d'un noeud {s} ({i})\n"),
      whenArcAdded=lambda i, j: self.bufferize(f"ajout d'un arc ({i}->{j})\n"),
      whenNodeDeleted=lambda i: self.bufferize(f"suppression du noeud {i}\n"),
      whenArcDeleted=lambda i, j: self.bufferize(f"suppression de l'arc ({i}->{j})\n"),
    )

  def testMessagesChange(self):
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize(f"ajout d'un noeud {s} ({i})\n"))
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize(f"noeud++ {s} ({i}) ... OK\n"))


class TestListeners(PythonBNListenerTestCase):
  def test1Listener(self):
    self.bn.addStructureListener(
      whenNodeAdded=lambda i, s: self.bufferize(f"Noeud++ {s} ({i})\n"),
      whenArcAdded=lambda i, j: self.bufferize(f"Arc++ ({i}->{j})\n"),
      whenNodeDeleted=lambda i: self.bufferize(f"Noeud-- {i}\n"),
      whenArcDeleted=lambda i, j: self.bufferize(f"Arc-- ({i}->{j})\n"),
    )

    a = self.bn.add(self.var1)
    self.assertEqual(self.buffer0, f"Noeud++ a ({a})\n")
    b = self.bn.add(self.var2)
    self.assertEqual(self.buffer0, f"Noeud++ b ({b})\n")
    c = self.bn.add(self.var3)
    self.assertEqual(self.buffer0, f"Noeud++ c ({c})\n")

    self.bn.addArc(a, b)
    self.assertEqual(self.buffer0, f"Arc++ ({a}->{b})\n")
    self.bn.addArc(b, c)
    self.assertEqual(self.buffer0, f"Arc++ ({b}->{c})\n")

    self.assertRaises(gum.InvalidDirectedCycle, self.bn.addArc, c, a)

    self.buffer1 = ""
    self.bn.erase(b)
    self.assertEqual(self.buffer1, f"Arc-- ({a}->{b})\nArc-- ({b}->{c})\nNoeud-- {b}\n")

  def test2Listeners(self):
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize(f"Node++ {s} ({i})\n"))
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize(f"Noeud++ {s} ({i})\n"))

    self.buffer1 = ""
    a = self.bn.add(self.var1)
    self.assertTrue(f"Node++ a ({a})\n" in self.buffer1)
    self.assertTrue(f"Noeud++ a ({a})\n" in self.buffer1)


ts = unittest.TestSuite()
addTests(ts, TestMessagesSetting)
addTests(ts, TestListeners)
