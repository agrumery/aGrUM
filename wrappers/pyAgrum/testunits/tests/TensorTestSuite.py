############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
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
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
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

import math
import random
import sys

import numpy as np
import pyAgrum as gum
import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class TensorTestCase(pyAgrumTestCase):
  def setUp(self):
    self.var = {}
    self.var.update({"c": gum.LabelizedVariable("c", "nuages", 2)})
    self.var.update({"s": gum.LabelizedVariable("s", "arrosoir", 2)})
    r = gum.LabelizedVariable("r", "pluie", 0).addLabel('bof') \
      .addLabel('carrement')
    self.var.update({"r": r})
    self.var.update({"w": gum.LabelizedVariable("w", "herbe mouillée", 2)})

  def fillBN(self, bn, id_list):
    id_list.append(bn.add(self.var["c"]))
    id_list.append(bn.add(self.var["s"]))
    id_list.append(bn.add(self.var["r"]))
    id_list.append(bn.add(self.var["w"]))

    bn.addArc(id_list[0], id_list[1])
    bn.addArc(id_list[0], id_list[2])
    bn.addArc(id_list[1], id_list[3])
    bn.addArc(id_list[2], id_list[3])


class TestInsertions(TensorTestCase):
  def testVariableInsertion(self):
    pot = gum.Tensor()
    self.assertTrue(pot.empty())

    pot.add(self.var['c'])
    self.assertFalse(pot.empty())
    self.assertEqual(pot.nbrDim(), 1)
    pot.add(self.var['s']).add(self.var['r'])
    self.assertEqual(pot.nbrDim(), 3)

    for id, var in enumerate([self.var['c'], self.var['s'],
                              self.var['r']]):
      self.assertTrue(pot.contains(var))
      self.assertEqual(pot.variable(id), var)

    self.assertFalse(pot.contains(gum.LabelizedVariable("a", "", 5)))

    a = gum.LabelizedVariable("a", "a", 2)
    other_a = gum.LabelizedVariable("a", "a", 2)
    p = gum.Tensor()
    p.add(a)
    with self.assertRaises(gum.DuplicateElement):
      p.add(a)  # once again
    with self.assertRaises(gum.DuplicateElement):
      p.add(other_a)  # with the same name

  def testVariableDeletion(self):
    pot = gum.Tensor()
    pot.add(self.var['s'])
    pot.add(self.var['r'])
    self.assertEqual(pot.nbrDim(), 2)
    pot.remove(self.var['s'])
    self.assertEqual(pot.nbrDim(), 1)
    self.assertFalse(pot.contains(self.var['s']))
    pot.remove(self.var['r'])
    self.assertTrue(pot.empty())

  def testDimensionIncreasing(self):
    bn = gum.BayesNet()
    id_list = []
    self.fillBN(bn, id_list)
    bn.cpt(id_list[3]).fillWith([1, 0,
                                 0.1, 0.9,
                                 0.1, 0.9,
                                 0.01, 0.99])
    addvar = gum.LabelizedVariable("d", "rosée", 4)
    addvar_id = bn.add(addvar)
    bn.addArc(addvar_id, id_list[3])

    list3 = bn.cpt(id_list[3])
    self.assertEqual(list3.names, ('w', 's', 'r', 'd'))
    list3[0, :, :, :] = \
      [[[0.99, 0.1], [0.1, 0]], [[0.01, 0.9], [0.9, 1]]]
    list3[1, :, :, :] = \
      [[[0.99, 0.1], [0.1, 0]], [[0.01, 0.9], [0.9, 1]]]
    list3[2, :, :, :] = \
      [[[0.99, 0.1], [0.1, 0]], [[0.01, 0.9], [0.9, 1]]]
    list3[3, :, :, :] = \
      [[[0.99, 0.1], [0.1, 0]], [[0.01, 0.9], [0.9, 1]]]

    list3[2, :, :, :] = 0.25
    self.assertListsAlmostEqual(list3[2, :, :, :].tolist(),
                                [[[0.25, 0.25], [0.25, 0.25]], [[0.25, 0.25], [0.25, 0.25]]])
    self.assertListsAlmostEqual(list3[{"d": 1}].tolist(),
                                [[[0.99, 0.1], [0.1, 0]], [[0.01, 0.9], [0.9, 1]]])

  def testWithInstantiation(self):
    bn = gum.BayesNet()
    id_list = []
    self.fillBN(bn, id_list)
    list3 = bn.cpt(id_list[3])
    list3[:] = [[[1, 0], [0.1, 0.9]],
                [[0.1, 0.9], [0.01, 0.99]]]
    i = gum.Instantiation(list3)
    list3.set(i, 0)
    i.inc()
    list3.set(i, 1)
    self.assertListsAlmostEqual(list3[:],
                                [[[0, 1], [0.1, 0.9]],
                                 [[0.1, 0.9], [0.01, 0.99]]])
    self.assertListsAlmostEqual(list3[:], bn.cpt(id_list[3])[:])

  def testCopyConstructor(self):
    pot = gum.Tensor()
    pot.add(self.var['c'])
    pot.add(self.var['s'])
    pot.add(self.var['r'])

    i = gum.Instantiation(pot)
    val = 1
    i.setFirst()
    while not i.end():
      pot.set(i, val)
      val += 1
      i.inc()
    self.assertEqual(pot.sum(), 36.0)

    pot2 = gum.Tensor(pot)
    self.assertEqual(pot2.sum(), 36.0)

    i.setFirst()
    pot.set(i, 0)  # instead of 1
    self.assertEqual(pot.sum(), 35.0)
    self.assertEqual(pot2.sum(), 36.0)


