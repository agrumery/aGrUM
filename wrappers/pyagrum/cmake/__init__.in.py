############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
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

# selection of imports extracted from dir(.pyagrum)

# submodule BASE
from .pyagrum import log2
from .pyagrum import statsObj
from .pyagrum import fastGraph
from .pyagrum import config
from .pyagrum import (
  DiscretizedVariable,
  LabelizedVariable,
  RangeVariable,
  DiscreteVariable,
  IntegerVariable,
  NumericalDiscreteVariable,
)
from .pyagrum import Tensor, Instantiation, Tensor
from .pyagrum import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, PDAG, CliqueGraph
from .pyagrum import JunctionTreeGenerator
from .pyagrum import MeekRules
from .pyagrum import ApproximationScheme
from .pyagrum import fastVariable
from .pyagrum import (
  initRandom,
  randomProba,
  randomDistribution,
  randomGeneratorSeed,
  randomValue,
)
from .pyagrum import (
  isOMP,
  getNumberOfThreads,
  getMaxNumberOfThreads,
  getNumberOfLogicalProcessors,
  setNumberOfThreads,
)
from .pyagrum import (
  VarType_DISCRETIZED,
  VarType_LABELIZED,
  VarType_RANGE,
  VarType_INTEGER,
  VarType_NUMERICAL,
)
from .pyagrum import (
  DefaultInLabel,
  DuplicateElement,
  DuplicateLabel,
  GumException,
  FatalError,
  FormatNotFound,
  GraphError,
  IOError,
  InvalidArc,
  InvalidArgument,
  InvalidArgumentsNumber,
  InvalidDirectedCycle,
  InvalidEdge,
  InvalidNode,
  DatabaseError,
  MissingValueInDatabase,
  MissingVariableInDatabase,
  NoChild,
  NoNeighbour,
  NoParent,
  NotFound,
  NullElement,
  OperationNotAllowed,
  OutOfBounds,
  ArgumentError,
  SizeError,
  SyntaxError,
  UndefinedElement,
  UndefinedIteratorKey,
  UndefinedIteratorValue,
  UnknownLabelInDatabase,
  CPTError,
)

# submodule BN
from .pyagrum import fastBN
from .pyagrum import availableBNExts, loadBN, saveBN
from .pyagrum import randomBN, generateSample
from .pyagrum import mutilateBN
from .pyagrum import BayesNet, BayesNetFragment, EssentialGraph, MarkovBlanket
from .pyagrum import ExactBNdistance, GibbsBNdistance, StructuralComparator
from .pyagrum import LazyPropagation, ShaferShenoyInference, VariableElimination
from .pyagrum import (
  LoopyBeliefPropagation,
  GibbsSampling,
  MonteCarloSampling,
  ImportanceSampling,
  WeightedSampling,
)
from .pyagrum import (
  LoopyImportanceSampling,
  LoopyGibbsSampling,
  LoopyWeightedSampling,
  LoopyMonteCarloSampling,
)
from .pyagrum import (
  PythonApproximationListener,
  PythonBNListener,
  PythonLoadListener,
  PythonDatabaseGeneratorListener,
)
from .pyagrum import BNGenerator
from .pyagrum import BNDatabaseGenerator, InformationTheory
from .pyagrum import BNLearner
from .pyagrum import PRMexplorer

import sys

# sublodule MRF
try:
  from .pyagrum import getPosterior
  from .pyagrum import availableMRFExts, loadMRF, saveMRF
  from .pyagrum import fastMRF
  from .pyagrum import MarkovRandomField, ShaferShenoyMRFInference
except ImportError:
  sys.stderr.write("** pyagrum ** : Could not load Markov Random Field submodule")

# submodule CN
try:
  from .pyagrum import CredalNet, CNMonteCarloSampling, CNLoopyPropagation
except ImportError:
  sys.stderr.write("** pyagrum ** : Could not load Credal Net submodule")

# submodule ID
try:
  from .pyagrum import IDGenerator
  from .pyagrum import availableIDExts, loadID, saveID
  from .pyagrum import fastID
  from .pyagrum import InfluenceDiagram, ShaferShenoyLIMIDInference
