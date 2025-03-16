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
Causality in pyAgrum mainly consists in the ability to build a causal model, i.e. a (observational) Bayesian network
and a set of latent variables and their relation with observation variables and in the ability to compute using
do-calculus the causal impact in such a model.
"""

__author__ = "Pierre-Henri Wuillemin, Paul Alam, Ibrahim Merad"
__copyright__ = "(c) 2019-2024 PARIS"

from ._causalImpact import causalImpact, counterfactualModel, counterfactual
from ._doorCriteria import backdoor_generator, frontdoor_generator
from ._doAST import ASTtree, ASTjointProba, ASTmult, ASTdiv, ASTsum, ASTposteriorProba, ASTminus, ASTplus, ASTBinaryOp
from ._doCalculus import doCalculusWithObservation, identifyingIntervention
from ._CausalFormula import CausalFormula
from ._CausalModel import CausalModel
from ._exceptions import HedgeException, UnidentifiableException
from .causalEffectEstimation import CausalEffectEstimation
import sys

__all__ = ['CausalModel', 'CausalFormula', 'CausalEffectEstimation',
           'causalImpact', 'counterfactualModel', 'counterfactual',
           'doCalculusWithObservation', 'identifyingIntervention',
           'backdoor_generator',
           'HedgeException', 'UnidentifiableException',
           'ASTtree', 'ASTjointProba', 'ASTmult', 'ASTdiv', 'ASTsum', 'ASTposteriorProba', 'ASTminus', 'ASTplus',
           'ASTBinaryOp']
