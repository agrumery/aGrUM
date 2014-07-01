#! /usr/bin/env python
# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-
from __future__ import print_function

import sys

# to force to use local pyAgrum for the tests (and not installed one)
sys.path.insert(1,".")

import unittest
#import SequenceTestSuite
import VariablesTestSuite
import BayesNetTestSuite
import PythonBNListenerTestSuite
import PotentialTestSuite
import LazyPropagationTestSuite
import GibbsTestSuite
import ICIModelsForBNTestSuite
import BNLearnerTestSuite   

import time

tests = unittest.TestSuite([
                            VariablesTestSuite.ts,
                            BayesNetTestSuite.ts,
                            PythonBNListenerTestSuite.ts,
                            PotentialTestSuite.ts,
                            LazyPropagationTestSuite.ts, 
                            GibbsTestSuite.ts,
                            ICIModelsForBNTestSuite.ts,
                            BNLearnerTestSuite.ts
                            ])


print( """
========================
PyAgrum Test Unit Module
========================

using python unittest
""",end='\n',file=sys.stdout)

runner=unittest.TextTestRunner(stream=sys.stdout,verbosity=2)

result=runner._makeResult()

startTime=time.time()
tests(result)
duration=time.time()-startTime

result.printErrors() 
runner.stream.writeln(result.separator2) 

failed, errored = map(len, (result.failures, result.errors)) 
errs=failed+errored

print("",end='\n',file=sys.stdout)
print("## Profiling : %5.0f ms ##"%(1000.0*duration),end='\n',file=sys.stdout)
print("Failed %d of %d tests"%(errs,result.testsRun),end='\n',file=sys.stdout)
print("Success rate: %d%%"%(((result.testsRun-errs)*100)/result.testsRun),end='\n',file=sys.stdout)
print("",end='\n',file=sys.stdout)