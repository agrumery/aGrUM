/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/CM/causalFormula.h>
#include <agrum/CM/causalModel.h>
#include <agrum/CM/doCalculus.h>
#include <agrum/BN/inference/lazyPropagation.h>

#include <vector>
#include <algorithm>

namespace gum_tests {

  class CausalFormulaTestSuite : public CxxTest::TestSuite {
    public:
    using NameSet = gum::Set<std::string>;
    using NodeSet = gum::NodeSet;


    GUM_ACTIVE_TEST(LatexQueryRendering) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y<-Z; W->X");
      gum::CausalModel<double> cm(bn);
      auto dummyAST = std::make_unique<gum::ASTjointProba<double>>(NameSet{"Y"});

      // Case 1: P(Y | do(X))
      {
        gum::CausalFormula<double> cf(cm, dummyAST->copy(), NameSet{"Y"}, NameSet{"X"});
        TS_ASSERT_EQUALS(cf.latexQuery(), "P\\left(Y \\mid do(X)\\right)");
      }

      // Case 2: P(Y | do(X), Z)
      {
        gum::CausalFormula<double> cf(cm, dummyAST->copy(), NameSet{"Y"}, NameSet{"X"}, NameSet{"Z"});
        TS_ASSERT_EQUALS(cf.latexQuery(), "P\\left(Y \\mid do(X),Z\\right)");
      }

      // Case 3: P(Y | do(X,W)) - multiple interventions
      {
        gum::CausalFormula<double> cf(cm, dummyAST->copy(), NameSet{"Y"}, NameSet{"X", "W"});
        TS_ASSERT_EQUALS(cf.latexQuery(), "P\\left(Y \\mid do(W,X)\\right)");
      }

      // Case 4: P(Y,Z | do(X)) - multiple outcomes
      {
        gum::CausalFormula<double> cf(cm, dummyAST->copy(), NameSet{"Y", "Z"}, NameSet{"X"});
        TS_ASSERT_EQUALS(cf.latexQuery(), "P\\left(Y,Z \\mid do(X)\\right)");
      }

      // Case 5: P(Y | Z) - observational (no 'do')
      {
        gum::CausalFormula<double> cf(cm, dummyAST->copy(), NameSet{"Y"}, NameSet{}, NameSet{"Z"});
        TS_ASSERT_EQUALS(cf.latexQuery(), "P\\left(Y \\mid Z\\right)");
      }
    }

    GUM_ACTIVE_TEST(FullLatex_And_Eval) {
      // Setup a model where backdoor adjustment is needed
      auto bn = gum::BayesNet<double>::fastPrototype("Z->X;Z->Y;X->Y");

      // Randomly initialize CPTs.
      bn.generateCPTs();

      gum::CausalModel<double> cm(bn);
      gum::DoCalculus<double> dc(cm);

      // Get the AST for P(Y | do(X)) using backdoor on Z
      // Use NameList (vector) instead of NameSet for the z-set argument
      auto ast = dc.getBackDoorTree("X", "Y", gum::DoCalculus<double>::NameList{"Z"});

      // Create the CausalFormula object
      gum::CausalFormula<double> cf(cm, std::move(ast), NameSet{"Y"}, NameSet{"X"});

      // Test full LaTeX string
      std::string expected_latex = "P\\left(Y \\mid do(X)\\right) = \\sum_{Z}{P\\left(Y\\mid X,Z\\right) \\cdot P\\left(Z\\right)}";
      TS_ASSERT_EQUALS(cf.toLatex(), expected_latex);

      // Test eval()
      auto result_pot = cf.eval();

      auto P_Y_given_XZ = bn.cpt(bn.idFromName("Y"));
      auto P_Z = bn.cpt(bn.idFromName("Z"));

      // Use VariableSet for sumOut
      gum::VariableSet z_var_set;
      z_var_set.insert(&bn.variable(cm.idFromName("Z")));
      auto expected_pot = (P_Y_given_XZ * P_Z).sumOut(z_var_set);

      TS_GUM_TENSOR_ALMOST_EQUALS(result_pot, expected_pot);
    }

    GUM_ACTIVE_TEST(ToString_DelegatesToRoot) {
        auto bn = gum::BayesNet<double>::fastPrototype("A->B");
        gum::CausalModel<double> cm(bn);
        auto ast = std::make_unique<gum::ASTposteriorProba<double>>(bn, NameSet{"B"}, NameSet{"A"});
        auto expected_str = ast->toString();

        gum::CausalFormula<double> cf(cm, std::move(ast), NameSet{"B"}, NameSet{"A"});
        TS_ASSERT_EQUALS(cf.toString(), expected_str);
    }

    GUM_ACTIVE_TEST(Copy_IsDeep) {
        auto bn = gum::BayesNet<double>::fastPrototype("A->B");
        gum::CausalModel<double> cm(bn);
        auto ast = std::make_unique<gum::ASTposteriorProba<double>>(bn, NameSet{"B"}, NameSet{"A"});

        gum::CausalFormula<double> original(cm, std::move(ast), NameSet{"B"}, NameSet{"A"});
        auto clone = original.copy();

        // Pointers to root AST must be different
        TS_ASSERT_DIFFERS(&original.root(), &clone->root());

        // Check for value equality
        TS_ASSERT_EQUALS(original.toLatex(), clone->toLatex());
        TS_GUM_TENSOR_ALMOST_EQUALS(original.eval(), clone->eval());
    }

  };

} // namespace gum_tests
