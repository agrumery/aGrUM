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

import json
import os
import re
import subprocess
import sys
import tempfile
import unittest
import warnings

import pyagrum as gum
from pyagrum.pyagrumcpp import _Configuration
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

# The logic (parsing of the schema, validation, deprecations, ...) is the one of gum::Configuration and is
# tested in C++ (ConfigurationTestSuite.h): this suite only tests the wrapper (conversions of types,
# exceptions, warnings and Python hooks).


class TestConfig(pyAgrumTestCase):
  def testConfigIsTheConfigurationOfTheLibrary(self):
    # no Python layer: pyagrum.config is the object of the C++ library
    self.assertIsInstance(gum.config, _Configuration)
    with self.assertRaises(ModuleNotFoundError):
      __import__("pyagrum.config")

  def testReadOnlyConfigurationStructure(self):
    with self.assertRaises(SyntaxError):
      gum.config["gogo", "gaga"] = 0
    with self.assertRaises(SyntaxError):
      gum.config["theme", "gaga"] = 0

  def testTypeValidation(self):
    gum.config.push()
    try:
      with self.assertRaises(ValueError):
        gum.config["notebook", "tensor_visible_digits"] = "abc"
      gum.config["notebook", "tensor_visible_digits"] = 7
      self.assertEqual(gum.config["notebook", "tensor_visible_digits"], "7")

      with self.assertRaises(ValueError):
        gum.config["notebook", "histogram_epsilon"] = "notafloat"
      gum.config["notebook", "histogram_epsilon"] = 1e-7

      with self.assertRaises(ValueError):
        gum.config["notebook", "tensor_with_colors"] = "maybe"
      # boolean values are canonicalized to True/False whatever the accepted spelling used
      gum.config["notebook", "tensor_with_colors"] = "off"
      self.assertEqual(gum.config["notebook", "tensor_with_colors"], "False")
      gum.config["notebook", "tensor_with_colors"] = "YES"
      self.assertEqual(gum.config["notebook", "tensor_with_colors"], "True")
      gum.config["notebook", "tensor_with_colors"] = "1"
      self.assertEqual(gum.config["notebook", "tensor_with_colors"], "True")

      with self.assertRaises(ValueError):
        gum.config["notebook", "graph_format"] = "jpg"
      # enumerated values are canonicalized (case-insensitive match)
      gum.config["notebook", "graph_format"] = "PNG"
      self.assertEqual(gum.config["notebook", "graph_format"], "png")
    finally:
      gum.config.pop()

  def testTyped(self):
    self.assertIsInstance(gum.config.typed["notebook", "tensor_visible_digits"], int)
    self.assertIsInstance(gum.config.typed["notebook", "histogram_epsilon"], float)
    self.assertIsInstance(gum.config.typed["notebook", "tensor_with_colors"], bool)
    self.assertIsInstance(gum.config.typed["notebook", "graph_format"], str)

    gum.config.push()
    try:
      gum.config.typed["notebook", "tensor_visible_digits"] = 3
      self.assertEqual(gum.config.typed["notebook", "tensor_visible_digits"], 3)
      with self.assertRaises(ValueError):
        gum.config.typed["notebook", "tensor_visible_digits"] = "abc"
    finally:
      gum.config.pop()

  def testDeprecatedAccessors(self):
    # asInt/asFloat/asBool are deprecated since 3.1.0 and delegate to typed
    for accessor, key, expected in (
      ("asInt", ("notebook", "tensor_visible_digits"), int),
      ("asFloat", ("notebook", "histogram_epsilon"), float),
      ("asBool", ("notebook", "tensor_with_colors"), bool),
    ):
      with warnings.catch_warnings(record=True) as w:
        warnings.simplefilter("always")
        value = getattr(gum.config, accessor)[key]
      self.assertIsInstance(value, expected)
      self.assertEqual(len(w), 1)
      self.assertTrue(issubclass(w[0].category, DeprecationWarning))
      self.assertIn("3.1.0", str(w[0].message))

  def testSchema(self):
    found = False
    for section, option, meta, default in gum.config.schema():
      self.assertIn(meta.kind, ("int", "float", "bool", "str", "enum", "deprecated"))
      self.assertNotEqual(meta.doc, "")
      if meta.kind == "deprecated":
        self.assertIsNone(default)
      else:
        self.assertIsInstance(default, str)
      if (section, option) == ("notebook", "graph_format"):
        found = True
        self.assertEqual(meta.kind, "enum")
        self.assertEqual(meta.values, ("svg", "png"))
        self.assertEqual(default, "svg")
    self.assertTrue(found)

  def testDomainOfValidity(self):
    # the domain of a key is part of its documentation and is checked by set()
    self.assertTrue(gum.config.doc("core", "epsilonStandard").startswith("float[1e-8,1e-5] ::"))
    gum.config.push()
    try:
      for value in (0, 1, -1e-3, 2, 1e-4, 1e-9):
        with self.assertRaises(ValueError):
          gum.config["core", "epsilonStandard"] = value
      gum.config["core", "epsilonStandard"] = 1e-5
      self.assertEqual(gum.config.typed["core", "epsilonStandard"], 1e-5)
    finally:
      gum.config.pop()

  def testDoc(self):
    self.assertEqual(
      gum.config.doc("notebook", "tensor_visible_digits"),
      "int[0,] :: number of visible digits when displaying a tensor in a notebook",
    )
    self.assertIn("histogram|bar", gum.config.doc("notebook", "histogram_discretized_visualisation"))
    with self.assertRaises(SyntaxError):
      gum.config.doc("notebook", "gaga")
    with self.assertRaises(SyntaxError):
      gum.config.doc("gogo", "gaga")

  def testSchemaOfACustomConfiguration(self):
    cfg = self._freshConfig("[s]\n### int :: a doc\nk = 1\n### a|b :: values\nl = a\n")
    schema = {(section, option): (meta, default) for section, option, meta, default in cfg.schema()}
    self.assertEqual(schema[("s", "k")][0].kind, "int")
    self.assertEqual(schema[("s", "k")][0].doc, "a doc")
    self.assertEqual(schema[("s", "k")][1], "1")
    self.assertEqual(schema[("s", "l")][0].kind, "enum")
    self.assertEqual(schema[("s", "l")][0].values, ("a", "b"))

    with self.assertRaises(SyntaxError):  # a malformed schema is refused by the C++ configuration
      self._freshConfig("[s]\n### int doc\nk = 1\n")

  @staticmethod
  def _freshConfig(content):
    # a configuration independent from gum.config, around a C++ configuration built from a text
    return _Configuration(content)

  def testDeprecatedKey(self):
    cfg = self._freshConfig(
      "[sec]\n### int :: the answer\nnewkey = 42\n### deprecated -> sec.newkey :: renamed\noldkey = 0\n"
    )

    with warnings.catch_warnings(record=True) as w:
      warnings.simplefilter("always")
      self.assertEqual(cfg["sec", "oldkey"], "42")
      # typed uses the type declared for the replacement key
      self.assertEqual(cfg.typed["sec", "oldkey"], 42)
      cfg["sec", "oldkey"] = 13
      self.assertEqual(cfg["sec", "newkey"], "13")
    self.assertEqual(len(w), 3)
    for warning in w:
      self.assertTrue(issubclass(warning.category, DeprecationWarning))

    # the deprecated key is hidden from displays and validated against the target type
    self.assertNotIn("oldkey", str(cfg))
    self.assertTrue(cfg.doc("sec", "oldkey").startswith("deprecated -> sec.newkey"))
    with self.assertRaises(ValueError):
      cfg["sec", "oldkey"] = "abc"

  def testJgumIndentComesFromConfiguration(self):
    bn = gum.fastBN("A->B")
    gum.config.push()
    try:
      gum.config["core", "jgumIndent"] = -1
      self.assertNotIn("\n", bn.saveGUMstring())
      gum.config["core", "jgumIndent"] = 2
      self.assertIn("\n", bn.saveGUMstring())
      # an explicit indentation never uses the configuration
      self.assertNotIn("\n", bn.saveGUMstring(indent=-1))
      self.assertNotIn("\n", bn.saveGUMstring(-1))
      gum.config["core", "jgumIndent"] = -1
      self.assertIn("\n", bn.saveGUMstring(indent=2))
    finally:
      gum.config.pop()

  def testDefaultsOfAgrumThenOfPyAgrum(self):
    sections = []
    for section, _, _, _ in gum.config.schema():
      if section not in sections:
        sections.append(section)
    # [core] comes from the defaults of aGrUM, the other sections from the defaults of pyAgrum
    self.assertEqual(sections[0], "core")
    self.assertIn("notebook", sections)
    self.assertEqual(gum.config.getTyped("core", "allowModificationWhenSaving"), False)
    self.assertEqual(gum.config.getTyped("notebook", "tensor_visible_digits"), 4)

  def testNumericalTolerances(self):
    self.assertEqual(gum.config.getTyped("core", "epsilonCoarse"), 1e-2)
    self.assertEqual(gum.config.getTyped("core", "epsilonStandard"), 1e-6)
    self.assertEqual(gum.config.getTyped("core", "epsilonFine"), 1e-10)

    gum.config.push()
    try:
      # epsilonStandard is in [1e-8,1e-5]
      for bad in ("0", "1", "-1e-3", "2", "nan", "abc", "1e-4", "1e-9"):
        with self.assertRaises(ValueError):
          gum.config["core", "epsilonStandard"] = bad
        self.assertEqual(gum.config.getTyped("core", "epsilonStandard"), 1e-6)

      # IBayesNet.check() uses epsilonStandard : the sum of the distribution is 1 + 1e-7
      bn = gum.fastBN("a->b")
      bn.cpt("a").fillWith([0.5, 0.5 + 1e-7])
      self.assertEqual(len(bn.check()), 0)
      gum.config["core", "epsilonStandard"] = 1e-8
      self.assertEqual(len(bn.check()), 1)
    finally:
      gum.config.pop()
    self.assertEqual(gum.config.getTyped("core", "epsilonStandard"), 1e-6)

  def testUserFileChangesButNeverCreates(self):
    # a new process: pyagrum.ini is read when the library is loaded, after the defaults of pyAgrum
    code = (
      "import json, warnings\n"
      "with warnings.catch_warnings(record=True) as w:\n"
      "  warnings.simplefilter('always')\n"
      "  import pyagrum as gum\n"
      "keys = {(s, o) for s, o, _, _ in gum.config.schema()}\n"
      "print(json.dumps({'digits': gum.config['notebook', 'tensor_visible_digits'],\n"
      "  'indent': gum.config['core', 'jgumIndent'],\n"
      "  'created': sorted(s + '.' + o for s, o in keys if s == 'newsection' or o == 'notakey'),\n"
      "  'warnings': [str(x.message) for x in w if 'pyagrum.ini' in str(x.message)]}))\n"
    )
    with tempfile.TemporaryDirectory() as tmp:
      with open(os.path.join(tmp, "pyagrum.ini"), "w") as f:
        f.write("[notebook]\ntensor_visible_digits = 7\nnotakey = 1\n[newsection]\nk = 1\n[core]\njgumIndent = 3\n")
      with open(os.path.join(tmp, "agrum.ini"), "w") as f:  # the file of aGrUM is not read by pyAgrum
        f.write("[notebook]\ntensor_visible_digits = 9\n")
      # the local pyAgrum is found through sys.path (not installed), which a new process does not inherit
      env = dict(os.environ, PYTHONPATH=os.pathsep.join(p for p in sys.path if p))
      out = subprocess.run([sys.executable, "-c", code], cwd=tmp, capture_output=True, text=True, env=env)
      self.assertEqual(out.returncode, 0, out.stderr)
    res = json.loads(out.stdout.strip().splitlines()[-1])
    self.assertEqual(res["digits"], "7")  # a key created by the defaults of pyAgrum is modified ...
    self.assertEqual(res["indent"], "3")  # ... as well as a key of aGrUM
    self.assertEqual(res["created"], [])  # ... but nothing is created
    self.assertEqual(len(res["warnings"]), 2)  # unknown key, unknown section

  def testNoVisibleKeyStartsWithDefault(self):
    for section, option, meta, _ in gum.config.schema():
      if meta.kind != "deprecated":
        self.assertFalse(option.startswith("default_"), f"{section}.{option}: 'default' is implied in defaults.ini")

  def testLibraryDoesNotReadDeprecatedKeys(self):
    # a deprecated key read by the library itself would emit a warning each time it is used
    deprecated = {
      (section, option.lower()) for section, option, meta, _ in gum.config.schema() if meta.kind == "deprecated"
    }
    pattern = re.compile(r"""config\[\s*["'](\w+)["']\s*,\s*["'](\w+)["']""")
    root = os.path.dirname(gum.__file__)
    for dirpath, _, filenames in os.walk(root):
      for filename in filenames:
        if not filename.endswith(".py"):
          continue
        path = os.path.join(dirpath, filename)
        with open(path, encoding="utf-8") as f:
          for numline, line in enumerate(f, 1):
            for section, option in pattern.findall(line):
              self.assertNotIn(
                (section, option.lower()), deprecated, f"{path}:{numline} uses the deprecated key {section}.{option}"
              )

  # keys renamed after aGrUM 3.2.1 (section, old name, new name, value)
  RENAMED_AFTER_3_2_1 = (
    ("core", "default_jgumIndent", "jgumIndent", "2"),
    ("notebook", "default_node_bgcolor", "nodeBgColor", "red"),
    ("core", "allow_modification_when_saving", "allowModificationWhenSaving", "True"),
  )

  def testRenamedKeysAfter321StillWork(self):
    # transitory: to be removed together with these deprecated keys in defaults.ini
    # the schema gives the names of the keys in lower case
    deprecated = {(section, option) for section, option, meta, _ in gum.config.schema() if meta.kind == "deprecated"}
    if any((section, old.lower()) not in deprecated for section, old, _, _ in self.RENAMED_AFTER_3_2_1):
      self.skipTest("the keys renamed after aGrUM 3.2.1 are no more deprecated keys")

    gum.config.push()
    try:
      for section, old, new, value in self.RENAMED_AFTER_3_2_1:
        with warnings.catch_warnings(record=True) as w:
          warnings.simplefilter("always")
          gum.config[section, old] = value
        self.assertEqual(len(w), 1)
        self.assertTrue(issubclass(w[0].category, DeprecationWarning))
        self.assertIn("3.2.1", str(w[0].message))
        self.assertEqual(gum.config[section, new], value)
    finally:
      gum.config.pop()

  # keys moved to the defaults of aGrUM and renamed in lowerCamelCase (section -> old name -> new name)
  MOVED_TO_AGRUM = {
    "notebook": {
      "arc_color": "arcColor",
      "node_bgcolor": "nodeBgColor",
      "node_fgcolor": "nodeFgColor",
      "graph_size": "graphSize",
      "graph_rankdir": "graphRankDir",
      "graph_layout": "graphLayout",
      "markovrandomfield_view": "markovRandomFieldView",
      "junctiontree_graph_size": "junctionTreeGraphSize",
      "junctiontree_with_names": "junctionTreeWithNames",
      "junctiontree_separator_bgcolor": "junctionTreeSeparatorBgColor",
      "junctiontree_separator_fgcolor": "junctionTreeSeparatorFgColor",
      "junctiontree_separator_fontsize": "junctionTreeSeparatorFontSize",
      "junctiontree_clique_bgcolor": "junctionTreeCliqueBgColor",
      "junctiontree_clique_fgcolor": "junctionTreeCliqueFgColor",
      "junctiontree_clique_fontsize": "junctionTreeCliqueFontSize",
      "junctiontree_map_cliquescale": "junctionTreeMapCliqueScale",
      "junctiontree_map_sepscale": "junctionTreeMapSepScale",
      "junctiontree_map_edgelen": "junctionTreeMapEdgeLen",
      "junctiontree_map_size": "junctionTreeMapSize",
    },
    "factorgraph": {
      "node_bgcolor": "nodeBgColor",
      "node_fgcolor": "nodeFgColor",
      "factor_bgcolor": "factorBgColor",
      "edge_length": "edgeLength",
      "graph_layout": "graphLayout",
    },
    "dynamicBN": {
      "graph_size": "graphSize",
    },
    "influenceDiagram": {
      "graph_size": "graphSize",
      "id_size": "graphSize",
      "chance_bgcolor": "chanceBgColor",
      "chance_fgcolor": "chanceFgColor",
      "utility_bgcolor": "utilityBgColor",
      "utility_fgcolor": "utilityFgColor",
      "decision_bgcolor": "decisionBgColor",
      "decision_fgcolor": "decisionFgColor",
      "chance_shape": "chanceShape",
      "utility_shape": "utilityShape",
      "decision_shape": "decisionShape",
      "decision_arc_style": "decisionArcStyle",
      "utility_arc_style": "utilityArcStyle",
    },
    "credalnet": {
      "node_bgcolor": "nodeBgColor",
      "node_fgcolor": "nodeFgColor",
    },
    "causal": {
      "show_latent_names": "showLatentNames",
      "graph_size": "graphSize",
      "node_bgcolor": "nodeBgColor",
      "node_fgcolor": "nodeFgColor",
      "latent_bgcolor": "latentBgColor",
      "latent_fgcolor": "latentFgColor",
    },
  }

  def testEveryDeprecatedKeyRedirectsToItsTarget(self):
    # generic: it stays valid when the deprecated keys are removed
    target = re.compile(r"deprecated -> (\w+)\.(\w+)")
    for section, option, meta, _ in gum.config.schema():
      if meta.kind != "deprecated":
        continue
      found = target.match(gum.config.doc(section, option))
      self.assertIsNotNone(found, f"{section}.{option}")
      tsection, toption = found.groups()
      self.assertNotEqual(
        next(m.kind for s, o, m, _ in gum.config.schema() if (s, o) == (tsection, toption.lower())), "deprecated"
      )
      with warnings.catch_warnings(record=True) as w:
        warnings.simplefilter("always")
        value = gum.config[section, option]
        typed = gum.config.typed[section, option]
      self.assertEqual(len(w), 2, f"{section}.{option}")
      self.assertTrue(all(issubclass(x.category, DeprecationWarning) for x in w))
      self.assertEqual(value, gum.config[tsection, toption], f"{section}.{option}")
      self.assertEqual(typed, gum.config.typed[tsection, toption], f"{section}.{option}")

  def testKeysMovedToAgrumStillWork(self):
    # transitory: to be removed together with these deprecated keys in defaults.ini
    # the schema gives the names of the keys in lower case
    schema = {(section, option): meta for section, option, meta, _ in gum.config.schema()}
    moved = [(section, old, new) for section, keys in self.MOVED_TO_AGRUM.items() for old, new in keys.items()]
    if any(
      schema.get((section, old.lower())) is None or schema[(section, old.lower())].kind != "deprecated"
      for section, old, _ in moved
    ):
      self.skipTest("the keys moved to aGrUM are no more deprecated keys")

    self.assertEqual(len(moved), 46)
    gum.config.push()
    try:
      for section, old, new in moved:
        self.assertNotEqual(schema[(section, new.lower())].kind, "deprecated", f"{section}.{new}")
        value = gum.config[section, new]
        # the old name reads the value of the new one ...
        with warnings.catch_warnings(record=True) as w:
          warnings.simplefilter("always")
          self.assertEqual(gum.config[section, old], value, f"{section}.{old}")
        self.assertEqual(len(w), 1)
        self.assertTrue(issubclass(w[0].category, DeprecationWarning))
        # ... and writes it
        with warnings.catch_warnings(record=True):
          warnings.simplefilter("always")
          gum.config[section, old] = value
        self.assertEqual(gum.config[section, new], value)

      # a value written with an old name is the value of the new name
      with warnings.catch_warnings(record=True):
        warnings.simplefilter("always")
        gum.config["notebook", "graph_size"] = "7"
        gum.config["influenceDiagram", "id_size"] = "8"
        self.assertEqual(gum.config["notebook", "default_graph_size"], "7")
        self.assertEqual(gum.config["influenceDiagram", "default_graph_size"], "8")
      self.assertEqual(gum.config["notebook", "graphSize"], "7")
      self.assertEqual(gum.config["influenceDiagram", "graphSize"], "8")
    finally:
      gum.config.pop()

  def testSectionsSharedByAgrumAndPyAgrum(self):
    keys = {}
    for section, option, meta, _ in gum.config.schema():
      self.assertNotIn(option, keys.setdefault(section, []), f"{section}.{option} twice")
      keys[section].append(option)
    for section in ("notebook", "factorgraph", "dynamicBN", "influenceDiagram", "credalnet", "causal"):
      self.assertIn(section, keys)
    # a key of the defaults of aGrUM and a key of the defaults of pyAgrum in the same section, aGrUM first
    notebook = keys["notebook"]
    self.assertLess(notebook.index("nodebgcolor"), notebook.index("tensor_visible_digits"))
    self.assertLess(notebook.index("graphsize"), notebook.index("graph_inference_size"))
    self.assertLess(keys["causal"].index("graphsize"), keys["causal"].index("latex_do_prefix"))

  def testGraphsDoNotReadDeprecatedKeys(self):
    import pyagrum.lib.bn2graph as gbn2
    import pyagrum.lib.id2graph as gid2
    import pyagrum.lib.notebook as gnb
    import pyagrum.lib.utils as gutils

    bn = gum.fastBN("A->B->C;A->C", 3)
    mrf = gum.fastMRF("A--B--C;A--C", 2)
    diag = gum.fastID("A->B->*D->$U;B->$U")
    ie = gum.ShaferShenoyLIMIDInference(diag)
    ie.makeInference()

    gum.config.push()
    try:
      with warnings.catch_warnings():
        # only the notices of the configuration: the libraries we use may have their own deprecation warnings
        warnings.filterwarnings("error", message=r"'\w+\.\w+' is deprecated", category=DeprecationWarning)
        gnb.BN2dot(bn)
        gnb.MRF2UGdot(mrf)
        gnb.MRF2FactorGraphdot(mrf)
        gnb.ID2dot(diag)
        gbn2.BNinference2dot(bn, evs={"A": 1}, targets={"C"})
        gid2.LIMIDinference2dot(diag, "6", ie, {}, set())
        gutils.setDarkTheme()
        gutils.setLightTheme()
        gutils.getBlackInTheme()
    finally:
      gum.config.pop()

  # the font colors, which can be 'adaptive' (black or white according to their background color)
  FONT_COLORS = (
    ("notebook", "nodeFgColor"),
    ("notebook", "junctionTreeSeparatorFgColor"),
    ("notebook", "junctionTreeCliqueFgColor"),
    ("factorgraph", "nodeFgColor"),
    ("influenceDiagram", "chanceFgColor"),
    ("influenceDiagram", "utilityFgColor"),
    ("influenceDiagram", "decisionFgColor"),
    ("credalnet", "nodeFgColor"),
    ("causal", "nodeFgColor"),
    ("causal", "latentFgColor"),
  )

  def testFontColorsAreAdaptiveByDefault(self):
    gum.config.push()
    try:
      gum.config.reset()
      for section, key in self.FONT_COLORS:
        self.assertEqual(gum.config[section, key], "adaptive", f"{section}.{key}")
        # the old names (here of the keys of the notebook) give the same value
      with warnings.catch_warnings(record=True):
        warnings.simplefilter("always")
        self.assertEqual(gum.config["notebook", "node_fgcolor"], "adaptive")
        self.assertEqual(gum.config["notebook", "default_node_fgcolor"], "adaptive")
        self.assertEqual(gum.config["influenceDiagram", "default_chance_fgcolor"], "adaptive")
    finally:
      gum.config.pop()

  def testAdaptiveFontColor(self):
    import pyagrum.lib._colors as gumcols

    gum.config.push()
    try:
      gum.config["notebook", "nodeFgColor"] = "adaptive"
      gum.config["notebook", "arcColor"] = "#AAAAAA"

      gum.config["notebook", "nodeBgColor"] = "#404040"
      self.assertEqual(gumcols.configFgColor("notebook", "nodeFgColor"), "white")
      gum.config["notebook", "nodeBgColor"] = "burlywood"
      self.assertEqual(gumcols.configFgColor("notebook", "nodeFgColor"), "black")
      # a background which is not a color: the color of the arcs (so the theme)
      gum.config["notebook", "nodeBgColor"] = "transparent"
      self.assertEqual(gumcols.configFgColor("notebook", "nodeFgColor"), "#AAAAAA")
      # a color of the configuration is never changed
      gum.config["notebook", "nodeFgColor"] = "red"
      self.assertEqual(gumcols.configFgColor("notebook", "nodeFgColor"), "red")

      # the background is the twin key of the font color
      gum.config["influenceDiagram", "chanceFgColor"] = "adaptive"
      gum.config["influenceDiagram", "chanceBgColor"] = "#000000"
      self.assertEqual(gumcols.configFgColor("influenceDiagram", "chanceFgColor"), "white")
      gum.config["influenceDiagram", "chanceBgColor"] = "#FFFFFF"
      self.assertEqual(gumcols.configFgColor("influenceDiagram", "chanceFgColor"), "black")
    finally:
      gum.config.pop()

  def testGraphsWithAdaptiveFontColors(self):
    import pyagrum.lib.bn2graph as gbn2
    import pyagrum.lib.id2graph as gid2
    import pyagrum.lib.notebook as gnb

    bn = gum.fastBN("A->B->C;A->C", 3)
    mrf = gum.fastMRF("A--B--C;A--C", 2)
    diag = gum.fastID("A->B->*D->$U;B->$U")
    ie = gum.ShaferShenoyLIMIDInference(diag)
    ie.makeInference()

    gum.config.push()
    try:
      for section, key in self.FONT_COLORS:
        gum.config[section, key] = "adaptive"

      for graph in (
        gnb.BN2dot(bn),
        gnb.MRF2UGdot(mrf),
        gnb.MRF2FactorGraphdot(mrf),
        gnb.ID2dot(diag),
        gbn2.BNinference2dot(bn, evs={"A": 1}, targets={"C"}),
        gid2.LIMIDinference2dot(diag, "6", ie, {}, set()),
      ):
        self.assertNotIn("adaptive", graph if isinstance(graph, str) else graph.to_string())
    finally:
      gum.config.pop()

  def testHooks(self):
    cfg = self._freshConfig("[s]\n### int :: x\nk = 1\n")
    calls = []
    cfg.add_hook(lambda: calls.append(1))
    cfg["s", "k"] = 2
    self.assertEqual(len(calls), 1)
    cfg.set("s", "k", 3, no_hook=True)
    self.assertEqual(len(calls), 1)
    cfg.push()
    cfg["s", "k"] = 4
    cfg.pop()
    self.assertEqual(cfg["s", "k"], "3")
    cfg.reset()
    self.assertEqual(cfg["s", "k"], "1")
    self.assertEqual(len(calls), 4)  # set, set, pop, reset (not the no_hook set, nor push)
    with self.assertRaises(IndexError):
      cfg.pop()

  def testHookIsCalledWithPythonErrors(self):
    cfg = self._freshConfig("[s]\n### int :: x\nk = 1\n")

    with self.assertRaises(TypeError):  # a hook must be callable
      cfg.add_hook(3)

    def failing():
      raise RuntimeError("boom in a hook")

    cfg.add_hook(failing)
    with self.assertRaises(RuntimeError) as cm:  # the Python exception is the one received
      cfg["s", "k"] = 2
    self.assertIn("boom in a hook", str(cm.exception))

  def testLoadAndSave(self):
    cwd = os.getcwd()
    gum.config.push()
    try:
      with tempfile.TemporaryDirectory() as tmp:
        os.chdir(tmp)
        try:
          with self.assertRaises(FileNotFoundError):
            gum.config.load()

          with open(gum.pyagrumcpp._Configuration.userFileName(), "w") as f:
            f.write("[notebook]\ntensor_visible_digits = 7\nnotakey = 1\n")
          with warnings.catch_warnings(record=True) as w:
            warnings.simplefilter("always")
            gum.config.load()
          self.assertEqual(gum.config["notebook", "tensor_visible_digits"], "7")
          self.assertEqual(len(w), 1)  # the unknown key is reported as a warning

          # a malformed file changes nothing
          with open(gum.pyagrumcpp._Configuration.userFileName(), "w") as f:
            f.write("[notebook]\ntensor_visible_digits = 3\nmalformed line\n")
          with self.assertRaises(SyntaxError):
            gum.config.load()
          self.assertEqual(gum.config["notebook", "tensor_visible_digits"], "7")

          gum.config.save()
          with open(gum.pyagrumcpp._Configuration.userFileName()) as f:
            self.assertIn("tensor_visible_digits = 7", f.read())
        finally:
          os.chdir(cwd)
    finally:
      gum.config.pop()

  def testRepr(self):
    cfg = self._freshConfig("[s]\n### int :: x\nk = 1\n")
    self.assertTrue(repr(cfg).startswith("# no customized property"))
    cfg["s", "k"] = 2
    self.assertEqual(repr(cfg), "[s]\n  k = 2")


ts = unittest.TestSuite()
addTests(ts, TestConfig)
