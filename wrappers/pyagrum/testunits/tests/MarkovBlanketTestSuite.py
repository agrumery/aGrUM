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

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestMarkovBlanket(pyAgrumTestCase):
  def testChain(self):
    bn = gum.fastBN("a->b->c")
    _ = gum.MarkovBlanket(bn, "a")
    _ = gum.MarkovBlanket(bn, 1)

  def testMarkovBlanketSpecialArcs(self):
    bn = gum.fastBN("aa->bb->cc->dd->ee;ff->dd->gg;hh->ii->gg;ff->ii;ff->gg")
    mb = gum.fastBN("cc->dd->ee;ff->dd->gg;ff->gg;ff->ii->gg")
    self.assertTrue(gum.MarkovBlanket(bn, "dd").hasSameStructure(mb))

  def testMarkovBlanketStructure(self):
    bn = gum.fastBN("a->b->c->d->e;f->d->g;h->i->g")
    self.assertFalse(gum.MarkovBlanket(bn, "a").hasSameStructure(gum.fastBN("b->a")))

    self.assertTrue(gum.MarkovBlanket(bn, "a").hasSameStructure(gum.fastBN("a->b")))
    self.assertTrue(gum.MarkovBlanket(bn, "b").hasSameStructure(gum.fastBN("a->b->c")))
    self.assertTrue(gum.MarkovBlanket(bn, "c").hasSameStructure(gum.fastBN("b->c->d;f->d")))
    self.assertTrue(gum.MarkovBlanket(bn, "d").hasSameStructure(gum.fastBN("c->d->e;f->d->g;i->g")))
    self.assertTrue(gum.MarkovBlanket(bn, "e").hasSameStructure(gum.fastBN("d->e")))
    self.assertTrue(gum.MarkovBlanket(bn, "f").hasSameStructure(gum.fastBN("c->d;f->d;")))
    self.assertTrue(gum.MarkovBlanket(bn, "g").hasSameStructure(gum.fastBN("d->g;i->g;")))
    self.assertTrue(gum.MarkovBlanket(bn, "h").hasSameStructure(gum.fastBN("h->i;")))
    self.assertTrue(gum.MarkovBlanket(bn, "i").hasSameStructure(gum.fastBN("d->g;h->i->g;;")))

  def testMarkovBlanketMultiLevel(self):
    bn = gum.fastBN("Z<-A->B->C->D->E<-Y;X->G<-F<-C<-I<-H->W")
    self.assertEqual(gum.MarkovBlanket(bn, "C", 1).size(), 5)
    self.assertEqual(gum.MarkovBlanket(bn, "C", 2).size(), 11)
    self.assertEqual(gum.MarkovBlanket(bn, "C", 3).size(), 13)
    with self.assertRaises(gum.InvalidArgument):
      _ = gum.MarkovBlanket(bn, "C", 0)
    self.assertEqual(len(gum.MarkovBlanket(bn, "C", 1).nodes()), 5)


ts = unittest.TestSuite()
addTests(ts, TestMarkovBlanket)
