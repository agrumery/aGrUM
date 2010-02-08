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
#import BayesNet_doubleTestSuite
#import Gibbs_doubleTestSuite
#import LazyPropagation_doubleTestSuite
#import List_doubleTestSuite
#import Potential_doubleTestSuite
#import PythonBNListener_doubleTestSuite


tests = unittest.TestSuite([SequenceTestSuite.ts, VariablesTestSuite.ts,
                            BayesNetTestSuite.ts,
                            PythonBNListenerTestSuite.ts,
                            PotentialTestSuite.ts, ListTestSuite.ts,
                            LazyPropagationTestSuite.ts, GibbsTestSuite.ts
														#,
                            #BayesNet_doubleTestSuite.ts,
                            #Gibbs_doubleTestSuite.ts,
                            #LazyPropagation_doubleTestSuite.ts,
                            #List_doubleTestSuite.ts,
                            #Potential_doubleTestSuite.ts,
                            #PythonBNListener_doubleTestSuite.ts
														])

unittest.TextTestRunner(verbosity=2).run(tests)
