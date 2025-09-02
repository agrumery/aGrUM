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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum as gum
from pyagrum.explain import ConditionalShapValues
import pandas as pd
import numpy as np
import itertools


class ShapConditionalTestCase(pyAgrumTestCase):
  def test__shap_1dim(self):
    bn = gum.fastBN(
      "Id[1,150];SepalLengthCm[5];SepalWidthCm[5];PetalLengthCm[5];PetalWidthCm[5];Species{Iris-setosa|Iris-versicolor|Iris-virginica};PetalLengthCm->SepalLengthCm;Species->PetalLengthCm;PetalWidthCm->Species;Species->SepalWidthCm;Id"
    )
    explainer = ConditionalShapValues(bn, 5)

    instance_0 = {"SepalLengthCm": 1, "SepalWidthCm": 3, "PetalLengthCm": 0, "PetalWidthCm": 0}
    instance_1 = {"SepalLengthCm": 0, "SepalWidthCm": 2, "PetalLengthCm": 0, "PetalWidthCm": 0}
    instance_2 = {"SepalLengthCm": 0, "SepalWidthCm": 3, "PetalLengthCm": 0, "PetalWidthCm": 0}
    instance_3 = {"SepalLengthCm": 0, "SepalWidthCm": 3, "PetalLengthCm": 0}
    instance_4 = {"SepalLengthCm": 0, "SepalWidthCm": 3}

    posterior0 = sum(list(explainer.compute((instance_0, False))._values[1].values())) + explainer.baseline[1]
    posterior1 = sum(list(explainer.compute((instance_1, False))._values[1].values())) + explainer.baseline[1]
    posterior2 = sum(list(explainer.compute((instance_2, False))._values[1].values())) + explainer.baseline[1]
    posterior3 = sum(list(explainer.compute((instance_3, False))._values[1].values())) + explainer.baseline[1]
    posterior4 = sum(list(explainer.compute((instance_4, False))._values[1].values())) + explainer.baseline[1]

    explainer.ie.eraseAllEvidence()

    explainer.ie.updateEvidence(instance_0)
    x = explainer.func(explainer.ie.posterior(explainer.target)[1])
    self.assertAlmostEqual(x, posterior0, 5)

    explainer.ie.updateEvidence(instance_1)
    x = explainer.func(explainer.ie.posterior(explainer.target)[1])
    self.assertAlmostEqual(x, posterior1, 5)

    explainer.ie.updateEvidence(instance_2)
    x = explainer.func(explainer.ie.posterior(explainer.target)[1])
    self.assertAlmostEqual(x, posterior2, 5)

    explainer.ie.eraseAllEvidence()
    explainer.ie.updateEvidence(instance_3)
    x = explainer.func(explainer.ie.posterior(explainer.target)[1])
    self.assertAlmostEqual(x, posterior3, 5)

    explainer.ie.eraseAllEvidence()
    explainer.ie.updateEvidence(instance_4)
    x = explainer.func(explainer.ie.posterior(explainer.target)[1])
    self.assertAlmostEqual(x, posterior4, 5)

  def test_globalShap(self):
    # Load the data
    data = pd.read_csv("tests/resources/iris.csv")
    data["PetalLengthCm"] = pd.cut(data["PetalLengthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
    data["PetalWidthCm"] = pd.cut(data["PetalWidthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
    data["SepalLengthCm"] = pd.cut(data["SepalLengthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
    data["SepalWidthCm"] = pd.cut(data["SepalWidthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

    # Create the Bayesian Network and the ConditionalShapValues instance
    learner = gum.BNLearner(data)
    bn = learner.learnBN()
    explainer = ConditionalShapValues(bn, 5)

    # Test the global_shap function
    combinations = np.array(list(itertools.product([1], range(5), range(5), range(5), range(5), range(1)))).astype(int)
    df = pd.DataFrame(
      combinations, columns=["Id", "SepalLengthCm", "SepalWidthCm", "PetalLengthCm", "PetalWidthCm", "Species"]
    )
    expl = explainer.compute((df, False))
    self.assertAlmostEqual(expl.importances[1]["Id"], 0.0, 5)
    self.assertAlmostEqual(expl.importances[1]["SepalLengthCm"], 0.62684, 5)
    self.assertAlmostEqual(expl.importances[1]["SepalWidthCm"], 2.11577, 5)
    self.assertAlmostEqual(expl.importances[1]["PetalLengthCm"], 3.68744, 5)
    self.assertAlmostEqual(expl.importances[1]["PetalWidthCm"], 3.38405, 5)


ts = unittest.TestSuite()
addTests(ts, ShapConditionalTestCase)
