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
import pandas as pd
from pyagrum.explain import CausalShapValues


class ShapCausalTestCase(pyAgrumTestCase):
  @staticmethod
  def create_data():
    # Load the data
    data = pd.read_csv("tests/resources/iris.csv")
    data.drop(columns="Id", inplace=True)
    data["PetalLengthCm"] = pd.cut(data["PetalLengthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
    data["PetalWidthCm"] = pd.cut(data["PetalWidthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
    data["SepalLengthCm"] = pd.cut(data["SepalLengthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
    data["SepalWidthCm"] = pd.cut(data["SepalWidthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

    # Create the Bayesian Network and the CausalShapValues instance
    learner = gum.BNLearner(data)
    bn = learner.learnBN()

    return bn, data

  def test__shap_1dim(self):
    # BN with known structure: R is not an ancestor of Y.
    # Causal SHAP of R must be zero regardless of R's observed value.
    bn = gum.fastBN(
      "X0[2];X1[2];X2[2];X3[2];Z[2];Y[2];R[5];"
      "X1->X2;X2->Y;X3->Z;Z->Y;X0->Z;X1->Z;X2->R;Z->R;X1->Y"
    )
    df = pd.DataFrame(gum.generateSample(bn, 30, with_labels=True)[0])
    explainer = CausalShapValues(bn, "Y", df)
    for r_val in ["0", "1", "2", "3", "4"]:
      instance = {"X0": "0", "X1": "1", "X2": "0", "X3": "1", "Z": "0", "R": r_val}
      result = explainer.compute((instance, True))
      self.assertAlmostEqual(result._values[1].get("R", 0.0), 0.0, 5)

  def test_shap_ndim(self):
    # BN with known causal structure: X1->X2->Y;X3->Z->Y;X0->Z;X1->Z;X2->R[5];Z->R;X1->Y
    # R is not an ancestor of Y → causal importance must be zero.
    # Ancestors of Y are X0, X1, X2, X3, Z → non-zero importance expected.
    bn = gum.fastBN(
      "X0[2];X1[2];X2[2];X3[2];Z[2];Y[2];R[5];"
      "X1->X2;X2->Y;X3->Z;Z->Y;X0->Z;X1->Z;X2->R;Z->R;X1->Y"
    )
    df = pd.DataFrame(gum.generateSample(bn, 50, with_labels=True)[0])
    explainer = CausalShapValues(bn, "Y", df)
    expl = explainer.compute(df.head(10)).importances[1]
    self.assertAlmostEqual(expl.get("R", 0.0), 0.0, 5)
    ancestor_importance = sum(expl.get(a, 0.0) for a in ["X0", "X1", "X2", "X3", "Z"])
    self.assertGreater(ancestor_importance, 0.0)


  def test_plain_dataframe_syntax(self):
    bn, data = self.create_data()
    bg = data.head(10)
    expl_tuple = CausalShapValues(bn, 4, (bg, True))
    expl_plain = CausalShapValues(bn, 4, bg)

    for i in range(len(expl_tuple.baseline)):
      self.assertAlmostEqual(expl_tuple.baseline[i], expl_plain.baseline[i], 5)

    df = data.head(5)
    imp_tuple = expl_tuple.compute((df, True)).importances[1]
    imp_plain = expl_tuple.compute(df).importances[1]
    for feat in imp_tuple:
      self.assertAlmostEqual(imp_tuple[feat], imp_plain[feat], 5)


ts = unittest.TestSuite()
addTests(ts, ShapCausalTestCase)
