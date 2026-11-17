############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import copy
import itertools
import math
import unittest
import warnings

import numpy as np
import pandas as pd

from pyagrum.lib.discreteTypeProcessor import DiscreteTypeProcessor

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class DiscreteTypeProcessorTestCase(pyAgrumTestCase):
  def testCreateVariableFromDf(self):
    X = pd.DataFrame.from_dict(
      {
        "var1": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1, 2, 3],
        "var2": [
          "a",
          "b",
          "c",
          "d",
          "e",
          "f",
          "g",
          "h",
          "i",
          "j",
          "k",
          "l",
          "m",
          "n",
        ],
        "var3": [1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2],
        "var4": [
          1.11,
          2.213,
          3.33,
          4.23,
          5.42,
          6.6,
          7.5,
          8.9,
          9.19,
          10.11,
          11.12,
          12.21,
          13.3,
          14.5,
        ],
        "var5": [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1],
      }
    )

    discretizer = DiscreteTypeProcessor()
    self.assertEqual(str(discretizer._createVariable("var1", X["var1"])), "var1:Range([1,11])")
    self.assertEqual(
      str(discretizer._createVariable("var2", X["var2"])),
      "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})",
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
      str(discretizer._createVariable("var2", X["var2"])),
      "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})",
    )
    self.assertEqual(str(discretizer._createVariable("var3", X["var3"])), "var3:Integer({1|2|5})")
    self.assertEqual(
      str(discretizer._createVariable("var4", X["var4"])),
      "var4:Discretized(<(1.11;2.213[,[2.213;3.33[,[3.33;5.42[,[5.42;6.6[,[6.6;8.2[,[8.2;9.19[,[9.19;10.11[,[10.11;12.21[,[12.21;13.3[,[13.3;14.5)>)",
    )
    self.assertEqual(str(discretizer._createVariable("var5", X["var5"])), "var5:Range([1,13])")

    discretizer = DiscreteTypeProcessor(discretizationThreshold=11)
    self.assertEqual(str(discretizer._createVariable("var1", X["var1"])), "var1:Range([1,11])")
    self.assertEqual(
      str(discretizer._createVariable("var2", X["var2"])),
      "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})",
    )
    self.assertEqual(str(discretizer._createVariable("var3", X["var3"])), "var3:Integer({1|2|5})")
    self.assertEqual(
      str(discretizer._createVariable("var4", X["var4"])),
      "var4:Discretized(<(1.11;2.213[,[2.213;3.33[,[3.33;5.42[,[5.42;6.6[,[6.6;8.2[,[8.2;9.19[,[9.19;10.11[,[10.11;12.21[,[12.21;13.3[,[13.3;14.5)>)",
    )
    # var5 holds a duplicated value (1): the quantile strategy yields a zero-width bin that sklearn removes
    # (hence fewer bins than requested, as expected below) and warns about. The warning is expected here.
    with warnings.catch_warnings():
      warnings.filterwarnings("ignore", message="Bins whose width are too small", category=UserWarning)
      self.assertEqual(
        str(discretizer._createVariable("var5", X["var5"])),
        "var5:Discretized(<(1;2[,[2;4[,[4;5[,[5;6.5[,[6.5;8[,[8;9[,[9;11[,[11;12[,[12;13)>)",
      )

  def testBooleanCSVLeBiannic(self):
    X = pd.DataFrame.from_dict(
      {
        "var1": [
          True,
          False,
          True,
          False,
          True,
          False,
          True,
          False,
          True,
          False,
          True,
          False,
          True,
          False,
        ],
        "var2": [
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
        ],
        "var3": [
          False,
          False,
          False,
          False,
          False,
          False,
          False,
          False,
          False,
          False,
          False,
          False,
          False,
          False,
        ],
        "var4": [
          True,
          False,
          True,
          False,
          True,
          False,
          True,
          False,
          True,
          False,
          True,
          False,
          True,
          False,
        ],
        "var5": [
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
          True,
        ],
      }
    )

    tmpfilename = self.agrumSrcDir("testBooleanCSVLeBiannic.csv")
    X.to_csv(tmpfilename, index=False)
    discretizer = DiscreteTypeProcessor(
      defaultDiscretizationMethod="quantile",
      defaultNumberOfBins=7,
      discretizationThreshold=10,
    )
    template = discretizer.discretizedTemplate(tmpfilename)

    self.assertEqual(
      template.toFast(),
      "var1{False|True};var2{True};var3{False};var4{False|True};var5{True}",
    )

  def testCreateVariableFromExpert(self):
    X = pd.DataFrame.from_dict(
      {
        "var1": [1, 3, 5, 6, 1, 3, 5, 6, 1, 3, 5, 6, 1, 3],
        "var2": [
          1.11,
          2.213,
          3.33,
          4.23,
          5.42,
          6.6,
          7.5,
          8.9,
          9.19,
          10.11,
          11.12,
          12.21,
          13.3,
          14.5,
        ],
      }
    )
    discretizer = DiscreteTypeProcessor()
    template = discretizer.discretizedTemplate(X)
    self.assertEqual(
      template.toFast(),
      "var1{1|3|5|6};var2{1.11|2.213|3.33|4.23|5.42|6.6|7.5|8.9|9.19|10.11|11.12|12.21|13.3|14.5}",
    )

    discretizer = DiscreteTypeProcessor()
    discretizer.setDiscretizationParameters("var2", "expert", [0, 5, 10, 15])
    template = discretizer.discretizedTemplate(X)
    self.assertEqual(template.toFast(), "var1{1|3|5|6};var2+[0,5,10,15]")

    discretizer = DiscreteTypeProcessor()
    discretizer.setDiscretizationParameters("var1", "NoDiscretization", "{1|2|3|5|6|7}")
    discretizer.setDiscretizationParameters("var2", "expert", [0, 5, 10, 15])
    template = discretizer.discretizedTemplate(X)
    self.assertEqual(template.toFast(), "var1{1|2|3|5|6|7};var2+[0,5,10,15]")

    with self.assertRaises(ValueError):
      discretizer = DiscreteTypeProcessor()
      discretizer.setDiscretizationParameters("var1", "NoDiscretization", "{1|3|5|7}")
      discretizer.setDiscretizationParameters("var2", "expert", [0, 5, 10, 15])
      template = discretizer.discretizedTemplate(X)
      self.assertEqual(template.toFast(), "var1{1|2|3|5|6|7};var2+[0,5,10,15]")

    with self.assertRaises(ValueError):
      discretizer = DiscreteTypeProcessor()
      discretizer.setDiscretizationParameters("var1", "NoDiscretization", None)
      discretizer.setDiscretizationParameters("var2", "expert", [0, 5, 10, 15])
      template = discretizer.discretizedTemplate(X)
      self.assertEqual(template.toFast(), "var1{1|2|3|5|6|7};var2+[0,5,10,15]")

  def testCreateVariableFromExpert2(self):
    X = pd.DataFrame.from_dict(
      {
        "var1": [1, 3, 5, 6, 1, 2, 5, 6, 1, 3, 4, 6, 1, 3],
        "var2": [
          "A",
          "B",
          "C",
          "D",
          "E",
          "B",
          "G",
          "B",
          "B",
          "B",
          "B",
          "B",
          "A",
          "A",
        ],
      }
    )
    discretizer = DiscreteTypeProcessor()
    template = discretizer.discretizedTemplate(X)
    self.assertEqual(template.toFast(), "var1[1,6];var2{A|B|C|D|E|G}")

    discretizer = DiscreteTypeProcessor()
    discretizer.setDiscretizationParameters("var1", "NoDiscretization", "[0,6]")
    template = discretizer.discretizedTemplate(X)
    self.assertEqual(template.toFast(), "var1[7];var2{A|B|C|D|E|G}")

    with self.assertRaises(ValueError):
      discretizer = DiscreteTypeProcessor()
      discretizer.setDiscretizationParameters("var1", "NoDiscretization", "[3,5]")
      template = discretizer.discretizedTemplate(X)
      self.assertEqual(template.toFast(), "var1{1|2|3|5|6|7};var2+[0,5,10,15]")

    with self.assertRaises(ValueError):
      discretizer = DiscreteTypeProcessor()
      discretizer.setDiscretizationParameters("var2", "expert", [0, 5, 10, 15])
      template = discretizer.discretizedTemplate(X)
      self.assertEqual(template.toFast(), "var1{1|2|3|5|6|7};var2+[0,5,10,15]")

  def testDeepCopyPreservesRulesAndIsIndependent(self):
    discretizer = DiscreteTypeProcessor(
      defaultDiscretizationMethod="uniform",
      defaultNumberOfBins=5,
      discretizationThreshold=10,
    )
    discretizer.setDiscretizationParameters("var1", "kmeans", 4)
    discretizer.setDiscretizationParameters("var2", "quantile", 3)
    discretizer.setDiscretizationParameters("var3", "NoDiscretization", "[1,5]")

    discretizerCopy = copy.deepcopy(discretizer)

    # same content right after the copy
    self.assertIsNot(discretizerCopy, discretizer)
    self.assertEqual(discretizerCopy.discretizationParametersDictionary, discretizer.discretizationParametersDictionary)
    self.assertEqual(discretizerCopy.defaultMethod, discretizer.defaultMethod)
    self.assertEqual(discretizerCopy.defaultParamDiscretizationMethod, discretizer.defaultParamDiscretizationMethod)
    self.assertEqual(discretizerCopy.discretizationThreshold, discretizer.discretizationThreshold)

    # mutating the copy (as BNClassifier.fit does) must not affect the original: this is exactly
    # the property that lets fit() work on a deepcopy of type_processor without mutating the
    # constructor parameter (see check_estimators_overwrite_params compliance)
    discretizerCopy.setDiscretizationParameters("var4", "MDLP")
    discretizerCopy.numberOfContinuous += 3
    discretizerCopy.totalNumberOfBins += 12
    discretizerCopy.clear()

    self.assertNotIn("var4", discretizer.discretizationParametersDictionary)
    self.assertIn("var4", discretizerCopy.discretizationParametersDictionary)
    self.assertEqual(discretizer.numberOfContinuous, 0)
    self.assertEqual(discretizer.totalNumberOfBins, 0)
    self.assertEqual(
      discretizer.discretizationParametersDictionary,
      {
        "var1": {"method": "kmeans", "param": 4},
        "var2": {"method": "quantile", "param": 3},
        "var3": {"method": "NoDiscretization", "param": "[1,5]"},
      },
    )

  def testMDLPWithoutInformativeCutRaises(self):
    # x carries no information about y: MDLP accepts no cut point, and a variable with a single state is refused
    rng = np.random.default_rng(0)
    X = pd.DataFrame({"x": rng.normal(size=300)})
    y = pd.Series((rng.random(300) < 0.5).astype(int))
    discretizer = DiscreteTypeProcessor(defaultDiscretizationMethod="MDLP")
    with self.assertRaisesRegex(ValueError, "MDLP"):
      discretizer.discretizedTemplate(X, y)

  def testMDLPFindsTheThreshold(self):
    rng = np.random.default_rng(1)
    x = rng.uniform(0, 10, size=300)
    X = pd.DataFrame({"x": x})
    y = pd.Series((x > 6).astype(int))
    discretizer = DiscreteTypeProcessor(defaultDiscretizationMethod="MDLP")
    var = discretizer.discretizedTemplate(X, y).variable("x")
    self.assertEqual(var.domainSize(), 2)
    self.assertAlmostEqual(var.ticks()[1], 6, delta=0.2)

  def testMDLPWithThreeClasses(self):
    rng = np.random.default_rng(3)
    x = rng.uniform(0, 9, size=300)
    X = pd.DataFrame({"x": x})
    y = pd.Series(np.where(x < 3, "a", np.where(x < 6, "b", "c")))
    discretizer = DiscreteTypeProcessor(defaultDiscretizationMethod="MDLP")
    var = discretizer.discretizedTemplate(X, y).variable("x")
    self.assertEqual(var.domainSize(), 3)
    self.assertAlmostEqual(var.ticks()[1], 3, delta=0.2)
    self.assertAlmostEqual(var.ticks()[2], 6, delta=0.2)

  def testCAIMWithThreeClasses(self):
    rng = np.random.default_rng(4)
    x = rng.uniform(0, 9, size=300)
    X = pd.DataFrame({"x": x})
    y = pd.Series(np.where(x < 3, 0, np.where(x < 6, 1, 2)))
    discretizer = DiscreteTypeProcessor(defaultDiscretizationMethod="CAIM")
    var = discretizer.discretizedTemplate(X, y).variable("x")
    # at least as many intervals as classes, and the two class boundaries are found
    self.assertGreaterEqual(var.domainSize(), 3)
    for boundary in (3, 6):
      self.assertTrue(any(abs(t - boundary) < 0.2 for t in var.ticks()[1:-1]))

  @staticmethod
  def _nmlCriterion(cuts, candidates, counts, epsilon, E):
    # criterion (24) of Kontkanen and Myllymaki (2007) for the histogram whose bins end at candidates[cuts]
    n = int(counts[-1])

    def regret(K, h):
      if h == 0 or K == 1:
        return 1.0
      c1 = 1.0
      c2 = sum(math.comb(h, k) * (k / h) ** k * ((h - k) / h) ** (h - k) for k in range(h + 1))
      for j in range(3, K + 1):
        c1, c2 = c2, c2 + h * c1 / (j - 2)
      return c2

    value, previous = 0.0, 0
    for e in cuts:
      h = counts[e] - counts[previous]
      if h > 0:
        value -= h * (math.log(epsilon * h) - math.log((candidates[e] - candidates[previous]) * n))
      previous = e
    K = len(cuts)
    # E counts both end points, hence E - 2 inner candidate cut points
    return value + math.log(regret(K, n)) + math.log(math.comb(E - 2, K - 1))

  def testNMLIsOptimal(self):
    # exhaustive search over all the cut point sets: NML must return the one with the smallest criterion
    rng = np.random.default_rng(5)
    values = np.array([0.0, 1.0, 2.0, 3.0, 5.0, 6.0, 7.0, 9.0])
    kMax = 4
    for _ in range(10):
      x = rng.choice(values, size=40, p=rng.dirichlet(np.ones(len(values))))
      possible = np.unique(x)
      # candidate cut points: midpoints of consecutive values, range extended by half the smallest gap on both sides
      epsilon = np.amin(possible[1:] - possible[:-1])
      candidates = np.concatenate(
        [[possible[0] - epsilon / 2], (possible[1:] + possible[:-1]) / 2, [possible[-1] + epsilon / 2]]
      )
      counts = np.array([np.sum(x <= c) for c in candidates])
      E = len(candidates)
      best = min(
        (self._nmlCriterion(list(inner) + [E - 1], candidates, counts, epsilon, E), list(inner) + [E - 1])
        for K in range(2, kMax + 1)
        for inner in itertools.combinations(range(1, E - 1), K - 1)
      )
      expected = [candidates[0]] + [candidates[e] for e in best[1]]
      discretizer = DiscreteTypeProcessor(discretizationThreshold=1)
      discretizer.setDiscretizationParameters("x", "NML", kMax)
      found = discretizer.discretizedTemplate(pd.DataFrame({"x": x})).variable("x").ticks()
      np.testing.assert_allclose(found, expected)

  def testNMLWithManySamples(self):
    # more than a thousand samples in a bin: the regret must stay finite
    rng = np.random.default_rng(6)
    x = np.round(np.concatenate([rng.normal(0, 1, size=1000), rng.normal(10, 1, size=1000)]), 1)
    X = pd.DataFrame({"x": x})
    discretizer = DiscreteTypeProcessor(defaultDiscretizationMethod="NML", discretizationThreshold=10)
    ticks = discretizer.discretizedTemplate(X).variable("x").ticks()
    self.assertTrue(np.all(np.isfinite(ticks)))
    # the bin that contains the gap between the two clusters contains few samples
    counts, _ = np.histogram(x, bins=ticks)
    self.assertLess(counts[np.searchsorted(ticks, 5) - 1], 0.05 * len(x))

  def testElbowMethod(self):
    rng = np.random.default_rng(2)
    X = pd.DataFrame({"z": rng.uniform(0, 1, size=300)})
    discretizer = DiscreteTypeProcessor(defaultDiscretizationMethod="uniform", defaultNumberOfBins="elbowMethod")
    var = discretizer.discretizedTemplate(X).variable("z")
    self.assertTrue(2 <= var.domainSize() <= 15)

    # three well separated clusters: the elbow is at three bins
    X = pd.DataFrame({"z": np.concatenate([rng.normal(m, 0.1, size=100) for m in (0, 5, 10)])})
    discretizer = DiscreteTypeProcessor(defaultDiscretizationMethod="kmeans", defaultNumberOfBins="elbowMethod")
    self.assertEqual(discretizer.discretizedTemplate(X).variable("z").domainSize(), 3)


ts = unittest.TestSuite()
addTests(ts, DiscreteTypeProcessorTestCase)
