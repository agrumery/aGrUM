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


tests = unittest.TestSuite([SequenceTestSuite.ts, VariablesTestSuite.ts,
                            BayesNetTestSuite.ts,
                            PythonBNListenerTestSuite.ts,
                            PotentialTestSuite.ts, ListTestSuite.ts,
                            LazyPropagationTestSuite.ts, GibbsTestSuite.ts
                            ])

unittest.TextTestRunner(verbosity=2).run(tests)
