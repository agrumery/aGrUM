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
# e.g. importing pyagrum.markov_random_field would end up also importing
# credal_net/influence_diagram/causal_model.
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
  "getNumberOfThreads", "getPosterior", "initRandom", "isOMP", "loadBN", "log2", "mutilateBN", "pyagrumcpp",
  "randomBN", "randomDistribution", "randomGeneratorSeed", "randomProba", "randomValue",
  "saveBN", "setNumberOfThreads", "statsObj",
]

# Windows has no RPATH mechanism: a DLL's search path must be extended explicitly,
# once per process, before the first DLL that needs it is loaded. libagrumBASE.dll/
# libagrumBN.dll are installed alongside this file (see the
# install(TARGETS agrumBASE agrumBN ...) call in wrappers/pyagrum/CMakeLists.txt) --
# add_dll_directory here covers _pyagrumcpp.pyd below *and* every split submodule's
# _<mod>.pyd, since Windows DLL search directories are process-wide, not per-import.
import sys as _sys

if _sys.platform == "win32":
  import os as _os

  _os.add_dll_directory(_os.path.dirname(__file__))
  del _os
del _sys

# --- Cleaner tracebacks for exceptions raised from the C++ core ----------
# SWIG's kwargs/proxy layer (e.g. fastBN -> BayesNet.fastPrototype ->
# pyagrumcpp.BayesNet_fastPrototype, see wrappers/pyagrum/swigsrc/exceptions.i)
# adds one Python frame per pass-through wrapper before an exception reaches
# user code. Those frames carry no information -- they are one-line
# `return _xxxcpp.Foo(*args)` calls generated by SWIG -- but clutter every
# traceback. We strip frames coming from pyAgrum's own generated proxy
# modules (pyagrumcpp, mrfcpp, cncpp, idcpp, cmcpp, ktbncpp, prmcpp -- all
# named "<mod>cpp") before display, in both plain Python (sys.excepthook)
# and IPython/Jupyter (set_custom_exc). Set PYAGRUM_FULL_TRACEBACK=1 to see
# the untouched traceback, e.g. when debugging pyAgrum itself rather than a
# user script.
import os as _os
import sys as _sys
import types as _types


_PYAGRUM_WRAPPER_MODULES = frozenset(
    {"pyagrumcpp", "mrfcpp", "cncpp", "idcpp", "cmcpp", "ktbncpp", "prmcpp"}
)


def _pyagrum_is_wrapper_frame(frame):
  module_name = frame.f_globals.get("__name__", "")
  return module_name.rsplit(".", 1)[-1] in _PYAGRUM_WRAPPER_MODULES


def _pyagrum_filter_traceback(tb):
  # iterative, not recursive: a traceback can be as deep as the user's own
  # call stack (e.g. a RecursionError's), and recursing one Python frame per
  # traceback frame here would risk a second RecursionError while formatting
  # the first one.
  frames = []
  while tb is not None:
    frames.append(tb)
    tb = tb.tb_next

  rest = None
  for frame_tb in reversed(frames):
    if _pyagrum_is_wrapper_frame(frame_tb.tb_frame):
      continue
    rest = _types.TracebackType(rest, frame_tb.tb_frame, frame_tb.tb_lasti, frame_tb.tb_lineno)
  return rest


def _pyagrum_filter_exception(exc):
  seen = set()
  current = exc
  while current is not None and id(current) not in seen:
    seen.add(id(current))
    current.__traceback__ = _pyagrum_filter_traceback(current.__traceback__)
    current = current.__cause__ or current.__context__


