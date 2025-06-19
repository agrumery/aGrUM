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

import os
from importlib.util import find_spec
import platform
import sys
from sys import platform as os_platform

if __name__ == "__main__":
  print(
    "[pyAgrum] Please use 'act test pyAgrum release {installed|local} -m [module]|quick|quick_[module]|all' -t {all|test_suite_name}'",
    end="\n",
  )
  sys.exit(0)


def runTests(local: bool, test_module: str, test_suite: str, log) -> int:
  if local:
    log.info("[pyAgrum] Adding local pyAgrum's path")
    p = os.getcwd()
    sys.path.insert(1, p)  # to force to use local pyAgrum for the tests (and not installed one)

  if test_suite != "":
    print(f"Test Suite : {test_suite}")
  else:
    print(f"Modules : {test_module if test_module != '' else 'all'}")

  import pyagrum as gum

  res = find_spec("sklearn")
  sklearnFound = res is not None

  res = find_spec("pandas")
  pandasFound = res is not None

  import unittest

  from tests import AggregatorsForBNTestSuite
  from tests import AllIncrementalInferenceTestSuite
  from tests import BayesNetTestSuite
  from tests import BayesNetFragmentTestSuite

  if pandasFound and sklearnFound:
    from tests import BNClassifierTestSuite
  else:
    log.warning("[pyAgrum] pyagrum.lib.classifier needs pandas and scikit-learn")

  from tests import BNDatabaseGeneratorTestSuite
  from tests import BNLearnerTestSuite
  from tests import BNListenerTestSuite
  from tests import ConfigTestSuite

  if pandasFound:
    from tests import DiscreteTypeProcessorTestSuite
  from tests import EssentialGraphTestSuite
  from tests import EvidenceTestSuite
  from tests import GraphTestSuite
  from tests import ICIModelsForBNTestSuite
  from tests import ImportTestSuite
  from tests import InfluenceDiagramTestSuite
  from tests import InstantiationTestSuite
  from tests import JTInferenceTestSuite
  from tests import JunctionTreeTestSuite
  from tests import LazyPropagationTestSuite
  from tests import LoopyBeliefPropagationTestSuite
  from tests import MarkovBlanketTestSuite
  from tests import MarkovRandomFieldTestSuite
  from tests import PicklerTestSuite
  from tests import RandomGeneratorTestSuite
  from tests import TensorTestSuite
  from tests import PRMexplorerTestSuite
  from tests import SamplingTestSuite

  if pandasFound and sklearnFound:
    from tests import SkbnTestSuite
  else:
    log.warning("[pyAgrum] pyagrum.lib.classifier needs pandas and scikit-learn")

  from tests import VariablesTestSuite

  if pandasFound:
    from tests import CausalASTTestSuite
    from tests import CausalDSepTestSuite
    from tests import CausalModelTestSuite
    from tests import CausalNonRegressionTestSuite
    from tests import CausalEffectEstimationTestSuite

  from tests import WorkaroundTestSuite

  from tests import CtbnCimTestSuite
  from tests import CtbnModelTestSuite
  from tests import CtbnTrajectoryTestSuite
  from tests import CtbnIndependenceTestSuite
  from tests import CtbnLearnerTestSuite

  if pandasFound:
    from tests import CLGLearningTestSuite
    from tests import CLGSamplingTestSuite
    from tests import CLGCanonicalFormTestSuite
    from tests import CLGInferenceTestSuite

  from tests import MixtureModelTestSuite

  import time

  tl = list()
  if test_suite != "":
    tl.append(eval(test_suite + "TestSuite.ts"))
  else:
    if test_module in {"", "main"}:
      log.info("testing 'main'")
      tl.append(AggregatorsForBNTestSuite.ts)
      tl.append(AllIncrementalInferenceTestSuite.ts)
      tl.append(BayesNetTestSuite.ts)
      tl.append(BayesNetFragmentTestSuite.ts)
      tl.append(BNDatabaseGeneratorTestSuite.ts)
      tl.append(BNLearnerTestSuite.ts)
      tl.append(BNListenerTestSuite.ts)
      tl.append(ConfigTestSuite.ts)
      if pandasFound:
        tl.append(DiscreteTypeProcessorTestSuite.ts)
      tl.append(EssentialGraphTestSuite.ts)
      tl.append(EvidenceTestSuite.ts)
      tl.append(GraphTestSuite.ts)
      tl.append(ICIModelsForBNTestSuite.ts)
      tl.append(ImportTestSuite.ts)
      tl.append(InfluenceDiagramTestSuite.ts)
      tl.append(InstantiationTestSuite.ts)
      tl.append(JTInferenceTestSuite.ts)
      tl.append(JunctionTreeTestSuite.ts)
      tl.append(LazyPropagationTestSuite.ts)
      tl.append(LoopyBeliefPropagationTestSuite.ts)
      tl.append(MarkovBlanketTestSuite.ts)
      tl.append(MarkovRandomFieldTestSuite.ts)
      tl.append(PicklerTestSuite.ts)
      tl.append(RandomGeneratorTestSuite.ts)
      tl.append(TensorTestSuite.ts)
      tl.append(PRMexplorerTestSuite.ts)
      tl.append(SamplingTestSuite.ts)
      tl.append(VariablesTestSuite.ts)
      tl.append(WorkaroundTestSuite.ts)

    if test_module in {"", "causal"}:
      log.info("testing 'causal'")
      if pandasFound:
        tl.append(CausalASTTestSuite.ts)
        tl.append(CausalDSepTestSuite.ts)
        tl.append(CausalModelTestSuite.ts)
        tl.append(CausalNonRegressionTestSuite.ts)
        tl.append(CausalEffectEstimationTestSuite.ts)
      else:
        log.warning("Pandas or sklearn not found.")

    if test_module in {"", "skbn"}:
      log.info("testing 'skbn'")
      if pandasFound and sklearnFound:
        tl.append(BNClassifierTestSuite.ts)
        tl.append(SkbnTestSuite.ts)
      else:
        log.warning("Pandas or sklearn not found.")

    if test_module in {"", "ctbn"}:
      log.info("testing 'ctbn'")
      tl.append(CtbnCimTestSuite.ts)
      tl.append(CtbnModelTestSuite.ts)
      tl.append(CtbnTrajectoryTestSuite.ts)
      tl.append(CtbnIndependenceTestSuite.ts)
      tl.append(CtbnLearnerTestSuite.ts)

    if test_module in {"", "clg"}:
      log.info("testing 'clg'")
      if pandasFound:
        tl.append(CLGLearningTestSuite.ts)
        tl.append(CLGSamplingTestSuite.ts)
        tl.append(CLGCanonicalFormTestSuite.ts)
        tl.append(CLGInferenceTestSuite.ts)
      else:
        log.warning("Pandas or sklearn not found.")

    if test_module in {"", "bnmixture"}:
      log.info("testing 'bnmixture'")
      tl.append(MixtureModelTestSuite.ts)

  tests = unittest.TestSuite(tl)

  print(
    """
  ========================
  PyAgrum Test Unit Module 
  ========================
  
  using python unittest
  """,
    end="\n",
    file=sys.stdout,
  )

  print(
    "pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform),
    end="\n",
    file=sys.stdout,
  )
  print("pyAgrum path : {}".format(gum.__file__), end="\n", file=sys.stdout)
  print("", end="\n", file=sys.stdout)

  runner = unittest.TextTestRunner(stream=sys.stdout, verbosity=2)

  result = runner._makeResult()

  startTime = time.time()
  tests(result)
  duration = time.time() - startTime

  result.printErrors()
  runner.stream.writeln(result.separator2)

  failed, errored = map(len, (result.failures, result.errors))
  errs = failed + errored

  runner = None

  import gc

  gc.collect()
  gum.statsObj()  # reporting on objects in debug mode

  print("## Profiling : %5.0f ms ##" % (1000.0 * duration), end="\n", file=sys.stdout)
  print("Failed %d of %d tests" % (errs, result.testsRun), end="\n", file=sys.stdout)
  print(
    "Success rate: %d%%" % (((result.testsRun - errs) * 100) / result.testsRun),
    end="\n",
    file=sys.stdout,
  )
  print("", end="\n", file=sys.stdout)
  print(
    "pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform),
    end="\n",
    file=sys.stdout,
  )
  print(
    "pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform),
    end="\n",
    file=sys.stdout,
  )
  print("pyAgrum path : {}".format(gum.__file__), end="\n", file=sys.stdout)
  print("", end="\n", file=sys.stdout)

  return errs
