# -*- encoding: UTF-8 -*-

import unittest
from pyAgrum import Sequence_node, Sequence_string, DuplicateElement
from pyAgrumTestSuite import pyAgrumTestCase


class SequenceTestCase(pyAgrumTestCase):

    def fill_d(self, seq):
        seq.append(1)
        seq.append(3)
        seq.append(5)
        seq.append(2)
        seq.append(4)


    def fill_s(self, seq):
        seq.append("agrum")
        seq.append("pyagrum")



class TestConstructors(SequenceTestCase):

    def testConstructor(self):
        seq = Sequence_node()
        self.fill_d(seq)
        seq = Sequence_string()
        self.fill_s(seq)


    def testCopyConstructor(self):
        seq1 = Sequence_node()
        seq2 = Sequence_node(seq1)
        self.fill_d(seq2)
        seq1 = Sequence_string()
        seq2 = Sequence_string(seq1)
        self.fill_s(seq2)



class TestFeatures(SequenceTestCase):

    def testEquality(self):
        seq1 = Sequence_node()
        seq2 = Sequence_node()
        self.fill_d(seq1)
        self.fill_d(seq2)
        self.assertEqual(seq1, seq2)

        seq3 = Sequence_node()
        self.assertNotEqual(seq1, seq3)
        seq3.append(1)
        self.assertNotEqual(seq1, seq3)
        seq3.append(2)
        seq3.append(3)
        seq3.append(4)
        seq3.append(5)
        self.assertNotEqual(seq1, seq3)


    def testGettersAndSetters(self):
        seq = Sequence_string()
        self.fill_s(seq)
        seq.append("qlementine")

        self.assertRaises(DuplicateElement, seq.append,"qlementine")
        self.assertEqual(seq.__str__(), "[0:agrum - 1:pyagrum - 2:qlementine]")
        self.assertEqual(seq.index("pyagrum"), 1)
        self.assertEqual(seq[1], "pyagrum")
        self.assertTrue(seq.exists("agrum"))
        self.assertFalse(seq.exists("agrur"))
        self.assertRaises(IndexError, seq.__getitem__,15)
        self.assertRaises(IndexError, seq.index,"aaaa")
        self.assertEqual(seq.__str__(), "[0:agrum - 1:pyagrum - 2:qlementine]")

        seq.setAtPos(2, "pyGuiAgrum")
        self.assertEqual(seq.__str__(), "[0:agrum - 1:pyagrum - 2:pyGuiAgrum]")
        self.assertRaises(IndexError, seq.setAtPos,15,"crach")
        self.assertRaises(DuplicateElement, seq.setAtPos,0,"agrum")

        seq.swap(0, 2)
        self.assertEqual(seq.__str__(), "[0:pyGuiAgrum - 1:pyagrum - 2:agrum]")
        seq.swap(1, 0)
        self.assertEqual(seq.__str__(), "[0:pyagrum - 1:pyGuiAgrum - 2:agrum]")
        self.assertRaises(IndexError, seq.swap,1,10)
        self.assertRaises(IndexError, seq.swap,10,1)
        self.assertEqual(seq.__str__(), "[0:pyagrum - 1:pyGuiAgrum - 2:agrum]")



ts = unittest.TestSuite()
ts.addTest(TestConstructors('testConstructor'))
ts.addTest(TestConstructors('testCopyConstructor'))
ts.addTest(TestFeatures('testEquality'))
ts.addTest(TestFeatures('testGettersAndSetters'))
