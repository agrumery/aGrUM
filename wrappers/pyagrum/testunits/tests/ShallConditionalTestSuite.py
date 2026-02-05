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

import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum as gum
from pyagrum.explain import ConditionalShallValues

import pandas as pd


class ShallConditionalTest(pyAgrumTestCase):
  @staticmethod
  def create_data():
    # Load the data
    data = pd.read_csv("tests/resources/iris.csv", index_col="Id").drop(columns=["Species"])

    data["PetalLengthCm"] = pd.cut(data["PetalLengthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
    data["PetalWidthCm"] = pd.cut(data["PetalWidthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
    data["SepalLengthCm"] = pd.cut(data["SepalLengthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
    data["SepalWidthCm"] = pd.cut(data["SepalWidthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

    learner = gum.BNLearner(data)
    bn = learner.learnBN()

    explainer = ConditionalShallValues(bn, background=(data.head(150), True), log=True)

    inst = gum.Instantiation()
    for var in bn.ids(data.columns):
      inst.add(bn.variable(var))

    return bn, data, inst, explainer

  def test__shap_1dim(self):
    bn, data, inst, explainer = ShallConditionalTest.create_data()

    instance_0 = {"SepalLengthCm": 1, "SepalWidthCm": 3, "PetalLengthCm": 0, "PetalWidthCm": 0}
    instance_1 = {"SepalLengthCm": 0, "SepalWidthCm": 2, "PetalLengthCm": 0, "PetalWidthCm": 0}
    instance_2 = {"SepalLengthCm": 1, "SepalWidthCm": 3, "PetalLengthCm": 0, "PetalWidthCm": 0}
    instance_3 = {"SepalLengthCm": 1, "SepalWidthCm": 3, "PetalLengthCm": 0, "PetalWidthCm": 0}
    instance_4 = {"SepalLengthCm": 4, "SepalWidthCm": 3, "PetalLengthCm": 4, "PetalWidthCm": 3}

    joint0 = sum(list(explainer.compute((instance_0, False)).values())) + explainer.baseline
    joint1 = sum(list(explainer.compute((instance_1, False)).values())) + explainer.baseline
    joint2 = sum(list(explainer.compute((instance_2, False)).values())) + explainer.baseline
    joint3 = sum(list(explainer.compute((instance_3, False)).values())) + explainer.baseline
    joint4 = sum(list(explainer.compute((instance_4, False)).values())) + explainer.baseline

    inst.fromdict(instance_0)
    x = explainer.func(bn.jointProbability(inst))
    self.assertAlmostEqual(x, joint0, 5)

    inst.fromdict(instance_1)
    x = explainer.func(bn.jointProbability(inst))
    self.assertAlmostEqual(x, joint1, 5)

    inst.fromdict(instance_2)
    x = explainer.func(bn.jointProbability(inst))
    self.assertAlmostEqual(x, joint2, 5)

    inst.fromdict(instance_3)
    x = explainer.func(bn.jointProbability(inst))
    self.assertAlmostEqual(x, joint3, 5)

    inst.fromdict(instance_4)
    x = explainer.func(bn.jointProbability(inst))
    self.assertAlmostEqual(x, joint4, 5)


ts = unittest.TestSuite()
addTests(ts, ShallConditionalTest)
