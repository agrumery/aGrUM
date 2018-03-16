# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestInstantiation(pyAgrumTestCase):
  def testSetterAndGetter(self):
    bn = gum.fastBN("a{chaud|tiede|froid}->b[5]<-c->d->e;c->e");
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
    bn = gum.fastBN("a{chaud|tiede|froid}->b[5]<-c->d->e;c->e");
    i = bn.completeInstantiation()
    self.assertEqual(i.todict(), {'a': 0, 'b': 0, 'c': 0, 'd': 0, 'e': 0})
    self.assertEqual(i.todict(withLabels=True), {'a': 'chaud', 'b': '0', 'c': '0', 'd': '0', 'e': '0'})
    i.fromdict({'a': 'tiede', 'bar': 'foo'})
    self.assertEqual(i.todict(), {'a': 1, 'b': 0, 'c': 0, 'd': 0, 'e': 0})


ts = unittest.TestSuite()
addTests(ts, TestInstantiation)