if not _os.environ.get("PYAGRUM_FULL_TRACEBACK"):
  _pyagrum_original_excepthook = _sys.excepthook

  def _pyagrum_excepthook(etype, value, tb):
    _pyagrum_filter_exception(value)
    _pyagrum_original_excepthook(etype, value, value.__traceback__)

  _sys.excepthook = _pyagrum_excepthook

  # Jupyter/IPython bypasses sys.excepthook for exceptions raised in a cell,
  # so it needs its own hook, reusing IPython's own display machinery
  # (shell.showtraceback) with the filtered traceback.
  _pyagrum_ipython_module = _sys.modules.get("IPython")
  if _pyagrum_ipython_module is not None:
    _pyagrum_ip_shell = _pyagrum_ipython_module.get_ipython()
    if _pyagrum_ip_shell is not None:

      def _pyagrum_ipython_handler(shell, etype, value, tb, tb_offset=None):
        _pyagrum_filter_exception(value)
        shell.showtraceback((etype, value, value.__traceback__), tb_offset=tb_offset)

      _pyagrum_ip_shell.set_custom_exc((BaseException,), _pyagrum_ipython_handler)
    del _pyagrum_ip_shell
  del _pyagrum_ipython_module

del _os
del _sys

# selection of imports extracted from dir(.pyagrum)

