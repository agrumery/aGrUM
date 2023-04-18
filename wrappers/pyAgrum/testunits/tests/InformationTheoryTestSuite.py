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
import math

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestInformationTheory(pyAgrumTestCase):
  def testEntropyBN(self):
    bn = gum.fastBN("A->B->C")
    entropy1 = bn.cpt("A").expectedValue(lambda x: -math.log2(bn.cpt("A")[x]))

    ie = gum.LazyPropagation(bn)
    it = gum.InformationTheory(ie, ['A'], ['B'])
    entropy2 = it.entropyX()

    self.assertAlmostEquals(entropy1, entropy2, delta=1e-6)

  def testEntropyMRF(self):
    mrf = gum.fastMRF("A--B--C;C--D")
    qA = (mrf.factor(["A", "B", "C"]) * mrf.factor(["C", "D"])).normalize().margSumIn("A")
    entropy1 =qA.expectedValue(lambda x:-math.log2(qA[x]))

    ie = gum.LazyPropagation(bn)
    it = gum.InformationTheory(ie, ['A'], ['B'])
    entropy2 = it.entropyX()

    self.assertAlmostEquals(entropy1, entropy2, delta=1e-6)
