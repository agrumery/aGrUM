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
from .base import Tensor, Instantiation, Tensor
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
    from .mrf import availableMRFExts, loadMRF, saveMRF
    from .mrf import fastMRF
    from .mrf import MarkovRandomField, ShaferShenoyMRFInference
except ImportError:
    sys.stderr.write("Could not load pyagrum.mrf")

try:
    from .cn import CredalNet, CNMonteCarloSampling, CNLoopyPropagation
except ImportError:
    sys.stderr.write("Could not load pyagrum.cn")

try:
    from .id import IDGenerator
    from .id import availableIDExts, loadID, saveID
    from .id import fastID
    from .id import InfluenceDiagram, ShaferShenoyLIMIDInference
except ImportError:
    sys.stderr.write("Could not load pyagrum.id")

from .deprecated import *
from .common import __version__, __license__, __project_url__, __project_name__, __project_description__, __project__, about

# deprecated
from .base import Potential
