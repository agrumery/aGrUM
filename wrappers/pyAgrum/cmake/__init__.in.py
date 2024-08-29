# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
# (pierre-henri.wuillemin@lip6.fr)

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
`pyAgrum <http://agrum.org>`_ a scientific C++ and Python library dedicated to Bayesian Networks and other Probabilistic Graphical Models. Based on the C++ `aGrUM <https://agrum.lip6.fr>`_ library, it provides a high-level interface to the C++ part of aGrUM allowing to create, manage and perform efficient computations with Bayesian networks and others probabilsitic graphical models (Markov random fields, influence diagrams and LIMIDs, dynamic BN, probabilistic relational models).

The module is generated using the `SWIG <https://www.swig.org>`_ interface generator. Custom-written code was added to make the interface more friendly.

pyAgrum includes :
  * API documentation <https://pyagrum.readthedocs.io>,
  * examples as `notebooks <https://pyagrum.readthedocs.io/en/latest/notebooks/01-Tutorial.html>`_,
  * a `website <http://agrum.org>`_.
"""

from .deprecated import *

# selection of imports extracted from dir(.pyAgrum)

from .pyAgrum import about
from .pyAgrum import fastBN, fastID, fastMRF
from .pyAgrum import availableBNExts, loadBN, saveBN
from .pyAgrum import availableMNExts, loadMN, saveMN
from .pyAgrum import availableIDExts, loadID, saveID
from .pyAgrum import randomBN, generateSample
from .pyAgrum import mutilateBN
from .pyAgrum import getPosterior
from .pyAgrum import log2
from .pyAgrum import statsObj
from .pyAgrum import fastGraph

from .pyAgrum import config

from .pyAgrum import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, PDAG, CliqueGraph
from .pyAgrum import BayesNet, BayesNetFragment, EssentialGraph, MarkovBlanket
from .pyAgrum import MarkovRandomField, ShaferShenoyMRFInference
from .pyAgrum import DiscretizedVariable, LabelizedVariable, RangeVariable, DiscreteVariable, IntegerVariable, \
  NumericalDiscreteVariable
from .pyAgrum import Potential, Instantiation, Potential
from .pyAgrum import ExactBNdistance, GibbsBNdistance, StructuralComparator
from .pyAgrum import LazyPropagation, ShaferShenoyInference, VariableElimination
from .pyAgrum import LoopyBeliefPropagation, GibbsSampling, MonteCarloSampling, ImportanceSampling, WeightedSampling
from .pyAgrum import LoopyImportanceSampling, LoopyGibbsSampling, LoopyWeightedSampling, LoopyMonteCarloSampling
from .pyAgrum import PythonApproximationListener, PythonBNListener, PythonLoadListener, PythonDatabaseGeneratorListener
from .pyAgrum import BNGenerator, IDGenerator, JunctionTreeGenerator
from .pyAgrum import BNLearner, InformationTheory
from .pyAgrum import BNDatabaseGenerator
from .pyAgrum import InfluenceDiagram, ShaferShenoyLIMIDInference
from .pyAgrum import CredalNet, CNMonteCarloSampling, CNLoopyPropagation
from .pyAgrum import PRMexplorer
from .pyAgrum import MeekRules

from .pyAgrum import ApproximationScheme
from .pyAgrum import fastVariable

from .pyAgrum import initRandom, randomProba, randomDistribution, randomGeneratorSeed, randomValue
from .pyAgrum import isOMP, getNumberOfThreads, getMaxNumberOfThreads, getNumberOfLogicalProcessors, setNumberOfThreads

from .pyAgrum import VarType_DISCRETIZED, VarType_LABELIZED, VarType_RANGE, VarType_INTEGER, VarType_NUMERICAL

from .pyAgrum import DefaultInLabel, DuplicateElement, DuplicateLabel, GumException, FatalError, FormatNotFound, \
  GraphError, IOError, InvalidArc, InvalidArgument, InvalidArgumentsNumber, InvalidDirectedCycle, InvalidEdge, \
  InvalidNode, DatabaseError, MissingValueInDatabase, MissingVariableInDatabase, NoChild, NoNeighbour, NoParent, \
  NotFound, NullElement, OperationNotAllowed, OutOfBounds, ArgumentError, SizeError, SyntaxError, UndefinedElement, \
  UndefinedIteratorKey, UndefinedIteratorValue, UnknownLabelInDatabase, CPTError


# selection of imports extracted from dir(pyAgrum)
__all__ = [
  'about',
  'fastBN', 'fastID', 'fastMRF',
  'availableBNExts', 'loadBN', 'saveBN',
  'availableMNExts', 'loadMN', 'saveMN',
  'availableIDExts', 'loadID', 'saveID',
  'randomBN', 'generateSample',
  'mutilateBN',
  'getPosterior',
  'log2',
  'statsObj',
  'fastGraph',

  'config',

  'Arc', 'Edge', 'DiGraph', 'UndiGraph', 'MixedGraph', 'DAG', 'PDAG', 'CliqueGraph',
  'BayesNet', 'BayesNetFragment', 'EssentialGraph', 'MarkovBlanket',
  'MarkovRandomField', 'ShaferShenoyMRFInference',
  'DiscretizedVariable', 'LabelizedVariable', 'RangeVariable', 'DiscreteVariable', 'IntegerVariable',
  'NumericalDiscreteVariable',
  'Potential', 'Instantiation', 'Potential',
  'ExactBNdistance', 'GibbsBNdistance', 'StructuralComparator',
  'LoopyBeliefPropagation', 'GibbsSampling', 'MonteCarloSampling', 'ImportanceSampling', 'WeightedSampling',
  'LoopyImportanceSampling', 'LoopyGibbsSampling', 'LoopyWeightedSampling', 'LoopyMonteCarloSampling'
                                                                            'LazyPropagation',
  'ShaferShenoyInference', 'VariableElimination',
  'PythonApproximationListener', 'PythonBNListener', 'PythonLoadListener', 'PythonDatabaseGeneratorListener',
  'BNGenerator', 'IDGenerator', 'JunctionTreeGenerator',
  'BNLearner', 'InformationTheory',
  'BNDatabaseGenerator',
  'InfluenceDiagram', 'ShaferShenoyLIMIDInference',
  'CredalNet', 'CNMonteCarloSampling', 'CNLoopyPropagation',
  'PRMexplorer',
  'MeekRules',

  'ApproximationScheme',
  'fastVariable',
  'initRandom', 'randomProba', 'randomDistribution', 'randomGeneratorSeed', 'randomValue',

  'isOMP', 'setNumberOfThreads', 'getNumberOfThreads', 'getMaxNumberOfThreads', 'getNumberOfLogicalProcessors',
  # 'getThreadNumber','getNumberOfRunningThreads','getDynamicThreadsNumber','setDynamicThreadsNumber','getNestedParallelism', 'setNestedParallelism',

  'VarType_DISCRETIZED', 'VarType_LABELIZED', 'VarType_RANGE', 'VarType_INTEGER', 'VarType_NUMERICAL',

  'DefaultInLabel', 'DuplicateElement', 'DuplicateLabel', 'GumException', 'FatalError', 'FormatNotFound',
  'GraphError', 'IOError', 'InvalidArc', 'InvalidArgument', 'InvalidArgumentsNumber', 'InvalidDirectedCycle',
  'InvalidEdge', 'InvalidNode', 'DatabaseError', 'MissingValueInDatabase', 'MissingVariableInDatabase', 'NoChild',
  'NoNeighbour', 'NoParent', 'NotFound', 'NullElement', 'OperationNotAllowed', 'OutOfBounds', 'ArgumentError',
  'SizeError', 'SyntaxError', 'UndefinedElement', 'UndefinedIteratorKey', 'UndefinedIteratorValue',
  'UnknownLabelInDatabase', "CPTError",

  "config"
]

__version__ = '@PYAGRUM_VERSION@'
__license__ = __doc__
__project_url__ = 'http://agrum.org'
__project_name__ = 'pyAgrum'
__project_description__ = __doc__
__project__ = __doc__
