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

#include <agrum/CM/tools/causalFormula.h>
#include <agrum/CM/causalModel.h>
#include <agrum/CM/tools/doCalculus.h>
#include <agrum/BN/inference/lazyPropagation.h>

#include <vector>
#include <algorithm>

namespace gum_tests {

  class CausalFormulaTestSuite : public CxxTest::TestSuite {
    public:
    using NameSet = gum::Set<std::string>;
    using NodeSet = gum::NodeSet;

    /**
    * Tests P(Y) = sum_X P(X,Y)
    * This verifies that a basic marginalization formula evaluates correctly,
    * matching the result from a standard inference engine.
    */
    GUM_ACTIVE_TEST(test_observational_marginal) {
      // Setup: A simple X -> Y model
      auto bn = gum::BayesNet<double>::fastPrototype("X{0|1}->Y{0|1}");
      bn.cpt(bn.idFromName("X")).fillWith({0.2, 0.8});
      bn.cpt(bn.idFromName("Y")).fillWith({0.9, 0.1, 0.4, 0.6});
      gum::CausalModel<double> cm(bn);

      // AST for sum_X P(X,Y)
      // The leaf node represents the full joint distribution P(X,Y)
      auto leaf = std::make_unique<gum::ASTjointProba<double>>(NameSet{"X", "Y"});
      // The sum node marginalizes out X
      auto ast = std::make_unique<gum::ASTsum<double>>("X", std::move(leaf));

      // Create the CausalFormula object for P(Y)
      gum::CausalFormula<double> cf(cm, std::move(ast), NameSet{"Y"}, NameSet{});

      // Test LaTeX generation
      TS_ASSERT_EQUALS(cf.latexQuery(), "P\\left(Y\\right)");
      TS_ASSERT_EQUALS(cf.toLatex(), "P\\left(Y\\right) = \\sum_{X}{P\\left(X,Y\\right)}");

      // Test evaluation
      auto result_pot = cf.eval();

      // Calculate expected result using standard inference
      gum::LazyPropagation<double> lp(&bn);
      lp.makeInference();
      auto expected_pot = lp.posterior(bn.idFromName("Y"));

      TS_GUM_TENSOR_ALMOST_EQUALS(result_pot, expected_pot);
    }

    /**
    * Tests P(Y|X) = P(X,Y) / sum_Y P(X,Y)
    */
    GUM_ACTIVE_TEST(test_observational_conditional) {
      // Setup: A simple X -> Y model
      auto bn = gum::BayesNet<double>::fastPrototype("X{0|1}->Y{0|1}");
      bn.cpt(bn.idFromName("X")).fillWith({0.2, 0.8});
      bn.cpt(bn.idFromName("Y")).fillWith({0.9, 0.1, 0.4, 0.6});
      gum::CausalModel<double> cm(bn);

      // AST for P(X,Y) / sum_Y P(X,Y)
      // Numerator is the joint P(X,Y)
      auto ast_numerator = std::make_unique<gum::ASTjointProba<double>>(NameSet{"X", "Y"});
      // Denominator is sum_Y P(X,Y), which is P(X)
      auto ast_denominator_leaf = std::make_unique<gum::ASTjointProba<double>>(NameSet{"X", "Y"});
      auto ast_denominator = std::make_unique<gum::ASTsum<double>>("Y", std::move(ast_denominator_leaf));

      auto ast = std::make_unique<gum::ASTdiv<double>>(std::move(ast_numerator), std::move(ast_denominator));

      // Create the CausalFormula object for P(Y|X)
      gum::CausalFormula<double> cf(cm, std::move(ast), {"Y"}, {"X"});

      // Test LaTeX
      TS_ASSERT_EQUALS(cf.latexQuery(), "P\\left(Y \\mid do(X)\\right)");
      TS_ASSERT_EQUALS(cf.toLatex(), "P\\left(Y \\mid do(X)\\right) =  \\frac {P\\left(X,Y\\right)}{\\sum_{Y}{P\\left(X,Y\'\\right)}}");

      // Test evaluation
      auto result_pot = cf.eval();
      // The expected result for P(Y|X) is simply the CPT of Y
      auto& expected_pot = bn.cpt(bn.idFromName("Y"));

      TS_GUM_TENSOR_ALMOST_EQUALS(result_pot, expected_pot);
    }

