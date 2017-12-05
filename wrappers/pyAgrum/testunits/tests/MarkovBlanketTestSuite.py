# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestMarkovBlanket(pyAgrumTestCase):
  def testChain(self):
    bn = gum.fastBN("a->b->c")
    eg = gum.MarkovBlanket(bn, "a")
    eg = gum.MarkovBlanket(bn, 1)

  def testMarkovBlanketSpecialArcs(self):
    bn = gum.fastBN("aa->bb->cc->dd->ee;ff->dd->gg;hh->ii->gg;ff->ii;ff->gg")
    mb = gum.fastBN("cc->dd->ee;ff->dd->gg;ff->gg;ff->ii->gg")
    self.assertTrue(gum.MarkovBlanket(bn, "dd").hasSameStructure(mb))

  def testMarkovBlanketStructure(self):
    bn = gum.fastBN("a->b->c->d->e;f->d->g;h->i->g");
    self.assertFalse(gum.MarkovBlanket(bn, "a").hasSameStructure(
        gum.fastBN("b->a")))

    self.assertTrue(gum.MarkovBlanket(bn, "a").hasSameStructure(
        gum.fastBN("a->b")))
    self.assertTrue(gum.MarkovBlanket(bn, "b").hasSameStructure(
        gum.fastBN("a->b->c")))
    self.assertTrue(gum.MarkovBlanket(bn, "c").hasSameStructure(
        gum.fastBN("b->c->d;f->d")))
    self.assertTrue(gum.MarkovBlanket(bn, "d").hasSameStructure(
        gum.fastBN("c->d->e;f->d->g;i->g")))
    self.assertTrue(gum.MarkovBlanket(bn, "e").hasSameStructure(
        gum.fastBN("d->e")))
    self.assertTrue(gum.MarkovBlanket(bn, "f").hasSameStructure(
        gum.fastBN("c->d;f->d;")))
    self.assertTrue(gum.MarkovBlanket(bn, "g").hasSameStructure(
        gum.fastBN("d->g;i->g;")))
    self.assertTrue(gum.MarkovBlanket(bn, "h").hasSameStructure(
        gum.fastBN("h->i;")))
    self.assertTrue(gum.MarkovBlanket(bn, "i").hasSameStructure(
        gum.fastBN("d->g;h->i->g;;")))


ts = unittest.TestSuite()
addTests(ts, TestMarkovBlanket)
