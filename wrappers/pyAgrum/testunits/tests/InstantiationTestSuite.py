# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
# (pierre-henri.wuillemin@lip6.fr)
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.
# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!
import unittest

import pyAgrum as gum
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


ts = unittest.TestSuite()
addTests(ts, TestInstantiation)
