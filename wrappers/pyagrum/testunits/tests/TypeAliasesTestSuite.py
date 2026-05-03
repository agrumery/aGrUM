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

import types
import typing
import unittest

import pyagrum as gum

from .pyAgrumTestSuite import pyAgrumTestCase, addTests


def _members(alias) -> set:
  """Return the set of types in alias (Union or single type)."""
  args = typing.get_args(alias)
  return set(args) if args else {alias}


def _is_multi_union(alias) -> bool:
  origin = typing.get_origin(alias)
  return origin is typing.Union or origin is types.UnionType


class TestTypeAliases(pyAgrumTestCase):
  def testAliasesExist(self):
    for name in ("BNInference", "MRFInference", "CNInference", "IDInference", "Graph", "PGM"):
      self.assertTrue(hasattr(gum, name), f"pyagrum.{name} not defined")

  def testBNInference(self):
    self.assertTrue(_is_multi_union(gum.BNInference))
    expected = {
      gum.LazyPropagation,
      gum.ShaferShenoyInference,
      gum.VariableElimination,
      gum.GibbsSampling,
      gum.ImportanceSampling,
      gum.WeightedSampling,
      gum.MonteCarloSampling,
      gum.LoopyBeliefPropagation,
      gum.LoopyGibbsSampling,
      gum.LoopyImportanceSampling,
      gum.LoopyWeightedSampling,
      gum.LoopyMonteCarloSampling,
    }
    self.assertEqual(_members(gum.BNInference), expected)

    bn = gum.BayesNet()
    bn.add(gum.LabelizedVariable("x", "", 2))
    self.assertIsInstance(gum.LazyPropagation(bn), tuple(_members(gum.BNInference)))

  def testMRFInference(self):
    self.assertIs(gum.MRFInference, gum.ShaferShenoyMRFInference)

    mrf = gum.MarkovRandomField()
    mrf.add(gum.LabelizedVariable("x", "", 2))
    self.assertIsInstance(gum.ShaferShenoyMRFInference(mrf), gum.MRFInference)

  def testCNInference(self):
    self.assertTrue(_is_multi_union(gum.CNInference))
    self.assertEqual(_members(gum.CNInference), {gum.CNLoopyPropagation, gum.CNMonteCarloSampling})

  def testIDInference(self):
    self.assertIs(gum.IDInference, gum.ShaferShenoyLIMIDInference)

  def testGraph(self):
    self.assertTrue(_is_multi_union(gum.Graph))
    self.assertEqual(_members(gum.Graph), {gum.DiGraph, gum.DAG, gum.UndiGraph, gum.MixedGraph})

    for cls in (gum.DAG, gum.DiGraph, gum.UndiGraph, gum.MixedGraph):
      self.assertIsInstance(cls(), tuple(_members(gum.Graph)))

  def testPGM(self):
    self.assertTrue(_is_multi_union(gum.PGM))
    expected = {gum.BayesNet, gum.MarkovRandomField, gum.InfluenceDiagram, gum.CredalNet}
    self.assertEqual(_members(gum.PGM), expected)

    self.assertIsInstance(gum.BayesNet(), tuple(_members(gum.PGM)))
    self.assertIsInstance(gum.MarkovRandomField(), tuple(_members(gum.PGM)))
    self.assertIsInstance(gum.InfluenceDiagram(), tuple(_members(gum.PGM)))

  def testAnnotationsFunctions(self):
    # typing.get_type_hints() forces evaluation of all annotations in the module
    # namespace — catches missing type references even with lazy annotations
    # (from __future__ import annotations) and on Python 3.10-3.13 eager eval.
    from pyagrum.lib import bn2graph, mrf2graph, cn2graph, image, notebook

    for func, label in (
      (bn2graph.BNinference2dot, "bn2graph.BNinference2dot"),
      (mrf2graph.MRFinference2UGdot, "mrf2graph.MRFinference2UGdot"),
      (mrf2graph.MRFinference2FactorGraphdot, "mrf2graph.MRFinference2FactorGraphdot"),
      (cn2graph.CNinference2dot, "cn2graph.CNinference2dot"),
      (image.prepareShowInference, "image.prepareShowInference"),
      (notebook.showInference, "notebook.showInference"),
      (notebook.getInference, "notebook.getInference"),
    ):
      with self.subTest(func=label):
        # raises NameError/AttributeError if any annotation cannot be resolved
        hints = typing.get_type_hints(func)
        self.assertIsInstance(hints, dict)


ts = unittest.TestSuite()
addTests(ts, TestTypeAliases)
