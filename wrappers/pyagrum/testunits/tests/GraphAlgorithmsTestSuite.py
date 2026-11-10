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

import contextlib
import io
import textwrap
import unittest

import pyagrum as gum
import pyagrum.graph as gg

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


def _graphs() -> dict:
  """One graph of each class, with the nodes 0 (A) and 1 (B) linked."""
  return {
    "DiGraph": gum.fastDiGraph("A->B"),
    "DAG": gum.fastDAG("A->B"),
    "UndiGraph": gum.fastUndiGraph("A-B"),
    "MixedGraph": gum.fastMixedGraph("A-B"),
    "PDAG": gum.fastPDAG("A-B"),
    "ChainGraph": gum.fastChainGraph("A-B"),
    "CPDAG": gum.CPDAG.fromDAG(gum.fastDAG("A->B")),
    "MPDAG": gum.MPDAG.fromPDAG(gum.fastPDAG("A-B")),
    "EssentialGraph": gum.EssentialGraph(gum.fastBN("A->B")),
    "MarkedGraph": gum.fastMarkedGraph("A->B"),
    "PAG": gum.fastPAG("A->B"),
    "MAG": gum.MAG.fromMarkedGraph(gum.fastMarkedGraph("A->B")),
  }


_A = {"DiGraph", "DAG", "MixedGraph", "PDAG", "ChainGraph", "CPDAG", "MPDAG", "EssentialGraph"}
_E = {"UndiGraph", "MixedGraph", "PDAG", "ChainGraph", "CPDAG", "MPDAG", "EssentialGraph"}
_AE = {"MixedGraph", "PDAG", "ChainGraph", "CPDAG", "MPDAG", "EssentialGraph"}
_K = {"MarkedGraph", "PAG", "MAG"}

# function -> (call, accepted classes)
_CALLS = {
  "topologicalOrder": (lambda g: gg.topologicalOrder(g), _A),
  "hasDirectedCycle": (lambda g: gg.hasDirectedCycle(g), _A),
  "ancestors": (lambda g: gg.ancestors(g, 1), _A | _K),
  "descendants": (lambda g: gg.descendants(g, 0), _A | _K),
  "directedPath": (lambda g: gg.directedPath(g, 0, 1), _A),
  "hasDirectedPath": (lambda g: gg.hasDirectedPath(g, 0, 1), _A),
  "hasUndirectedCycle": (lambda g: gg.hasUndirectedCycle(g), _E),
  "isChordal": (lambda g: gg.isChordal(g), _E),
  "lexBFS": (lambda g: gg.lexBFS(g), _E),
  "maximumCardinalitySearch": (lambda g: gg.maximumCardinalitySearch(g), _E),
  "isPerfectEliminationOrdering": (lambda g: gg.isPerfectEliminationOrdering(g, [0, 1]), _E),
  "undirectedPath": (lambda g: gg.undirectedPath(g, 0, 1), _E),
  "hasPartiallyDirectedCycle": (lambda g: gg.hasPartiallyDirectedCycle(g), _AE),
  "mixedOrientedPath": (lambda g: gg.mixedOrientedPath(g, 0, 1), _AE),
  "degeneracyOrdering": (lambda g: gg.degeneracyOrdering(g), _AE),
  "anteriors": (lambda g: gg.anteriors(g, 1), _AE | _K),
  "isAncestral": (lambda g: gg.isAncestral(g), _K),
  "isMaximal": (lambda g: gg.isMaximal(g), _K),
  "cpdag": (lambda g: gg.cpdag(g), {"DAG"}),
  "consistentExtension": (
    lambda g: gg.consistentExtension(g),
    {"PDAG", "ChainGraph", "CPDAG", "MPDAG", "EssentialGraph"},
  ),
  "meekClosure": (lambda g: gg.meekClosure(g), {"PDAG", "ChainGraph"}),
  "orientAsDAG": (lambda g: gg.orientAsDAG(g, {0: 2, 1: 2}), {"PDAG", "ChainGraph"}),
  "separation": (
    lambda g: gg.separation(g, 0, 1),
    {"DAG", "PDAG", "ChainGraph", "CPDAG", "MPDAG", "EssentialGraph", "MAG"},
  ),
}


def _codeBlocks(doc: str) -> list[str]:
  """The python code blocks of a docstring: the lines indented under ``.. code-block:: python``."""
  blocks = []
  lines = doc.splitlines()
  for i, line in enumerate(lines):
    if line.strip() != ".. code-block:: python":
      continue
    indent = len(line) - len(line.lstrip())
    block = []
    for next_line in lines[i + 1 :]:
      if next_line.strip() and len(next_line) - len(next_line.lstrip()) <= indent:
        break
      block.append(next_line)
    blocks.append(textwrap.dedent("\n".join(block)))
  return blocks