# submodule BASE
from .pyagrumcpp import log2
from .pyagrumcpp import statsObj
# Re-exported so split submodules (mrf/id/cn/cm) can do
# `from pyagrum import _gum_pickle_load, ...` instead of reaching into the
# `pyagrum.pyagrumcpp` SWIG proxy submodule directly.
from .pyagrumcpp import _gum_pickle_load, _gum_pickle_save, _gum_set_name_property
from .pyagrumcpp import fastGraph, fastDiGraph, fastUndiGraph, fastMixedGraph, fastDAG, fastPDAG
from .pyagrumcpp import config
from .pyagrumcpp import (
  DiscretizedVariable,
  LabelizedVariable,
  RangeVariable,
  DiscreteVariable,
  IntegerVariable,
  NumericalDiscreteVariable,
)
from .pyagrumcpp import Tensor, Instantiation, Tensor
from .pyagrumcpp import Arc, Edge, DiGraph, UndiGraph, MixedGraph, DAG, PDAG, PAG, CliqueGraph
from .pyagrumcpp import EdgeMark_Circle, EdgeMark_Tail, EdgeMark_Arrowhead
from .pyagrumcpp import JunctionTreeGenerator
from .pyagrumcpp import MeekRules
from .pyagrumcpp import ApproximationScheme
from .pyagrumcpp import fastVariable
from .pyagrumcpp import (
  initRandom,
  randomProba,
  randomDistribution,
  randomGeneratorSeed,
  randomValue,
)
from .pyagrumcpp import (
  isOMP,
  getNumberOfThreads,
  getMaxNumberOfThreads,
  getNumberOfLogicalProcessors,
  setNumberOfThreads,
)
from .pyagrumcpp import (
  VarType_DISCRETIZED,
  VarType_LABELIZED,
  VarType_RANGE,
  VarType_INTEGER,
  VarType_NUMERICAL,
)
from .pyagrumcpp import (
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
from .pyagrumcpp import fastBN
from .pyagrumcpp import availableBNExts, loadBN, saveBN
from .pyagrumcpp import randomBN, generateSample
from .pyagrumcpp import mutilateBN
from .pyagrumcpp import getPosterior
from .pyagrumcpp import BayesNet, BayesNetFragment, EssentialGraph, MarkovBlanket
from .pyagrumcpp import ExactBNdistance, GibbsBNdistance, MCBNDistance, StructuralMetrics
from .pyagrumcpp import LazyPropagation, ShaferShenoyInference, VariableElimination
from .pyagrumcpp import (
  LoopyBeliefPropagation,
  GibbsSampling,
  MonteCarloSampling,
  ImportanceSampling,
  WeightedSampling,
)
from .pyagrumcpp import (
  LoopyImportanceSampling,
  LoopyGibbsSampling,
  LoopyWeightedSampling,
  LoopyMonteCarloSampling,
)
from .pyagrumcpp import (
  PythonApproximationListener,
  PythonBNListener,
  PythonLoadListener,
  PythonDatabaseGeneratorListener,
)
from .pyagrumcpp import BNGenerator
from .pyagrumcpp import BNDatabaseGenerator, InformationTheory
from .pyagrumcpp import BNLearner

import sys

# submodules MRF, CN, ID, CM, PRM and KTBN are compiled as separate
# extensions (pyagrum.markov_random_field, pyagrum.credal_net,
# pyagrum.influence_diagram, pyagrum.causal_model, pyagrum.prm,
# pyagrum.ktbn), each importable on its own. Backward compat:
# pyagrum.MarkovRandomField etc. keep working without an explicit
# `import pyagrum.markov_random_field` first, via a lazy module __getattr__
# (PEP 562) -- the submodule is only actually imported on first access of
# one of its names, so `import pyagrum` alone stays light (does not pull in
# submodules that are never used).
_LAZY_SUBMODULE_ATTRS = {
  "availableMRFExts": "markov_random_field", "loadMRF": "markov_random_field", "saveMRF": "markov_random_field",
  "fastMRF": "markov_random_field",
  "MarkovRandomField": "markov_random_field", "ShaferShenoyMRFInference": "markov_random_field",

  "CredalNet": "credal_net", "CNMonteCarloSampling": "credal_net", "CNLoopyPropagation": "credal_net",

  "IDGenerator": "influence_diagram",
  "availableIDExts": "influence_diagram", "loadID": "influence_diagram", "saveID": "influence_diagram",
  "fastID": "influence_diagram",
  "InfluenceDiagram": "influence_diagram", "ShaferShenoyLIMIDInference": "influence_diagram",

  "DoorCriteria": "causal_model",
  "CausalModel": "causal_model",
  "CausalImpact": "causal_model",
  "Counterfactual": "causal_model",
  "causalImpact": "causal_model", "counterfactual": "causal_model", "counterfactualModel": "causal_model",

  "PRMexplorer": "prm",

  "KTBNModality": "ktbn", "KTBN": "ktbn", "KTBNGenerator": "ktbn",
  "KTBNInference": "ktbn", "KTBNDatabaseGenerator": "ktbn",
  "KTBNLearner": "ktbn", "KTBNAdaptiveLearner": "ktbn",
  "availableKTBNExts": "ktbn", "loadKTBN": "ktbn", "saveKTBN": "ktbn",
}


def _lazy_import_submodule(submodule_name):
  import importlib
  try:
    submodule = importlib.import_module(f".{submodule_name}", __name__)
  except ImportError as e:
    raise AttributeError(
      f"module {__name__!r}: pyagrum.{submodule_name} failed to import ({e})"
    ) from e

  if submodule_name == "causal_model":
    # CausalModel.causalBN deprecated-alias patch (see _patch_causal_model
    # below): applied here, on first successful lazy import of
    # pyagrum.causal_model, rather than unconditionally at module top-level --
    # CausalModel is no longer an eagerly-imported name once this shim
    # replaces the old direct `from .pyagrumcpp import CausalModel`.
    # Idempotent: safe to run again if causal_model attributes are accessed
    # via several different names.
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
  return (
    BayesNet
    | DAG
    | _lazy_import_submodule("causal_model").CausalModel
    | _lazy_import_submodule("influence_diagram").InfluenceDiagram
  )

def _build_mrf_inference():
  return _lazy_import_submodule("markov_random_field").ShaferShenoyMRFInference

def _build_cn_inference():
  cn = _lazy_import_submodule("credal_net")
  return cn.CNLoopyPropagation | cn.CNMonteCarloSampling

def _build_id_inference():
  return _lazy_import_submodule("influence_diagram").ShaferShenoyLIMIDInference

def _build_pgm():
  mrf = _lazy_import_submodule("markov_random_field")
  id_ = _lazy_import_submodule("influence_diagram")
  cn = _lazy_import_submodule("credal_net")
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
  from .markov_random_field import MarkovRandomField, ShaferShenoyMRFInference
  from .credal_net import CredalNet, CNLoopyPropagation, CNMonteCarloSampling
  from .influence_diagram import InfluenceDiagram, ShaferShenoyLIMIDInference
  from .causal_model import CausalModel

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
