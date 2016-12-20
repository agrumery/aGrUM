#! /usr/bin/env python
# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-
from __future__ import print_function

import os
import sys

import platform
from sys import platform as os_platform

if len(sys.argv) > 1:
  p = os.getcwd() + "\\" + sys.argv[1]
  sys.path.insert(1, p)  # to force to use local pyAgrum for the tests (and not installed one)

import pyAgrum as gum

import unittest

import VariablesTestSuite
import BayesNetTestSuite
import PythonBNListenerTestSuite
import PotentialTestSuite
import JTInferenceTestSuite
import JunctionTreeTestSuite
import GibbsTestSuite
import ICIModelsForBNTestSuite
import BNLearnerTestSuite
import AggregatorsForBNTestSuite
import PRMexplorerTestSuite
import AllIncrementalInferenceTestSuite

import time

tests = list()
#tests.append(VariablesTestSuite.ts)
#tests.append(BayesNetTestSuite.ts)
#tests.append(PythonBNListenerTestSuite.ts)
tests.append(PotentialTestSuite.ts)
#tests.append(JTInferenceTestSuite.ts)
##tests.append(AllIncrementalInferenceTestSuite.ts)
#tests.append(JunctionTreeTestSuite.ts)
#tests.append(GibbsTestSuite.ts)
#tests.append(ICIModelsForBNTestSuite.ts)
#tests.append(BNLearnerTestSuite.ts)
#tests.append(AggregatorsForBNTestSuite.ts)
#tests.append(PRMexplorerTestSuite.ts)

tests = unittest.TestSuite(tests)

print("""
========================
PyAgrum Test Unit Module
========================

using python unittest
""", end='\n', file=sys.stdout)

print()
print("pyAgrum on Python {0} - {1}".format(platform.python_version(), os_platform), end='\n', file=sys.stdout)
print("pyAgrum version : {}".format(gum.__file__), end='\n', file=sys.stdout)

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
