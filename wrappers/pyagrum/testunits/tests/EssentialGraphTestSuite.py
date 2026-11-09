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


class TestEssentialGraph(pyAgrumTestCase):
  def testChain(self):
    bn = gum.fastBN("a->b->c")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 3)
    self.assertEqual(eg.sizeArcs(), 0)
    self.assertEqual(eg.sizeEdges(), 2)
    self.assertEqual(len(eg.nodes()), eg.size())

  def testVstructure(self):
    bn = gum.fastBN("a->b;c->b")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 3)
    self.assertEqual(eg.sizeArcs(), 2)
    self.assertEqual(eg.sizeEdges(), 0)

  def testCaseD(self):
    bn = gum.fastBN("a->b;c1->b;c2->b;a->c1;a->c2")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 4)
    self.assertEqual(eg.sizeArcs(), 3)
    self.assertEqual(eg.sizeEdges(), 2)

  def testNotebook1(self):
    bn = gum.fastBN("A->B->C->D;E->B;F->G->D;F->H->I;E->J->K->I->M;K->L")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 13)
    self.assertEqual(eg.sizeArcs(), 8)
    self.assertEqual(eg.sizeEdges(), 5)

  def testNotebook2(self):
    bn = gum.fastBN("A->B;C->B;C->D;B->D;A->C")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 4)
    self.assertEqual(eg.sizeArcs(), 0)
    self.assertEqual(eg.sizeEdges(), 5)

  def testNotebook3(self):
    bn = gum.fastBN("Z->X->U;Y->X;Y->W")
    eg = gum.EssentialGraph(bn)

    self.assertEqual(eg.size(), 5)
    self.assertEqual(eg.sizeArcs(), 3)
    self.assertEqual(eg.sizeEdges(), 1)

    self.assertEqual(eg.idFromName("Z"), 0)
    self.assertEqual(eg.idFromName("X"), 1)
    self.assertEqual(eg.idFromName("U"), 2)
    self.assertEqual(eg.idFromName("Y"), 3)
    self.assertEqual(eg.idFromName("W"), 4)

    self.assertEqual(eg.nameFromId(0), "Z")
    self.assertEqual(eg.nameFromId(1), "X")
    self.assertEqual(eg.nameFromId(2), "U")
    self.assertEqual(eg.nameFromId(3), "Y")
    self.assertEqual(eg.nameFromId(4), "W")

  def testConnectedComponents(self):
    # BN with two disconnected components: A->B and C->D
    bn = gum.fastBN("A->B;C->D")
    eg = gum.EssentialGraph(bn)
    self.assertEqual(eg.connectedComponentsCount(), 2)
    ccl = eg.connectedComponentsList()
    self.assertEqual(len(ccl), 2)
    sizes = sorted(len(s) for s in ccl.values())
    self.assertEqual(sizes, [2, 2])

    # All three methods are consistent
    cc = eg.connectedComponents()
    self.assertEqual(len(set(cc.values())), eg.connectedComponentsCount())

    # Fully connected BN: one component
    bn2 = gum.fastBN("A->B->C->D")
    eg2 = gum.EssentialGraph(bn2)
    self.assertEqual(eg2.connectedComponentsCount(), 1)

  def testDoesNotDependOnTheModel(self):
    eg = gum.EssentialGraph(gum.fastBN("a->b->c;d->c"))  # the BN is not kept alive
    self.assertEqual(eg.nameFromId(eg.idFromName("d")), "d")
    self.assertIsNone(eg.idFromName("z"))
    self.assertEqual(gum.EssentialGraph(eg.pdag()).idFromName("a"), eg.idFromName("a"))

  def testNotACPDAGIsRejected(self):
    bn = gum.fastBN("a->b->c;d->c")
    with self.assertRaises(gum.InvalidArgument):
      gum.EssentialGraph(bn, gum.PDAG(bn.dag()))  # a->b is not compelled: not a CPDAG
    with self.assertRaises(gum.InvalidArgument):
      gum.EssentialGraph(gum.PDAG(bn.dag()))
    eg = gum.EssentialGraph(bn, gum.EssentialGraph(bn).pdag())  # a CPDAG
    self.assertEqual(eg.sizeArcs(), 2)
    self.assertEqual(gum.EssentialGraph(eg).sizeArcs(), 2)

  def testIsACPDAG(self):
    bn = gum.fastBN("a->b->c;d->c")
    eg = gum.EssentialGraph(bn)
    self.assertIsInstance(eg, gum.CPDAG)
    self.assertIn("EssentialGraph", repr(eg))
    a, c, d = (bn.idFromName(n) for n in "acd")
    self.assertTrue(eg.dSeparation(a, d))
    self.assertFalse(eg.dSeparation(a, d, c))
    self.assertTrue(eg.contains(eg.consistentExtension()))
    self.assertEqual(eg.parents(c), {bn.idFromName("b"), d})
    self.assertEqual(eg.parents({c}), {bn.idFromName("b"), d})
    self.assertIn('subgraph "cluster_', gum.EssentialGraph(gum.fastBN("a->b")).toDot())
    self.assertTrue(gum.EssentialGraph().empty())
    self.assertEqual(gum.EssentialGraph(gum.CPDAG.fromDAG(bn.dag())), eg)

  def testCPDAG(self):
    bn = gum.fastBN("a->b->c;d->c")
    cp = gum.EssentialGraph(bn).cpdag()
    self.assertIsInstance(cp, gum.CPDAG)
    self.assertTrue(cp.contains(bn.dag()))
    self.assertEqual(cp.nameFromId(bn.idFromName("d")), "d")


ts = unittest.TestSuite()
addTests(ts, TestEssentialGraph)