class TestIndexs(TensorTestCase):
  def testNumpyIndex(self):
    bn = gum.BayesNet()
    id_list = []

    self.fillBN(bn, id_list)
    list1 = bn.cpt(id_list[1])
    list3 = bn.cpt(id_list[3])
    list1[:] = [[0.5, 0.5], [0.9, 0.1]]
    list3[0, 0, :] = [1, 0]
    list3[0, 1, :] = [0.1, 0.9]
    list3[1, 0, :] = [0.1, 0.9]
    list3[1, 1, :] = [0.01, 0.99]

    self.assertAlmostEqual(list1[0, 1], 0.5)
    self.assertListsAlmostEqual(list1[1, :].tolist(), [0.9, 0.1])
    self.assertListsAlmostEqual(list1[:].tolist(),
                                [[0.5, 0.5], [0.9, 0.1]])

    list1[0, :] = [0.3, 0.7]
    list1[1, :] = [0.6, 0.4]
    self.assertListsAlmostEqual(list1[:].tolist(),
                                [[0.3, 0.7], [0.6, 0.4]])
    list1[1, 0] = 0.9
    self.assertListsAlmostEqual(list1[:].tolist(),
                                [[0.3, 0.7], [0.9, 0.4]])

    list3[:, 0, 1] = 0.333
    self.assertAlmostEqual(list3[0, 0, 1], 0.333)
    self.assertAlmostEqual(list3[1, 0, 1], 0.333)

    self.assertListsAlmostEqual(list3[:, 0].tolist(), list3[:, 0, :].tolist())

  def testDictIndex(self):
    bn = gum.BayesNet()
    id_list = []

    self.fillBN(bn, id_list)
    list2 = bn.cpt(id_list[2])
    list2[{}] = [[0.8, 0.2], [0.2, 0.8]]
    self.assertEqual(list2.names, ("r", "c"))

    self.assertListsAlmostEqual(list2[{"r": 0}].tolist(),
                                list2[:, 0].tolist())
    self.assertListsAlmostEqual(list2[{"c": 1}].tolist(),
                                list2[1, :].tolist())
    self.assertAlmostEqual(list2[{"r": 0, "c": 1}], list2[{"c": 1, "r": 0}])

    list2[{"r": 1}] = 0.5
    self.assertListsAlmostEqual(list2[{"r": 1}].tolist(), [0.5, 0.5])
    self.assertListsAlmostEqual(list2[{"r": 0}].tolist(), [0.8, 0.2])

    list2[{'r': 0}] = [0.1, 0.9]
    list2[{'r': 'carrement'}] = [0.6, 0.4]
    self.assertListsAlmostEqual(list2[{'r': 'bof'}].tolist(), [0.1, 0.9])
    self.assertListsAlmostEqual(list2[{'r': 1}].tolist(), [0.6, 0.4])

    self.assertListsAlmostEqual(list2[{'c': 0}].tolist(), [0.1, 0.6])
    self.assertListsAlmostEqual(list2[{'c': 1}].tolist(), [0.9, 0.4])

    self.assertListsAlmostEqual(
      list2[{'r': 'bof', 'existepas': 0}].tolist(), [0.1, 0.9])
    self.assertListsAlmostEqual(
      list2[{'r': 1, 'existepas': 'et non'}].tolist(), [0.6, 0.4])

    self.assertRaises(gum.OutOfBounds, list2.__getitem__, {'r': 99})

    self.assertListsAlmostEqual(
      list2.toarray(), np.array([[0.1, 0.6], [0.9, 0.4]]))


