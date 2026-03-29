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

import unittest

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestCausalImpact(pyAgrumTestCase):
  def setUp(self):
    # Simple chain X->Y->Z, no confounder — always identifiable
    self.bn_chain = gum.fastBN("X->Y->Z")
    self.cm_chain = gum.CausalModel(self.bn_chain)

    # Smoking->Cancer, no confounder
    self.bn_tobacco = gum.fastBN("Smoking->Cancer")
    self.cm_tobacco = gum.CausalModel(self.bn_tobacco)

    # Non-identifiable: w1->x->z->y; w2->x; w2->z with 4 latent confounders
    bn_ni = gum.fastBN("Z1->X->Z2->Y")
    self.cm_ni = gum.CausalModel(
      bn_ni,
      [("L1", ["Z1", "X"]), ("L2", ["Z1", "Z2"]), ("L3", ["Z1", "Y"]), ("L4", ["Y", "X"])],
      True
    )

  # ------------------------------------------------------------------
  # Constructor
  # ------------------------------------------------------------------

  def test_constructor_keyword_only(self):
    # on, doing, knowing must be keyword-only: positional use raises TypeError
    with self.assertRaises(TypeError):
      gum.CausalImpact(self.cm_chain, "Z", "X")

  def test_constructor_str_converted_to_set(self):
    ci_str = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    ci_set = gum.CausalImpact(self.cm_chain, on={"Z"}, doing={"X"})
    self.assertTrue(ci_str.isIdentified())
    self.assertTrue(ci_set.isIdentified())
    self.assertEqual(ci_str.onNames(), ci_set.onNames())
    self.assertEqual(ci_str.doingNames(), ci_set.doingNames())

  def test_constructor_knowing_str_converted_to_set(self):
    # X->Y->Z: conditioning on Y as string
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X", knowing="Y")
    self.assertIsNotNone(ci)

  def test_constructor_knowing_defaults_to_empty(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    self.assertEqual(len(ci.knowing()), 0)

  # ------------------------------------------------------------------
  # Identification
  # ------------------------------------------------------------------

  def test_is_identified_simple_chain(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    self.assertTrue(ci.isIdentified())

  def test_is_not_identified(self):
    ci = gum.CausalImpact(self.cm_ni, on="Y", doing="X")
    self.assertFalse(ci.isIdentified())

  # ------------------------------------------------------------------
  # Accessors: node sets and names
  # ------------------------------------------------------------------

  def test_on_names(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    self.assertIn("Z", ci.onNames())

  def test_doing_names(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    self.assertIn("X", ci.doingNames())

  def test_knowing_names_empty(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    self.assertEqual(ci.knowingNames(), ())

  def test_knowing_names_nonempty(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X", knowing="Y")
    self.assertIn("Y", ci.knowingNames())

  def test_on_returns_nodeids(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    self.assertEqual(len(ci.on()), 1)

  def test_doing_returns_nodeids(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    self.assertEqual(len(ci.doing()), 1)

  def test_cm_accessor(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    # cm() should return the same causal model (same underlying BN)
    self.assertEqual(
      ci.cm().observationalBN().names(),
      self.cm_chain.observationalBN().names(),
    )

  # ------------------------------------------------------------------
  # Explanation
  # ------------------------------------------------------------------

  def test_explanation_nonempty(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    self.assertIsInstance(ci.explanation(), str)
    self.assertGreater(len(ci.explanation()), 0)

  def test_explanation_not_identified(self):
    ci = gum.CausalImpact(self.cm_ni, on="Y", doing="X")
    self.assertIsInstance(ci.explanation(), str)
    self.assertGreater(len(ci.explanation()), 0)

  # ------------------------------------------------------------------
  # eval()
  # ------------------------------------------------------------------

  def test_eval_identified_returns_tensor(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    t = ci.eval()
    self.assertIsInstance(t, gum.Tensor)
    self.assertIn("Z", t.names)

  def test_eval_not_identified_raises(self):
    ci = gum.CausalImpact(self.cm_ni,on="Y", doing="X")
    with self.assertRaises(gum.OperationNotAllowed):
      ci.eval()

  def test_eval_simple_chain_no_confounder(self):
    # X->Y: no confounder => P(Y|do(X)) = P(Y|X)
    ci = gum.CausalImpact(self.cm_tobacco, on="Cancer", doing="Smoking")
    t = ci.eval()
    self.assertAlmostEqual(t.sum(), 2.0, places=6)

  # ------------------------------------------------------------------
  # String representations
  # ------------------------------------------------------------------

  def test_to_string_nonempty(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    s = ci.toString()
    self.assertIsInstance(s, str)
    self.assertGreater(len(s), 0)

  def test_to_latex_nonempty(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    s = ci.toLatex()
    self.assertIsInstance(s, str)
    self.assertGreater(len(s), 0)

  def test_latex_query_nonempty(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    s = ci.latexQuery()
    self.assertIsInstance(s, str)
    self.assertGreater(len(s), 0)

  def test_latex_custom_do_operator(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    s = ci.toLatex(doOperatorPrefix="[", doOperatorSuffix="]")
    self.assertIn("[", s)

  # ------------------------------------------------------------------
  # toDict()
  # ------------------------------------------------------------------

  def test_toDict_not_identified_returns_none(self):
    ci = gum.CausalImpact(self.cm_ni, on="Y", doing="X")
    self.assertIsNone(ci.toDict())

  def test_toDict_identified_returns_dict(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    d = ci.toDict()
    self.assertIsInstance(d, dict)

  def test_toDict_has_op_key(self):
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    d = ci.toDict()
    self.assertIn("op", d)

  def test_toDict_no_confounder_is_posterior(self):
    # Smoking->Cancer with no confounder: P(Cancer|do(Smoking)) = P(Cancer|Smoking)
    # The AST root should be a posterior proba node.
    ci = gum.CausalImpact(self.cm_tobacco, on="Cancer", doing="Smoking")
    d = ci.toDict()
    self.assertEqual(d["op"], "P")
    self.assertIn("vars", d)
    self.assertIn("Cancer", d["vars"])
    self.assertIn("knowing", d)
    self.assertIn("Smoking", d["knowing"])

  def test_toDict_is_json_serialisable(self):
    import json
    ci = gum.CausalImpact(self.cm_chain, on="Z", doing="X")
    try:
      json.dumps(ci.toDict())
    except (TypeError, ValueError) as e:
      self.fail(f"ast() is not JSON-serialisable: {e}")

  def test_toDict_recursive_structure(self):
    # For a backdoor adjustment W->X->Y, W->Y:
    # P(Y|do(X)) = sum_W P(Y|X,W)*P(W)
    # The AST should contain a sum node at some level.
    bn = gum.fastBN("W->X->Y;W->Y")
    cm = gum.CausalModel(bn)
    ci = gum.CausalImpact(cm, on="Y", doing="X")
    self.assertTrue(ci.isIdentified())
    d = ci.toDict()
    self.assertIsInstance(d, dict)

    def _collect_ops(node):
      ops = {node["op"]}
      for k in ("op1", "op2", "term"):
        if k in node:
          ops |= _collect_ops(node[k])
      return ops

    ops = _collect_ops(d)
    # Backdoor formula involves marginalisation (sum) and products
    self.assertIn("sum", ops)


ts = unittest.TestSuite()
addTests(ts, TestCausalImpact)
