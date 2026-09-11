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

# Explicit export list. Keeps "from .. import *" -- used by the split
# submodules (pyagrum/mrf/__init__.py etc., see wrappers/pyagrum/CMakeLists.txt)
# to re-export the core -- from also pulling in incidental module-level names
# (stdlib imports, typing helpers) that merely happen to lack a leading
# underscore. Names reachable only through the lazy __getattr__ shim below
# (MarkovRandomField, InfluenceDiagram, DirectedModel, ...) are deliberately
# NOT listed here: "import *" resolves __all__ entries via getattr(), which
# would trigger the shim and pull in other split submodules transitively --
# e.g. importing pyagrum.mrf would end up also importing cn/id/cm.
__all__ = [
  "ApproximationScheme", "Arc", "ArcSet", "ArgumentError", "ArrayLike", "BNDatabaseGenerator",
  "BNGenerator", "BNInference", "BNLearner", "BayesNet", "BayesNetFragment", "CPTError",
  "CliqueGraph", "DAG", "DatabaseError", "DefaultInLabel", "DiGraph", "DiscreteVariable",
  "DiscretizedVariable", "DuplicateElement", "DuplicateLabel", "Edge", "EdgeMark_Arrowhead",
  "EdgeMark_Circle", "EdgeMark_Tail", "EssentialGraph", "ExactBNdistance", "FatalError",
  "FormatNotFound", "GibbsBNdistance", "GibbsSampling", "Graph", "GraphError", "GumException",
  "HedgeException", "IOError", "ImportanceSampling", "InformationTheory", "Instantiation",
  "IntegerVariable", "InvalidArc", "InvalidArgument", "InvalidArgumentsNumber",
  "InvalidDirectedCycle", "InvalidEdge", "InvalidNode", "JunctionTreeGenerator",
  "LabelizedVariable", "LatentDescriptor", "LatentDescriptorList", "LazyPropagation",
  "LoopyBeliefPropagation", "LoopyGibbsSampling", "LoopyImportanceSampling",
  "LoopyMonteCarloSampling", "LoopyWeightedSampling", "MCBNDistance", "MarkovBlanket",
  "MatrixLike", "MeekRules", "MissingValueInDatabase", "MissingVariableInDatabase", "MixedGraph",
  "MonteCarloSampling", "NameSet", "NoChild", "NoNeighbour", "NoParent", "NodeId", "NodeList",
  "NodeSet", "NotFound", "NullElement", "NumericalDiscreteVariable", "OperationNotAllowed",
  "OutOfBounds", "PAG", "PDAG", "PythonApproximationListener", "PythonBNListener",
  "PythonDatabaseGeneratorListener", "PythonLoadListener", "RangeVariable",
  "ShaferShenoyInference", "SizeError", "StructuralMetrics", "SyntaxError", "Tensor",
  "UndefinedElement", "UndefinedIteratorKey", "UndefinedIteratorValue", "UndiGraph",
  "UnknownLabelInDatabase", "VarType_DISCRETIZED", "VarType_INTEGER", "VarType_LABELIZED",
  "VarType_NUMERICAL", "VarType_RANGE", "VariableElimination", "WeightedSampling", "about",
  "availableBNExts", "common", "config", "deprecated", "deprecatedCausalBN", "fastBN", "fastDAG",
  "fastDiGraph", "fastGraph", "fastMixedGraph", "fastPDAG", "fastUndiGraph", "fastVariable",
  "generateSample", "getMaxNumberOfThreads", "getNumberOfLogicalProcessors",
  "getNumberOfThreads", "getPosterior", "initRandom", "isOMP", "loadBN", "log2", "mutilateBN", "pyagrum",
  "randomBN", "randomDistribution", "randomGeneratorSeed", "randomProba", "randomValue",
  "saveBN", "setNumberOfThreads", "statsObj",
]

# Windows has no RPATH mechanism: a DLL's search path must be extended explicitly,
# once per process, before the first DLL that needs it is loaded. libagrumBASE.dll/
# libagrumBN.dll are installed alongside this file (see the
# install(TARGETS agrumBASE agrumBN ...) call in wrappers/pyagrum/CMakeLists.txt) --
# add_dll_directory here covers _pyagrum.pyd below *and* every split submodule's
# _<mod>.pyd, since Windows DLL search directories are process-wide, not per-import.
import sys as _sys

