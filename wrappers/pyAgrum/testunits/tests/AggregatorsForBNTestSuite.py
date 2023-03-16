# (c) Copyright by Pierre-Henri Wuillemin, 2023
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


class AggregatorsForBNTestCase(pyAgrumTestCase):
  def setUp(self):
    pass

  def testAnd(self):
    bn = gum.BayesNet()
    c1, c2 = [bn.add(gum.LabelizedVariable(item, item, 2)) for item in ['C1', 'C2']]
    a = bn.addAND(gum.LabelizedVariable('a', 'a', 2))
    bn.addArc(c1, a)
    bn.addArc(c2, a)

    for i in range(2):
      bn.cpt(c1)[:] = [i, 1 - i]
      for j in range(2):
        bn.cpt(c2)[:] = [j, 1 - j]
        ie = gum.LazyPropagation(bn)
        ie.makeInference()
        if i + j == 0:
          self.assertEqual(ie.posterior(a)[:][0], 0.0)
          self.assertEqual(ie.posterior(a)[:][1], 1.0)
        else:
          self.assertEqual(ie.posterior(a)[:][0], 1.0)
          self.assertEqual(ie.posterior(a)[:][1], 0.0)

  def testOr(self):
    bn = gum.BayesNet()
    c1, c2 = [bn.add(gum.LabelizedVariable(item, item, 2)) for item in ['C1', 'C2']]
    a = bn.addOR(gum.LabelizedVariable('a', 'a', 2))
    bn.addArc(c1, a)
    bn.addArc(c2, a)

    for i in range(2):
      bn.cpt(c1)[:] = [i, 1 - i]
      for j in range(2):
        bn.cpt(c2)[:] = [j, 1 - j]
        ie = gum.LazyPropagation(bn)
        ie.makeInference()
        if i * j == 0:
          self.assertEqual(ie.posterior(a)[:][0], 0.0)
          self.assertEqual(ie.posterior(a)[:][1], 1.0)
        else:
          self.assertEqual(ie.posterior(a)[:][0], 1.0)
          self.assertEqual(ie.posterior(a)[:][1], 0.0)

  def testZeroParent(self):
    bn = gum.BayesNet()

    a = bn.addOR(gum.LabelizedVariable('a', 'a', 2))
    self.assertEqual(bn.cpt(a).tolist(), [1.0, 0.0])

    b = bn.addAND(gum.LabelizedVariable('b', 'b', 2))
    self.assertEqual(bn.cpt(b).tolist(), [0.0, 1.0])


ts = unittest.TestSuite()
addTests(ts, AggregatorsForBNTestCase)
