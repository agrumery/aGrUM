# -*- encoding: UTF-8 -*-

import unittest
from numpy import ndarray

import os

class pyAgrumTestCase(unittest.TestCase):
    def agrumSrcDir(self,s):
      return __file__.split('wrappers')[0]+"/"+s
    
      
    def assertListsAlmostEqual(self, seq1, seq2, places=7):
        sequence = (tuple, list, ndarray)
        if len(seq1) != len(seq2):
            raise AssertionError("%s != %s"%(str(seq1), str(seq2)))
        for i, j in zip(seq1, seq2):
            if isinstance(i, sequence) and isinstance(j,  (list, sequence)):
                self.assertListsAlmostEqual(i, j, places)
            else:
                self.assertAlmostEqual(i, j, places)


    def assertDelta(self, x, y, delta=0.05):
        number = (int, float)
        sequence = (list, tuple)

        if isinstance(x, number) and isinstance(y, number):
            if not ((max(x,y) - min(x, y)) <= delta):
                raise AssertionError("%s != %s"%(str(x), str(y)))

        elif isinstance(x, sequence) and isinstance(y, sequence):
            if len(x) != len(y):
                raise AssertionError("%s != %s"%(str(x), str(y)))
            for i, j in zip(x, y):
                self.assertDelta(i, j, delta)
        else:
            raise TypeError("assertDelta parameters must have the same type")
