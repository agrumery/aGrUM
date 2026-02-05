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
from pyagrum.explain._ComputationCausal import CausalComputation

import numpy as np
import pandas as pd

# Load the data
data = pd.read_csv("tests/resources/iris.csv")
data.drop(columns="Id", inplace=True)
data["PetalLengthCm"] = pd.cut(data["PetalLengthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data["PetalWidthCm"] = pd.cut(data["PetalWidthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data["SepalLengthCm"] = pd.cut(data["SepalLengthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data["SepalWidthCm"] = pd.cut(data["SepalWidthCm"], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)


class ExplainCausalTest(pyAgrumTestCase):
  def test_doCalculus(self):
    bn1 = gum.fastBN("A->B")
    bn2 = gum.fastBN("B->A<-C")
    bn3 = gum.fastBN("B->A<-C; A->D->E<-F")
    doNet1 = CausalComputation._doCalculus(bn1, [bn1.idFromName("A")])
    doNet2 = CausalComputation._doCalculus(bn2, [bn2.idFromName("A")])
    doNet3 = CausalComputation._doCalculus(bn3, [bn3.idFromName("A"), bn3.idFromName("E")])

    assert doNet1.sizeArcs() == 1
    assert doNet2.sizeArcs() == 0
    assert doNet3.sizeArcs() == 1

  def test_chgCpt(self):
    bn = gum.fastBN("X0->X1->X2<-X3")
    CausalComputation._chgCpt(bn, [0, 3], [0, 1])
    assert bn.cpt(0).sum() == 1.0
    assert bn.cpt(0)[0] == 1.0
    assert bn.cpt(3).sum() == 1.0
    assert bn.cpt(3)[1] == 1.0


ts = unittest.TestSuite()
addTests(ts, ExplainCausalTest)
