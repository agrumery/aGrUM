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

# Imports
import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests
import pyagrum as gum
import numpy as np
import pandas as pd
from pyagrum.explain._CausalShapValues import CausalShapValues

# Load the data
data = pd.read_csv('tests/resources/iris.csv')
data.drop(columns='Id', inplace=True)
data['PetalLengthCm'] = pd.cut(data['PetalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['PetalWidthCm'] = pd.cut(data['PetalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalLengthCm'] = pd.cut(data['SepalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalWidthCm'] = pd.cut(data['SepalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

# Create the Bayesian Network and the CausalShapValues instance
learner = gum.BNLearner(data)
bn = learner.learnBN()
explainer = CausalShapValues(bn, 4, (data.head(10), True))

class CausalTest(pyAgrumTestCase) :

    def test_doCalculus(self) :
        bn1 = gum.fastBN("A->B")
        bn2 = gum.fastBN("B->A<-C")
        bn3 = gum.fastBN("B->A<-C; A->D->E<-F")
        doNet1 = explainer._doCalculus(bn1, [bn1.idFromName('A')])
        doNet2 = explainer._doCalculus(bn2, [bn2.idFromName('A')])
        doNet3 = explainer._doCalculus(bn3, [bn3.idFromName('A'), bn3.idFromName('E')])

        assert doNet1.sizeArcs() == 1
        assert doNet2.sizeArcs() == 0
        assert doNet3.sizeArcs() == 1
    
    def test_chgCpt(self) :
        bn = gum.fastBN("X0->X1->X2<-X3")
        explainer._chgCpt(bn, [0, 3], [0, 1])
        assert bn.cpt(0).sum() == 1.
        assert bn.cpt(0)[0] == 1.
        assert bn.cpt(3).sum() == 1.
        assert bn.cpt(3)[1] == 1.
    
    def test_extract(self) :
        assert np.all(explainer._extract([0, 1], [2, 3], [1, 3]) == np.array([[1, 3, 0, 0, 0]]))
        assert np.all(explainer._extract([0, 1], [2, 3], [0, 2]) == np.array([[0, 2, 0, 0, 0]]))
        assert len(explainer._extract([2, 3], [0, 0], [1, 0])) == 0
    
    def test__shap_1dim(self) :
        instance_0 = {'SepalLengthCm': 1, 'SepalWidthCm': 3, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_1 = {'SepalLengthCm': 0, 'SepalWidthCm': 2, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_2 = {'SepalLengthCm': 0, 'SepalWidthCm': 3, 'PetalLengthCm': 0, 'PetalWidthCm': 0}
        instance_3 = {'SepalLengthCm': 0, 'SepalWidthCm': 3, 'PetalLengthCm': 0}
        instance_4 = {'SepalLengthCm': 0, 'SepalWidthCm': 3}


        posterior0 = sum(list(explainer.compute((instance_0, False))._values[1].values())) + explainer.baseline[1]
        posterior1 = sum(list(explainer.compute((instance_1, False))._values[1].values())) + explainer.baseline[1]
        posterior2 = sum(list(explainer.compute((instance_2, False))._values[1].values())) + explainer.baseline[1]
        posterior3 = sum(list(explainer.compute((instance_3, False))._values[1].values())) + explainer.baseline[1]
        posterior4 = sum(list(explainer.compute((instance_4, False))._values[1].values())) + explainer.baseline[1]

        explainer.ie.eraseAllEvidence()

        explainer.ie.updateEvidence(instance_0)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior0, 5), f'{x} ?= {round(posterior0, 5)}'

        explainer.ie.updateEvidence(instance_1)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior1, 5), f'{x} ?= {round(posterior1, 5)}'

        explainer.ie.updateEvidence(instance_2)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior2, 5), f'{x} ?= {round(posterior2, 5)}'

        explainer.ie.eraseAllEvidence()
        explainer.ie.updateEvidence(instance_3)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior3, 5), f'{x} ?= {round(posterior3, 5)}'

        explainer.ie.eraseAllEvidence()
        explainer.ie.updateEvidence(instance_4)
        x = explainer.func( explainer.ie.posterior(explainer.target)[1] ).round(5)
        assert x == round(posterior4, 5), f'{x} ?= {round(posterior4, 5)}'
    
    def test_shap_ndim(self) :
        expl = explainer.compute((data.head(10), True)).importances[1]
        assert round(expl['SepalLengthCm'], 5) == 3.83845
        assert round(expl['SepalWidthCm'], 5) == 4.54750
        assert round(expl['PetalLengthCm'], 5) == 3.14068
        assert round(expl['PetalWidthCm'], 5) == 3.15554
    
ts = unittest.TestSuite()
addTests(ts, CausalTest)
