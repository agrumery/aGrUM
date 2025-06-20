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

import pandas as pd
import numpy as np
import math as m

import pyagrum as gum
from pyagrum.lib.shapley import ShapValues

import itertools

# Load the data
data = pd.read_csv('tests/resources/iris.csv')
data['PetalLengthCm'] = pd.cut(data['PetalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['PetalWidthCm'] = pd.cut(data['PetalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalLengthCm'] = pd.cut(data['SepalLengthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)
data['SepalWidthCm'] = pd.cut(data['SepalWidthCm'], 5, right=True, labels=[0, 1, 2, 3, 4], include_lowest=False)

# Create the Bayesian Network and the ShapValues_opt instance
learner = gum.BNLearner(data)
bn = learner.learnBN()
explainer = ShapValues(bn, 5)

class ShapleyTest(pyAgrumTestCase) :
    def test_conditional(self):
        # Test the local_shap function
        # With pandas Series
        instance_1 = pd.Series({'Id': 1, 'PetalLengthCm': 0, 'PetalWidthCm': 0, 'SepalLengthCm': 0, 'SepalWidthCm': 0, 'Species': 0})

        # With pandas DataFrame
        instance_2 = pd.DataFrame({'Id': [1], 'PetalLengthCm': [0], 'PetalWidthCm': [0], 'SepalLengthCm': [1], 'SepalWidthCm': [0], 'Species': [0]})
        instance_3 = pd.DataFrame({'Id': [1], 'PetalLengthCm': [0], 'PetalWidthCm': [0], 'SepalLengthCm': [2], 'SepalWidthCm': [0], 'Species': [0]})

        # With dict
        instance_4 = {'Id': 1, 'PetalLengthCm': 0, 'PetalWidthCm': 0, 'SepalLengthCm': 3, 'SepalWidthCm': 0, 'Species': 0}
        instance_5 = {'Id': 1, 'PetalLengthCm': 0, 'PetalWidthCm': 0, 'SepalLengthCm': 4, 'SepalWidthCm': 0, 'Species': 0}

        assert np.all( np.array( list(explainer.conditional(instance_1).values()) ).round(3) == np.array([0, -0.747, 2.140, -5.413, -6.369]) )
        assert np.all( np.array( list(explainer.conditional(instance_2).values()) ).round(3) == np.array([0, 0.125, 2.140, -6.3, -6.354]) )
        assert np.all( np.array( list(explainer.conditional(instance_3).values()) ).round(3) == np.array([0, 0.703, 2.140, -7.353, -5.88]) )
        assert np.all( np.array( list(explainer.conditional(instance_4).values()) ).round(3) == np.array([0, 0.661, 2.140, -7.659, -5.531]) )
        assert np.all( np.array( list(explainer.conditional(instance_5).values()) ).round(3) == np.array([0, -1.192, 2.140, -5.750, -5.588]) )

        # Testing global explanations.
        combinaitions = list(itertools.product([1], range(5), range(5), range(5), range(5), range(1)))
        df = pd.DataFrame(combinaitions, columns=['Id', 'PetalLengthCm', 'PetalWidthCm', 'SepalLengthCm', 'SepalWidthCm', 'Species'])
        assert np.all( np.array( list(explainer.conditional(df, 1).values()) ).round(3) == np.array([0, 0.627, 2.116, 3.687, 3.384]))

ts = unittest.TestSuite()
addTests(ts, ShapleyTest)
    





    
        
    


    

        

    
