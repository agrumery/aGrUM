############################################################################
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest

import pydot as dot

from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class WorkaroundTestCase(pyAgrumTestCase):
  """
  Those tests will hopefully failed one day : they check bugs that did need workarounds in pyAgrum's code
  """

  def testExtraNodeBecauseDotParser(self):
    # pydot (and pydot) creates an extra-node with newline as content.
    g = dot.graph_from_dot_data('''graph G {
  A;B;
  A--B;
}''')[0]

    # g should have 2 nodes if no bug while parsing but 3 with the bug.
    # workaround : cf. pyAgrum.lib.notebook.py:255 and everywhere using dot.graph_from_dot_data
    g.del_node('"\\n"')
    self.assertEqual(len(g.get_node_list()), 2)
    # if we try to remove a non-existing node : no problem
    g.del_node('"\\n"')
    self.assertEqual(len(g.get_node_list()), 2)


ts = unittest.TestSuite()
addTests(ts, WorkaroundTestCase)