    /**
    * Tests P(Y|do(X)) = sum_Z P(Y|X,Z) * P(Z)
    */
    GUM_ACTIVE_TEST(test_interventional_backdoor) {
      // Setup: A model requiring backdoor adjustment
      auto bn = gum::BayesNet<double>::fastPrototype("Z->X;Z->Y;X->Y");
      bn.cpt(bn.idFromName("Z")).fillWith({0.6, 0.4});
      bn.cpt(bn.idFromName("X")).fillWith({0.8, 0.2, 0.3, 0.7});
      bn.cpt(bn.idFromName("Y")).fillWith({0.9, 0.1, 0.8, 0.2, 0.7, 0.3, 0.6, 0.4});
      gum::CausalModel<double> cm(bn);

      // AST for sum_Z P(Y|X,Z) * P(Z)
      auto ast_p_y_given_xz = std::make_unique<gum::ASTposteriorProba<double>>(NameSet{"Y"}, NameSet{"X", "Z"});
      auto ast_p_z = std::make_unique<gum::ASTjointProba<double>>(NameSet{"Z"});
      auto ast_product = std::make_unique<gum::ASTmult<double>>(std::move(ast_p_y_given_xz), std::move(ast_p_z));
      auto ast = std::make_unique<gum::ASTsum<double>>("Z", std::move(ast_product));

      // Create CausalFormula for P(Y | do(X))
      gum::CausalFormula<double> cf(cm, std::move(ast), {"Y"}, NameSet{}, {"X"});

      // Test LaTeX
      TS_ASSERT_EQUALS(cf.latexQuery(), "P\\left(Y \\mid X\\right)");
      TS_ASSERT_EQUALS(cf.toLatex(), "P\\left(Y \\mid X\\right) = \\sum_{Z}{P\\left(Y\\mid X,Z\\right) \\cdot P\\left(Z\\right)}");

      // Test evaluation
      auto result_pot = cf.eval();

      // Calculate expected result dynamically
      auto& P_Y_given_XZ = bn.cpt(bn.idFromName("Y"));
      auto& P_Z = bn.cpt(bn.idFromName("Z"));
      gum::VariableSet z_var_set;
      z_var_set.insert(&bn.variable(cm.idFromName("Z")));
      auto expected_pot = (P_Y_given_XZ * P_Z).sumOut(z_var_set);

      TS_GUM_TENSOR_ALMOST_EQUALS(result_pot, expected_pot);
    }

    /**
    * Tests that the CausalFormula constructor throws an exception for unknown variables.
    */
    GUM_ACTIVE_TEST(test_error_handling_unknown_variable) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y");
      gum::CausalModel<double> cm(bn);
      auto ast = std::make_unique<gum::ASTjointProba<double>>(NameSet{"X"});

      // Should throw because "W" is not in the model.
      // The check happens when CausalFormula validates its variable sets.
      TS_ASSERT_THROWS(
        gum::CausalFormula<double>(cm, std::move(ast), {"Y"}, {"W"}),
        const gum::NotFound&
      );
    }

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

    // Custom do-operator prefix/suffix test (must be last)
    GUM_ACTIVE_TEST(test_CustomDoOperatorLatex) {
      auto bn = gum::BayesNet<double>::fastPrototype("X->Y");
      gum::CausalModel<double> cm(bn);
      auto dummyAST = std::make_unique<gum::ASTjointProba<double>>(NameSet{"Y"});
      gum::CausalFormula<double> cf(cm, std::move(dummyAST), NameSet{"Y"}, NameSet{"X"});
      std::string latex = cf.latexQuery("DO<", ">");
      TS_ASSERT_DIFFERS(latex.find("DO<"), std::string::npos);
      TS_ASSERT_DIFFERS(latex.find(">"), std::string::npos);
      std::string latex2 = cf.toLatex("DO<", ">");
      TS_ASSERT_DIFFERS(latex2.find("DO<"), std::string::npos);
      TS_ASSERT_DIFFERS(latex2.find(">"), std::string::npos);
    }

  };

} // namespace gum_tests
