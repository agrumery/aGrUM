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

"""
This module provides classes for explaining predictions and other computations made by the bayesian networks.
"""

__author__ = "Pierre-Henri Wuillemin"
__copyright__ = "(c) 2019-2025 PARIS"

# Shapley Values
from ._ShapConditionalValues import ConditionalShapValues
from ._ShapMarginalValues import MarginalShapValues
from ._ShapCausalValues import CausalShapValues
from ._Explanation import Explanation

# Shall Values
from ._ShallConditionalValues import ConditionalShallValues
from ._ShallMarginalValues import MarginalShallValues
from ._ShallCausalValues import CausalShallValues

# Independence List For Pairs
from ._explIndependenceListForPairs import independenceListForPairs

# Generalized Markov Blanket
from ._explGeneralizedMarkovBlanket import generalizedMarkovBlanket

# Entropy and Mutual Information
from ._explInformationGraph import getInformationGraph, getInformation, showInformation

__all__ = [
    "ConditionalShapValues",
    "MarginalShapValues",
    "CausalShapValues",
    "ConditionalShallValues",
    "MarginalShallValues",
    "CausalShallValues",
    "Explanation",
    "independenceListForPairs",
    "generalizedMarkovBlanket",
    "getInformationGraph",
    "getInformation",
    "showInformation"
]


