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

import os
import platform
import sys
from sys import platform as os_platform

if __name__ == "__main__":
  print("[pyAgrum] Please use 'act test pyAgrum release -t [module]|quick|quick_[module]|all'.")
  sys.exit(0)


def runTests(local: bool, test_module, log) -> int:
  if len(sys.argv) > 1:
    log.info("[pyAgrum] Adding local pyAgrum's path")
    p = os.getcwd() + "\\" + sys.argv[1]
    sys.path.insert(1, p)  # to force to use local pyAgrum for the tests (and not installed one)

  print(f"Modules : {test_module if test_module!='' else 'all'}")
  import pyAgrum as gum

  try:
    import sklearn

    sklearnFound = True
  except ImportError:
    sklearnFound = False

  try:
    import pandas

    pandasFound = True
  except ImportError:
    pandasFound = False

  import unittest

  from tests import AggregatorsForBNTestSuite
  from tests import AllIncrementalInferenceTestSuite
  from tests import BayesNetTestSuite
  from tests import BayesNetFragmentTestSuite

  if pandasFound and sklearnFound:
    from tests import BNClassifierTestSuite
  else:
    log.warning("[pyAgrum] pyAgrum.lib.classifier needs pandas and scikit-learn")

  from tests import BNDatabaseGeneratorTestSuite
  from tests import BNLearnerTestSuite
  from tests import BNListenerTestSuite
  from tests import ConfigTestSuite
  if pandasFound:
      from tests import DiscretizerTestSuite
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
  from tests import PotentialTestSuite
  from tests import PRMexplorerTestSuite
  from tests import SamplingTestSuite

  if pandasFound and sklearnFound:
    from tests import SkbnTestSuite
  else:
    log.warning("[pyAgrum] pyAgrum.lib.classifier needs pandas and scikit-learn")

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

  # from tests import MixtureModelTestSuite

  import time

  tl = list()
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
        tl.append(DiscretizerTestSuite.ts)
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
    tl.append(PotentialTestSuite.ts)
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

  tests = unittest.TestSuite(tl)

  print("""
  ========================
  PyAgrum Test Unit Module 
  ========================
  
  using python unittest
  """, end='\n', file=sys.stdout)

  print("pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform), end='\n', file=sys.stdout)
  print("pyAgrum path : {}".format(gum.__file__), end='\n', file=sys.stdout)
  print("", end='\n', file=sys.stdout)

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

  print("## Profiling : %5.0f ms ##" % (1000.0 * duration), end='\n', file=sys.stdout)
  print("Failed %d of %d tests" % (errs, result.testsRun), end='\n', file=sys.stdout)
  print("Success rate: %d%%" % (((result.testsRun - errs) * 100) / result.testsRun), end='\n', file=sys.stdout)
  print("", end='\n', file=sys.stdout)
  print("pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform), end='\n', file=sys.stdout)
  print("pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform), end='\n', file=sys.stdout)
  print("pyAgrum path : {}".format(gum.__file__), end='\n', file=sys.stdout)
  print("", end='\n', file=sys.stdout)

  return errs
