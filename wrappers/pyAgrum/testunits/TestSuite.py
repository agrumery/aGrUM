#! /usr/bin/env python
# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-

import unittest
import SequenceTestSuite
import VariablesTestSuite
import BayesNetTestSuite
import PythonBNListenerTestSuite
import PotentialTestSuite
import ListTestSuite
import LazyPropagationTestSuite
import GibbsTestSuite

import time

tests = unittest.TestSuite([SequenceTestSuite.ts, VariablesTestSuite.ts,
                            BayesNetTestSuite.ts,
                            PythonBNListenerTestSuite.ts,
                            PotentialTestSuite.ts, ListTestSuite.ts,
                            LazyPropagationTestSuite.ts, GibbsTestSuite.ts
                            ])

print """
========================
PyAgrum Test Unit Module
========================

using python unittest
"""

runner=unittest.TextTestRunner(verbosity=2)

result=runner._makeResult()

startTime=time.time()
tests(result)
duration=time.time()-startTime

result.printErrors() 
runner.stream.writeln(result.separator2) 

failed, errored = map(len, (result.failures, result.errors)) 
errs=failed+errored

print("")
print("## Profiling : %5.0f ms ##"%(1000.0*duration))
print("Failed %d of %d tests"%(errs,result.testsRun))
print("Success rate: %d%%"%(((result.testsRun-errs)*100)/result.testsRun))
print("")
