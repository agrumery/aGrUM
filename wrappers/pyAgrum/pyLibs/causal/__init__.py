# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin (LIP6), 2020  (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

"""
Causality in pyAgrum mainly consists in the ability to build a causal model, i.e. a (observational) Bayesian network
and a set of latent variables and their relation with observation variables and in the abilidy to compute using
do-calculus the causal impact in such a model.
"""

__author__ = "Pierre-Henri Wuillemin, Paul Alam, Ibrahim Merad"
__copyright__ = "(c) 2019 PARIS"
from ._causalImpact import causalImpact, counterfactualModel, counterfactual
from ._doorCriteria import backdoor_generator,frontdoor_generator
from ._doAST import ASTtree, ASTjointProba, ASTmult, ASTdiv, ASTsum, ASTposteriorProba, ASTminus, ASTplus, ASTBinaryOp
from ._doCalculus import doCalculusWithObservation, identifyingIntervention
from ._CausalFormula import CausalFormula
from ._CausalModel import CausalModel
from ._exceptions import HedgeException, UnidentifiableException
import sys

__all__ = ['CausalModel', 'CausalFormula',
           'causalImpact', 'counterfactualModel', 'counterfactual',
           'doCalculusWithObservation', 'identifyingIntervention',
           'backdoor_generator',
           'HedgeException', 'UnidentifiableException',
           'ASTtree', 'ASTjointProba', 'ASTmult', 'ASTdiv', 'ASTsum', 'ASTposteriorProba', 'ASTminus', 'ASTplus',
           'ASTBinaryOp']
