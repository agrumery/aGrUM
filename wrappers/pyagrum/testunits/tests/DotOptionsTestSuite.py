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
import shutil
import sys
import unittest

import pyagrum as gum

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


def _credalNet(bn):
  """A credal net whose intervals are the cpts of bn scaled by 0.9 and 1.1."""
  bmin = gum.BayesNet(bn)
  bmax = gum.BayesNet(bn)
  for name in bn.names():
    bmin.cpt(name).scale(0.9)
    bmax.cpt(name).scale(1.1)
  cn = gum.CredalNet(bmin, bmax)
  cn.intervalToCredal()
  return cn


class TestDotOptions(pyAgrumTestCase):
  """What the caller gives to a toDot(options), beyond the configuration."""

  def setUp(self):
    gum.config.push()
    gum.config.reset()

  def tearDown(self):
    gum.config.pop()

  def testNothingGivenByDefault(self):
    options = gum.DotOptions()
    self.assertTrue(options.empty())
    self.assertEqual(options.size(), "")
    self.assertEqual(options.rankDir(), "")
    self.assertEqual(options.layout(), "")
    self.assertEqual(options.label(), "")
    self.assertTrue(gum.DotOptions is gum.pyagrumcpp.DotOptions)

  def testGraphOptions(self):
    options = gum.DotOptions()
    options.setSize("12!")
    options.setRankDir("LR")
    options.setLayout("neato")
    options.setLabel("my graph")
    self.assertFalse(options.empty())
    self.assertEqual(
      (options.size(), options.rankDir(), options.layout(), options.label()), ("12!", "LR", "neato", "my graph")
    )

  def testInvalidWidth(self):
    options = gum.DotOptions()
    with self.assertRaises(gum.InvalidArgument):
      options.setArcWidth(0, 1, -0.5)
    with self.assertRaises(gum.InvalidArgument):
      options.setArcWidth(0, 1, float("nan"))
    self.assertTrue(options.empty())

  def testDocumentation(self):
    self.assertIn("toDot", gum.DotOptions.__doc__)
    self.assertIn("tail", gum.DotOptions.setArcWidth.__doc__)

  def testDiGraphAndUndiGraph(self):
    g = gum.DiGraph()
    g.addNodes(3)
    g.addArc(0, 1)
    options = gum.DotOptions()
    options.setSize("8")
    options.setNodeBgColor(0, "#FF7F64")
    options.setNodeLabel(2, "two")
    options.setArcWidth(0, 1, 2.5)
    options.setArcColor(0, 1, "green")
    dot = g.toDot(options)
    self.assertIn('size="8"', dot)
    self.assertIn('0 [fillcolor="#FF7F64", fontcolor="black"];', dot)
    self.assertIn('2 [label="two"];', dot)
    self.assertIn('0 -> 1 [color="green", penwidth="2.5"];', dot)
    # without options
    self.assertEqual(g.toDot(gum.DotOptions()), g.toDot())

    u = gum.UndiGraph()
    u.addNodes(2)
    u.addEdge(0, 1)
    options = gum.DotOptions()
    options.setArcColor(1, 0, "red")  # an edge, in any order
    self.assertIn('0 -- 1 [color="red"];', u.toDot(options))

  def testMixedPdagPag(self):
    m = gum.MixedGraph()
    m.addNodes(3)
    m.addEdge(0, 1)
    m.addArc(1, 2)
    options = gum.DotOptions()
    options.setArcWidth(0, 1, 3)
    self.assertIn('0 -> 1 [dir="none", penwidth="3"];', m.toDot(options))

    p = gum.PDAG()
    p.addNodes(3)
    p.addEdge(0, 1)
    p.addArc(1, 2)
    self.assertIn('0 -> 1 [dir="none", penwidth="3"];', p.toDot(options))

    pag = gum.PAG()
    pag.addNodes(2)
    pag.addEdge(0, 1)
    options = gum.DotOptions()
    options.setArcColor(0, 1, "blue")
    self.assertIn('color="blue"', pag.toDot(options))

  def testBayesNetAndFragment(self):
    bn = gum.fastBN("A->B->C")
    a, b, c = (bn.idFromName(n) for n in "ABC")
    options = gum.DotOptions()
    options.setNodeBgColor(a, "#FF7F64")
    options.setNodeTooltip(b, "(1) B : 0.50000")
    options.setNodeLabel(c, "see")
    options.setArcWidth(a, b, 2.5)
    options.setArcLabel(b, c, "weight")
    dot = bn.toDot(options)
    self.assertIn('"A" [tooltip="(0) A", fillcolor="#FF7F64", fontcolor="black"];', dot)
    self.assertIn('"B" [tooltip="(1) B : 0.50000"];', dot)
    self.assertIn('"C" [label="see", tooltip="(2) C"];', dot)
    self.assertIn('penwidth="2.5"', dot)
    self.assertIn('label="weight"', dot)

    fragment = gum.BayesNetFragment(bn)
    fragment.installAscendants("B")
    given = fragment.toDot(options)
    self.assertIn('fillcolor="#FF7F64"', given)
    self.assertIn("Fragment of", given)

  def testEssentialGraphAndMarkovBlanket(self):
    bn = gum.fastBN("A->B;C->B;B->D")
    options = gum.DotOptions()
    options.setArcColor(bn.idFromName("A"), bn.idFromName("B"), "green")
    self.assertIn('color="green"', gum.EssentialGraph(bn).toDot(options))
    options.setNodeBgColor(bn.idFromName("D"), "burlywood")
    self.assertIn('fillcolor="burlywood"', gum.MarkovBlanket(bn, "B").toDot(options))

  def testMarkovRandomField(self):
    mrf = gum.fastMRF("A--B--C;A--C", 2)
    a, b, c = (mrf.idFromName(n) for n in "ABC")
    options = gum.DotOptions()
    options.setNodeBgColor(a, "burlywood")
    options.setArcWidth(a, b, 3)
    self.assertIn('"A" [tooltip="(0) A", fillcolor="burlywood", fontcolor="black"]', mrf.toDot(options))
    self.assertIn('penwidth="3"', mrf.toDot(options))

    # the factors are designated by the ids of their variables, as a set or a list
    options = gum.DotOptions()
    options.setFactorBgColor({a, c}, "red")
    options.setFactorTooltip([c, b, a], "on A, B and C")
    dot = mrf.toDotAsFactorGraph(options)
    self.assertIn('"f#A#C" [shape="point", width="0.1", height="0.1", fillcolor="red", fontcolor="white"];', dot)
    self.assertIn('tooltip="on A, B and C"', dot)
    self.assertIn('"f#A#B#C" [fillcolor="burlywood"', dot)

  def testInfluenceDiagram(self):
    diag = gum.fastID("A->B->*D->$U;B->$U")
    options = gum.DotOptions()
    options.setNodeBgColor(diag.idFromName("A"), "burlywood")
    options.setArcColor(diag.idFromName("B"), diag.idFromName("D"), "green")
    dot = diag.toDot(options)
    self.assertIn('"A" [tooltip="(0) A", fillcolor="burlywood", fontcolor="black"];', dot)
    self.assertIn('"B" -> "D" [style="tapered, bold, dotted", color="green"];', dot)

  def testCausalModel(self):
    cm = gum.CausalModel(gum.fastBN("A->B->C;A->C"), [("L", ["A", "C"])], True)
    options = gum.DotOptions()
    options.setSize("3")
    dot = cm.toDot(options)
    self.assertIn('size="3"', dot)
    self.assertIn('"L" -> "A" [style="dashed"];', dot)
    # the old forms still work
    self.assertIn('shape="ellipse"', cm.toDot(True))
    self.assertIn('fillcolor="#123456"', cm.toDot(True, "#123456", "pink", "#654321"))

  def testCredalNet(self):
    cn = _credalNet(gum.fastBN("A->B", 2))
    options = gum.DotOptions()
    options.setNodeBgColor(0, "red")
    dot = cn.toDot(options)
    self.assertIn('shape="polygon", sides="7", peripheries="1"', dot)
    self.assertIn('fillcolor="red"', dot)

  def testJunctionTree(self):
    bn = gum.fastBN("A->B->C;A->C;D->C;D->E", 3)
    jt = gum.LazyPropagation(bn).junctionTree()
    options = gum.DotOptions()
    options.setNodeLabel(0, "A B C D")
    options.setArcLabel(0, 1, "D")
    dot = jt.toDot(options)
    self.assertIn('label="A B C D"', dot)
    self.assertIn('label="D"', dot)
    self.assertNotIn('label="0-1-2-3"', dot)
    # the map takes the values of the configuration
    self.assertEqual(jt.__map_str__(0.3, 0.1, 1.0), jt.__map_str__())

  def testKTBN(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.addArc("X", 0, "X", 1)
    m.addArc("C", -1, "X", 1)
    bn = m.unroll(3)
    options = gum.DotOptions()
    options.setSize("12!")
    options.setNodeBgColor(bn.idFromName("C"), "red")
    dot = m.toUnrolledDot(3, False, options)
    self.assertIn('size="12!"', dot)
    self.assertIn('"C" [label="C", fillcolor="red", fontcolor="white"];', dot)
    with self.assertRaises(gum.OperationNotAllowed):
      m.toUnrolledDot(1, False, options)

  def testPrepareDotKeepsTheGraphsOfAgrum(self):
    """The graphs written by the toDot() of aGrUM are complete: prepareDot adds nothing which would change them."""
    import pydot as dot

    import pyagrum.lib.dotstyle as gds

    dag = gum.DAG()
    dag.addNodes(2)
    dag.addArc(0, 1)
    options = gum.DotOptions()
    options.setRankDir("LR")
    options.setNodeBgColor(0, "burlywood")
    options.setNodeFgColor(1, "red")
    graph = gds.dotFromAgrum(dag.toDot(options))
    gds.prepareDot(graph)

    nodes = {n.get_name(): n for n in graph.get_nodes()}
    # the font color of a node is the one given (no color added to the nodes which have nothing to say)
    self.assertEqual(nodes["0"].get_fontcolor().strip('"'), "black")
    self.assertEqual(nodes["1"].get_fontcolor().strip('"'), "red")
    # the defaults of the graph are not replaced
    self.assertEqual(nodes["node"].get_attributes()["fontcolor"], '"white"')
    self.assertEqual(nodes["graph"].get_attributes()["rankdir"], '"LR"')
    self.assertEqual(graph.to_string().count("rankdir"), 1)  # not set a second time (it would win over the given LR)
    self.assertIsInstance(graph, dot.Dot)

  def testPrepareDotStillCompletesTheOtherGraphs(self):
    import pydot as dot

    import pyagrum.lib.dotstyle as gds

    graph = dot.graph_from_dot_data(
      'digraph { "A" [fillcolor="#202020", style=filled]; "B" [fillcolor="yellow", style=filled]; "A"->"B"; }'
    )[0]
    gds.prepareDot(graph)
    nodes = {n.get_name().strip('"'): n for n in graph.get_nodes()}
    self.assertEqual(nodes["A"].get_fontcolor(), "white")
    self.assertEqual(nodes["B"].get_fontcolor(), "black")
    self.assertEqual(graph.get_rankdir(), "TB")
    self.assertEqual(graph.get_edges()[0].get_color(), "#4A4A4A")

  def testProbaToColorGivesValidColors(self):
    """A component of a colormap equal to 1.0 must give 'ff', not '100' (the yellow of Pastel1 for instance)."""
    import matplotlib

    import pyagrum.lib.dotstyle as gds

    for name in ["Pastel1", "BuGn", "Greys", "bwr"]:
      cmap = matplotlib.colormaps[name]
      for i in range(101):
        p = i / 100
        self.assertEqual("".join(gds.proba2hex(p, cmap, False)), matplotlib.colors.to_hex(cmap(p))[1:])
    self.assertEqual(gds.proba2color(0.6, matplotlib.colormaps["Pastel1"]), "#ffffcc")

  def testPythonBuiltGraphsAreComplete(self):
    """The graphs built in Python by pyAgrum are already completed: prepareDot changes nothing on them."""
    try:
      import pydot as dot

      import pyagrum.bnmixture as BNM
      import pyagrum.bnmixture.notebook as bnmnb
      import pyagrum.clg as gclg
      import pyagrum.clg.notebook as clgnb
      import pyagrum.explain as gexpl
      import pyagrum.lib.bn2graph as gbn2
      import pyagrum.lib.bn_vs_bn as gcm
      import pyagrum.lib.cn2graph as gcn2
      import pyagrum.lib.dotstyle as gds
      import pyagrum.lib.dynamicBN as gdyn
      import pyagrum.lib.id2graph as gid2
      import pyagrum.lib.mrf2graph as gmrf2
      from pyagrum import ctbn
    except ImportError:
      self.skipTest("ipython and matplotlib are needed for the graphs of pyAgrum")

    bn = gum.fastBN("A->B->C<-D")
    cn = _credalNet(bn)
    mrf = gum.fastMRF("A--B;B--C--D")
    diag = gum.fastID("A->B->*D->$U;B->$U")
    clg = gclg.CLG()
    clg.add(gclg.GaussianVariable("X", 0, 1))
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn, w=1.0)
    bnm.add("bn1", gum.fastBN("A->B<-C<-D"), w=2.0)

    graphs = {
      "BNinference2dot": gbn2.BNinference2dot(bn),
      "CNinference2dot": gcn2.CNinference2dot(cn),
      "MRFinference2UGdot": gmrf2.MRFinference2UGdot(mrf),
      "MRFinference2FactorGraphdot": gmrf2.MRFinference2FactorGraphdot(mrf),
      "LIMIDinference2dot": gid2.LIMIDinference2dot(diag, "6", gum.ShaferShenoyLIMIDInference(diag), {}, set()),
      "_TimeSlicesToDot": gdyn._TimeSlicesToDot(gum.fastBN("a0->a1->at")),
      "graphDiffLegend": gcm.graphDiffLegend(),
      "CLG2dot": clgnb.CLG2dot(clg),
      "CLGInference2dot": clgnb.CLGInference2dot(clg),
      "CTBN.toDot": dot.graph_from_dot_data(ctbn.randomCTBN((0.5, 1.2), n=3).toDot())[0],
      "BNMixtureInference2dot": bnmnb.BNMixtureInference2dot(bnm),
      "arcsCompLegend": bnmnb.arcsCompLegend(),
      "generalizedMarkovBlanket": gexpl.generalizedMarkovBlanket(bn, "B"),
    }
    if shutil.which("dot"):  # graphDiff positions the nodes with graphviz
      graphs["graphDiff"] = gcm.graphDiff(bn, gum.fastBN("A->B<-C<-D"))
    for name, graph in graphs.items():
      with self.subTest(builder=name):
        before = graph.to_string()
        gds.prepareDot(graph)
        self.assertEqual(graph.to_string(), before)
        self.assertEqual(graph.get_bgcolor(), "transparent")
    self.assertEqual(graphs["generalizedMarkovBlanket"].get_name(), "MB(B,1)")

  @unittest.skipUnless(shutil.which("dot"), "graphviz is not installed")
  def testShowInferenceOfCTBNAndCLG(self):
    """The show functions of the inferences of CTBN and CLG display the graph built in Python."""
    try:
      import pyagrum.clg as gclg
      import pyagrum.clg.notebook as clgnb
      import pyagrum.ctbn.notebook as ctbnnb
      from pyagrum import ctbn
    except ImportError:
      self.skipTest("ipython and matplotlib are needed for the notebooks of CTBN and CLG")

    model = ctbn.randomCTBN((0.5, 1.2), n=2)
    clg = gclg.CLG()
    clg.add(gclg.GaussianVariable("X", 0, 1))
    with contextlib.redirect_stdout(io.StringIO()):  # without a notebook, IPython prints the displayed objects
      ctbnnb.showCtbnInference(model, ctbn.SimpleInference(model))
      clgnb.showInference(clg)

  def testDeprecatedColorsModule(self):
    """pyagrum.lib._colors is a deprecated relay to pyagrum.lib.dotstyle."""
    try:
      import pyagrum.lib.dotstyle as gds
    except ImportError:
      self.skipTest("matplotlib is needed for pyagrum.lib.dotstyle")

    sys.modules.pop("pyagrum.lib._colors", None)
    with self.assertWarns(DeprecationWarning):
      import pyagrum.lib._colors as gcols
    self.assertIs(gcols.prepareDot, gds.prepareDot)
    self.assertIs(gcols.getBlackInTheme, gds.getBlackInTheme)

  @unittest.skipUnless(shutil.which("dot"), "graphviz is not installed")
  def testAGraphOfTheUserIsShownAsItIs(self):
    """Showing a graph only applies its size: a graph given by the user is not completed."""
    try:
      import pydot as dot

      import pyagrum.lib.notebook as gnb
    except ImportError:
      self.skipTest("ipython and matplotlib are needed for pyagrum.lib.notebook")

    graph = dot.graph_from_dot_data("digraph { A [fillcolor=black, style=filled]; A -> B; }")[0]
    gnb.getGraph(graph, size="3")
    self.assertEqual(graph.get_size(), "3")
    self.assertIsNone(graph.get_bgcolor())
    self.assertIsNone(graph.get_node("A")[0].get_fontcolor())
    self.assertIsNone(graph.get_edges()[0].get_color())

  @unittest.skipUnless(shutil.which("dot"), "graphviz is not installed")
  def testGraphDiffIsADiffOfDAGs(self):
    """graphDiff compares the structures of two models which are DAGs with names: Bayesian networks, CLG..."""
    import pyagrum.lib.bn_vs_bn as gcmp
    from pyagrum.clg import CLG
    from pyagrum.clg.GaussianVariable import GaussianVariable

    def clg(arcs):
      res = CLG()
      ids = {name: res.add(GaussianVariable(mu=0, sigma=1, name=name)) for name in "XYZ"}
      for tail, head in arcs:
        res.addArc(ids[tail], ids[head], 1)
      return res

    graph = gcmp.graphDiff(clg(["XY", "YZ"]), clg(["XY", "XZ"]))
    names = {n.get_name().strip('"') for n in graph.get_nodes()}
    self.assertTrue({"X", "Y", "Z"} <= names)
    self.assertEqual(len(graph.get_edges()), 3)  # X->Y common, Y->Z removed, X->Z added
    # the nodes are positioned (by graphviz) as in the reference
    self.assertTrue(all(n.get_pos() is not None for n in graph.get_nodes() if n.get_name().strip('"') in "XYZ"))

    bn = gum.fastBN("A->B->C")
    self.assertEqual(len(gcmp.graphDiff(bn, gum.fastBN("A->B->C;A->C")).get_edges()), 3)  # 2 common, 1 added

  @unittest.skipUnless(shutil.which("dot"), "graphviz is not installed")
  def testDotLayoutOfTheGraphsOfBNs(self):
    """The layout of a graph can be applied to another one: the statements `graph [...]` are not nodes."""
    import pyagrum.lib.bn2graph as gbn2
    import pyagrum.lib.utils as gutils

    layout = gutils.dot_layout(gbn2.BN2dot(gum.fastBN("A->B<-C<-D")))
    graph = gbn2.BN2dot(gum.fastBN("A->B->C<-D"))
    gutils.apply_dot_layout(graph, layout)
    positions = {n.get_name().strip('"'): n.get_pos() for n in graph.get_nodes() if n.get_name().strip('"') in layout}
    self.assertEqual(set(positions), {"A", "B", "C", "D"})
    self.assertNotIn(None, positions.values())


ts = unittest.TestSuite()
addTests(ts, TestDotOptions)
