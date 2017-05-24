# -*- coding: utf-8 -*-
"""
`pyAgrum <http://agrum.gitlab.io>`_ is a Python wrapper for the C++ `aGrUM <https://agrum.lip6.fr>`_ library.
It provides a high-level interface to the part of aGrUM allowing to create, handle and make computations into Bayesian Networks.

The module is generated using the `SWIG <https://www.swig.org>`_ interface generator. Custom-written code was added to make the interface more friendly.

pyAgrum includes :
  * API documentation <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/pyAgrum/>,
  * examples as `notebooks <http://www-desir.lip6.fr/~phw/aGrUM/docs/last/notebooks/01-tutorial.ipynb.html>`_,
  * a `website <http://agrum.gitlab.io>`_.
"""

# aGrum Licence (GPL)
# -------------------
#*   This program is free software; you can redistribute it and/or modify  *
#*   it under the terms of the GNU General Public License as published by  *
#*   the Free Software Foundation; either version 2 of the License, or     *
#*   (at your option) any later version.                                   *
#*                                                                         *
#*   This program is distributed in the hope that it will be useful,       *
#*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
#*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
#*   GNU General Public License for more details.                          *
#*                                                                         *
#*   You should have received a copy of the GNU General Public License     *
#*   along with this program; if not, write to the                         *
#*   Free Software Foundation, Inc.,                                       *
#*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

__version__ = '@PYAGRUM_VERSION@'
__license__ = __doc__
__project_url__ = 'http://agrum.gitlab.io'

import numpy as np
from .functions import *

# selection of imports extracted from dir(.pyAgrum)
from .pyAgrum import statsObj
from .pyAgrum import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, CliqueGraph
from .pyAgrum import BayesNet, EssentialGraph, MarkovBlanket
from .pyAgrum import DiscretizedVariable, LabelizedVariable, RangeVariable, DiscreteVariable
from .pyAgrum import Potential, Instantiation, UtilityTable
from .pyAgrum import BruteForceKL, GibbsKL
from .pyAgrum import LoopyBeliefPropagation, GibbsInference, LazyPropagation, ShaferShenoyInference, VariableElimination
from .pyAgrum import PythonApproximationListener, PythonBNListener, PythonLoadListener
from .pyAgrum import BNGenerator, IDGenerator, JTGenerator
from .pyAgrum import BNLearner
from .pyAgrum import InfluenceDiagram, InfluenceDiagramInference
from .pyAgrum import CredalNet, CNMonteCarloSampling, CNLoopyPropagation
from .pyAgrum import PRMexplorer

from .pyAgrum import initRandom, randomProba, randomDistribution
from .pyAgrum import isOMP, setNumberOfThreads, getNumberOfLogicalProcessors, getMaxNumberOfThreads

from .pyAgrum import DefaultInLabel, DuplicateElement, DuplicateLabel, EmptyBSTree, EmptySet, GumException, FatalError, FormatNotFound, GraphError, IOError, IdError, InvalidArc, InvalidArgument, InvalidArgumentsNumber, InvalidDirectedCycle, InvalidEdge, InvalidNode, NoChild, NoNeighbour, NoParent, NotFound, NullElement, OperationNotAllowed, OutOfBounds, OutOfLowerBound, OutOfUpperBound, ReferenceError, SizeError, SyntaxError, UndefinedElement, UndefinedIteratorKey, UndefinedIteratorValue, UnknownLabelInDatabase

# selection of imports extracted from dir(pyAgrum)
__all__=[
  '__version__','__license__','__project_url__',        # here
  'about','availableBNExts','loadBN','saveBN','loadID','getPosterior', # in fuctions.py

  'Arc','Edge','DiGraph','UndiGraph','MixedGraph','DAG','CliqueGraph',
  'BayesNet','EssentialGraph'
  'DiscretizedVariable','LabelizedVariable','RangeVariable','DiscreteVariable',
  'Potential','Instantiation','UtilityTable',
  'BruteForceKL','GibbsKL',
  'LoopyBeliefPropagation','GibbsInference','LazyPropagation','ShaferShenoyInference','VariableElimination',
  'PythonApproximationListener','PythonBNListener','PythonLoadListener',
  'BNGenerator','IDGenerator','JTGenerator',
  'BNLearner',
  'InfluenceDiagram','InfluenceDiagramInference',

  'initRandom','randomProba','randomDistribution',
  'isOMP','setNumberOfThreads','getNumberOfLogicalProcessors','getMaxNumberOfThreads',

  'CredalNet','CNMonteCarloSampling','CNLoopyPropagation',

  'PRMexplorer',

  'DefaultInLabel','DuplicateElement','DuplicateLabel','EmptyBSTree','EmptySet','GumException','FatalError','FormatNotFound','GraphError','IOError','IdError','InvalidArc','InvalidArgument','InvalidArgumentsNumber','InvalidDirectedCycle','InvalidEdge','InvalidNode','NoChild','NoNeighbour','NoParent','NotFound','NullElement','OperationNotAllowed','OutOfBounds','OutOfLowerBound','OutOfUpperBound','ReferenceError','SizeError','SyntaxError','UndefinedElement','UndefinedIteratorKey','UndefinedIteratorValue','UnknownLabelInDatabase'
  ]

# seed is chosen randomly :)
initRandom(0)