if _sys.platform == "win32":
  import os as _os

  _os.add_dll_directory(_os.path.dirname(__file__))
  del _os
del _sys

# selection of imports extracted from dir(.pyagrum)

# submodule BASE
from .pyagrum import log2
from .pyagrum import statsObj
# Re-exported so split submodules (mrf/id/cn/cm) can do
# `from pyagrum import _gum_pickle_load, ...` instead of reaching into the
# `pyagrum.pyagrum` SWIG proxy submodule directly.
from .pyagrum import _gum_pickle_load, _gum_pickle_save, _gum_set_name_property
from .pyagrum import fastGraph, fastDiGraph, fastUndiGraph, fastMixedGraph, fastDAG, fastPDAG
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
from .pyagrum import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, PDAG, PAG, CliqueGraph
from .pyagrum import EdgeMark_Circle, EdgeMark_Tail, EdgeMark_Arrowhead
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
  HedgeException,
)

# submodule BN
from .pyagrum import fastBN
from .pyagrum import availableBNExts, loadBN, saveBN
from .pyagrum import randomBN, generateSample
from .pyagrum import mutilateBN
from .pyagrum import getPosterior
from .pyagrum import BayesNet, BayesNetFragment, EssentialGraph, MarkovBlanket
from .pyagrum import ExactBNdistance, GibbsBNdistance, MCBNDistance, StructuralMetrics
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

import sys

# submodules MRF, CN, ID, CM are compiled as separate extensions
# (pyagrum.mrf, pyagrum.cn, pyagrum.id, pyagrum.cm), each importable on its
# own. Backward compat: pyagrum.MarkovRandomField etc. keep working without
# an explicit `import pyagrum.mrf` first, via a lazy module __getattr__
# (PEP 562) -- the submodule is only actually imported on first access of
# one of its names, so `import pyagrum` alone stays light (does not pull in
# submodules that are never used).
_LAZY_SUBMODULE_ATTRS = {
  "availableMRFExts": "mrf", "loadMRF": "mrf", "saveMRF": "mrf",
  "fastMRF": "mrf",
  "MarkovRandomField": "mrf", "ShaferShenoyMRFInference": "mrf",

  "CredalNet": "cn", "CNMonteCarloSampling": "cn", "CNLoopyPropagation": "cn",

  "IDGenerator": "id",
  "availableIDExts": "id", "loadID": "id", "saveID": "id",
  "fastID": "id",
  "InfluenceDiagram": "id", "ShaferShenoyLIMIDInference": "id",

  "DoorCriteria": "cm",
  "CausalModel": "cm",
  "CausalImpact": "cm",
  "Counterfactual": "cm",
  "causalImpact": "cm", "counterfactual": "cm", "counterfactualModel": "cm",

  "PRMexplorer": "prm",
}


def _lazy_import_submodule(submodule_name):
  import importlib
  try:
    submodule = importlib.import_module(f".{submodule_name}", __name__)
  except ImportError as e:
    raise AttributeError(
      f"module {__name__!r}: pyagrum.{submodule_name} failed to import ({e})"
    ) from e

  if submodule_name == "cm":
    # CausalModel.causalBN deprecated-alias patch (see _patch_causal_model
    # below): applied here, on first successful lazy import of pyagrum.cm,
    # rather than unconditionally at module top-level -- CausalModel is no
    # longer an eagerly-imported name once this shim replaces the old
    # direct `from .pyagrum import CausalModel`. Idempotent: safe to run
    # again if cm attributes are accessed via several different names.
    _patch_causal_model(submodule.CausalModel)

  return submodule


# Composite type aliases (DirectedModel, PGM, ...) reference classes from
# several split submodules at once. They must resolve to the *real* union
# at runtime too (existing tests do `gum.MRFInference is
# gum.ShaferShenoyMRFInference` / isinstance-style checks), not a typing
# placeholder -- so, unlike _LAZY_SUBMODULE_ATTRS, each one is built by
# actually importing whatever submodule(s) it spans. Accessing one of these
# 5 names is the one case where the lazy shim pulls in more than a single
# submodule; they are rarely-used convenience aliases, not on the
# `import pyagrum` hot path.
def _build_directed_model():
  return BayesNet | DAG | _lazy_import_submodule("cm").CausalModel | _lazy_import_submodule("id").InfluenceDiagram