class TestOperators(pyAgrumTestCase):
  def testSimpleOperators(self):
    a, b, c = [gum.LabelizedVariable(s, s, 3) for s in "abc"]
    p = gum.Tensor()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    q = gum.Tensor()
    q.add(b).add(c)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    self.assertEqual((p + q).names, ('b', 'c', 'a'))
    self.assertEqual((q + p).names, ('b', 'a', 'c'))
    self.assertEqual((p + q).tolist(), [[[2.0, 6.0, 10.0], [5.0, 9.0, 13.0], [8.0, 12.0, 16.0]],
                                        [[3.0, 7.0, 11.0], [6.0, 10.0, 14.0],
                                         [9.0, 13.0, 17.0]],
                                        [[4.0, 8.0, 12.0], [7.0, 11.0, 15.0], [10.0, 14.0, 18.0]]])
    self.assertEqual((q + p).tolist(), [[[2.0, 6.0, 10.0], [3.0, 7.0, 11.0], [4.0, 8.0, 12.0]],
                                        [[5.0, 9.0, 13.0], [6.0, 10.0, 14.0],
                                         [7.0, 11.0, 15.0]],
                                        [[8.0, 12.0, 16.0], [9.0, 13.0, 17.0], [10.0, 14.0, 18.0]]])

    self.assertEqual((p - q).names, ('b', 'c', 'a'))
    self.assertEqual((q - p).names, ('b', 'a', 'c'))
    self.assertEqual((p - q).tolist(), [[[0.0, 2.0, 4.0], [-3.0, -1.0, 1.0], [-6.0, -4.0, -2.0]],
                                        [[1.0, 3.0, 5.0], [-2.0, 0.0, 2.0],
                                         [-5.0, -3.0, -1.0]],
                                        [[2.0, 4.0, 6.0], [-1.0, 1.0, 3.0], [-4.0, -2.0, 0.0]]])
    self.assertEqual((q - p).tolist(), [[[0.0, -2.0, -4.0], [-1.0, -3.0, -5.0], [-2.0, -4.0, -6.0]],
                                        [[3.0, 1.0, -1.0], [2.0, 0.0, -2.0],
                                         [1.0, -1.0, -3.0]],
                                        [[6.0, 4.0, 2.0], [5.0, 3.0, 1.0], [4.0, 2.0, 0.0]]])

    self.assertEqual((p * q).names, ('b', 'c', 'a'))
    self.assertEqual((q * p).names, ('b', 'a', 'c'))
    self.assertEqual((p * q).tolist(), [[[1.0, 8.0, 21.0], [4.0, 20.0, 42.0], [7.0, 32.0, 63.0]],
                                        [[2.0, 10.0, 24.0], [8.0, 25.0, 48.0],
                                         [14.0, 40.0, 72.0]],
                                        [[3.0, 12.0, 27.0], [12.0, 30.0, 54.0], [21.0, 48.0, 81.0]]])
    self.assertEqual((q * p).tolist(), [[[1.0, 8.0, 21.0], [2.0, 10.0, 24.0], [3.0, 12.0, 27.0]],
                                        [[4.0, 20.0, 42.0], [8.0, 25.0, 48.0],
                                         [12.0, 30.0, 54.0]],
                                        [[7.0, 32.0, 63.0], [14.0, 40.0, 72.0], [21.0, 48.0, 81.0]]])

    self.assertEqual((p / q).names, ('b', 'c', 'a'))
    self.assertEqual((q / p).names, ('b', 'a', 'c'))
    self.assertEqual((p / q).tolist(), [[[1.0, 2.0, 2.3333333333333335], [0.25, 0.8, 1.1666666666666667],
                                         [0.14285714285714285, 0.5, 0.7777777777777778]],
                                        [[2.0, 2.5, 2.6666666666666665], [0.5, 1.0, 1.3333333333333333],
                                         [0.2857142857142857, 0.625, 0.8888888888888888]],
                                        [[3.0, 3.0, 3.0], [0.75, 1.2, 1.5], [0.42857142857142855, 0.75, 1.0]]])
    self.assertEqual((q / p).tolist(), [[[1.0, 0.5, 0.42857142857142855], [0.5, 0.4, 0.375],
                                         [0.3333333333333333, 0.3333333333333333, 0.3333333333333333]],
                                        [[4.0, 1.25, 0.8571428571428571], [2.0, 1.0, 0.75],
                                         [1.3333333333333333, 0.8333333333333334, 0.6666666666666666]],
                                        [[7.0, 2.0, 1.2857142857142858], [3.5, 1.6, 1.125],
                                         [2.3333333333333335, 1.3333333333333333, 1.0]]])

    z = p * q - p / q
    self.assertEqual(z.names, ('b', 'c', 'a'))
    self.assertEqual(z.tolist(), [[[0.0, 6.0, 18.666666666666668], [3.75, 19.2, 40.833333333333336],
                                   [6.857142857142857, 31.5, 62.22222222222222]],
                                  [[0.0, 7.5, 21.333333333333332], [7.5, 24.0, 46.666666666666664],
                                   [13.714285714285714, 39.375, 71.11111111111111]],
                                  [[0.0, 9.0, 24.0], [11.25, 28.8, 52.5], [20.571428571428573, 47.25, 80.0]]])

    self.assertEqual((p + q).sum(), 270)
    self.assertEqual((p + q).max(), 18)

    # test on fillWith returning tensor
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9]).normalize()
    self.assertEqual(p.tolist(), [[0.022222222222222223, 0.044444444444444446, 0.06666666666666667],
                                  [0.08888888888888889, 0.1111111111111111,
                                   0.13333333333333333],
                                  [0.15555555555555556, 0.17777777777777778, 0.2]])

  def testEquality(self):
    a, b, c = [gum.LabelizedVariable(s, s, 3) for s in "abc"]
    q = gum.Tensor()
    q.add(b).add(c)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    p = gum.Tensor()  # same data, difference dims
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    r = gum.Tensor()  # same data, same dims
    r.add(a).add(b)
    r.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    t = gum.Tensor()  # same dims, different data
    t.add(a).add(b)
    t.fillWith([1, 2, 3, 0, 5, 6, 7, 8, 9])

    u = gum.Tensor()  # same dims, same data, different order
    u.add(b).add(a)
    u.fillWith([1, 4, 7, 2, 5, 8, 3, 6, 9])

    self.assertTrue(p == p)
    self.assertFalse(p == q)
    self.assertTrue(p == r)
    self.assertFalse(p == t)
    self.assertTrue(p == u)

    self.assertFalse(p != p)
    self.assertTrue(p != q)
    self.assertFalse(p != r)
    self.assertTrue(p != t)
    self.assertFalse(p != u)

  def testMinMax(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]
    p = gum.Tensor()
    p.add(a).add(b).fillWith([1, 9, 3, 0, 5, 0, 7, 8, 9]).normalizeAsCPT()

    self.assertTrue(p.min() == 0.0)
    self.assertTrue(p.max() == 1.0)
    self.assertTrue(p.minNonZero() == 1.0 / 13.0)
    self.assertTrue(p.maxNonOne() == 9.0 / 13.0)

  def testIsNonZeroMap(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]
    p = gum.Tensor()
    p.add(a).add(b).fillWith([1, 9, 3, 0, 5, 0, 7, 8, 9]).normalizeAsCPT()

    q = gum.Tensor()
    q.add(a).add(b).fillWith([1, 1, 1, 0, 1, 0, 1, 1, 1])

    self.assertTrue(p.isNonZeroMap() == q)

  def testSimpleInPLaceOperators(self):
    a, b, c = [gum.LabelizedVariable(s, s, 3) for s in "abc"]
    q = gum.Tensor()
    q.add(b).add(c)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    p = gum.Tensor()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    z = p + q
    p += q
    self.assertEqual(z.tolist(), p.tolist())

    p = gum.Tensor()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    z = p - q
    p -= q
    self.assertEqual(z.tolist(), p.tolist())

    p = gum.Tensor()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    z = p * q
    p *= q
    self.assertEqual(z.tolist(), p.tolist())

    p = gum.Tensor()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    z = p / q
    p /= q
    self.assertEqual(z.tolist(), p.tolist())

  def testMargOutOperators(self):
    a, b, c, d = [gum.LabelizedVariable(s, s, 3) for s in "abcd"]
    p = gum.Tensor()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    p.normalize()

    q = gum.Tensor()
    q.add(c).add(d)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q.normalize()

    joint = p * q

    margAB = joint.sumOut(["c", "d"])
    self.assertEqual(margAB.names, p.names)
    self.assertEqual(margAB.tolist(), p.tolist())

    margCD = joint.sumOut(
      ["b", "a", "x"])  # note the vars in a different order and with one not present in the tensor
    self.assertEqual(margCD.names, q.names)
    self.assertEqual(margCD.tolist(), q.tolist())

    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    self.assertEqual(p.prodOut(["a"]).tolist(), [6, 120, 504])
    self.assertEqual(p.prodOut(["b"]).tolist(), [28, 80, 162])
    self.assertEqual(p.maxOut(["a"]).tolist(), [3, 6, 9])
    self.assertEqual(p.maxOut(["b"]).tolist(), [7, 8, 9])
    self.assertEqual(p.minOut(["a"]).tolist(), [1, 4, 7])
    self.assertEqual(p.minOut(["b"]).tolist(), [1, 2, 3])

  def testMargInOperators(self):
    a, b, c, d = [gum.LabelizedVariable(s, s, 3) for s in "abcd"]
    p = gum.Tensor()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    q = gum.Tensor()
    q.add(c).add(d)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    joint = p * q

    self.assertEqual(joint.sumIn(
      ['a', 'b']), joint.sumOut(['c', 'd']))
    self.assertEqual(joint.sumIn(
      ['b', 'a']), joint.sumOut(['c', 'd']))
    self.assertEqual(joint.sumIn(
      ['a', 'b']), joint.sumOut(['d', 'c']))
    self.assertEqual(joint.sumIn(
      ['b', 'a']), joint.sumOut(['d', 'c']))

    self.assertEqual(joint.prodIn(
      ['a', 'b']), joint.prodOut(['c', 'd']))

    self.assertEqual(joint.maxIn(
      ['a', 'b']), joint.maxOut(['c', 'd']))

    self.assertEqual(joint.minIn(
      ['a', 'b']), joint.minOut(['c', 'd']))

    # one can not margIn on an invalid variable
    try:
      p.sumIn(['d'])
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

  def testAbsTensor(self):
    a, b = [gum.LabelizedVariable(s, s, 2) for s in "ab"]

    p = gum.Tensor().add(a).add(b).fillWith([0, 1, 2, 3])
    q = gum.Tensor().add(a).add(b).fillWith([0, 3, 0, 3])

    self.assertEqual((p - q).abs().tolist(), [[0, 2], [2, 0]])
    self.assertEqual((q - p).abs().tolist(), [[0, 2], [2, 0]])
    self.assertEqual((q - p).abs().max(), 2)
    self.assertEqual((q - p).abs().min(), 0)

  def testLog2Tensor(self):
    a, b = [gum.LabelizedVariable(s, s, 2) for s in "ab"]

    p = gum.Tensor().add(a).add(b)
    p.random()
    pp = gum.Tensor(p)
    pp[:] = np.log2(pp[:])
    self.assertTrue((p.log2() - pp).abs().max() < 1e-8)

  def testSgnTensor(self):
    a, b = [gum.LabelizedVariable(s, s, 2) for s in "ab"]

    p = gum.Tensor().add(a).add(b)
    p.random()
    r = p * 2

    self.assertEqual((r - p).sgn().min(), 1.0);
    self.assertEqual((r - p).sgn().max(), 1.0);
    self.assertEqual((p - r).sgn().min(), -1.0);
    self.assertEqual((p - r).sgn().max(), -1.0);
    self.assertEqual((p - p).sgn().min(), 0.0);
    self.assertEqual((p - p).sgn().max(), 0.0);

  def testSqTensor(self):
    a, b = [gum.LabelizedVariable(s, s, 2) for s in "ab"]

    p = gum.Tensor().add(a).add(b).fillWith([0, 1, 2, 3])
    q = gum.Tensor().add(a).add(b).fillWith([0, 3, 0, 3])

    self.assertEqual((p - q).sq().tolist(), [[0, 4], [4, 0]])
    self.assertEqual((q - p).sq().tolist(), [[0, 4], [4, 0]])
    self.assertEqual((q - p).sq().max(), 4)
    self.assertEqual((q - p).sq().min(), 0)

  def testEntropyTensor(self):
    a = gum.LabelizedVariable("a", "a", 2)
    p = gum.Tensor().add(a)

    self.assertEqual(p.fillWith([0, 1]).entropy(), 0.0)
    self.assertEqual(p.fillWith([1, 0]).entropy(), 0.0)
    self.assertEqual(p.fillWith([0.5, 0.5]).entropy(), 1.0)

  def testReorganizeTensor(self):
    a, b, c, d = [gum.LabelizedVariable(s, s, 3) for s in "abcd"]
    p = gum.Tensor()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    q = gum.Tensor()
    q.add(c).add(d)
    q.fillWith([4, 5, 6, 3, 2, 1, 4, 3, 2])

    self.assertNotEqual(str(p * q), str(q * p))
    self.assertEqual(str(p * q), str((q * p).reorganize(['c', 'd', 'a', 'b'])))
    self.assertNotEqual(
      str(p * q), str((q * p).reorganize(['c', 'a', 'd', 'b'])))

    try:
      q.reorganize(['a'])
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

    try:
      q.reorganize(['d'])
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

  def testReorganizeTensorWithAccents(self):
    if sys.version_info >= (3, 0):
      a, b, c, d = [gum.LabelizedVariable(s, s, 3) for s in "éèàû"]
      p = gum.Tensor()
      p.add(a).add(b)
      p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

      q = gum.Tensor()
      q.add(c).add(d)
      q.fillWith([4, 5, 6, 3, 2, 1, 4, 3, 2])

      self.assertNotEqual(str(p * q), str(q * p))
      self.assertEqual(
        str(p * q), str((q * p).reorganize(['à', 'û', 'é', 'è'])))
      self.assertNotEqual(
        str(p * q), str((q * p).reorganize(['à', 'é', 'û', 'è'])))

      try:
        q.reorganize(['é'])
        self.assertTrue(False)
      except gum.InvalidArgument:
        self.assertTrue(True)

      try:
        q.reorganize(['û'])
        self.assertTrue(False)
      except gum.InvalidArgument:
        self.assertTrue(True)

  def testPutFirstTensor(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]
    p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    self.assertNotEqual(str(p), str(p.putFirst("b")))
    self.assertEqual(str(p), str(p.putFirst("b").putFirst("a")))
    self.assertEqual(str(p), str(p.putFirst("a")))

    try:
      p.putFirst('x')
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

  def testExtraction(self):
    a, b, c = [gum.LabelizedVariable(s, s, 3) for s in "abc"]
    p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = gum.Tensor().add(c).fillWith([1, 2, 3])

    pot = q * p

    I = gum.Instantiation()
    I.add(c)
    I.chgVal(c, 0)
    self.assertEqual(pot.extract(I), p)

    I.chgVal(c, 2)
    r = gum.Tensor().add(a).add(b).fillWith(
      [3, 6, 9, 12, 15, 18, 21, 24, 27])
    self.assertEqual(pot.reorganize(['b', 'c', 'a']).extract(I), r)

  def testExtractionWithDict(self):
    a, b, c = [gum.LabelizedVariable(s, s, 3) for s in "abc"]
    p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = gum.Tensor().add(c).fillWith([1, 2, 3])

    pot = q * p

    self.assertEqual(pot.extract({"c": 0}), p)

    r = gum.Tensor().add(a).add(b).fillWith(
      [3, 6, 9, 12, 15, 18, 21, 24, 27])
    self.assertEqual(pot.reorganize(['b', 'c', 'a']).extract({"c": 2}), r)

    try:
      q = pot.extract({"x": 1})
      self.assertEqual(pot, q)
    except gum.InvalidArgument:
      self.assertTrue(False)

    try:
      pot.extract({"c": 16})
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

    try:
      pot.extract({"c": "2"})
      self.assertTrue(True)
    except gum.GumException:
      self.assertTrue(False)

    try:
      pot.extract({"c": "r"})
      self.assertTrue(False)
    except gum.OutOfBounds:
      self.assertTrue(True)

    try:
      pot.extract({2: 2})
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

  def testOperatorEqual(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]
    p = gum.Tensor()
    q = gum.Tensor()

    self.assertTrue(p == q)

    p.add(a).fillWith([1, 2, 3])
    self.assertTrue(p == p)

    q = gum.Tensor().add(a).fillWith([1, 2, 3])
    self.assertTrue(p == q)
    self.assertEqual(p, q)

    q = gum.Tensor().add(a).fillWith([3, 6, 9])
    self.assertTrue(p != q)

    q = gum.Tensor().add(b).fillWith([1, 2, 3])
    self.assertTrue(p != q)

    q = gum.Tensor().add(a).add(b).fillWith(1)
    self.assertTrue(p != q)

  def testScaleAndTranslate(self):
    a = gum.LabelizedVariable("a", "a", 3)

    p = gum.Tensor().add(a)
    q = gum.Tensor().add(a).fillWith([3, 6, 9])
    r = gum.Tensor().add(a).fillWith([2, 3, 4])
    s = gum.Tensor().add(a).fillWith([4, 7, 10])

    self.assertEqual(p.fillWith([1, 2, 3]).scale(3), q)
    self.assertEqual(p.fillWith([1, 2, 3]).translate(1), r)
    self.assertEqual(p.fillWith([1, 2, 3]).scale(3).translate(1), s)

  def testNormalizeAsCPT(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]

    p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = p / p.sumOut(["a"])
    p.normalizeAsCPT()
    self.assertTrue(p == q)

    p2 = gum.Tensor()
    p2.add(a).add(b).fillWith(0)

    with self.assertRaises(gum.FatalError):
      p2.normalizeAsCPT()

    p3 = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 0, 0, 0, 7, 8, 9])
    with self.assertRaises(gum.FatalError):
      p3.normalizeAsCPT()

    p4 = gum.Tensor().add(a).fillWith([1, 3, 6])
    witness = gum.Tensor().add(a).fillWith([0.1, 0.3, 0.6])
    p4.normalizeAsCPT()
    self.assertTrue(p4 == witness)

  def testOperationForEmptyTensor(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]

    p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = gum.Tensor().fillWith([1])
    self.assertEqual(
      p + q, gum.Tensor().add(a).add(b).fillWith([2, 3, 4, 5, 6, 7, 8, 9, 10]))
    tmp = p
    tmp += q
    self.assertEqual(tmp, gum.Tensor().add(a).add(
      b).fillWith([2, 3, 4, 5, 6, 7, 8, 9, 10]))
    p = gum.Tensor().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = gum.Tensor().fillWith([1])
    e = str(q + p)
    self.assertEqual(tmp, gum.Tensor().add(a).add(
      b).fillWith([2, 3, 4, 5, 6, 7, 8, 9, 10]))

  def testKL(self):
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 2)

    p = gum.Tensor().add(v).fillWith([0.0, 1.0])
    q = gum.Tensor().add(v).fillWith([0.5, 0.5])
    r = gum.Tensor().add(v).fillWith([0.7, 0.3])
    s = gum.Tensor().add(v).add(w).fillWith([0.0, 1.0, 0.2, .08])

    self.assertEqual(p.KL(p), 0.0)

    with self.assertRaises(gum.InvalidArgument):
      res = p.KL(s)
    with self.assertRaises(gum.InvalidArgument):
      res = s.KL(p)

    res = p.KL(q)
    self.assertAlmostEqual(res, 0.0 + 1.0 * math.log(1.0 / 0.5, 2))

    with self.assertRaises(gum.FatalError):
      res = q.KL(p)

    res = p.KL(r)
    self.assertAlmostEqual(res, 0.0 + 1.0 * math.log(1.0 / 0.3, 2))

    with self.assertRaises(gum.FatalError):
      res = r.KL(p)

    self.assertAlmostEqual(
      q.KL(r), 0.5 * math.log(0.5 / 0.7, 2) + 0.5 * math.log(0.5 / 0.3, 2))
    self.assertAlmostEqual(
      r.KL(q), 0.7 * math.log(0.7 / 0.5, 2) + 0.3 * math.log(0.3 / 0.5, 2))

  def testVariableAccessor(self):
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 2)
    p = gum.Tensor().add(v).add(w)
    self.assertEqual(p.variable(0), p.variable('v'))
    self.assertEqual(p.variable(1), p.variable('w'))
    self.assertNotEqual(p.variable(1), p.variable('v'))
    self.assertNotEqual(p.variable(0), p.variable('w'))

    with self.assertRaises(gum.NotFound):
      x = p.variable("zz")

  def testFillWithTensor(self):
    bn = gum.fastBN("A->B->C")
    pABC = bn.cpt("A") * bn.cpt("B") * bn.cpt("C")

    bn2 = gum.fastBN("A->B->C")
    bn2.cpt("A").fillWith(bn.cpt("A"))
    bn2.cpt("B").fillWith(pABC.sumIn(["A", "B"]) / pABC.sumIn(["A"]))
    bn2.cpt("C").fillWith(pABC.sumIn(["B", "C"]) / pABC.sumIn(["B"]))
    pABC2 = (bn2.cpt("A") * bn2.cpt("B") * bn2.cpt("C"))

    self.assertAlmostEqual(np.max(pABC2.reorganize(['A', 'B', 'C']).toarray() -
                                  pABC.reorganize(['A', 'B', 'C']).toarray()), 0)
    self.assertAlmostEqual(np.max(pABC.reorganize(['A', 'B', 'C']).toarray() -
                                  pABC2.reorganize(['A', 'B', 'C']).toarray()), 0)

  def testFillWithTensorAndMap(self):
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 3)
    p = gum.Tensor().add(v).add(w)
    p.fillWith([1, 2, 3, 4, 5, 6])

    vv = gum.LabelizedVariable("vv", "vv", 2)
    ww = gum.LabelizedVariable("ww", "ww", 3)
    pp = gum.Tensor().add(ww).add(vv)
    pp.fillWith(p, ["w", "v"])
    self.assertAlmostEqual(np.max(p.reorganize(['v', 'w']).toarray() -
                                  pp.reorganize(['vv', 'ww']).toarray()), 0)

    vvv = gum.LabelizedVariable("vvv", "vvv", 2)
    www = gum.LabelizedVariable("www", "www", 2)
    ppp = gum.Tensor().add(vvv).add(www)
    with self.assertRaises(gum.InvalidArgument):
      ppp.fillWith(p, ["w", "v"])

  def testfillFromFunction(self):
    bn = gum.fastBN(
      "C[0,1,2,3,4,5,6,7,8]<-A[0,3]->B{0|1|2|3|4|5|6|7|8|12};A->D[9]")

    bn.cpt("D").fillFromFunction("3*A+2")

    I = gum.Instantiation(bn.cpt("D"))
    while not I.end():
      v = 3 * I.val(1) + 2
      if v >= I.variable(0).domainSize():
        v = I.variable(0).domainSize() - 1
      self.assertEqual(bn.cpt("D").get(I), 1 if I.val(0) == v else 0)
      I.inc()

    bn.cpt("B").fillFromFunction("3*A+2")

  def __test_val_for_set(self, p, val, soi, nbr):
    self.assertEqual(len(soi), nbr)
    for i in soi:
      self.assertEqual(p[i], val)

  def testArgMaxMinFindAll(self):
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 3)

    p = gum.Tensor()
    self.__test_val_for_set(p, 10, p.findAll(10), 0)

    p.add(v).add(w)
    p.fillWith([1, 3, 2, 4, 1, 4])

    self.__test_val_for_set(p, 3, p.findAll(3), 1)
    self.__test_val_for_set(p, 10, p.findAll(10), 0)
    self.__test_val_for_set(p, 4, p.argmax()[0], 2)
    self.__test_val_for_set(p, 1, p.argmin()[0], 2)

  def testAddDummyVariables(self):
    # === LabelizedVariable
    v = gum.LabelizedVariable("v", "v", 0)
    p = gum.Tensor()
    self.assertEqual(v.domainSize(), 0)
    with self.assertRaises(gum.InvalidArgument):
      p.add(v)

    v.addLabel("first")
    self.assertEqual(v.domainSize(), 1)
    p.add(v)

    p = gum.Tensor()
    v.addLabel("second")
    self.assertEqual(v.domainSize(), 2)
    p.add(v)

    # === RangeVariable
    v = gum.RangeVariable("v", "v", 1, 0)
    p = gum.Tensor()
    self.assertEqual(v.domainSize(), 0)
    with self.assertRaises(gum.InvalidArgument):
      p.add(v)

    v.setMaxVal(1)
    self.assertEqual(v.domainSize(), 1)
    p.add(v)

    p = gum.Tensor()
    v.setMaxVal(2)
    self.assertEqual(v.domainSize(), 2)
    p.add(v)

    # === DiscretizedVariable
    v = gum.DiscretizedVariable("v", "v")
    p = gum.Tensor()

    self.assertEqual(v.domainSize(), 0)
    with self.assertRaises(gum.InvalidArgument):
      p.add(v)

    v.addTick(1)
    self.assertEqual(v.domainSize(), 0)
    with self.assertRaises(gum.InvalidArgument):
      p.add(v)

    v.addTick(2)
    self.assertEqual(v.domainSize(), 1)
    p.add(v)

    p = gum.Tensor()
    v.addTick(3)
    self.assertEqual(v.domainSize(), 2)
    p.add(v)

  def testRandomTensor(self):
    u = gum.LabelizedVariable("u", "u", 4)
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 3)

    p = gum.Tensor().add(u).add(v).add(w)
    I = gum.Instantiation(p)

    p.random()
    I.setFirst()
    while not I.end():
      self.assertTrue(p.get(I) <= 1.0)
      I.inc()

    p.randomDistribution()
    I.setFirst()
    cum = 0
    while not I.end():
      self.assertTrue(p.get(I) <= 1.0)
      cum += p.get(I)
      I.inc()
    self.assertAlmostEqual(cum, 1.0, 6)

    p.randomCPT()
    v = p.variable(0)
    I.setFirst()
    while not I.end():
      cum = 0.0
      I.setFirstVar(v)
      while not I.end():
        self.assertTrue(p.get(I) <= 1.0)
        cum += p.get(I)
        I.incVar(v)
      self.assertAlmostEqual(cum, 1.0, 6)

      I.unsetEnd()
      I.incNotVar(v)

    p.fillWith(1).normalizeAsCPT()
    I.setFirst()
    while not I.end():
      self.assertAlmostEqual(p.get(I), 0.25, 6)
      I.inc()

    alpha = 0.0
    while alpha <= 1.0:
      p.fillWith(1).normalizeAsCPT()
      p.noising(alpha)
      min = (1 - alpha) * 0.25 + alpha * 0.0
      max = (1 - alpha) * 0.25 + alpha * 1.0
      I.setFirst()
      while not I.end():
        self.assertTrue(min <= p.get(I) <= max)
        I.inc()
      alpha += 0.1

  def testLoopIn(self):
    u = gum.LabelizedVariable("u", "u", 4)
    v = gum.RangeVariable("v", "v", 1, 5)
    w = gum.DiscretizedVariable("w", "w", [-2, -0.5, 1, 2.5])
    p = gum.Tensor().fillWith(0)
    s = 0
    for i in p.loopIn():
      s += p.get(i)
    self.assertEqual(s, 0)

    p = gum.Tensor().fillWith(42)
    s = 0
    for i in p.loopIn():
      s += p.get(i)
    self.assertEqual(s, 42)

    p.add(u).add(v).add(w)
    for i in p.loopIn():
      p.set(i, random.choice([1, 2, 3]))

    s = 0
    for i in p.loopIn():
      s += p.get(i)
    self.assertEqual(s, p.sum())

  def testEqualities(self):
    u = gum.LabelizedVariable("u", "u", 4)
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 3)
    p = gum.Tensor().add(u).add(v).add(w)
    p.random()

    q = gum.Tensor(p)
    self.assertEqual(p, q)

    i = gum.Instantiation(q)
    i.setLast()
    q[i] = 0
    self.assertNotEqual(p, q)

    q.fillWith(p)
    self.assertEqual(p, q)

    q.fillWith(1)
    self.assertNotEqual(p, q)

    q.fillWith(p)
    self.assertEqual(p, q)

    x = gum.LabelizedVariable("x", "Unknown", 5)
    q.add(x)
    self.assertNotEqual(p, q)

    q = gum.Tensor().add(v).add(w)
    self.assertNotEqual(p, q)

  def testInverse(self):
    u = gum.LabelizedVariable("u", "u", 4)
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 3)
    p = gum.Tensor().add(u).add(v).add(w)
    p.random()

    q = gum.Tensor(p).inverse()

    self.assertAlmostEqual((q * p).max(), 1.0, delta=1e-7)
    self.assertAlmostEqual((q * p).min(), 1.0, delta=1e-7)

  def testOperatorsWithScalars(self):
    u = gum.LabelizedVariable("u", "u", 4)
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 3)
    p = gum.Tensor().add(u).add(v).add(w)
    p.random()

    self.assertAlmostEqual(gum.Tensor(p).translate(2), p + 2, 1e-10)
    self.assertAlmostEqual(gum.Tensor(p).translate(2), 2 + p, 1e-10)

    self.assertAlmostEqual(gum.Tensor(p).translate(-2), p - 2, 1e-10)

    self.assertAlmostEqual((p + (3.45 - p)).max(), 3.45, 1e-10)
    self.assertAlmostEqual((p + (3.45 - p)).min(), 3.45, 1e-10)

    self.assertAlmostEqual(gum.Tensor(p).scale(2.5), p * 2.5, 1e-10)
    self.assertAlmostEqual(gum.Tensor(p).scale(2.5), 2.5 * p, 1e-10)

    self.assertAlmostEqual(gum.Tensor(p).scale(1 / 2.5), p / 2.5, 1e-10)
    self.assertAlmostEqual(gum.Tensor(
      p).inverse().scale(2.5), 2.5 / p, 1e-7)

    r = gum.Tensor(p)
    r += 4.3
    self.assertAlmostEqual(gum.Tensor(p).translate(4.3), r, 1e-10)

    r = gum.Tensor(p)
    r -= 4.3
    self.assertAlmostEqual(gum.Tensor(p).translate(-4.3), r, 1e-10)

    r = gum.Tensor(p)
    r *= 4.3
    self.assertAlmostEqual(gum.Tensor(p).scale(4.3), r, 1e-10)

    r = gum.Tensor(p)
    r /= 4.3
    self.assertAlmostEqual(gum.Tensor(p).scale(1 / 4.3), r, 1e-10)

    self.assertAlmostEqual(gum.Tensor(p).log2(), gum.log2(p), 1e-10)
    self.assertAlmostEqual(gum.Tensor(p - 0.5).log2(), abs(p - 0.5), 1e-10)

  def testOperationWithDifferentVariablesFromMadsLindskou(self):
    px = gum.Tensor()
    py = gum.Tensor()

    for s in "ab":
      px.add(gum.LabelizedVariable(s, s, 2))

    for s in "bc":
      py.add(gum.LabelizedVariable(s, s, 2))

    # @todo waiting for resolution of https://github.com/swig/swig/issues/1783
    # with self.assertRaises(gum.DuplicateElement):
    #  pxy=px*py


ts = unittest.TestSuite()
addTests(ts, TestInsertions)
addTests(ts, TestIndexs)
addTests(ts, TestOperators)
