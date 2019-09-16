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

  def testMarkovBlanketMultiLevel(self):
    bn=gum.fastBN("Z<-A->B->C->D->E<-Y;X->G<-F<-C<-I<-H->W")
    self.assertEquals(gum.MarkovBlanket(bn, "C", 1).size(),5)
    self.assertEquals(gum.MarkovBlanket(bn, "C", 2).size(),11)
    self.assertEquals(gum.MarkovBlanket(bn, "C", 3).size(),13)
    with self.assertRaises(gum.InvalidArgument):
      err=gum.MarkovBlanket(bn, "C", 0)


ts = unittest.TestSuite()
addTests(ts, TestMarkovBlanket)
