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
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum as gum
import pyagrum.skbn as skbn
import pandas as pd
from sklearn import datasets


class SkbnTestCase(pyAgrumTestCase):
  def test_no_discretization(self):
    X = pd.DataFrame(
      [
        [1, 1.5, "A", True],
        [2, 2.6, "B", False],
        [3, 3.14, "B", True],
        [1, 0.5, "A", False],
        [1, 0.15, "A", True],
      ]
    )
    y = [3, 2, 3, 1, 2]

    classifier = skbn.BNClassifier()
    classifier.fit(X, y)
    res = classifier.preparedData(X, y)
    self.assertEqual(str(res["X1"][1]), "2.6")
    self.assertEqual(str(res["X3"][3]), "False")

    X = pd.DataFrame([[1, 0, "A", True]])
    y = [3]
    with self.assertRaises(gum.NotFound):
      res = classifier.preparedData(X, y)

  def test_with_discretization(self):
    X = pd.DataFrame(
      [
        [1, 1.5, "A", True],
        [2, 2.6, "B", False],
        [3, 3.14, "B", True],
        [1, 0.5, "A", False],
        [1, 0.15, "A", True],
      ]
    )
    y = [3, 2, 3, 1, 2]
    classifier = skbn.BNClassifier(discretizationThreshold=3, discretizationNbBins=3)
    classifier.fit(X, y)
    res = classifier.preparedData(X, y)
    self.assertEqual(res["X1"][1], "[2.6;3.14)")
    self.assertEqual(str(res["X3"][3]), "False")

    X = pd.DataFrame(
      [
        [1, 0, "A", True],
        [1, 4, "B", False],
        [2, 3.11, "B", True],
        [2, 0.5, "A", False],
        [3, 0.15, "A", True],
        [3, 203, "A", True],
      ]
    )
    y = [3, 2, 3, 1, 2, 1]
    res = classifier.preparedData(X, y)
    self.assertEqual(res["X1"][0], "(0.15;0.5[")
    self.assertEqual(str(res["X3"][2]), "True")

  def test_with_nparray(self):
    iris = datasets.load_iris()
    X = iris.data[:, 0:2]  # we only take the first two features for visualization
    y = iris.target
    classifier = skbn.BNClassifier(discretizationThreshold=3, discretizationNbBins=3)
    classifier.fit(X, y)
    res = classifier.preparedData(X, y)
    # X0 and X1 are discretized so the labels should start with '[' but the rest is random (chosen by load_iris)...
    self.assertEqual(res["x0"][149][0], "[")
    self.assertEqual(res["x1"][149][0], "[")

  def test_with_file(self):
    classifier = skbn.BNClassifier()
    classifier.fit(data=self.agrumSrcDir("miniasia.csv"), targetName="dyspnoea")
    res = classifier.preparedData(data=self.agrumSrcDir("miniasia.csv"))
    self.assertEqual(str(res["lung_cancer"][0]), "0")

  def test_with_df(self):
    classifier = skbn.BNClassifier()
    df = pd.read_csv(self.agrumSrcDir("miniasia.csv"))
    classifier.fit(data=df, targetName="dyspnoea")
    res = classifier.preparedData(data=df)
    self.assertEqual(str(res["lung_cancer"][0]), "0")


ts = unittest.TestSuite()
addTests(ts, SkbnTestCase)
