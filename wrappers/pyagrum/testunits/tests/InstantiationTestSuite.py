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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestInstantiation(pyAgrumTestCase):
  def testSetterAndGetter(self):
    bn = gum.fastBN("a{chaud|tiede|froid}->b[5]<-c->d->e;c->e")
    i = bn.completeInstantiation()
    self.assertEqual(str(i), "<a:chaud|b:0|c:0|d:0|e:0>")
    i["c"] = 1
    self.assertEqual(str(i), "<a:chaud|b:0|c:1|d:0|e:0>")
    i['e'] = i["c"]
    self.assertEqual(str(i), "<a:chaud|b:0|c:1|d:0|e:1>")
    i['a'] = 1
    self.assertEqual(str(i), "<a:tiede|b:0|c:1|d:0|e:1>")
    i['a'] = 'froid'
    self.assertEqual(str(i), "<a:froid|b:0|c:1|d:0|e:1>")

    with self.assertRaises(gum.OutOfBounds):
      i['e'] = 1000

    with self.assertRaises(gum.NotFound):
      i['z'] = 0

  def testFromDictAndToDict(self):
    bn = gum.fastBN("a{chaud|tiede|froid}->b[5]<-c->d->e;c->e")
    i = bn.completeInstantiation()
    self.assertEqual(i.todict(withLabels=False), {'a': 0, 'b': 0, 'c': 0, 'd': 0, 'e': 0})
    self.assertEqual(i.todict(withLabels=True), {'a': 'chaud', 'b': '0', 'c': '0', 'd': '0', 'e': '0'})
    self.assertEqual(i.todict(), {'a': 0, 'b': 0, 'c': 0, 'd': 0, 'e': 0})
    i.fromdict({'a': 'tiede', 'bar': 'foo'})
    self.assertEqual(i.todict(withLabels=False), {'a': 1, 'b': 0, 'c': 0, 'd': 0, 'e': 0})
    self.assertEqual(i.todict(withLabels=True), {'a': 'tiede', 'b': '0', 'c': '0', 'd': '0', 'e': '0'})
    self.assertEqual(i.todict(), {'a': 1, 'b': 0, 'c': 0, 'd': 0, 'e': 0})

  def testOperatorEqual(self):
    bn = gum.fastBN("a{chaud|tiede|froid}->b[5]<-c->d->e;c->e")
    i = bn.completeInstantiation()
    j = bn.completeInstantiation()
    self.assertEqual(i, j)
    while not i.end():
      self.assertEqual(i, j)
      i.inc()
      self.assertNotEqual(i, j)
      j.inc()

    k = gum.Instantiation()
    self.assertNotEqual(i, k)  # not the same size
    k.inc()
    self.assertEqual(i, k)  # both are in overflow => equals

  def testChainingAdd(self):
    bn = gum.fastBN("A->B")
    i = bn.completeInstantiation()
    m1 = gum.Instantiation().add(bn.variable("A")).add(bn.variable("B"))
    m2 = gum.Instantiation().add(bn.variable("B")).add(bn.variable("A"))

    self.assertEqual(i, m1)
    self.assertEqual(i, m2)
    m1.inc()
    self.assertNotEqual(i, m1)

    i = gum.Instantiation().addVarsFromModel(bn, ["A", "B"])
    m1 = gum.Instantiation().add(bn.variable("A")).add(bn.variable("B"))
    m2 = gum.Instantiation().add(bn.variable("B")).add(bn.variable("A"))

    self.assertEqual(i, m1)
    self.assertEqual(i, m2)
    m1.inc()
    self.assertNotEqual(i, m1)

    i = gum.Instantiation().addVarsFromModel(bn, ["B", "A"])
    m1 = gum.Instantiation().add(bn.variable("A")).add(bn.variable("B"))
    m2 = gum.Instantiation().add(bn.variable("B")).add(bn.variable("A"))

    self.assertEqual(i, m1)
    self.assertEqual(i, m2)
    m1.inc()
    self.assertNotEqual(i, m1)

  def testLoopIn(self):
    bn = gum.fastBN("a{chaud|tiede|froid}->b[5]<-c->d->e;c->e")
    for p, i in enumerate(bn.completeInstantiation().loopIn()):
      if p == 0:
        self.assertEqual(str(i), "<a:chaud|b:0|c:0|d:0|e:0>")
        if p == 63:
          self.assertEqual(str(i), "<a:chaud|b:1|c:0|d:0|e:1>")
        if p == 104:
          self.assertEqual(str(i), "<a:froid|b:4|c:0|d:1|e:1>")
    self.assertEqual(p, 119)
    self.assertEqual(str(i), "<a:froid|b:4|c:1|d:1|e:1>")

ts = unittest.TestSuite()
addTests(ts, TestInstantiation)
