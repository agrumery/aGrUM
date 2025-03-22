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
import math

import pyagrum as gum
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
    qA = (mrf.factor(["A", "B", "C"]) * mrf.factor(["C", "D"])).normalize().sumIn("A")
    entropy1 = qA.expectedValue(lambda x: -math.log2(qA[x]))

    ie = gum.LazyPropagation(bn)
    it = gum.InformationTheory(ie, ['A'], ['B'])
    entropy2 = it.entropyX()

    self.assertAlmostEquals(entropy1, entropy2, delta=1e-6)
