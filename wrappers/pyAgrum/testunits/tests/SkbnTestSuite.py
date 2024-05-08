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
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyAgrum as gum
import pyAgrum.skbn as skbn
import pandas as pd
from sklearn import datasets


class SkbnTestCase(pyAgrumTestCase):
  def test_no_discretization(self):
    X = pd.DataFrame([[1, 1.5, "A", True],
                      [2, 2.6, "B", False],
                      [3, 3.14, "B", True],
                      [1, 0.5, "A", False],
                      [1, 0.15, "A", True],
                      ])
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
    X = pd.DataFrame([[1, 1.5, "A", True],
                      [2, 2.6, "B", False],
                      [3, 3.14, "B", True],
                      [1, 0.5, "A", False],
                      [1, 0.15, "A", True],
                      ])
    y = [3, 2, 3, 1, 2]
    classifier = skbn.BNClassifier(discretizationThreshold=3, discretizationNbBins=3)
    classifier.fit(X, y)
    res = classifier.preparedData(X, y)
    self.assertEqual(res["X1"][1], "[2.23333;3.14)")
    self.assertEqual(str(res["X3"][3]), "False")

    X = pd.DataFrame([[1, 0, "A", True],
                      [1, 4, "B", False],
                      [2, 3.11, "B", True],
                      [2, 0.5, "A", False],
                      [3, 0.15, "A", True],
                      [3, 203, "A", True],
                      ])
    y = [3, 2, 3, 1, 2, 1]
    res = classifier.preparedData(X, y)
    self.assertEqual(res["X1"][0], "(0.15;0.833333[")
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
