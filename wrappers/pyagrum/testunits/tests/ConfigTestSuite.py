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

import importlib
import os
import tempfile
import unittest
import warnings

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

# the attribute pyagrum.config is the configuration singleton: the module itself
# (which holds PyAgrumConfiguration and the schema parser) needs an explicit import
gumconfig = importlib.import_module("pyagrum.config")


class TestConfig(pyAgrumTestCase):
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
      gum.config["notebook", "tensor_with_colors"] = "off"

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

  def testDoc(self):
    self.assertEqual(
      gum.config.doc("notebook", "tensor_visible_digits"),
      "int :: number of visible digits when displaying a tensor",
    )
    self.assertIn("histogram|bar", gum.config.doc("notebook", "histogram_discretized_visualisation"))
    with self.assertRaises(SyntaxError):
      gum.config.doc("notebook", "gaga")
    with self.assertRaises(SyntaxError):
      gum.config.doc("gogo", "gaga")

  def testSchemaParser(self):
    schema = gumconfig._parseSchema("[s]\n### int :: a doc\nk = 1\n### a|b :: values\nl = a\n")
    self.assertEqual(schema[("s", "k")].kind, "int")
    self.assertEqual(schema[("s", "k")].doc, "a doc")
    self.assertEqual(schema[("s", "l")].kind, "enum")
    self.assertEqual(schema[("s", "l")].values, ("a", "b"))

    # decorative lines with 4+ '#' are not metadata
    self.assertEqual(gumconfig._parseSchema("[s]\n#### decorative\nk = 1\n"), {})

    with self.assertRaises(SyntaxError):  # missing '::'
      gumconfig._parseSchema("[s]\n### int doc\nk = 1\n")
    with self.assertRaises(SyntaxError):  # unknown type
      gumconfig._parseSchema("[s]\n### uint :: doc\nk = 1\n")
    with self.assertRaises(SyntaxError):  # metadata not immediately followed by a key
      gumconfig._parseSchema("[s]\n### int :: doc\n\nk = 1\n")
    with self.assertRaises(SyntaxError):  # metadata at end of file
      gumconfig._parseSchema("[s]\nk = 1\n### int :: doc\n")

  @staticmethod
  def _freshConfig(content):
    fn = os.path.join(tempfile.mkdtemp(), "defaults.ini")
    with open(fn, "w") as f:
      f.write(content)

    class FreshConfig(gumconfig.PyAgrumConfiguration):
      pass  # new class => new singleton, independent from gum.config

    return FreshConfig(defaultsfn=fn)

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

  def testInvalidDefaultsFailFast(self):
    with self.assertRaises(SyntaxError):  # deprecated -> unknown target
      self._freshConfig("[sec]\nnewkey = 1\n### deprecated -> sec.nokey :: gone\noldkey = 0\n")
    with self.assertRaises(SyntaxError):  # deprecated -> deprecated chain
      self._freshConfig("[sec]\nk = 1\n### deprecated -> sec.b :: x\na = 0\n### deprecated -> sec.a :: y\nb = 0\n")
    with self.assertRaises(ValueError):  # default value violating its own declared type
      self._freshConfig("[sec]\n### int :: doc\nk = notanint\n")


ts = unittest.TestSuite()
addTests(ts, TestConfig)
