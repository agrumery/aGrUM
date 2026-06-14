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
  qiskitFound = find_spec("qiskit") is not None and find_spec("qiskit_aer") is not None

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

  tl = []
  module_map: dict[str, str] = {}

  def _reg(suite_mod, mod_name: str) -> None:
    """Append suite to tl and register all test case classes in module_map."""
    if suite_mod is None:
      return
    tl.append(suite_mod.ts)
    for test in suite_mod.ts:
      module_map[type(test).__name__] = mod_name

  # (suite_name, condition [, warning_msg])
  MODULE_SUITES: dict[str, list] = {
    mod: [_try_import(e[0], e[1], e[2] if len(e) > 2 else "") for e in entries]
    for mod, entries in {
      "main": [
        ("AggregatorsForBNTestSuite",          True),
        ("AllIncrementalInferenceTestSuite",    True),
        ("BayesNetTestSuite",                   True),
        ("BayesNetFragmentTestSuite",           True),
        ("BNDatabaseGeneratorTestSuite",        True),
        ("BNLearnerTestSuite",                  True),
        ("BNListenerTestSuite",                 True),
        ("ConfigTestSuite",                     True),
        ("DiscreteTypeProcessorTestSuite",      _pd,     "[pyAgrum] pyagrum.lib.discreteTypeProcessor needs pandas"),
        ("EssentialGraphTestSuite",             True),
        ("EvidenceTestSuite",                   True),
        ("GraphTestSuite",                      True),
        ("GumFormatTestSuite",                  True),
        ("ICIModelsForBNTestSuite",             True),
        ("ImportTestSuite",                     True),
        ("InfluenceDiagramTestSuite",           True),
        ("InformationTheoryTestSuite",          True),
        ("InstantiationTestSuite",              True),
        ("JTInferenceTestSuite",                True),
        ("JunctionTreeTestSuite",               True),
        ("LazyPropagationTestSuite",            True),
        ("LoopyBeliefPropagationTestSuite",     True),
        ("MarkovBlanketTestSuite",              True),
        ("MarkovRandomFieldTestSuite",          True),
        ("PicklerTestSuite",                    True),
        ("PRMexplorerTestSuite",                True),
        ("RandomGeneratorTestSuite",            True),
        ("SamplingTestSuite",                   True),
        ("StructuralMetricsTestSuite",          True),
        ("TensorTestSuite",                     True),
        ("TypeAliasesTestSuite",                True),
        ("VariablesTestSuite",                  True),
        ("WorkaroundTestSuite",                 True),
      ],
      "causaleffect": [
        ("CausalEffectEstimationTestSuite",     _pd,     "[pyAgrum] CausalEffectEstimationTestSuite needs pandas"),
      ],
      "causal": [
        ("CausalDSepTestSuite",                 _pd,     "[pyAgrum] Causal*TestSuite needs pandas"),
        ("CausalImpactTestSuite",               _pd),
        ("CausalModelTestSuite",                _pd),
        ("CausalNonRegressionTestSuite",        _pd),
      ],
      "skbn": [
        ("BNClassifierTestSuite",               _pd_sk,  "[pyAgrum] pyagrum.lib.classifier needs pandas and scikit-learn"),
        ("SkbnTestSuite",                       _pd_sk),
      ],
      "qbn": [
        ("QBNTestSuite",                        qiskitFound, "[pyAgrum] pyagrum.qbn needs qiskit and qiskit-aer"),
      ],
      "ctbn": [
        ("CtbnCimTestSuite",                    True),
        ("CtbnModelTestSuite",                  True),
        ("CtbnTrajectoryTestSuite",             True),
        ("CtbnIndependenceTestSuite",           True),
        ("CtbnLearnerTestSuite",                True),
      ],
      "clg": [
        ("CLGLearningTestSuite",                _pd,     "[pyAgrum] CLG*TestSuite needs pandas"),
        ("CLGModelTestSuite",                   _pd),
        ("CLGSamplingTestSuite",                _pd),
        ("CLGRandomTestSuite",                  _pd),
        ("CLGCanonicalFormTestSuite",           _pd),
        ("CLGInferenceTestSuite",               _pd),
        ("CLGSEMTestSuite",                     _pd),
      ],
      "bnmixture": [
        ("MixtureModelTestSuite",               True),
      ],
      "explain": [
        ("ExplainCausalTestSuite",              _pd,     "[pyAgrum] ExplainCausalTestSuite needs pandas"),
        ("ShapCausalTestSuite",                 _pd,     "[pyAgrum] Shapley(s) needs pandas"),
        ("ShapConditionalTestSuite",            _pd),
        ("ShapCustomCacheTestSuite",            _pd),
        ("ShapMarginalTestSuite",               _pd),
        ("ShallCausalTestSuite",                _pd),
        ("ShallConditionalTestSuite",           _pd),
        ("ShallMarginalTestSuite",              _pd),
      ],
    }.items()
  }

  # reverse maps: suite module name → category name, suite module name → suite module
  _suite_to_module: dict[str, str] = {}
  _suite_by_name: dict[str, object] = {}
  for _mod_name, _suites in MODULE_SUITES.items():
    for _suite in _suites:
      if _suite is not None:
        _key = _suite.__name__.split(".")[-1]
        _suite_to_module[_key] = _mod_name
        _suite_by_name[_key] = _suite

  if test_suite != "":
    _key = test_suite + "TestSuite"
    suite_mod = _suite_by_name.get(_key)
    mod_name = _suite_to_module.get(_key, "main")
    _reg(suite_mod, mod_name)
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
