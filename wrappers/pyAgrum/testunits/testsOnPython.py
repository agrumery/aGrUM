#! /usr/bin/env python
# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-
from __future__ import print_function

import os
import platform
import sys
from sys import platform as os_platform
import logging

if __name__ == "__main__":
  print("[pyAgrum] Please use 'act test -t quick|all pyAgrum release'.")
  sys.exit(0)

if sys.version_info < (3, 7):
  print("[pyAgrum] python>=3.7 needed.")
  sys.exit(0)

if len(sys.argv) > 1:
  logging.info("[pyAgrum] Adding local pyAgrum's path")
  p = os.getcwd() + "\\" + sys.argv[1]
  sys.path.insert(1, p)  # to force to use local pyAgrum for the tests (and not installed one)

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
  logging.warning("[pyAgrum] pyAgrum.lib.classifier needs pandas and scikit-learn")

from tests import BNDatabaseGeneratorTestSuite
from tests import BNLearnerTestSuite
from tests import BNListenerTestSuite
from tests import ConfigTestSuite
from tests import EssentialGraphTestSuite
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
from tests import MarkovNetTestSuite
from tests import PotentialTestSuite
from tests import PRMexplorerTestSuite
from tests import SamplingTestSuite
from tests import VariablesTestSuite

from tests import CausalASTTestSuite
from tests import CausalDSepTestSuite
from tests import CausalModelTestSuite
from tests import CausalNonRegressionTestSuite

import time

tl = list()
tl.append(AggregatorsForBNTestSuite.ts)
tl.append(AllIncrementalInferenceTestSuite.ts)
tl.append(BayesNetTestSuite.ts)
tl.append(BayesNetFragmentTestSuite.ts)

if pandasFound and sklearnFound:
  tl.append(BNClassifierTestSuite.ts)

tl.append(BNDatabaseGeneratorTestSuite.ts)
tl.append(BNLearnerTestSuite.ts)
tl.append(BNListenerTestSuite.ts)
tl.append(ConfigTestSuite.ts)
tl.append(EssentialGraphTestSuite.ts)
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
tl.append(MarkovNetTestSuite.ts)
tl.append(PotentialTestSuite.ts)
tl.append(PRMexplorerTestSuite.ts)
tl.append(SamplingTestSuite.ts)
tl.append(VariablesTestSuite.ts)
tl.append(CausalASTTestSuite.ts)
tl.append(CausalDSepTestSuite.ts)
tl.append(CausalModelTestSuite.ts)
tl.append(CausalNonRegressionTestSuite.ts)

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
