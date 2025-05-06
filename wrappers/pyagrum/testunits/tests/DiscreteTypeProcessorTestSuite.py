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

import pandas as pd
from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class DiscreteTypeProcessorTestCase(pyAgrumTestCase):
  def testCreateVariableFromDf(self):
    X = pd.DataFrame.from_dict(
      {
        "var1": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1, 2, 3],
        "var2": ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n"],
        "var3": [1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2],
        "var4": [1.11, 2.213, 3.33, 4.23, 5.42, 6.6, 7.5, 8.9, 9.19, 10.11, 11.12, 12.21, 13.3, 14.5],
        "var5": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1],
      }
    )

    discretizer = DiscreteTypeProcessor()
    self.assertEqual(str(discretizer._createVariable("var1", X["var1"])), "var1:Range([1,11])")
    self.assertEqual(
      str(discretizer._createVariable("var2", X["var2"])), "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})"
    )
    self.assertEqual(str(discretizer._createVariable("var3", X["var3"])), "var3:Integer({1|2|5})")
    self.assertEqual(
      str(discretizer._createVariable("var4", X["var4"])),
      "var4:NumericalDiscrete({1.11|2.213|3.33|4.23|5.42|6.6|7.5|8.9|9.19|10.11|11.12|12.21|13.3|14.5})",
    )
    self.assertEqual(str(discretizer._createVariable("var5", X["var5"])), "var5:Range([1,13])")

    discretizer = DiscreteTypeProcessor(discretizationThreshold=13)
    self.assertEqual(str(discretizer._createVariable("var1", X["var1"])), "var1:Range([1,11])")
    self.assertEqual(
      str(discretizer._createVariable("var2", X["var2"])), "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})"
    )
    self.assertEqual(str(discretizer._createVariable("var3", X["var3"])), "var3:Integer({1|2|5})")
    self.assertEqual(
      str(discretizer._createVariable("var4", X["var4"])),
      "var4:Discretized(<(1.11;2.5481[,[2.5481;3.87[,[3.87;5.301[,[5.301;6.78[,[6.78;8.2[,[8.2;9.132[,[9.132;10.211[,[10.211;11.556[,[11.556;12.973[,[12.973;14.5)>)",
    )
    self.assertEqual(str(discretizer._createVariable("var5", X["var5"])), "var5:Range([1,13])")

    discretizer = DiscreteTypeProcessor(discretizationThreshold=11)
    self.assertEqual(str(discretizer._createVariable("var1", X["var1"])), "var1:Range([1,11])")
    self.assertEqual(
      str(discretizer._createVariable("var2", X["var2"])), "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})"
    )
    self.assertEqual(str(discretizer._createVariable("var3", X["var3"])), "var3:Integer({1|2|5})")
    self.assertEqual(
      str(discretizer._createVariable("var4", X["var4"])),
      "var4:Discretized(<(1.11;2.5481[,[2.5481;3.87[,[3.87;5.301[,[5.301;6.78[,[6.78;8.2[,[8.2;9.132[,[9.132;10.211[,[10.211;11.556[,[11.556;12.973[,[12.973;14.5)>)",
    )
    self.assertEqual(
      str(discretizer._createVariable("var5", X["var5"])),
      "var5:Discretized(<(1;1.3[,[1.3;2.6[,[2.6;3.9[,[3.9;5.2[,[5.2;6.5[,[6.5;7.8[,[7.8;9.1[,[9.1;10.4[,[10.4;11.7[,[11.7;13)>)",
    )

  def testBooleanCSVLeBiannic(self):
    X = pd.DataFrame.from_dict(
      {
        "var1": [True, False, True, False, True, False, True, False, True, False, True, False, True, False],
        "var2": [True, True, True, True, True, True, True, True, True, True, True, True, True, True],
        "var3": [False, False, False, False, False, False, False, False, False, False, False, False, False, False],
        "var4": [True, False, True, False, True, False, True, False, True, False, True, False, True, False],
        "var5": [True, True, True, True, True, True, True, True, True, True, True, True, True, True],
      }
    )

    tmpfilename = self.agrumSrcDir("testBooleanCSVLeBiannic.csv")
    X.to_csv(tmpfilename, index=False)
    discretizer = DiscreteTypeProcessor(
      defaultDiscretizationMethod="quantile", defaultNumberOfBins=7, discretizationThreshold=10
    )
    template = discretizer.discretizedTemplate(tmpfilename)

    self.assertEqual(template.toFast(), "var1{False|True};var2{True};var3{False};var4{False|True};var5{True}")


ts = unittest.TestSuite()
addTests(ts, DiscreteTypeProcessorTestCase)
