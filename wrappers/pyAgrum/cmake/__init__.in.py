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

# selection of imports extracted from dir(.pyAgrum)

from .base import log2
from .base import statsObj
from .base import fastGraph
from .base import config
from .base import DiscretizedVariable, LabelizedVariable, RangeVariable, DiscreteVariable, IntegerVariable, \
  NumericalDiscreteVariable
from .base import Potential, Instantiation, Potential
from .base import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, PDAG, CliqueGraph
from .base import JunctionTreeGenerator
from .base import MeekRules
from .base import ApproximationScheme
from .base import fastVariable
from .base import initRandom, randomProba, randomDistribution, randomGeneratorSeed, randomValue
from .base import isOMP, getNumberOfThreads, getMaxNumberOfThreads, getNumberOfLogicalProcessors, setNumberOfThreads
from .base import VarType_DISCRETIZED, VarType_LABELIZED, VarType_RANGE, VarType_INTEGER, VarType_NUMERICAL
from .base import DefaultInLabel, DuplicateElement, DuplicateLabel, GumException, FatalError, FormatNotFound, \
  GraphError, IOError, InvalidArc, InvalidArgument, InvalidArgumentsNumber, InvalidDirectedCycle, InvalidEdge, \
  InvalidNode, DatabaseError, MissingValueInDatabase, MissingVariableInDatabase, NoChild, NoNeighbour, NoParent, \
  NotFound, NullElement, OperationNotAllowed, OutOfBounds, ArgumentError, SizeError, SyntaxError, UndefinedElement, \
  UndefinedIteratorKey, UndefinedIteratorValue, UnknownLabelInDatabase, CPTError

from .bn import fastBN
from .bn import availableBNExts, loadBN, saveBN
from .bn import randomBN, generateSample
from .bn import mutilateBN
from .bn import BayesNet, BayesNetFragment, EssentialGraph, MarkovBlanket
from .bn import ExactBNdistance, GibbsBNdistance, StructuralComparator
from .bn import LazyPropagation, ShaferShenoyInference, VariableElimination
from .bn import LoopyBeliefPropagation, GibbsSampling, MonteCarloSampling, ImportanceSampling, WeightedSampling
from .bn import LoopyImportanceSampling, LoopyGibbsSampling, LoopyWeightedSampling, LoopyMonteCarloSampling
from .bn import PythonApproximationListener, PythonBNListener, PythonLoadListener, PythonDatabaseGeneratorListener
from .bn import BNGenerator
from .bn import BNDatabaseGenerator, InformationTheory
from .bn import BNLearner
from .bn import PRMexplorer

import sys

try:
  from .mrf import getPosterior
  from .mrf import availableMNExts, loadMN, saveMN
  from .mrf import fastMRF
  from .mrf import MarkovRandomField, ShaferShenoyMRFInference
except ImportError:
  sys.stderr.write("Could not load pyAgrum.mrf")

try:
  from .cn import CredalNet, CNMonteCarloSampling, CNLoopyPropagation
except ImportError:
  sys.stderr.write("Could not load pyAgrum.cn")

try:
  from .id import IDGenerator
  from .id import availableIDExts, loadID, saveID
  from .id import fastID
  from .id import InfluenceDiagram, ShaferShenoyLIMIDInference
except ImportError:
  sys.stderr.write("Could not load pyAgrum.id")

from .deprecated import *
from .common import __version__, __license__, __project_url__, __project_name__, __project_description__, __project__, about
