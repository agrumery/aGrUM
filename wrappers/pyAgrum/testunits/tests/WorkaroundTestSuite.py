# (c) Copyright by Pierre-Henri Wuillemin, 2023
# (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!
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
