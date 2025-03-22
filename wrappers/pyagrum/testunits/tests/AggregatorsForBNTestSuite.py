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


class AggregatorsForBNTestCase(pyAgrumTestCase):

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