except ImportError:
  sys.stderr.write("** pyagrum ** : Could not load Influence Diagram submodule")

# submodule CM
try:
  from .pyagrum import DoorCriteria
  from .pyagrum import CausalModel
  from .pyagrum import CausalImpact
  from .pyagrum import Counterfactual
  from .pyagrum import causalImpact, counterfactual, counterfactualModel
except ImportError:
  sys.stderr.write("** pyagrum ** : Could not load Causal submodule")

from .common import (
  __version__,
  __license__,
  __project_url__,
  __project_name__,
  __project_description__,
  __project__,
  about,
)

# deprecated
from .deprecated import deprecated_arg
import warnings

def deprecatedCausalBN(obj,*args,**kwargs):
  """
  Deprecated alias for ``causalDAG``. Use :meth:`CausalModel.causalDAG` instead.

  .. deprecated:: 2.3.2
  """
  warnings.warn("causalBN is deprecated since 2.3.2. Please use causalDAG.", DeprecationWarning, stacklevel=2)
  return obj.causalDAG(*args,**kwargs)

_causalBN_lambda = lambda s: deprecatedCausalBN(s)
_causalBN_lambda.__doc__ = """
Deprecated alias for ``causalDAG``. Use :meth:`causalDAG` instead.

.. deprecated:: 2.3.2
    Use :meth:`causalDAG` instead.

Returns
-------
pyagrum.DAG
    the causal DAG of the model
"""
CausalModel.causalBN = _causalBN_lambda



# type aliases
from typing import NewType, TypeAlias

#: Any directed probabilistic model: BayesNet, DAG, CausalModel or InfluenceDiagram.
DirectedModel: TypeAlias = BayesNet | DAG | CausalModel | InfluenceDiagram

#: Any Bayesian Network inference engine (exact or approximate).
#:
#: Exact: :class:`LazyPropagation`, :class:`ShaferShenoyInference`, :class:`VariableElimination`.
#:
#: Sampling: :class:`GibbsSampling`, :class:`ImportanceSampling`, :class:`WeightedSampling`,
#: :class:`MonteCarloSampling`.
#:
#: Loopy: :class:`LoopyBeliefPropagation`, :class:`LoopyGibbsSampling`,
#: :class:`LoopyImportanceSampling`, :class:`LoopyWeightedSampling`, :class:`LoopyMonteCarloSampling`.
BNInference: TypeAlias = (
    LazyPropagation | ShaferShenoyInference | VariableElimination
    | GibbsSampling | ImportanceSampling | WeightedSampling | MonteCarloSampling
    | LoopyBeliefPropagation | LoopyGibbsSampling | LoopyImportanceSampling
    | LoopyWeightedSampling | LoopyMonteCarloSampling
)

#: Markov Random Field inference engine (:class:`ShaferShenoyMRFInference`).
MRFInference: TypeAlias = ShaferShenoyMRFInference

#: Any Credal Network inference engine: :class:`CNLoopyPropagation` or :class:`CNMonteCarloSampling`.
CNInference: TypeAlias = CNLoopyPropagation | CNMonteCarloSampling

#: Influence Diagram inference engine (:class:`ShaferShenoyLIMIDInference`).
IDInference: TypeAlias = ShaferShenoyLIMIDInference

#: Any graph structure: :class:`DiGraph`, :class:`DAG`, :class:`UndiGraph` or :class:`MixedGraph`.
Graph: TypeAlias = DiGraph | DAG | UndiGraph | MixedGraph

#: Any probabilistic graphical model: :class:`BayesNet`, :class:`MarkovRandomField`,
#: :class:`InfluenceDiagram` or :class:`CredalNet`.
PGM: TypeAlias = BayesNet | MarkovRandomField | InfluenceDiagram | CredalNet

NodeId = NewType("NodeId", int)
NodeList = list[NodeId]
NodeSet = set[NodeId]

ArcSet = set[tuple[NodeId, NodeId]]
NameSet = set[str]

LatentDescriptor = tuple[str, tuple[str, str]]
LatentDescriptorList = list[LatentDescriptor]
