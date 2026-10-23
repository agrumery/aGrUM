############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import builtins
import os
import subprocess
import sys
import unittest
import warnings

import pyagrum as gum

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class ExceptionNamesTestCase(pyAgrumTestCase):
  # every aGrUM exception is reachable from pyagrum, without shadowing a Python builtin
  EXPORTED = (
    "AbstractScheduleMultiDim",
    "ArgumentError",
    "CPTError",
    "DatabaseError",
    "DefaultInLabel",
    "DuplicateElement",
    "DuplicateLabel",
    "DuplicateScheduleMultiDim",
    "FactoryError",
    "FactoryInvalidState",
    "FatalError",
    "FormatNotFound",
    "GraphError",
    "GumException",
    "GumIOError",
    "GumTypeError",
    "HedgeException",
    "IncompatibleEvidence",
    "IncompatibleScorePrior",
    "InvalidArc",
    "InvalidArgument",
    "InvalidArgumentsNumber",
    "InvalidDirectedCycle",
    "InvalidEdge",
    "InvalidNode",
    "InvalidPartiallyDirectedCycle",
    "IteratorError",
    "LearningError",
    "MissingValueInDatabase",
    "MissingVariableInDatabase",
    "NoChild",
    "NoNeighbour",
    "NoParent",
    "NotFound",
    "NotImplementedYet",
    "NullElement",
    "OperationNotAllowed",
    "OutOfBounds",
    "PRMTypeError",
    "PossiblyIncompatibleScorePrior",
    "ScheduleMultiDimError",
    "ScheduleOperationError",
    "SizeError",
    "UnavailableScheduleOperation",
    "UndefinedElement",
    "UndefinedIteratorKey",
    "UndefinedIteratorValue",
    "UnexecutedScheduleOperation",
    "UnknownLabelInDatabase",
    "UnknownScheduleMultiDim",
    "UnknownScheduleOperation",
    "WrongClassElement",
  )

  def testAllExceptionsAreExported(self):
    for name in self.EXPORTED:
      with self.subTest(name=name):
        self.assertIn(name, gum.__all__)
        self.assertTrue(issubclass(getattr(gum, name), gum.GumException))
        self.assertFalse(hasattr(builtins, name))

  def testNoWrappedExceptionIsForgotten(self):
    wrapped = {
      name for name, obj in vars(gum.pyagrumcpp).items() if isinstance(obj, type) and issubclass(obj, gum.GumException)
    }
    self.assertEqual(wrapped - set(gum.__all__), set())

  def testStarImportDoesNotShadowBuiltins(self):
    code = (
      "from pyagrum import *\n"
      "import builtins\n"
      "assert TypeError is builtins.TypeError\n"
      "assert IOError is builtins.IOError\n"
      "assert SyntaxError is builtins.SyntaxError\n"
    )
    env = dict(os.environ, PYTHONPATH=os.pathsep.join(p for p in sys.path if p))
    res = subprocess.run([sys.executable, "-c", code], check=False, capture_output=True, text=True, env=env)
    self.assertEqual(res.returncode, 0, res.stderr)

  def testRefusedAggregatorParentRaisesGumTypeError(self):
    bn = gum.BayesNet()
    s = bn.addSUM(gum.RangeVariable("S", "", 0, 4))
    l = bn.add(gum.LabelizedVariable("L", "", 2))
    with self.assertRaises(gum.GumTypeError):
      bn.addArc(l, s)
    self.assertFalse(bn.existsArc(l, s))

  def testIOErrorIsRaisedAsGumIOError(self):
    with self.assertRaises(gum.GumIOError):
      gum.loadBN(self.agrumSrcDir("this_file_does_not_exist.bif"))

  def testDeprecatedNames(self):
    with warnings.catch_warnings(record=True) as w:
      warnings.simplefilter("always")
      self.assertIs(gum.IOError, gum.GumIOError)
      self.assertIs(gum.SyntaxError, builtins.SyntaxError)
    self.assertEqual(len(w), 2)
    for warning in w:
      self.assertTrue(issubclass(warning.category, DeprecationWarning))
      self.assertIn("deprecated after 3.2.1", str(warning.message))


ts = unittest.TestSuite()
addTests(ts, ExceptionNamesTestCase)
