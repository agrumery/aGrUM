############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import inspect
import logging
import os
import unittest

from numpy import ndarray


def addTests(ts, cl):
  """
  adding test methods (which names begin by 'test')of class cl in testsuite ts

  Parameters
  ----------
  ts
    the test suite
  cl
    the test class t odd
  """
  for met, _ in inspect.getmembers(cl):
    if met[0:4] == 'test':
      ts.addTest(cl(met))


class pyAgrumTestCase(unittest.TestCase):
  def __init__(self, *args, **kwargs):
    super(pyAgrumTestCase, self).__init__(*args, **kwargs)
    self.log = logging.getLogger('gumTestLog')
    self.nbLoopForApproximatedTest = 10

  @staticmethod
  def agrumSrcDir(s: str) -> str:
    return f"{os.path.dirname(__file__)}/resources/{s}"

  def assertListsAlmostEqual(self, seq1, seq2, places=7, delta=None):
    sequence = (tuple, list, ndarray)
    if len(seq1) != len(seq2):
      raise AssertionError("%s != %s" % (str(seq1), str(seq2)))
    for i, j in zip(seq1, seq2):
      if isinstance(i, sequence) and isinstance(j, sequence):
        self.assertListsAlmostEqual(i, j, delta)
      else:
        if delta is None:
          self.assertAlmostEqual(i, j, places=places)
        else:
          self.assertAlmostEqual(i, j, delta=delta)