def _build_mrf_inference():
  return _lazy_import_submodule("mrf").ShaferShenoyMRFInference

def _build_cn_inference():
  cn = _lazy_import_submodule("cn")
  return cn.CNLoopyPropagation | cn.CNMonteCarloSampling

def _build_id_inference():
  return _lazy_import_submodule("id").ShaferShenoyLIMIDInference

def _build_pgm():
  mrf = _lazy_import_submodule("mrf")
  id_ = _lazy_import_submodule("id")
  cn = _lazy_import_submodule("cn")
  return BayesNet | mrf.MarkovRandomField | id_.InfluenceDiagram | cn.CredalNet

_LAZY_COMPOSITE_TYPE_ALIASES = {
  "DirectedModel": _build_directed_model,
  "MRFInference": _build_mrf_inference,
  "CNInference": _build_cn_inference,
  "IDInference": _build_id_inference,
  "PGM": _build_pgm,
}


def __getattr__(name):
  submodule_name = _LAZY_SUBMODULE_ATTRS.get(name)
  if submodule_name is not None:
    submodule = _lazy_import_submodule(submodule_name)
    value = getattr(submodule, name)
    globals()[name] = value  # cache: subsequent access skips __getattr__
    return value

  builder = _LAZY_COMPOSITE_TYPE_ALIASES.get(name)
  if builder is not None:
    value = builder()
    globals()[name] = value  # cache: subsequent access skips __getattr__
    return value

  raise AttributeError(f"module {__name__!r} has no attribute {name!r}")

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


def _patch_causal_model(causal_model_cls):
  causal_model_cls.causalBN = _causalBN_lambda



# type aliases
from typing import NewType, TypeAlias, TYPE_CHECKING
import numpy as np
# pandas is imported only for static type checkers (mypy, pyright): TYPE_CHECKING is False at
# runtime so pandas is never loaded, keeping `import pyagrum` fast (~240 ms saved).
if TYPE_CHECKING:
  import pandas as pd
  #: Matrix-like input: np.ndarray or pd.DataFrame.
  MatrixLike: TypeAlias = np.ndarray | pd.DataFrame
  #: Array-like input: np.ndarray or pd.Series.
  ArrayLike: TypeAlias = np.ndarray | pd.Series
else:
  MatrixLike: TypeAlias = np.ndarray
  ArrayLike: TypeAlias = np.ndarray

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

#: Any graph structure: :class:`DiGraph`, :class:`DAG`, :class:`UndiGraph` or :class:`MixedGraph`.
Graph: TypeAlias = DiGraph | DAG | UndiGraph | MixedGraph

# The next 5 aliases reference classes from the MRF/CN/ID/CM submodules,
# which are lazily imported (see _LAZY_SUBMODULE_ATTRS above) so that
# `import pyagrum` alone does not pull them in. A plain top-level
# `X: TypeAlias = ... | CausalModel | ...` would defeat that (or crash with
# NameError, since these names are no longer eagerly imported). Static type
# checkers (TYPE_CHECKING=True, never execute this module) get the precise
# union here; at runtime, the *same* real union (not a placeholder -- code
# does `isinstance`/`is` checks against these) is built lazily by
# __getattr__ below, via _LAZY_COMPOSITE_TYPE_ALIASES.
if TYPE_CHECKING:
  from .mrf import MarkovRandomField, ShaferShenoyMRFInference
  from .cn import CredalNet, CNLoopyPropagation, CNMonteCarloSampling
  from .id import InfluenceDiagram, ShaferShenoyLIMIDInference
  from .cm import CausalModel

  #: Any directed probabilistic model: BayesNet, DAG, CausalModel or InfluenceDiagram.
  DirectedModel: TypeAlias = BayesNet | DAG | CausalModel | InfluenceDiagram
  #: Markov Random Field inference engine (:class:`ShaferShenoyMRFInference`).
  MRFInference: TypeAlias = ShaferShenoyMRFInference
  #: Any Credal Network inference engine: :class:`CNLoopyPropagation` or :class:`CNMonteCarloSampling`.
  CNInference: TypeAlias = CNLoopyPropagation | CNMonteCarloSampling
  #: Influence Diagram inference engine (:class:`ShaferShenoyLIMIDInference`).
  IDInference: TypeAlias = ShaferShenoyLIMIDInference
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
