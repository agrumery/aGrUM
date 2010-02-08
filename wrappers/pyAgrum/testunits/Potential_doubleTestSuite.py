# -*- encoding: UTF-8 -*-

import unittest
from pyAgrum import BayesNet_double, LabelizedVar, Potential_double
from pyAgrum import Instantiation
from pyAgrumTestSuite import pyAgrumTestCase


class PotentialTestCase(pyAgrumTestCase):

    def setUp(self):
        self.var = {}
        self.var.update({"c": LabelizedVar("c", "nuages", 2)})
        self.var.update({"s": LabelizedVar("s", "arrosoir", 2)})
        r = LabelizedVar("r", "pluie", 0).addLabel('bof')\
                                         .addLabel('carrement')
        self.var.update({"r": r})
        self.var.update({"w": LabelizedVar("w", "herbe mouillée", 2)})


    def fillBN(self, bn, id_list):
        id_list.append(bn.add(self.var["c"]))
        id_list.append(bn.add(self.var["s"]))
        id_list.append(bn.add(self.var["r"]))
        id_list.append(bn.add(self.var["w"]))

        bn.insertArc(id_list[0], id_list[1])
        bn.insertArc(id_list[0], id_list[2])
        bn.insertArc(id_list[1], id_list[3])
        bn.insertArc(id_list[2], id_list[3])



class TestInsertions(PotentialTestCase):

    def testVariableInsertion(self):
        pot = Potential_double()
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

        self.assertFalse(pot.contains(LabelizedVar("a", "", 5)))


    def testVariableDeletion(self):
        pot = Potential_double()
        pot.add(self.var['s'])
        pot.add(self.var['r'])
        self.assertEqual(pot.nbrDim(), 2)
        pot.remove(self.var['s'])
        self.assertEqual(pot.nbrDim(), 1)
        self.assertFalse(pot.contains(self.var['s']))
        pot.remove(self.var['r'])
        self.assertTrue(pot.empty())


    def testDimentionIncreasing(self):
        bn = BayesNet_double()
        id_list = []
        self.fillBN(bn, id_list)
        list3 = bn.cpt(id_list[3])
        list3[:] = [ [ [ 1,  0     ], [ 0.1 ,  0.9   ] ], \
                     [ [ 0.1,  0.9 ], [ 0.01 ,  0.99 ] ] ]

        addvar = LabelizedVar("d", "rosée", 4)
        addvar_id = bn.add(addvar)
        bn.insertArc(addvar_id, id_list[3])

        list3 = bn.cpt(id_list[3])
        self.assertEqual(list3.var_names, ['d', 'r', 's', 'w'])
        list3[0,:,:,:] = \
                [ [ [0.99, 0.1], [0.1, 0] ], [ [0.01, 0.9], [0.9, 1] ] ]
        list3[1,:,:,:] = \
                [ [ [0.99, 0.1], [0.1, 0] ], [ [0.01, 0.9], [0.9, 1] ] ]
        list3[2,:,:,:] = \
                [ [ [0.99, 0.1], [0.1, 0] ], [ [0.01, 0.9], [0.9, 1] ] ]
        list3[3,:,:,:] = \
                [ [ [0.99, 0.1], [0.1, 0] ], [ [0.01, 0.9], [0.9, 1] ] ]

        list3[2,:,:,:] = 0.25
        self.assertListsAlmostEqual(list3[2,:,:,:].tolist(),
            [ [ [0.25, 0.25], [0.25, 0.25] ], [ [0.25, 0.25], [0.25, 0.25] ] ])
        self.assertListsAlmostEqual(list3[{"d":1}].tolist(),
            [ [ [0.99, 0.1], [0.1, 0] ], [ [0.01, 0.9], [0.9, 1] ] ])


    def testWithInstantiation(self):
        bn = BayesNet_double()
        id_list = []
        self.fillBN(bn, id_list)
        list3 = bn.cpt(id_list[3])
        list3[:] = [ [ [ 1,  0     ], [ 0.1 ,  0.9   ] ], \
                     [ [ 0.1,  0.9 ], [ 0.01 ,  0.99 ] ] ]
        i = Instantiation(list3)
        list3.set(i, 0)
        i.inc()
        list3.set(i, 1)
        self.assertListsAlmostEqual(list3[:],
            [ [ [ 0,  1     ], [ 0.1 ,  0.9   ] ],
              [ [ 0.1,  0.9 ], [ 0.01 ,  0.99 ] ] ] )
        self.assertListsAlmostEqual(list3[:], bn.cpt(id_list[3])[:])



