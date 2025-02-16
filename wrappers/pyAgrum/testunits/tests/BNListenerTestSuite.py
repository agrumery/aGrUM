############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
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
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
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

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


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
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize("ajout d'un noeud %s (%d)\n" % (s, i)),
                                 whenArcAdded=lambda i, j: self.bufferize("ajout d'un arc (%d->%d)\n" % (i, j)),
                                 whenNodeDeleted=lambda i: self.bufferize("suppression du noeud %d\n" % i),
                                 whenArcDeleted=lambda i, j: self.bufferize("suppression de l'arc (%d->%d)\n" % (i, j)))

  def testMessagesChange(self):
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize("ajout d'un noeud %s (%d)\n" % (s, i)))
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize("noeud++ %s (%d) ... OK\n" % (s, i)))


class TestListeners(PythonBNListenerTestCase):
  def test1Listener(self):
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize("Noeud++ %s (%d)\n" % (s, i)),
                                 whenArcAdded=lambda i, j: self.bufferize("Arc++ (%d->%d)\n" % (i, j)),
                                 whenNodeDeleted=lambda i: self.bufferize("Noeud-- %d\n" % i),
                                 whenArcDeleted=lambda i, j: self.bufferize("Arc-- (%d->%d)\n" % (i, j)))

    a = self.bn.add(self.var1)
    self.assertEqual(self.buffer0, "Noeud++ a (%d)\n" % a)
    b = self.bn.add(self.var2)
    self.assertEqual(self.buffer0, "Noeud++ b (%d)\n" % b)
    c = self.bn.add(self.var3)
    self.assertEqual(self.buffer0, "Noeud++ c (%d)\n" % c)

    self.bn.addArc(a, b)
    self.assertEqual(self.buffer0, "Arc++ (%d->%d)\n" % (a, b))
    self.bn.addArc(b, c)
    self.assertEqual(self.buffer0, "Arc++ (%d->%d)\n" % (b, c))

    self.assertRaises(gum.InvalidDirectedCycle, self.bn.addArc, c, a)

    self.buffer1 = ""
    self.bn.erase(b)
    self.assertEqual(self.buffer1,
                     "Arc-- (%d->%d)\nArc-- (%d->%d)\nNoeud-- %d\n" % (a, b, b, c, b))

  def test2Listeners(self):
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize("Node++ %s (%d)\n" % (s, i)))
    self.bn.addStructureListener(whenNodeAdded=lambda i, s: self.bufferize("Noeud++ %s (%d)\n" % (s, i)))

    self.buffer1 = ""
    a = self.bn.add(self.var1)
    self.assertTrue("Node++ a (%d)\n" % a in self.buffer1)
    self.assertTrue("Noeud++ a (%d)\n" % a in self.buffer1)


ts = unittest.TestSuite()
addTests(ts, TestMessagesSetting)
addTests(ts, TestListeners)
