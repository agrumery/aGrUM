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

import os
import sys
import tempfile
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


class TestOtherModelsListeners(pyAgrumTestCase):
  def setUp(self):
    self.events = []

  def testInfluenceDiagram(self):
    diag = gum.InfluenceDiagram()
    diag.addStructureListener(
      whenNodeAdded=lambda i, s: self.events.append(("node+", i, s)),
      whenNodeDeleted=lambda i: self.events.append(("node-", i)),
      whenArcAdded=lambda i, j: self.events.append(("arc+", i, j)),
      whenArcDeleted=lambda i, j: self.events.append(("arc-", i, j)),
    )
    c = diag.addChanceNode(gum.LabelizedVariable("c", "c", 2))
    d = diag.addDecisionNode(gum.LabelizedVariable("d", "d", 2))
    u = diag.addUtilityNode(gum.LabelizedVariable("u", "u", 1))
    diag.addArc(c, u)
    diag.addArc(d, u)
    diag.eraseArc(c, u)
    diag.erase(d)
    self.assertEqual(
      self.events,
      [
        ("node+", c, "c"),
        ("node+", d, "d"),
        ("node+", u, "u"),
        ("arc+", c, u),
        ("arc+", d, u),
        ("arc-", c, u),
        ("arc-", d, u),
        ("node-", d),
      ],
    )

  def testMarkovRandomField(self):
    mrf = gum.MarkovRandomField()
    mrf.addStructureListener(
      whenNodeAdded=lambda i, s: self.events.append(("node+", i, s)),
      whenNodeDeleted=lambda i: self.events.append(("node-", i)),
      whenEdgeAdded=lambda i, j: self.events.append(("edge+", min(i, j), max(i, j))),
      whenEdgeDeleted=lambda i, j: self.events.append(("edge-", min(i, j), max(i, j))),
    )
    a, b, c = (mrf.add(name, 2) for name in "abc")
    mrf.addFactor(["a", "b"])
    # the edge a-b is not signaled again when another factor is added
    mrf.addFactor(["b", "c"])
    mrf.eraseFactor(["a", "b"])
    mrf.erase("c")
    self.assertEqual(
      self.events,
      [
        ("node+", a, "a"),
        ("node+", b, "b"),
        ("node+", c, "c"),
        ("edge+", a, b),
        ("edge+", b, c),
        ("edge-", a, b),
        ("edge-", b, c),
        ("node-", c),
      ],
    )

  def testNoListenerIsANoOp(self):
    for model in (gum.InfluenceDiagram(), gum.MarkovRandomField()):
      model.addStructureListener()
      self.assertFalse(hasattr(model, "_listeners"))


class TestListenerCallbacks(pyAgrumTestCase):
  # an exception raised by a callback cannot go through the C++ code which notified the listener: it is
  # reported by sys.unraisablehook and must not leave a Python exception set (SystemError afterwards)
  def setUp(self):
    self.reported = []
    self._hook = sys.unraisablehook
    sys.unraisablehook = lambda unraisable: self.reported.append(unraisable.exc_type)

  def tearDown(self):
    sys.unraisablehook = self._hook

  @staticmethod
  def _raise(*args):
    raise ValueError("raised by the callback")

  def testRaisingStructureCallback(self):
    bn = gum.BayesNet()
    bn.addStructureListener(whenNodeAdded=self._raise)
    bn.add(gum.LabelizedVariable("a", "a", 2))
    bn.add(gum.LabelizedVariable("b", "b", 2))
    self.assertEqual(bn.size(), 2)
    self.assertEqual(self.reported, [ValueError, ValueError])

  def testRaisingApproximationCallback(self):
    ie = gum.GibbsSampling(gum.fastBN("A->B->C"))
    # no stop on epsilon: the progress is notified every 100 iterations
    ie.setMaxIter(1000)
    ie.setEpsilon(1e-9)
    ie.setMinEpsilonRate(1e-12)
    ie.setBurnIn(10)
    ie.setVerbosity(True)
    messages = []
    listener = gum.PythonApproximationListener(ie._asIApproximationSchemeConfiguration())
    listener.setWhenProgress(self._raise)
    listener.setWhenStop(messages.append)
    ie.listener = listener
    ie.makeInference()
    self.assertEqual(messages, [ie.messageApproximationScheme()])
    self.assertGreater(len(self.reported), 0)
    self.assertTrue(all(exc is ValueError for exc in self.reported))

  def testNonCallableIsRefused(self):
    ie = gum.GibbsSampling(gum.fastBN("A->B"))
    listener = gum.PythonApproximationListener(ie._asIApproximationSchemeConfiguration())
    with self.assertRaises(gum.InvalidArgument):
      listener.setWhenStop(42)
    with self.assertRaises(gum.InvalidArgument):
      gum.BayesNet().addStructureListener(whenNodeAdded=42)

  def testLoadListenerKeepsNoReference(self):
    progress = []
    listener = progress.append
    before = sys.getrefcount(listener)
    bn = gum.loadBN(self.agrumSrcDir("alarm.bif"), listeners=[listener])
    self.assertEqual(bn.size(), 37)
    self.assertGreater(len(progress), 0)
    self.assertEqual(sys.getrefcount(listener), before)

    with tempfile.TemporaryDirectory() as tmp:
      mrfFile = os.path.join(tmp, "mrf.uai")
      gum.fastMRF("A--B;B--C").saveUAI(mrfFile)
      progress.clear()
      mrf = gum.loadMRF(mrfFile, listeners=[listener])
    self.assertEqual(mrf.size(), 3)
    self.assertGreater(len(progress), 0)
    self.assertEqual(sys.getrefcount(listener), before)

  def testNonCallableLoadListenerIsRefused(self):
    with self.assertRaises(gum.InvalidArgument):
      gum.loadBN(self.agrumSrcDir("alarm.bif"), listeners=[print, 42])
    with self.assertRaises(gum.InvalidArgument):
      gum.loadBN(self.agrumSrcDir("alarm.bif"), listeners=42)

  def testUnreadableSequenceOfLoadListenersIsRefused(self):
    class BadSequence:
      def __getitem__(self, i):
        return print

      def __len__(self):
        raise RuntimeError("no length")

    with self.assertRaises(gum.InvalidArgument):
      gum.loadBN(self.agrumSrcDir("alarm.bif"), listeners=BadSequence())
    # no Python error is left pending
    gum.loadBN(self.agrumSrcDir("alarm.bif"), listeners=[])


ts = unittest.TestSuite()
addTests(ts, TestMessagesSetting)
addTests(ts, TestListeners)
addTests(ts, TestOtherModelsListeners)
addTests(ts, TestListenerCallbacks)
