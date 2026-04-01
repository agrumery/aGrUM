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

import importlib
import os
import platform
import sys
from importlib.util import find_spec
from sys import platform as os_platform

from .gumTestOutput import notif, warn, error, PrefixStream, ModuleAwareResult, notif_module_stats, Timer

if __name__ == "__main__":
  print(
    "[pyAgrum] Please use 'act test pyAgrum release {installed|local} -m all[+nb][-mod1[-mod2…]]|<module>' -t {all|test_suite_name}'",
    end="\n",
  )
  sys.exit(0)


def runTests(local: bool, test_module: str, test_suite: str, log) -> int:
  if local:
    log.info("[pyAgrum] Adding local pyAgrum's path")
    p = os.getcwd()
    sys.path.insert(1, p)  # to force to use local pyAgrum for the tests (and not installed one)

  if test_suite != "":
    notif(f"Test Suite : {test_suite}")
  else:
    notif(f"Modules : {test_module if test_module != '' else 'all'}")

  import pyagrum as gum

  pandasFound = find_spec("pandas") is not None
  sklearnFound = find_spec("sklearn") is not None

  import unittest

  def _try_import(name: str, condition: bool, msg: str = "") -> object:
    """Import `tests.<name>` if condition is met, else log a warning and return None."""
    if not condition:
      if msg:
        log.warning(msg)
      return None
    return importlib.import_module(f".{name}", package="tests")

  _pd = pandasFound
  _pd_sk = pandasFound and sklearnFound

  AggregatorsForBNTestSuite = _try_import(
    "AggregatorsForBNTestSuite",
    True,
  )
  AllIncrementalInferenceTestSuite = _try_import(
    "AllIncrementalInferenceTestSuite",
    True,
  )
  BayesNetTestSuite = _try_import(
    "BayesNetTestSuite",
    True,
  )
  BayesNetFragmentTestSuite = _try_import(
    "BayesNetFragmentTestSuite",
    True,
  )
  BNClassifierTestSuite = _try_import(
    "BNClassifierTestSuite", _pd_sk, "[pyAgrum] pyagrum.lib.classifier needs pandas and scikit-learn"
  )
  BNDatabaseGeneratorTestSuite = _try_import(
    "BNDatabaseGeneratorTestSuite",
    True,
  )
  BNLearnerTestSuite = _try_import(
    "BNLearnerTestSuite",
    True,
  )
  BNListenerTestSuite = _try_import(
    "BNListenerTestSuite",
    True,
  )
  CausalDSepTestSuite = _try_import("CausalDSepTestSuite", _pd, "[pyAgrum] Causal*TestSuite needs pandas")
  CausalEffectEstimationTestSuite = _try_import(
    "CausalEffectEstimationTestSuite", _pd, "[pyAgrum] CausalEffectEstimationTestSuite needs pandas"
  )
  CausalImpactTestSuite = _try_import(
    "CausalImpactTestSuite",
    _pd,
  )
  CausalModelTestSuite = _try_import(
    "CausalModelTestSuite",
    _pd,
  )
  CausalNonRegressionTestSuite = _try_import(
    "CausalNonRegressionTestSuite",
    _pd,
  )
  CLGCanonicalFormTestSuite = _try_import(
    "CLGCanonicalFormTestSuite",
    _pd,
  )
  CLGInferenceTestSuite = _try_import(
    "CLGInferenceTestSuite",
    _pd,
  )
  CLGLearningTestSuite = _try_import("CLGLearningTestSuite", _pd, "[pyAgrum] CLG*TestSuite needs pandas")
  CLGSamplingTestSuite = _try_import(
    "CLGSamplingTestSuite",
    _pd,
  )
  ConfigTestSuite = _try_import(
    "ConfigTestSuite",
    True,
  )
  CtbnCimTestSuite = _try_import(
    "CtbnCimTestSuite",
    True,
  )
  CtbnIndependenceTestSuite = _try_import(
    "CtbnIndependenceTestSuite",
    True,
  )
  CtbnLearnerTestSuite = _try_import(
    "CtbnLearnerTestSuite",
    True,
  )
  CtbnModelTestSuite = _try_import(
    "CtbnModelTestSuite",
    True,
  )
  CtbnTrajectoryTestSuite = _try_import(
    "CtbnTrajectoryTestSuite",
    True,
  )
  DiscreteTypeProcessorTestSuite = _try_import(
    "DiscreteTypeProcessorTestSuite", _pd, "[pyAgrum] pyagrum.lib.discreteTypeProcessor needs pandas"
  )
  EssentialGraphTestSuite = _try_import(
    "EssentialGraphTestSuite",
    True,
  )
  EvidenceTestSuite = _try_import(
    "EvidenceTestSuite",
    True,
  )
  ExplainCausalTest = _try_import("ExplainCausalTest", _pd, "[pyAgrum] ExplainCausalTest needs pandas")
  GraphTestSuite = _try_import(
    "GraphTestSuite",
    True,
  )
  ICIModelsForBNTestSuite = _try_import(
    "ICIModelsForBNTestSuite",
    True,
  )
  ImportTestSuite = _try_import(
    "ImportTestSuite",
    True,
  )
  InfluenceDiagramTestSuite = _try_import(
    "InfluenceDiagramTestSuite",
    True,
  )
  InformationTheoryTestSuite = _try_import(
    "InformationTheoryTestSuite",
    True,
  )
  InstantiationTestSuite = _try_import(
    "InstantiationTestSuite",
    True,
  )
  JTInferenceTestSuite = _try_import(
    "JTInferenceTestSuite",
    True,
  )
  JunctionTreeTestSuite = _try_import(
    "JunctionTreeTestSuite",
    True,
  )
  LazyPropagationTestSuite = _try_import(
    "LazyPropagationTestSuite",
    True,
  )
  LoopyBeliefPropagationTestSuite = _try_import(
    "LoopyBeliefPropagationTestSuite",
    True,
  )
  MarkovBlanketTestSuite = _try_import(
    "MarkovBlanketTestSuite",
    True,
  )
  MarkovRandomFieldTestSuite = _try_import(
    "MarkovRandomFieldTestSuite",
    True,
  )
  MixtureModelTestSuite = _try_import(
    "MixtureModelTestSuite",
    True,
  )
  PicklerTestSuite = _try_import(
    "PicklerTestSuite",
    True,
  )
  PRMexplorerTestSuite = _try_import(
    "PRMexplorerTestSuite",
    True,
  )
  RandomGeneratorTestSuite = _try_import(
    "RandomGeneratorTestSuite",
    True,
  )
  SamplingTestSuite = _try_import(
    "SamplingTestSuite",
    True,
  )
  ShallCausalTestSuite = _try_import(
    "ShallCausalTestSuite",
    _pd,
  )
  ShallConditionalTestSuite = _try_import(
    "ShallConditionalTestSuite",
    _pd,
  )
  ShallMarginalTestSuite = _try_import(
    "ShallMarginalTestSuite",
    _pd,
  )
  ShapCausalTestSuite = _try_import("ShapCausalTestSuite", _pd, "[pyAgrum] Shapley(s) needs pandas")
  ShapConditionalTestSuite = _try_import(
    "ShapConditionalTestSuite",
    _pd,
  )
  ShapCustomCacheTestSuite = _try_import(
    "ShapCustomCacheTestSuite",
    _pd,
  )
  ShapMarginalTestSuite = _try_import(
    "ShapMarginalTestSuite",
    _pd,
  )
  SkbnTestSuite = _try_import(
    "SkbnTestSuite",
    _pd_sk,
  )
  TensorTestSuite = _try_import(
    "TensorTestSuite",
    True,
  )
  VariablesTestSuite = _try_import(
    "VariablesTestSuite",
    True,
  )
  WorkaroundTestSuite = _try_import(
    "WorkaroundTestSuite",
    True,
  )

  tl = []
  module_map: dict[str, str] = {}

  def _reg(suite_mod, mod_name: str) -> None:
    """Append suite to tl and register its test case class in module_map."""
    if suite_mod is None:
      return
    tl.append(suite_mod.ts)
    first = next(iter(suite_mod.ts), None)
    if first is not None:
      module_map[type(first).__name__] = mod_name

  if test_suite != "":
    tl.append(eval(test_suite + "TestSuite.ts"))
  else:
    # Support "all-mod1-mod2" syntax: run all modules except the listed ones.
    # "all" alone is equivalent to "" (run everything).
    excluded_modules: set[str] = set()
    if test_module == "all":
      test_module = ""
    elif test_module.startswith("all-"):
      excluded_modules = set(test_module.split("-")[1:])
      test_module = ""

    def wants(module: str) -> bool:
      return test_module in {"", module} and module not in excluded_modules

    MODULE_SUITES: dict[str, list] = {
      "main": [
        AggregatorsForBNTestSuite,
        AllIncrementalInferenceTestSuite,
        BayesNetTestSuite,
        BayesNetFragmentTestSuite,
        BNDatabaseGeneratorTestSuite,
        BNLearnerTestSuite,
        BNListenerTestSuite,
        ConfigTestSuite,
        DiscreteTypeProcessorTestSuite,
        EssentialGraphTestSuite,
        EvidenceTestSuite,
        GraphTestSuite,
        ICIModelsForBNTestSuite,
        ImportTestSuite,
        InfluenceDiagramTestSuite,
        InformationTheoryTestSuite,
        InstantiationTestSuite,
        JTInferenceTestSuite,
        JunctionTreeTestSuite,
        LazyPropagationTestSuite,
        LoopyBeliefPropagationTestSuite,
        MarkovBlanketTestSuite,
        MarkovRandomFieldTestSuite,
        PicklerTestSuite,
        RandomGeneratorTestSuite,
        TensorTestSuite,
        PRMexplorerTestSuite,
        SamplingTestSuite,
        VariablesTestSuite,
        WorkaroundTestSuite,
      ],
      "causaleffect": [
        CausalEffectEstimationTestSuite,
      ],
      "causal": [
        CausalDSepTestSuite,
        CausalImpactTestSuite,
        CausalModelTestSuite,
        CausalNonRegressionTestSuite,
      ],
      "skbn": [
        BNClassifierTestSuite,
        SkbnTestSuite,
      ],
      "ctbn": [
        CtbnCimTestSuite,
        CtbnModelTestSuite,
        CtbnTrajectoryTestSuite,
        CtbnIndependenceTestSuite,
        CtbnLearnerTestSuite,
      ],
      "clg": [
        CLGLearningTestSuite,
        CLGSamplingTestSuite,
        CLGCanonicalFormTestSuite,
        CLGInferenceTestSuite,
      ],
      "bnmixture": [
        MixtureModelTestSuite,
      ],
      "explain": [
        ExplainCausalTest,
        ShapCausalTestSuite,
        ShapConditionalTestSuite,
        ShapCustomCacheTestSuite,
        ShapMarginalTestSuite,
        ShallCausalTestSuite,
        ShallConditionalTestSuite,
        ShallMarginalTestSuite,
      ],
    }

    for mod_name, suites in MODULE_SUITES.items():
      if wants(mod_name):
        log.info(f"testing '{mod_name}'")
        for suite in suites:
          _reg(suite, mod_name)

  tests = unittest.TestSuite(tl)

  notif()
  notif("========================")
  notif("PyAgrum Test Unit Module")
  notif("========================")
  notif()
  notif("using python unittest")
  notif()
  notif("pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform))
  notif("pyAgrum path : {}".format(gum.__file__))
  notif()

  runner = unittest.TextTestRunner(stream=PrefixStream(module_map), verbosity=2)

  result = ModuleAwareResult(runner.stream, runner.descriptions, runner.verbosity, module_map)

  with Timer() as t:
    tests(result)

  result.printErrors()
  notif(result.separator2)

  failed, errored = map(len, (result.failures, result.errors))
  errs = failed + errored

  runner = None

  import gc

  gc.collect()
  gum.statsObj()  # reporting on objects in debug mode

  notif("## Profiling : %5.0f ms ##" % (1000.0 * t.elapsed))
  notif_module_stats(result.module_stats)
  (error if errs > 0 else notif)("Failed %d of %d tests" % (errs, result.testsRun))
  notif("Success rate: %d%%" % (((result.testsRun - errs) * 100) / result.testsRun))
  notif()
  notif("pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform))
  notif("pyAgrum path : {}".format(gum.__file__))
  notif()

  return errs