class TestIndexs(PotentialTestCase):

    def testNumpyIndex(self):
        bn = BayesNet_double()
        id_list = []

        self.fillBN(bn, id_list)
        list1 = bn.cpt(id_list[1])
        list3 = bn.cpt(id_list[3])
        list1[:] = [[0.5, 0.5], [0.9, 0.1]]
        list3[0,0,:] = [1, 0]
        list3[0,1,:] = [0.1, 0.9]
        list3[1,0,:] = [0.1, 0.9]
        list3[1,1,:] = [0.01, 0.99]

        self.assertAlmostEqual(list1[0,1], 0.5)
        self.assertListsAlmostEqual(list1[1,:].tolist(), [0.9, 0.1])
        self.assertListsAlmostEqual(list1[:].tolist(),
                                    [[0.5, 0.5], [0.9, 0.1]])

        list1[0,:] = [0.3, 0.7]
        list1[1,:] = [0.6, 0.4]
        self.assertListsAlmostEqual(list1[:].tolist(),
                                    [[0.3, 0.7], [0.6, 0.4]])
        list1[1,0] = 0.9
        self.assertListsAlmostEqual(list1[:].tolist(),
                                    [[0.3, 0.7], [0.9, 0.4]])

        list3[:,0,1] = 0.333
        self.assertAlmostEqual(list3[0,0,1], 0.333)
        self.assertAlmostEqual(list3[1,0,1], 0.333)

        self.assertListsAlmostEqual(list3[:,0].tolist(), list3[:,0,:].tolist())


    def testDictIndex(self):
        bn = BayesNet_double()
        id_list = []

        self.fillBN(bn, id_list)
        list2 = bn.cpt(id_list[2])
        list2[{}] = [[0.8, 0.2], [0.2, 0.8]]
        self.assertEqual(list2.var_names, ["c", "r"])

        self.assertListsAlmostEqual(list2[{"r":0}].tolist(),
                                    list2[:,0].tolist())
        self.assertListsAlmostEqual(list2[{"c":1}].tolist(),
                                    list2[1,:].tolist())
        self.assertAlmostEqual(list2[{"r":0, "c":1}], list2[{"c":1, "r":0}])

        list2[{"r":1}] = 0.5
        self.assertListsAlmostEqual(list2[{"r":1}].tolist(), [0.5, 0.5])
        self.assertListsAlmostEqual(list2[{"r":0}].tolist(), [0.8, 0.2])

        list2[{'r': 0}] = [0.1, 0.9]
        list2[{'r': 'carrement'}] = [0.6, 0.4]
        self.assertListsAlmostEqual(list2[{'r': 'bof'}].tolist(), [0.1, 0.9])
        self.assertListsAlmostEqual(list2[{'r': 1}].tolist(), [0.6, 0.4])

        self.assertRaises(IndexError, list2.__getitem__,{'existepas':0})
        self.assertRaises(IndexError, list2.__getitem__,{'s':99})



ts = unittest.TestSuite()
ts.addTest(TestInsertions('testVariableInsertion'))
ts.addTest(TestInsertions('testVariableDeletion'))
ts.addTest(TestInsertions('testDimentionIncreasing'))
ts.addTest(TestInsertions('testWithInstantiation'))
ts.addTest(TestIndexs('testNumpyIndex'))
ts.addTest(TestIndexs('testDictIndex'))
