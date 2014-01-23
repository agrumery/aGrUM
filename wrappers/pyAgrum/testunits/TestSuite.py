#! /usr/bin/env python
# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-
from __future__ import print_function

import unittest
#import SequenceTestSuite
import VariablesTestSuite
import BayesNetTestSuite
import PythonBNListenerTestSuite
import PotentialTestSuite
import LazyPropagationTestSuite
import GibbsTestSuite

import time
import sys

tests = unittest.TestSuite([#SequenceTestSuite.ts, 
                            VariablesTestSuite.ts,
                            BayesNetTestSuite.ts,
                            PythonBNListenerTestSuite.ts,
                            PotentialTestSuite.ts,
                            LazyPropagationTestSuite.ts, GibbsTestSuite.ts
                            ])


print( """
========================
PyAgrum Test Unit Module
========================

using python unittest
""",end='\n',file=sys.stderr)

runner=unittest.TextTestRunner(verbosity=2)

result=runner._makeResult()

startTime=time.time()
tests(result)
duration=time.time()-startTime

result.printErrors() 
runner.stream.writeln(result.separator2) 

failed, errored = map(len, (result.failures, result.errors)) 
errs=failed+errored

print("",end='\n',file=sys.stderr)
print("## Profiling : %5.0f ms ##"%(1000.0*duration),end='\n',file=sys.stderr)
print("Failed %d of %d tests"%(errs,result.testsRun),end='\n',file=sys.stderr)
print("Success rate: %d%%"%(((result.testsRun-errs)*100)/result.testsRun),end='\n',file=sys.stderr)
print("",end='\n',file=sys.stderr)