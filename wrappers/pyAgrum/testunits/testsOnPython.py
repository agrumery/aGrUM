#! /usr/bin/env python
# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-
from __future__ import print_function

import os
import platform
import sys
from sys import platform as os_platform

if len(sys.argv) > 1:
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
  print("[pyAgrum] pyAgrum.lib.classifier needs pandas and sklearn")

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
from tests import PotentialTestSuite
from tests import PRMexplorerTestSuite
from tests import SamplingTestSuite
from tests import VariablesTestSuite

if sys.version_info >= (3, 7):
  from tests import CausalASTTestSuite
  from tests import CausalDSepTestSuite
  from tests import CausalModelTestSuite
  from tests import CausalNonRegressionTestSuite
else:
  print("[pyAgrum] Causality needs python>=3.7")

import time

tests = list()
tests.append(AggregatorsForBNTestSuite.ts)
tests.append(AllIncrementalInferenceTestSuite.ts)
tests.append(BayesNetTestSuite.ts)
tests.append(BayesNetFragmentTestSuite.ts)

if pandasFound and sklearnFound:
  tests.append(BNClassifierTestSuite.ts)

tests.append(BNDatabaseGeneratorTestSuite.ts)
tests.append(BNLearnerTestSuite.ts)
tests.append(BNListenerTestSuite.ts)
tests.append(ConfigTestSuite.ts)
tests.append(EssentialGraphTestSuite.ts)
tests.append(GraphTestSuite.ts)
tests.append(ICIModelsForBNTestSuite.ts)
tests.append(ImportTestSuite.ts)
tests.append(InfluenceDiagramTestSuite.ts)
tests.append(InstantiationTestSuite.ts)
tests.append(JTInferenceTestSuite.ts)
tests.append(JunctionTreeTestSuite.ts)
tests.append(LazyPropagationTestSuite.ts)
tests.append(LoopyBeliefPropagationTestSuite.ts)
tests.append(MarkovBlanketTestSuite.ts)
tests.append(PotentialTestSuite.ts)
tests.append(PRMexplorerTestSuite.ts)
tests.append(SamplingTestSuite.ts)
tests.append(VariablesTestSuite.ts)
if sys.version_info >= (3, 7):
  tests.append(CausalASTTestSuite.ts)
  tests.append(CausalDSepTestSuite.ts)
  tests.append(CausalModelTestSuite.ts)
  tests.append(CausalNonRegressionTestSuite.ts)

tests = unittest.TestSuite(tests)

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