class TestGraphAlgorithms(pyAgrumTestCase):
  def testFamilies(self):
    graphs = _graphs()
    self.assertEqual(set(_CALLS), {name for name in gg.__all__ if name not in ("fromGUMString", "load", "save")})
    for function, (call, accepted) in _CALLS.items():
      for name, g in graphs.items():
        with self.subTest(function=function, graph=name):
          if name in accepted:
            call(g)
          else:
            with self.assertRaises(TypeError) as context:
              call(g)
            self.assertIn(f"{function} needs", str(context.exception))
            self.assertIn(f"got {name}", str(context.exception))

  def testErrorMessage(self):
    with self.assertRaises(TypeError) as context:
      gg.isChordal(gum.MAG.fromMarkedGraph(gum.fastMarkedGraph("A->B")))
    self.assertEqual(
      str(context.exception),
      "isChordal needs a graph with edges (UndiGraph, MixedGraph, PDAG, ChainGraph, CPDAG, MPDAG, EssentialGraph), got MAG",
    )

  def testEachAlgorithmReadsItsFamily(self):
    pdag = gum.fastPDAG("A->B-C-D-A")
    # A: the edges are ignored
    self.assertEqual(gg.descendants(pdag, "A"), {1})
    self.assertFalse(gg.hasDirectedCycle(pdag))
    # E: the arcs are ignored (B-C-D-A is a path, not a cycle)
    self.assertFalse(gg.hasUndirectedCycle(pdag))
    self.assertEqual(gg.undirectedPath(pdag, "A", "B"), [0, 3, 2, 1])
    # AE: both
    self.assertTrue(gg.hasPartiallyDirectedCycle(pdag))

  def testImmutableGraphs(self):
    dag = gum.fastDAG("A->B->C;D->C")
    cpdag = gum.CPDAG.fromDAG(dag)
    mpdag = gum.MPDAG.fromPDAG(cpdag.graph())
    for g in (cpdag, mpdag):
      self.assertEqual(gg.ancestors(g, "C"), gg.ancestors(g.graph(), "C"))
      self.assertEqual(gg.isChordal(g), gg.isChordal(g.graph()))
      self.assertEqual(gg.degeneracyOrdering(g), gg.degeneracyOrdering(g.graph()))
      self.assertEqual(gg.consistentExtension(g), g.consistentExtension())

  def testAncestorsStartFromTheParents(self):
    g = gum.fastDiGraph("A->B->C->D;E->C")
    self.assertEqual(gg.ancestors(g, "C"), {0, 1, 4})
    # B is an ancestor of D, hence of {B, D}
    self.assertEqual(gg.ancestors(g, ["B", "D"]), {0, 1, 2, 4})
    self.assertEqual(gg.descendants(g, {"A", "C"}), {1, 2, 3})
    self.assertEqual(gg.ancestors(g, []), set())
    # a directed cycle
    cycle = gum.fastDiGraph("A->B->A;C->A")
    self.assertEqual(gg.ancestors(cycle, "A"), {0, 1, 2})
    self.assertEqual(gg.descendants(cycle, "A"), {0, 1})
    self.assertEqual(cycle.ancestors(0), {0, 1, 2})
    # marked graphs
    marked = gum.fastMarkedGraph("A-B->C<->D")
    self.assertEqual(gg.ancestors(marked, ["B", "C"]), {1})
    self.assertEqual(gg.anteriors(marked, ["B", "C"]), {0, 1})
    self.assertEqual(gg.anteriors(marked, "D"), set())
    self.assertEqual(gg.descendants(marked, "A"), set())
    self.assertEqual(gg.descendants(marked, "B"), {2})
    self.assertEqual(gg.descendants(gum.fastMarkedGraph("A->B->C<->D"), ["A", "B"]), {1, 2})

  def testAnteriorsFollowPaths(self):
    # going back and forth through an edge is not a cycle; a cycle of edges is one
    for build in (gum.fastMarkedGraph, gum.fastMixedGraph, gum.fastPDAG):
      with self.subTest(build=build.__name__):
        g = build("A-B")
        self.assertEqual(gg.anteriors(g, "A"), {1})
        self.assertEqual(gg.anteriors(g, ["A", "B"]), {0, 1})
    self.assertEqual(gg.anteriors(gum.fastMarkedGraph("A-B-C-A"), "A"), {0, 1, 2})
    self.assertEqual(gg.anteriors(gum.fastMixedGraph("A-B-C-A"), "A"), {0, 1, 2})
    # mixed graphs: arcs followed in their direction and edges
    pdag = gum.fastPDAG("A->B-C;D->A")
    self.assertEqual(gg.anteriors(pdag, "C"), {0, 1, 3})
    self.assertEqual(gg.anteriors(pdag, "A"), {3})
    self.assertEqual(gg.anteriors(pdag, ["A", "C"]), {0, 1, 3})
    # a partially directed cycle A->B-C-A
    self.assertEqual(gg.anteriors(gum.fastMixedGraph("A->B-C-A"), "A"), {0, 1, 2})
    # immutable graphs
    cpdag = gum.CPDAG.fromDAG(gum.fastDAG("A->B->C;D->C"))
    self.assertEqual(gg.anteriors(cpdag, "C"), gg.anteriors(cpdag.graph(), "C"))

  def testNodes(self):
    g = gum.fastDAG("A->B->C")
    self.assertEqual(gg.ancestors(g, 2), gg.ancestors(g, "C"))
    self.assertEqual(gg.ancestors(g, ("B", 2)), {0, 1})
    with self.assertRaises(gum.NotFound):
      gg.ancestors(g, "Z")

  def testOrientation(self):
    dag = gum.fastDAG("A->B->C;D->C")
    cpdag = gg.cpdag(dag)
    self.assertIsInstance(cpdag, gum.CPDAG)
    self.assertEqual(cpdag, gum.CPDAG.fromDAG(dag))
    self.assertIsInstance(gg.consistentExtension(cpdag), gum.DAG)
    self.assertIsNone(gg.consistentExtension(gum.fastPDAG("A->B-C;D->C")))
    self.assertIsNone(gg.meekClosure(gum.fastPDAG("A->B-C;D->C")))
    closure = gg.meekClosure(gum.fastPDAG("A->B-C"))
    self.assertIsInstance(closure, gum.PDAG)
    self.assertEqual(closure.arcs(), {(0, 1), (1, 2)})
    with self.assertRaises(gum.InvalidArgument):
      gg.orientAsDAG(gum.fastPDAG("A->B-C"), {0: 2, 1: 2})

  def testOrderings(self):
    g = gum.fastUndiGraph("A-B-C;B-D")
    with self.assertRaises(gum.InvalidArgument):
      gg.lexBFS(g, first=["A", "C"])
    with self.assertRaises(gum.InvalidArgument):
      gg.isPerfectEliminationOrdering(g, ["A", "B", "C"])
    with self.assertRaises(ValueError):
      gg.isPerfectEliminationOrdering(g, ["A", "B", "C", "C"])
    self.assertTrue(gg.isPerfectEliminationOrdering(g, list(reversed(gg.lexBFS(g)))))

  def testSeparation(self):
    dag = gum.fastDAG("A->C<-B;C->D")
    representations = [
      dag,
      gum.CPDAG.fromDAG(dag),
      gum.MPDAG.fromPDAG(gum.CPDAG.fromDAG(dag).graph()),
      gum.MAG.fromDAG(dag),
    ]
    for Z in (None, "C", "D", ["C", "D"]):
      expected = dag.dSeparation(gg._nodes(dag, "A"), gg._nodes(dag, "B"), gg._nodes(dag, Z))
      for g in representations:
        with self.subTest(graph=type(g).__name__, Z=Z):
          self.assertEqual(gg.separation(g, "A", "B", Z), expected)
    chain = gum.fastChainGraph("A->B-C<-D")
    self.assertTrue(gg.separation(chain, "A", "D"))
    self.assertFalse(gg.separation(chain, "A", "D", "B"))
    self.assertEqual(gg.separation(chain, "A", "D", "B"), chain.cSeparation(0, 3, {1}))
    with self.assertRaises(gum.InvalidArgument):
      gg.separation(dag, "A", ["A", "B"])

  def testInputOutput(self):
    self.assertIs(gg.fromGUMString, gum.graphFromGUMString)
    self.assertIs(gg.load, gum.loadGraph)
    self.assertIs(gg.save, gum.saveGraph)

  def testExampleOfTheModuleDocstring(self):
    """The example of the module (the examples of the functions are in doc/examples, see ExamplesTestSuite)."""
    blocks = _codeBlocks(gg.__doc__)
    self.assertTrue(blocks)
    for block in blocks:
      self._checkExample(block)

  def testExampleOfTheSphinxPage(self):
    with open("../doc/sphinx/graphAlgorithms.rst", encoding="utf-8") as f:
      blocks = _codeBlocks(f.read())
    self.assertTrue(blocks)
    for block in blocks:
      self._checkExample(block)

  def _checkExample(self, block: str):
    env = {}
    lines = block.strip().splitlines()
    for i, line in enumerate(lines):
      if not line.strip() or line.startswith("#"):
        continue
      code = line.split("  # ")[0]
      expected = lines[i + 1][2:] if i + 1 < len(lines) and lines[i + 1].startswith("# ") else None
      if expected is None:
        exec(code, env)
      elif code.startswith("print("):
        out = io.StringIO()
        with contextlib.redirect_stdout(out):
          exec(code, env)
        self.assertEqual(out.getvalue().strip(), expected, code)
      else:
        self.assertEqual(repr(eval(code, env)), expected, code)


ts = unittest.TestSuite()
addTests(ts, TestGraphAlgorithms)
