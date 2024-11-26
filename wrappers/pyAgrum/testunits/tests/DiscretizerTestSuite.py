# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
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

import numpy
import pandas as pd
from pyAgrum.lib.discretizer import Discretizer

from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class DiscretizerTestCase(pyAgrumTestCase):

  def testCreateVariableFromDf(self):
    X = pd.DataFrame.from_dict({
      'var1': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1, 2, 3],
      'var2': ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n'],
      'var3': [1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2],
      'var4': [1.11, 2.213, 3.33, 4.23, 5.42, 6.6, 7.5, 8.9, 9.19, 10.11, 11.12, 12.21, 13.3, 14.5],
      'var5': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1]
    })

    discretizer = Discretizer()
    self.assertEqual(str(discretizer._createVariable('var1', X['var1'])),
                     "var1:Range([1,11])")
    self.assertEqual(str(discretizer._createVariable('var2', X['var2'])),
                     "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})")
    self.assertEqual(str(discretizer._createVariable('var3', X['var3'])),
                     "var3:Integer({1|2|5})")
    self.assertEqual(str(discretizer._createVariable('var4', X['var4'])),
                     "var4:NumericalDiscrete({1.11|2.213|3.33|4.23|5.42|6.6|7.5|8.9|9.19|10.11|11.12|12.21|13.3|14.5})")
    self.assertEqual(str(discretizer._createVariable('var5', X['var5'])),
                     "var5:Range([1,13])")

    discretizer = Discretizer(discretizationThreshold=13)
    self.assertEqual(str(discretizer._createVariable('var1', X['var1'])),
                     "var1:Range([1,11])")
    self.assertEqual(str(discretizer._createVariable('var2', X['var2'])),
                     "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})")
    self.assertEqual(str(discretizer._createVariable('var3', X['var3'])),
                     "var3:Integer({1|2|5})")
    self.assertEqual(str(discretizer._createVariable('var4', X['var4'])),
                     "var4:Discretized(<(1.11;2.5481[,[2.5481;3.87[,[3.87;5.301[,[5.301;6.78[,[6.78;8.2[,[8.2;9.132[,[9.132;10.211[,[10.211;11.556[,[11.556;12.973[,[12.973;14.5)>)")
    self.assertEqual(str(discretizer._createVariable('var5', X['var5'])),
                     "var5:Range([1,13])")

    discretizer = Discretizer(discretizationThreshold=11)
    self.assertEqual(str(discretizer._createVariable('var1', X['var1'])),
                     "var1:Range([1,11])")
    self.assertEqual(str(discretizer._createVariable('var2', X['var2'])),
                     "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})")
    self.assertEqual(str(discretizer._createVariable('var3', X['var3'])),
                     "var3:Integer({1|2|5})")
    self.assertEqual(str(discretizer._createVariable('var4', X['var4'])),
                     "var4:Discretized(<(1.11;2.5481[,[2.5481;3.87[,[3.87;5.301[,[5.301;6.78[,[6.78;8.2[,[8.2;9.132[,[9.132;10.211[,[10.211;11.556[,[11.556;12.973[,[12.973;14.5)>)")
    self.assertEqual(str(discretizer._createVariable('var5', X['var5'])),
                     "var5:Discretized(<(1;1.3[,[1.3;2.6[,[2.6;3.9[,[3.9;5.2[,[5.2;6.5[,[6.5;7.8[,[7.8;9.1[,[9.1;10.4[,[10.4;11.7[,[11.7;13)>)")

  def testBooleanCSVLeBiannic(self):
    X = pd.DataFrame.from_dict({
      'var1': [True, False, True, False, True, False, True, False, True, False, True, False, True, False],
      'var2': [True, True, True, True, True, True, True, True, True, True, True, True, True, True],
      'var3': [False, False, False, False, False, False, False, False, False, False, False, False, False, False],
      'var4': [True, False, True, False, True, False, True, False, True, False, True, False, True, False],
      'var5': [True, True, True, True, True, True, True, True, True, True, True, True, True, True]
    })

    tmpfilename = self.agrumSrcDir("testBooleanCSVLeBiannic.csv")
    X.to_csv(tmpfilename, index=False)
    discretizer = Discretizer(defaultDiscretizationMethod='quantile',
                              defaultNumberOfBins=7,
                              discretizationThreshold=10)
    template = discretizer.discretizedTemplate(tmpfilename)

    self.assertEqual(template.toFast(),
                     "var1{False|True};var2{True};var3{False};var4{False|True};var5{True}")


ts = unittest.TestSuite()
addTests(ts, DiscretizerTestCase)
