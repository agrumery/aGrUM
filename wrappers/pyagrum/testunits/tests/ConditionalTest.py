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

# Les imports
import unittest
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum as gum
import sys
sys.path.append('..')

from pyagrum.lib._conditional import ConditionalShapValues, CustomShapleyCache

import pandas as pd
import numpy as np

import itertools

# Load the data
data = pd.read_csv('tests/resources/iris.csv')
data['PetalLengthCm'] = pd.cut(data['PetalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['PetalWidthCm'] = pd.cut(data['PetalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalLengthCm'] = pd.cut(data['SepalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalWidthCm'] = pd.cut(data['SepalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

# Create the Bayesian Network and the ConditionalShapValues instance
learner = gum.BNLearner(data)
bn = learner.learnBN()
explainer = ConditionalShapValues(bn, 5)

class ConditionalTest(unittest.TestCase) :
    def test_generate_keys(self):
        nodes = [0, 1, 2]
        target = 5
        feat = 2
        key1, key2, key3 = CustomShapleyCache.generate_keys(bn, target, feat, nodes)
        assert key1 == (0, 1, 2)
        assert key2 == (0, 1)
        assert key3 == (1, 2)

        nodes = [0, 1, 2, 3, 4]
        key1, key2, key3 = CustomShapleyCache.generate_keys(bn, target, feat, nodes)
        assert key1 == (0, 1, 2, 3, 4)
        assert key2 == (0, 1, 3, 4)
        assert key3 == (1, 2, 3, 4)

        nodes = [1, 2]
        key1, key2, key3 = CustomShapleyCache.generate_keys(bn, target, feat, nodes)
        assert key1 == (1, 2)
        assert key2 == (1,)
        assert key3 == (1, 2)

    def test_cache_operations(self) :
        cache = CustomShapleyCache(max_capacity=3)
        cache.set(1, (0, 1), 10)
        assert cache.get(1, (0, 1)) == 10
        cache.set(2, (0, 1), 20)
        assert cache.get(2, (0, 1)) == 20
        cache.set(2, (0, 1, 2), 30) == 30
        assert cache.get(2, (0, 1, 2)) == 30
        cache.set(3, (0, 1, 2, 3), 40)
        assert cache.get(3, (0, 1, 2, 3)) == 40
        assert (cache._current_size == 2)
        try :   # Should be purged due to capacity limit
            cache.get(1, (0, 1))
            assert False
        except KeyError :
            pass

    def test_logit(self):
        # Test the logit function
        assert np.all( explainer._logit(np.array([0, 0.5, 1.])) == np.array( [-np.inf, 0., np.inf] ) )

    def test_identity(self):
        # Test the identity function
        assert explainer._identity(0.5) == 0.5, "Identity of 0.5 should be 0.5"
        assert explainer._identity(1) == 1, "Identity of 1 should be 1"
        assert explainer._identity(0) == 0, "Identity of 0 should be 0"

    def test_coalitions(self):
        # Test the coalitions function
        assert explainer._coalitions([]) == [], "Coalitions of an empty set should be an empty list"
        assert explainer._coalitions([0]) == [[0]],  "Coalitions of [0] should be [0]"
        assert explainer._coalitions([1]) == [[1]],  "Coalitions of [1] should be [1]"
        assert explainer._coalitions([0, 1]) == [[0], [1], [0, 1]], "Coalitions of [0, 1] should be [[0], [1], [0, 1]]"
        assert explainer._coalitions([0, 1, 2]) == [[0], [1], [2], [0, 1], [0, 2], [1, 2], [0, 1, 2]], "Coalitions of [0, 1, 2] should be [[0], [1], [2], [0, 1], [0, 2], [1, 2], [0, 1, 2]]"

    def test_coalition_contribution(self):
        # Test the coalition_contribution function
        data_values = data.to_numpy()[:10]
        cache = CustomShapleyCache()
        cache.set(-1, '', explainer.func( explainer.base_line ))

        ie = gum.LazyPropagation(bn)

        coalitions = explainer._coalitions([0, 1, 2])
        for nodes_id in coalitions:
            for ex, nodes_vals in enumerate(data_values[:, nodes_id]):
                for k, feature in enumerate(nodes_id) :
                    if k == 0 :
                        evidces = {int(k) : int(v) for k, v in zip(nodes_id, nodes_vals)}
                        ie.setEvidence(evidces)

                    posterior_prob_with = explainer.func( ie.posterior(explainer.target).toarray() )

                    ie.eraseEvidence(int(feature))
                    posterior_prob_without = explainer.func( ie.posterior(explainer.target).toarray() )
                    
                    ie.addEvidence(int(feature), int(nodes_vals[k]))

                    contribution = (posterior_prob_with - posterior_prob_without) / explainer._invcoeff_shap(explainer.M - 1, len(nodes_id) - 1)
                    assert np.all( np.round(contribution, 4) == np.round(explainer._coalition_contribution(k, ex, feature, nodes_id, nodes_vals, cache), 4) ), f"Coalition contribution for feature {feature} in coalition {nodes_id} should match expected value" 

    def test_localShap(self):
        # Test the local_shap function
        instance_1 = np.array([1, 0, 0, 0, 0, 0])
        instance_2 = np.array([1, 1, 0, 0, 0, 0])
        instance_3 = np.array([1, 2, 0, 0, 0, 0])
        instance_4 = np.array([1, 3, 0, 0, 0, 0])
        instance_5 = np.array([1, 4, 0, 0, 0, 0])

        assert np.all( np.array(list(explainer.localShap(instance_1, [0, 1, 2, 3, 4], 1).values())).round(3) == np.array([0, -0.747, 2.140, -5.413, -6.369]) )
        assert np.all( np.array(list(explainer.localShap(instance_2, [0, 1, 2, 3, 4], 1).values())).round(3) == np.array([0, 0.125, 2.140, -6.3, -6.354]) )
        assert np.all( np.array(list(explainer.localShap(instance_3, [0, 1, 2, 3, 4], 1).values())).round(3) == np.array([0, 0.703, 2.140, -7.353, -5.88]) )
        assert np.all( np.array(list(explainer.localShap(instance_4, [0, 1, 2, 3, 4], 1).values())).round(3) == np.array([0, 0.661, 2.140, -7.659, -5.531]) )
        assert np.all( np.array(list(explainer.localShap(instance_5, [0, 1, 2, 3, 4], 1).values())).round(3) == np.array([0, -1.192, 2.140, -5.750, -5.588]) )

    def test_globalShap(self):
        # Test the global_shap function
        combinaitions = np.array(list(itertools.product([1], range(5), range(5), range(5), range(5), range(1)))).astype(int)
        assert np.all( np.array(list(explainer.globalShap(combinaitions, [0, 1, 2, 3, 4], 1).values())).round(3) == np.array([0, 0.627, 2.116, 3.687, 3.384]) )

ts = unittest.TestSuite()
addTests(ts, ConditionalTest)

