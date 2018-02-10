# -*- encoding: UTF-8 -*-

import math
import unittest

import numpy as np
import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class PotentialTestCase(pyAgrumTestCase):
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


class TestInsertions(PotentialTestCase):
  def testVariableInsertion(self):
    pot = gum.Potential()
    self.assertTrue(pot.empty())

    pot.add(self.var['c'])
    self.assertFalse(pot.empty())
    self.assertEqual(pot.nbrDim(), 1)
    pot.add(self.var['s'])
    pot.add(self.var['r'])
    self.assertEqual(pot.nbrDim(), 3)

    for id, var in enumerate([self.var['c'], self.var['s'],
                              self.var['r']]):
      self.assertTrue(pot.contains(var))
      self.assertEqual(pot.variable(id), var)

    self.assertFalse(pot.contains(gum.LabelizedVariable("a", "", 5)))

    a = gum.LabelizedVariable("a", "a", 2)
    other_a = gum.LabelizedVariable("a", "a", 2)
    p = gum.Potential()
    p.add(a)
    with self.assertRaises(gum.DuplicateElement):
      p.add(a)  # once again
    with self.assertRaises(gum.DuplicateElement):
      p.add(other_a)  # with the same name

  def testVariableDeletion(self):
    pot = gum.Potential()
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
    self.assertEqual(list3.var_names, ['d', 'r', 's', 'w'])
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
    list3[:] = [[[1, 0], [0.1, 0.9]], \
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
    pot = gum.Potential()
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

    pot2 = gum.Potential(pot)
    self.assertEqual(pot2.sum(), 36.0)

    i.setFirst()
    pot.set(i, 0)  # instead of 1
    self.assertEqual(pot.sum(), 35.0)
    self.assertEqual(pot2.sum(), 36.0)


class TestIndexs(PotentialTestCase):
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
    self.assertEqual(list2.var_names, ["c", "r"])

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

    self.assertListsAlmostEqual(list2[{'r': 'bof', 'existepas': 0}].tolist(), [0.1, 0.9])
    self.assertListsAlmostEqual(list2[{'r': 1, 'existepas': 'et non'}].tolist(), [0.6, 0.4])

    self.assertRaises(IndexError, list2.__getitem__, {'r': 99})

    self.assertListsAlmostEqual(list2.toarray(), np.array([[0.1, 0.6], [0.9, 0.4]]))


class TestOperators(pyAgrumTestCase):
  def testSimpleOperators(self):
    a, b, c = [gum.LabelizedVariable(s, s, 3) for s in "abc"]
    p = gum.Potential()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    q = gum.Potential()
    q.add(b).add(c)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    self.assertEqual((p + q).var_names, ['a', 'c', 'b'])
    self.assertEqual((q + p).var_names, ['c', 'a', 'b'])
    self.assertEqual((p + q).tolist(), [[[2.0, 6.0, 10.0], [5.0, 9.0, 13.0], [8.0, 12.0, 16.0]],
                                        [[3.0, 7.0, 11.0], [6.0, 10.0, 14.0], [9.0, 13.0, 17.0]],
                                        [[4.0, 8.0, 12.0], [7.0, 11.0, 15.0], [10.0, 14.0, 18.0]]])
    self.assertEqual((q + p).tolist(), [[[2.0, 6.0, 10.0], [3.0, 7.0, 11.0], [4.0, 8.0, 12.0]],
                                        [[5.0, 9.0, 13.0], [6.0, 10.0, 14.0], [7.0, 11.0, 15.0]],
                                        [[8.0, 12.0, 16.0], [9.0, 13.0, 17.0], [10.0, 14.0, 18.0]]])

    self.assertEqual((p - q).var_names, ['a', 'c', 'b'])
    self.assertEqual((q - p).var_names, ['c', 'a', 'b'])
    self.assertEqual((p - q).tolist(), [[[0.0, 2.0, 4.0], [-3.0, -1.0, 1.0], [-6.0, -4.0, -2.0]],
                                        [[1.0, 3.0, 5.0], [-2.0, 0.0, 2.0], [-5.0, -3.0, -1.0]],
                                        [[2.0, 4.0, 6.0], [-1.0, 1.0, 3.0], [-4.0, -2.0, 0.0]]])
    self.assertEqual((q - p).tolist(), [[[0.0, -2.0, -4.0], [-1.0, -3.0, -5.0], [-2.0, -4.0, -6.0]],
                                        [[3.0, 1.0, -1.0], [2.0, 0.0, -2.0], [1.0, -1.0, -3.0]],
                                        [[6.0, 4.0, 2.0], [5.0, 3.0, 1.0], [4.0, 2.0, 0.0]]])

    self.assertEqual((p * q).var_names, ['a', 'c', 'b'])
    self.assertEqual((q * p).var_names, ['c', 'a', 'b'])
    self.assertEqual((p * q).tolist(), [[[1.0, 8.0, 21.0], [4.0, 20.0, 42.0], [7.0, 32.0, 63.0]],
                                        [[2.0, 10.0, 24.0], [8.0, 25.0, 48.0], [14.0, 40.0, 72.0]],
                                        [[3.0, 12.0, 27.0], [12.0, 30.0, 54.0], [21.0, 48.0, 81.0]]])
    self.assertEqual((q * p).tolist(), [[[1.0, 8.0, 21.0], [2.0, 10.0, 24.0], [3.0, 12.0, 27.0]],
                                        [[4.0, 20.0, 42.0], [8.0, 25.0, 48.0], [12.0, 30.0, 54.0]],
                                        [[7.0, 32.0, 63.0], [14.0, 40.0, 72.0], [21.0, 48.0, 81.0]]])

    self.assertEqual((p / q).var_names, ['a', 'c', 'b'])
    self.assertEqual((q / p).var_names, ['c', 'a', 'b'])
    self.assertEqual((p / q).tolist(), [
      [[1.0, 2.0, 2.3333333333333335], [0.25, 0.8, 1.1666666666666667], [0.14285714285714285, 0.5, 0.7777777777777778]],
      [[2.0, 2.5, 2.6666666666666665], [0.5, 1.0, 1.3333333333333333], [0.2857142857142857, 0.625, 0.8888888888888888]],
      [[3.0, 3.0, 3.0], [0.75, 1.2, 1.5], [0.42857142857142855, 0.75, 1.0]]])
    self.assertEqual((q / p).tolist(), [[[1.0, 0.5, 0.42857142857142855], [0.5, 0.4, 0.375],
                                         [0.3333333333333333, 0.3333333333333333, 0.3333333333333333]],
                                        [[4.0, 1.25, 0.8571428571428571], [2.0, 1.0, 0.75],
                                         [1.3333333333333333, 0.8333333333333334, 0.6666666666666666]],
                                        [[7.0, 2.0, 1.2857142857142858], [3.5, 1.6, 1.125],
                                         [2.3333333333333335, 1.3333333333333333, 1.0]]])

    z = p * q - p / q
    self.assertEqual(z.var_names, ['a', 'c', 'b'])
    self.assertEqual(z.tolist(), [
      [[0.0, 6.0, 18.666666666666668], [3.75, 19.2, 40.833333333333336], [6.857142857142857, 31.5, 62.22222222222222]],
      [[0.0, 7.5, 21.333333333333332], [7.5, 24.0, 46.666666666666664],
       [13.714285714285714, 39.375, 71.11111111111111]],
      [[0.0, 9.0, 24.0], [11.25, 28.8, 52.5], [20.571428571428573, 47.25, 80.0]]])

    self.assertEqual((p + q).sum(), 270)
    self.assertEqual((p + q).max(), 18)

    # test on fillWith returning potential
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9]).normalize()
    self.assertEqual(p.tolist(), [[0.022222222222222223, 0.044444444444444446, 0.06666666666666667],
                                  [0.08888888888888889, 0.1111111111111111, 0.13333333333333333],
                                  [0.15555555555555556, 0.17777777777777778, 0.2]])

  def testEquality(self):
    a, b, c = [gum.LabelizedVariable(s, s, 3) for s in "abc"]
    q = gum.Potential()
    q.add(b).add(c)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    p = gum.Potential()  # same data, difference dims
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    r = gum.Potential()  # same data, same dims
    r.add(a).add(b)
    r.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    t = gum.Potential()  # same dims, different data
    t.add(a).add(b)
    t.fillWith([1, 2, 3, 0, 5, 6, 7, 8, 9])

    u = gum.Potential()  # same dims, same data, different order
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
    p = gum.Potential()
    p.add(a).add(b).fillWith([1, 9, 3, 0, 5, 0, 7, 8, 9]).normalizeAsCPT()

    self.assertTrue(p.min() == 0.0)
    self.assertTrue(p.max() == 1.0)
    self.assertTrue(p.minNonZero() == 1.0 / 13.0)
    self.assertTrue(p.maxNonOne() == 9.0 / 13.0)

  def testIsNonZeroMap(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]
    p = gum.Potential()
    p.add(a).add(b).fillWith([1, 9, 3, 0, 5, 0, 7, 8, 9]).normalizeAsCPT()

    q = gum.Potential()
    q.add(a).add(b).fillWith([1, 1, 1, 0, 1, 0, 1, 1, 1])

    self.assertTrue(p.isNonZeroMap() == q)

  def testSimpleInPLaceOperators(self):
    a, b, c = [gum.LabelizedVariable(s, s, 3) for s in "abc"]
    q = gum.Potential()
    q.add(b).add(c)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    p = gum.Potential()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    z = p + q
    p += q
    self.assertEquals(z.tolist(), p.tolist())

    p = gum.Potential()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    z = p - q
    p -= q
    self.assertEquals(z.tolist(), p.tolist())

    p = gum.Potential()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    z = p * q
    p *= q
    self.assertEquals(z.tolist(), p.tolist())

    p = gum.Potential()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    z = p / q
    p /= q
    self.assertEquals(z.tolist(), p.tolist())

  def testMargOutOperators(self):
    a, b, c, d = [gum.LabelizedVariable(s, s, 3) for s in "abcd"]
    p = gum.Potential()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    p.normalize()

    q = gum.Potential()
    q.add(c).add(d)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q.normalize()

    joint = p * q

    margAB = joint.margSumOut(["c", "d"])
    self.assertEquals(margAB.var_names, p.var_names)
    self.assertEquals(margAB.tolist(), p.tolist())

    margCD = joint.margSumOut(
        ["b", "a", "x"])  # note the vars in a different order and with one not present in the potential
    self.assertEquals(margCD.var_names, q.var_names)
    self.assertEquals(margCD.tolist(), q.tolist())

    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    self.assertEquals(p.margProdOut(["a"]).tolist(), [6, 120, 504])
    self.assertEquals(p.margProdOut(["b"]).tolist(), [28, 80, 162])
    self.assertEquals(p.margMaxOut(["a"]).tolist(), [3, 6, 9])
    self.assertEquals(p.margMaxOut(["b"]).tolist(), [7, 8, 9])
    self.assertEquals(p.margMinOut(["a"]).tolist(), [1, 4, 7])
    self.assertEquals(p.margMinOut(["b"]).tolist(), [1, 2, 3])

  def testMargInOperators(self):
    a, b, c, d = [gum.LabelizedVariable(s, s, 3) for s in "abcd"]
    p = gum.Potential()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    q = gum.Potential()
    q.add(c).add(d)
    q.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    joint = p * q

    self.assertEquals(joint.margSumIn(['a', 'b']), joint.margSumOut(['c', 'd']))
    self.assertEquals(joint.margSumIn(['b', 'a']), joint.margSumOut(['c', 'd']))
    self.assertEquals(joint.margSumIn(['a', 'b']), joint.margSumOut(['d', 'c']))
    self.assertEquals(joint.margSumIn(['b', 'a']), joint.margSumOut(['d', 'c']))

    self.assertEquals(joint.margProdIn(['a', 'b']), joint.margProdOut(['c', 'd']))

    self.assertEquals(joint.margMaxIn(['a', 'b']), joint.margMaxOut(['c', 'd']))

    self.assertEquals(joint.margMinIn(['a', 'b']), joint.margMinOut(['c', 'd']))

    # one can not margIn on an invalid variable
    try:
      p.margSumIn(['d'])
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

  def testAbsPotential(self):
    a, b = [gum.LabelizedVariable(s, s, 2) for s in "ab"]

    p = gum.Potential().add(a).add(b).fillWith([0, 1, 2, 3])
    q = gum.Potential().add(a).add(b).fillWith([0, 3, 0, 3])

    self.assertEquals((p - q).abs().tolist(), [[0, 2], [2, 0]])
    self.assertEquals((q - p).abs().tolist(), [[0, 2], [2, 0]])
    self.assertEquals((q - p).abs().max(), 2)
    self.assertEquals((q - p).abs().min(), 0)

  def testSqPotential(self):
    a, b = [gum.LabelizedVariable(s, s, 2) for s in "ab"]

    p = gum.Potential().add(a).add(b).fillWith([0, 1, 2, 3])
    q = gum.Potential().add(a).add(b).fillWith([0, 3, 0, 3])

    self.assertEquals((p - q).sq().tolist(), [[0, 4], [4, 0]])
    self.assertEquals((q - p).sq().tolist(), [[0, 4], [4, 0]])
    self.assertEquals((q - p).sq().max(), 4)
    self.assertEquals((q - p).sq().min(), 0)

  def testEntropyPotential(self):
    a = gum.LabelizedVariable("a", "a", 2)
    p = gum.Potential().add(a)

    self.assertEquals(p.fillWith([0, 1]).entropy(), 0.0)
    self.assertEquals(p.fillWith([1, 0]).entropy(), 0.0)
    self.assertEquals(p.fillWith([0.5, 0.5]).entropy(), 1.0)

  def testReorganizePotential(self):
    a, b, c, d = [gum.LabelizedVariable(s, s, 3) for s in "abcd"]
    p = gum.Potential()
    p.add(a).add(b)
    p.fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

    q = gum.Potential()
    q.add(c).add(d)
    q.fillWith([4, 5, 6, 3, 2, 1, 4, 3, 2])

    self.assertNotEqual(str(p * q), str(q * p))
    self.assertEqual(str(p * q), str((q * p).reorganize(['c', 'd', 'a', 'b'])))
    self.assertNotEqual(str(p * q), str((q * p).reorganize(['c', 'a', 'd', 'b'])))

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

  def testPutFirstPotential(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]
    p = gum.Potential().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])

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
    p = gum.Potential().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = gum.Potential().add(c).fillWith([1, 2, 3])

    pot = q * p

    I = gum.Instantiation()
    I.add(c)
    I.chgVal(c, 0)
    self.assertEqual(pot.extract(I), p)

    I.chgVal(c, 2)
    r = gum.Potential().add(a).add(b).fillWith([3, 6, 9, 12, 15, 18, 21, 24, 27])
    self.assertEqual(pot.reorganize(['b', 'c', 'a']).extract(I), r)

  def testExtractionWithDict(self):
    a, b, c = [gum.LabelizedVariable(s, s, 3) for s in "abc"]
    p = gum.Potential().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = gum.Potential().add(c).fillWith([1, 2, 3])

    pot = q * p

    self.assertEqual(pot.extract({"c": 0}), p)

    r = gum.Potential().add(a).add(b).fillWith([3, 6, 9, 12, 15, 18, 21, 24, 27])
    self.assertEqual(pot.reorganize(['b', 'c', 'a']).extract({"c": 2}), r)

    try:
      pot.extract({"x": 1})
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

    try:
      pot.extract({"c": 16})
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

    try:
      pot.extract({"c": "r"})
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

    try:
      pot.extract({2: 2})
      self.assertTrue(False)
    except gum.InvalidArgument:
      self.assertTrue(True)

  def testOperatorEqual(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]
    p = gum.Potential()
    q = gum.Potential()

    self.assertTrue(p == q)

    p.add(a).fillWith([1, 2, 3])
    self.assertTrue(p == p)

    q = gum.Potential().add(a).fillWith([1, 2, 3])
    self.assertTrue(p == q)
    self.assertEqual(p, q)

    q = gum.Potential().add(a).fillWith([3, 6, 9])
    self.assertTrue(p != q)

    q = gum.Potential().add(b).fillWith([1, 2, 3])
    self.assertTrue(p != q)

    q = gum.Potential().add(a).add(b).fillWith(1)
    self.assertTrue(p != q)

  def testScaleAndTranslate(self):
    a = gum.LabelizedVariable("a", "a", 3)

    p = gum.Potential().add(a)
    q = gum.Potential().add(a).fillWith([3, 6, 9])
    r = gum.Potential().add(a).fillWith([2, 3, 4])
    s = gum.Potential().add(a).fillWith([4, 7, 10])

    self.assertEqual(p.fillWith([1, 2, 3]).scale(3), q)
    self.assertEqual(p.fillWith([1, 2, 3]).translate(1), r)
    self.assertEqual(p.fillWith([1, 2, 3]).scale(3).translate(1), s)

  def testNormalizeAsCPT(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]

    p = gum.Potential().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = p / p.margSumOut(["a"])
    p.normalizeAsCPT()
    self.assertTrue(p == q)

    p2 = gum.Potential()
    p2.add(a).add(b).fillWith(0)

    with self.assertRaises(gum.FatalError):
      p2.normalizeAsCPT()

    p3 = gum.Potential().add(a).add(b).fillWith([1, 2, 3, 0, 0, 0, 7, 8, 9])
    with self.assertRaises(gum.FatalError):
      p3.normalizeAsCPT()

    p4 = gum.Potential().add(a).fillWith([1, 3, 6])
    witness = gum.Potential().add(a).fillWith([0.1, 0.3, 0.6])
    p4.normalizeAsCPT()
    self.assertTrue(p4 == witness)

  def testOperationForEmptyPotential(self):
    a, b = [gum.LabelizedVariable(s, s, 3) for s in "ab"]

    p = gum.Potential().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = gum.Potential().fillWith([1])
    self.assertEqual(p + q, gum.Potential().add(a).add(b).fillWith([2, 3, 4, 5, 6, 7, 8, 9, 10]))
    tmp = p
    tmp += q
    self.assertEqual(tmp, gum.Potential().add(a).add(b).fillWith([2, 3, 4, 5, 6, 7, 8, 9, 10]))
    p = gum.Potential().add(a).add(b).fillWith([1, 2, 3, 4, 5, 6, 7, 8, 9])
    q = gum.Potential().fillWith([1])
    e = str(q + p)
    self.assertEqual(tmp, gum.Potential().add(a).add(b).fillWith([2, 3, 4, 5, 6, 7, 8, 9, 10]))

  def testKL(self):
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 2)

    p = gum.Potential().add(v).fillWith([0.0, 1.0])
    q = gum.Potential().add(v).fillWith([0.5, 0.5])
    r = gum.Potential().add(v).fillWith([0.7, 0.3])
    s = gum.Potential().add(v).add(w).fillWith([0.0, 1.0, 0.2, .08])

    self.assertEqual(p.KL(p), 0.0)

    with self.assertRaises(gum.InvalidArgument):
      res = p.KL(s)
    with self.assertRaises(gum.InvalidArgument):
      res = s.KL(p)

    with self.assertRaises(gum.FatalError):
      res = p.KL(q)
    with self.assertRaises(gum.FatalError):
      res = q.KL(p)

    with self.assertRaises(gum.FatalError):
      res = p.KL(r)
    with self.assertRaises(gum.FatalError):
      res = r.KL(p)

    self.assertAlmostEqual(q.KL(r), 0.5 * math.log(0.5 / 0.7, 2) + 0.5 * math.log(0.5 / 0.3, 2), 1e-5)
    self.assertAlmostEqual(r.KL(q), 0.7 * math.log(0.7 / 0.5, 2) + 0.3 * math.log(0.3 / 0.5, 2), 1e-5)

  def testVariableAccessor(self):
    v = gum.LabelizedVariable("v", "v", 2)
    w = gum.LabelizedVariable("w", "w", 2)
    p = gum.Potential().add(v).add(w)
    self.assertEquals(p.variable(0), p.variable('v'))
    self.assertEquals(p.variable(1), p.variable('w'))
    self.assertNotEqual(p.variable(1), p.variable('v'))
    self.assertNotEqual(p.variable(0), p.variable('w'))

    with self.assertRaises(IndexError):
      x = p.variable("zz")

  def testFillWithPotential(self):
    bn = gum.fastBN("A->B->C")
    pABC = bn.cpt("A") * bn.cpt("B") * bn.cpt("C")

    bn2 = gum.fastBN("A->B->C")
    bn2.cpt("A").fillWith(bn.cpt("A"))
    bn2.cpt("B").fillWith(pABC.margSumIn(["A", "B"]) / pABC.margSumIn(["A"]))
    bn2.cpt("C").fillWith(pABC.margSumIn(["B", "C"]) / pABC.margSumIn(["B"]))
    pABC2 = (bn2.cpt("A") * bn2.cpt("B") * bn2.cpt("C"))

    self.assertAlmostEquals(np.max(pABC2.reorganize(['A', 'B', 'C']).toarray() -
                                   pABC.reorganize(['A', 'B', 'C']).toarray()), 0)
    self.assertAlmostEquals(np.max(pABC.reorganize(['A', 'B', 'C']).toarray() -
                                   pABC2.reorganize(['A', 'B', 'C']).toarray()), 0)


ts = unittest.TestSuite()
addTests(ts, TestInsertions)
addTests(ts, TestIndexs)
addTests(ts, TestOperators)
