/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/

#pragma once

#include <memory>
#include <string>

#include <gumtest/AgrumTestSuite.h>

#include <agrum/agrum.h>

#include <agrum/base/core/bijection.h>
#include <agrum/base/core/exceptions.h>
#include <agrum/base/core/set.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/CM/tools/doAST.h>


#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  DoAST
#define GUM_CURRENT_MODULE CM

namespace gum_tests {

  struct DoASTTestSuite {
    public:
    static gum::Bijection< gum::NodeId, std::string >
        id2name_fromBN(const gum::BayesNet< double >& bn) {
      gum::Bijection< gum::NodeId, std::string > id2name;
      for (const auto node: bn.nodes()) {
        id2name.insert(node, bn.variable(node).name());
      }
      return id2name;
    }

    // -------------------------------------------------------------------
    // Reproduce the pyAgrum __str__ tests
    // -------------------------------------------------------------------
    static void testStr_like_pyagrum() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");

      gum::Set< std::string > sAC, sB, sA, sC;
      sAC.insert("A");
      sAC.insert("C");
      sB.insert("B");
      sA.insert("A");
      sC.insert("C");

      gum::ASTposteriorProba< double > nod(bn, sAC, sB);
      CHECK_EQ(nod.toString(), "P(A,C|B)");

      gum::ASTposteriorProba< double > nod2(bn, sA, sC);
      CHECK_EQ(nod2.toString(), "P(A|C)");

      gum::ASTposteriorProba< double > nod3(bn.internalDag(), id2name_fromBN(bn), sAC, sB);
      CHECK_EQ(nod3.toString(), "P(A,C|B)");

      gum::ASTposteriorProba< double > nod4(bn.internalDag(), id2name_fromBN(bn), sA, sC);
      CHECK_EQ(nod4.toString(), "P(A|C)");
      // sum on A
      auto n1 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB);

      gum::ASTsum< double > su("A", std::move(n1));
      CHECK_EQ(su.toString(), "sum on A for\n| P(A,C|B)");

      // sum on A,C (accept flat or nested formatting)
      auto n2 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB);
      std::vector< std::string > twoVars;
      twoVars.push_back("A");
      twoVars.push_back("C");
      gum::ASTsum< double > su2(twoVars, std::move(n2));
      const std::string     flat   = "sum on A,C for\n| P(A,C|B)";
      const std::string     nested = "sum on A for\n| sum on C for\n| | P(A,C|B)";
      const std::string     got    = su2.toString();
      CHECK((got == flat || got == nested));

      // division
      auto                  p1 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB);
      auto                  p2 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC);
      gum::ASTdiv< double > op(std::move(p1), std::move(p2));
      CHECK_EQ(op.toString(), "/\n| P(A,C|B)\n| P(A|C)");

      // plus
      auto                   p3 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB);
      auto                   p4 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC);
      gum::ASTplus< double > op2(std::move(p3), std::move(p4));
      CHECK_EQ(op2.toString(), "+\n| P(A,C|B)\n| P(A|C)");

      // mult(op, op2)
      auto m1 = std::make_unique< gum::ASTdiv< double > >(
          std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB),
          std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC));
      auto m2 = std::make_unique< gum::ASTplus< double > >(
          std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB),
          std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC));
      gum::ASTmult< double > mult(std::move(m1), std::move(m2));
      CHECK_EQ((mult.toString()),
               (std::string()
                + "*\n"
                  "| /\n"
                  "| | P(A,C|B)\n"
                  "| | P(A|C)\n"
                  "| +\n"
                  "| | P(A,C|B)\n"
                  "| | P(A|C)"));

      // minus(op2, op)
      auto mm1 = std::make_unique< gum::ASTplus< double > >(
          std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB),
          std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC));
      auto mm2 = std::make_unique< gum::ASTdiv< double > >(
          std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB),
          std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC));
      gum::ASTminus< double > minu(std::move(mm1), std::move(mm2));
      CHECK_EQ((minu.toString()),
               (std::string()
                + "-\n"
                  "| +\n"
                  "| | P(A,C|B)\n"
                  "| | P(A|C)\n"
                  "| /\n"
                  "| | P(A,C|B)\n"
                  "| | P(A|C)"));

      gum::ASTjointProba< double > j1([] {
        gum::Set< std::string > s;
        s.insert("A");
        return s;
      }());
      CHECK_EQ(j1.toString(), "joint P(A)");
      gum::ASTjointProba< double > j2([] {
        gum::Set< std::string > s;
        s.insert("A");
        s.insert("C");
        return s;
      }());
      CHECK_EQ(j2.toString(), "joint P(A,C)");
    }

    // -------------------------------------------------------------------
    // Test conditional simplification
    // -------------------------------------------------------------------
    static void testConditionalSimplification() {
      auto m = gum::BayesNet< double >::fastPrototype("z2->x->z1->y;z2->z1;z2->z3->y");
      gum::Set< std::string > y, knw;
      y.insert("y");
      knw.insert("x");
      knw.insert("z1");
      knw.insert("z2");
      knw.insert("z3");
      gum::ASTposteriorProba< double > nod(m, y, knw);
      CHECK_EQ(nod.toString(), "P(y|z1,z3)");

      gum::ASTposteriorProba< double > nod2(m.internalDag(), id2name_fromBN(m), y, knw);
      CHECK_EQ(nod2.toString(), "P(y|z1,z3)");
    }

    // -------------------------------------------------------------------
    // EVAL TESTS (Tensor)
    // -------------------------------------------------------------------

    // 1) Posterior CPT shortcut: P(B | A) should return bn.cpt(B)
    static void testEval_Posterior_CPT_Shortcut() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B");

      gum::Set< std::string > varB, condA;
      varB.insert("B");
      condA.insert("A");

      gum::ASTposteriorProba< double > node(bn, varB, condA);
      auto                             got = node.eval(bn);

      auto ref = bn.cpt(bn.idFromName("B"));
      GUM_CHECK_TENSOR_ALMOST_EQUALS(got, ref);

      gum::ASTposteriorProba< double > node2(bn.internalDag(), id2name_fromBN(bn), varB, condA);
      auto                             got2 = node2.eval(bn);
    }

    // 2) Posterior without conditioning: P(C)
    static void testEval_Posterior_NoConditioning() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");

      gum::Set< std::string > varC;
      varC.insert("C");
      gum::Set< std::string > empty;

      gum::ASTposteriorProba< double > node(bn, varC, empty);
      auto                             got = node.eval(bn);

      gum::LazyPropagation< double > ie(&bn);
      ie.addTarget(bn.idFromName("C"));
      ie.makeInference();
      auto ref = ie.posterior(bn.idFromName("C"));

      GUM_CHECK_TENSOR_ALMOST_EQUALS(got, ref);

      gum::ASTposteriorProba< double > node2(bn.internalDag(), id2name_fromBN(bn), varC, empty);
      auto                             got2 = node2.eval(bn);
      GUM_CHECK_TENSOR_ALMOST_EQUALS(got2, ref);
    }

    // 3) Posterior with conditioning: P(C | B) = P(C,B) / P(B)
    static void testEval_Posterior_WithConditioning() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");

      gum::Set< std::string > varC, condB;
      varC.insert("C");
      condB.insert("B");
      gum::ASTposteriorProba< double > node(bn, varC, condB);
      auto                             got = node.eval(bn);

      gum::LazyPropagation< double > ie(&bn);
      gum::NodeSet                   cb;
      cb.insert(bn.idFromName("C"));
      cb.insert(bn.idFromName("B"));
      gum::NodeSet bset;
      bset.insert(bn.idFromName("B"));
      ie.addJointTarget(cb);
      ie.addJointTarget(bset);
      ie.makeInference();
      auto ref = ie.jointPosterior(cb) / ie.jointPosterior(bset);

      GUM_CHECK_TENSOR_ALMOST_EQUALS(got, ref);

      gum::ASTposteriorProba< double > node2(bn.internalDag(), id2name_fromBN(bn), varC, condB);
      auto                             got2 = node2.eval(bn);
      GUM_CHECK_TENSOR_ALMOST_EQUALS(got2, ref);
    }

    // 4) Joint: single and pair
    static void testEval_Joint() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");

      // single
      gum::Set< std::string > sA;
      sA.insert("A");
      gum::ASTjointProba< double > jA(sA);
      auto                         gotA = jA.eval(bn);

      gum::LazyPropagation< double > ie1(&bn);
      ie1.addTarget(bn.idFromName("A"));
      ie1.makeInference();
      auto refA = ie1.posterior(bn.idFromName("A"));
      GUM_CHECK_TENSOR_ALMOST_EQUALS(gotA, refA);

      // pair
      gum::Set< std::string > sBC;
      sBC.insert("B");
      sBC.insert("C");
      gum::ASTjointProba< double > jBC(sBC);
      auto                         gotBC = jBC.eval(bn);

      gum::LazyPropagation< double > ie2(&bn);
      gum::NodeSet                   bc;
      bc.insert(bn.idFromName("B"));
      bc.insert(bn.idFromName("C"));
      ie2.addJointTarget(bc);
      ie2.makeInference();
      auto refBC = ie2.jointPosterior(bc);
      GUM_CHECK_TENSOR_ALMOST_EQUALS(gotBC, refBC);
    }

    // 5) Sum: Σ_A P(A,C|B) = P(C|B)
    static void testEval_Sum() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");

      gum::Set< std::string > vars;
      vars.insert("A");
      vars.insert("C");
      gum::Set< std::string > knw;
      knw.insert("B");

      auto term = std::make_unique< gum::ASTposteriorProba< double > >(bn, vars, knw);
      gum::ASTsum< double > sumA("A", std::move(term));
      auto                  got = sumA.eval(bn);

      gum::LazyPropagation< double > ie(&bn);
      gum::NodeSet                   cb;
      cb.insert(bn.idFromName("C"));
      cb.insert(bn.idFromName("B"));
      gum::NodeSet bset;
      bset.insert(bn.idFromName("B"));
      ie.addJointTarget(cb);
      ie.addJointTarget(bset);
      ie.makeInference();
      auto ref = ie.jointPosterior(cb) / ie.jointPosterior(bset);

      GUM_CHECK_TENSOR_ALMOST_EQUALS(got, ref);
    }

    // 6) Binary ops (+,-,*,/): use same-scope factor P(A|C) with itself
    static void testEval_BinaryOps_SameScope() {
      auto bn = gum::BayesNet< double >::fastPrototype("C->A; D");

      gum::Set< std::string > varA;
      varA.insert("A");
      gum::Set< std::string > condC;
      condC.insert("C");

      // reference factor p = P(A|C)
      gum::ASTposteriorProba< double > pref(bn, varA, condC);
      auto                             p = pref.eval(bn);

      // plus
      auto t1 = std::make_unique< gum::ASTposteriorProba< double > >(bn, varA, condC);
      auto t2 = std::make_unique< gum::ASTposteriorProba< double > >(bn, varA, condC);
      gum::ASTplus< double > plus(std::move(t1), std::move(t2));
      GUM_CHECK_TENSOR_ALMOST_EQUALS(plus.eval(bn), p + p);

      // minus
      auto t3 = std::make_unique< gum::ASTposteriorProba< double > >(bn, varA, condC);
      auto t4 = std::make_unique< gum::ASTposteriorProba< double > >(bn, varA, condC);
      gum::ASTminus< double > minus(std::move(t3), std::move(t4));
      GUM_CHECK_TENSOR_ALMOST_EQUALS(minus.eval(bn), p - p);

      // mult
      auto t5 = std::make_unique< gum::ASTposteriorProba< double > >(bn, varA, condC);
      auto t6 = std::make_unique< gum::ASTposteriorProba< double > >(bn, varA, condC);
      gum::ASTmult< double > mult(std::move(t5), std::move(t6));
      GUM_CHECK_TENSOR_ALMOST_EQUALS(mult.eval(bn), p * p);

      // div
      auto t7 = std::make_unique< gum::ASTposteriorProba< double > >(bn, varA, condC);
      auto t8 = std::make_unique< gum::ASTposteriorProba< double > >(bn, varA, condC);
      gum::ASTdiv< double > div(std::move(t7), std::move(t8));
      GUM_CHECK_TENSOR_ALMOST_EQUALS(div.eval(bn), p / p);
    }

    // 7) productOfTrees: multiply identical factors
    static void testEval_ProductOfTrees() {
      auto bn = gum::BayesNet< double >::fastPrototype("X->Y");

      gum::Set< std::string > varY;
      varY.insert("Y");
      gum::Set< std::string > condX;
      condX.insert("X");

      std::vector< std::unique_ptr< gum::ASTtree< double > > > terms;
      terms.push_back(std::make_unique< gum::ASTposteriorProba< double > >(bn, varY, condX));
      terms.push_back(std::make_unique< gum::ASTposteriorProba< double > >(bn, varY, condX));

      auto tree = gum::productOfTrees< double >(std::move(terms));
      auto got  = tree->eval(bn);

      gum::ASTposteriorProba< double > refNode(bn, varY, condX);
      auto                             p = refNode.eval(bn);

      GUM_CHECK_TENSOR_ALMOST_EQUALS(got, p * p);
    }

    // 8) Constructor error paths
    static void testThrows_On_Empty_VarSets() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B");

      gum::Set< std::string > empty, B;
      B.insert("B");
      CHECK_THROWS_AS((gum::ASTposteriorProba< double >(bn, empty, B)),
                      const gum::InvalidArgument&);
      CHECK_THROWS_AS((gum::ASTjointProba< double >(empty)), const gum::InvalidArgument&);
    }

    // 9) Copy() deep-copy equivalence
    static void testCopy_DeepCopy() {
      auto                    bn = gum::BayesNet< double >::fastPrototype("A->B->C");
      gum::Set< std::string > C;
      C.insert("C");
      gum::Set< std::string > B;
      B.insert("B");

      auto orig  = std::make_unique< gum::ASTposteriorProba< double > >(bn, C, B);
      auto clone = orig->copy();

      CHECK_EQ(orig->toString(), clone->toString());
      CHECK_EQ(orig->toLatex(), clone->toLatex());
      GUM_CHECK_TENSOR_ALMOST_EQUALS(orig->eval(bn), clone->eval(bn));
    }

    // 10) productOfTrees: empty and single-term cases
    static void testProductOfTrees_EdgeCases() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B");

      gum::Set< std::string > B;
      B.insert("B");
      gum::Set< std::string > A;
      A.insert("A");

      // single term -> identity
      std::vector< std::unique_ptr< gum::ASTtree< double > > > one;
      one.push_back(std::make_unique< gum::ASTposteriorProba< double > >(bn, B, A));
      auto single = gum::productOfTrees< double >(std::move(one));
      auto ref    = gum::ASTposteriorProba< double >(bn, B, A).eval(bn);
      GUM_CHECK_TENSOR_ALMOST_EQUALS(single->eval(bn), ref);

      // empty -> throws
      std::vector< std::unique_ptr< gum::ASTtree< double > > > empty;
      CHECK_THROWS_AS((void)(gum::productOfTrees< double >(std::move(empty))),
                      const gum::InvalidArgument&);
    }

    // 11) Scope composition: P(A|C) * P(C) == P(A,C)
    static void testEval_Compose_DifferentScopes() {
      auto bn = gum::BayesNet< double >::fastPrototype("C->A");

      gum::Set< std::string > A;
      A.insert("A");
      gum::Set< std::string > C;
      C.insert("C");
      gum::Set< std::string > none;

      auto t1 = std::make_unique< gum::ASTposteriorProba< double > >(bn, A, C);      // P(A|C)
      auto t2 = std::make_unique< gum::ASTposteriorProba< double > >(bn, C, none);   // P(C)
      gum::ASTmult< double > prod(std::move(t1), std::move(t2));
      auto                   got = prod.eval(bn);

      gum::LazyPropagation< double > ie(&bn);
      gum::NodeSet                   AC;
      AC.insert(bn.idFromName("A"));
      AC.insert(bn.idFromName("C"));
      ie.addJointTarget(AC);
      ie.makeInference();
      auto ref = ie.jointPosterior(AC);

      GUM_CHECK_TENSOR_ALMOST_EQUALS(got, ref);

      auto t3 = std::make_unique< gum::ASTposteriorProba< double > >(bn.internalDag(),
                                                                     id2name_fromBN(bn),
                                                                     A,
                                                                     C);   // P(A|C)
      auto t4 = std::make_unique< gum::ASTposteriorProba< double > >(bn.internalDag(),
                                                                     id2name_fromBN(bn),
                                                                     C,
                                                                     none);   // P(C)
      gum::ASTmult< double > prod2(std::move(t3), std::move(t4));
      auto                   got2 = prod2.eval(bn);
      GUM_CHECK_TENSOR_ALMOST_EQUALS(got2, ref);
    }

    // 12) Sum flattening equivalence: Σ_A Σ_C == Σ_{A,C}
    static void testEval_Sum_Flattening_Equivalence() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");

      gum::Set< std::string > vars;
      vars.insert("A");
      vars.insert("C");
      gum::Set< std::string > knw;
      knw.insert("B");

      auto term1  = std::make_unique< gum::ASTposteriorProba< double > >(bn, vars, knw);
      auto nested = std::make_unique< gum::ASTsum< double > >(
          "A",
          std::make_unique< gum::ASTsum< double > >("C", std::move(term1)));
      auto got_nested = nested->eval(bn);

      auto term2 = std::make_unique< gum::ASTposteriorProba< double > >(bn, vars, knw);
      std::vector< std::string > AC;
      AC.push_back("A");
      AC.push_back("C");
      gum::ASTsum< double > flat(AC, std::move(term2));
      auto                  got_flat = flat.eval(bn);

      GUM_CHECK_TENSOR_ALMOST_EQUALS(got_nested, got_flat);
    }

    // 13) Deterministic ordering regardless of insertion order
    static void testDeterministic_Ordering() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");

      gum::Set< std::string > sCA, sB;   // insert in reverse
      sCA.insert("C");
      sCA.insert("A");
      sB.insert("B");

      gum::ASTposteriorProba< double > nod(bn, sCA, sB);
      CHECK_EQ(nod.toString(), "P(A,C|B)");
      CHECK_EQ(nod.toLatex(), "P\\left(A,C\\mid B\\right)");

      gum::ASTjointProba< double > j(sCA);
      CHECK_EQ(j.toString(), "joint P(A,C)");
      CHECK_EQ(j.toLatex(), "P\\left(A,C\\right)");
    }

    // -------------------------------------------------------------------
    // LaTeX rendering tests (match current fastToLatex behaviour exactly)
    // -------------------------------------------------------------------
    static void testLatex_like_pyagrum() {
      auto bn = gum::BayesNet< double >::fastPrototype("A->B->C<-D");

      gum::Set< std::string > sAC, sB, sA, sC;
      sAC.insert("A");
      sAC.insert("C");
      sB.insert("B");
      sA.insert("A");
      sC.insert("C");

      // P(A,C | B)
      gum::ASTposteriorProba< double > nod(bn, sAC, sB);
      CHECK_EQ(nod.toLatex(), "P\\left(A,C\\mid B\\right)");

      // P(A | C)
      gum::ASTposteriorProba< double > nod2(bn, sA, sC);
      CHECK_EQ(nod2.toLatex(), "P\\left(A\\mid C\\right)");

      // sum over A :  \sum_{A}{ P(A,C | B) }
      {
        auto n1 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB);
        auto su = gum::ASTsum< double >("A", std::move(n1));
        CHECK_EQ(su.toLatex(), "\\sum_{A}{P\\left(A,C\\mid B\\right)}");
      }

      // sum over A,C (flattened):  \sum_{A,C}{ P(A,C | B) }
      {
        auto n2 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB);
        std::vector< std::string > twoVars;
        twoVars.push_back("A");
        twoVars.push_back("C");
        gum::ASTsum< double > su2(twoVars, std::move(n2));
        CHECK_EQ(su2.toLatex(), "\\sum_{A,C}{P\\left(A,C\\mid B\\right)}");
      }

      // division: note the intentional leading space before \frac
      {
        auto p1 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB);
        auto p2 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC);
        gum::ASTdiv< double > op(std::move(p1), std::move(p2));
        CHECK_EQ((op.toLatex()),
                 (" \\frac {P\\left(A,C\\mid B\\right)}{P\\left(A\\mid C\\right)}"));
      }

      // plus: no parentheses, no spaces around '+'
      {
        auto p3 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB);
        auto p4 = std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC);
        gum::ASTplus< double > op2(std::move(p3), std::move(p4));
        CHECK_EQ(op2.toLatex(), "P\\left(A,C\\mid B\\right)+P\\left(A\\mid C\\right)");
      }

      // mult(div, plus): spaces around \cdot, no extra parentheses
      {
        auto m1 = std::make_unique< gum::ASTdiv< double > >(
            std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB),
            std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC));
        auto m2 = std::make_unique< gum::ASTplus< double > >(
            std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB),
            std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC));
        gum::ASTmult< double > mult(std::move(m1), std::move(m2));
        CHECK_EQ((mult.toLatex()),
                 (" \\frac {P\\left(A,C\\mid B\\right)}{P\\left(A\\mid C\\right)} \\cdot "
                  "P\\left(A,C\\mid B\\right)+P\\left(A\\mid C\\right)"));
      }

      // minus(plus, div): note the single space after '-' coming from div's leading space
      {
        auto mm1 = std::make_unique< gum::ASTplus< double > >(
            std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB),
            std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC));
        auto mm2 = std::make_unique< gum::ASTdiv< double > >(
            std::make_unique< gum::ASTposteriorProba< double > >(bn, sAC, sB),
            std::make_unique< gum::ASTposteriorProba< double > >(bn, sA, sC));
        gum::ASTminus< double > minu(std::move(mm1), std::move(mm2));
        CHECK_EQ((minu.toLatex()),
                 ("P\\left(A,C\\mid B\\right)+P\\left(A\\mid C\\right)- \\frac "
                  "{P\\left(A,C\\mid B\\right)}{P\\left(A\\mid C\\right)}"));
      }

      // joint
      {
        gum::ASTjointProba< double > j1([] {
          gum::Set< std::string > s;
          s.insert("A");
          return s;
        }());
        CHECK_EQ(j1.toLatex(), "P\\left(A\\right)");
        gum::ASTjointProba< double > j2([] {
          gum::Set< std::string > s;
          s.insert("A");
          s.insert("C");
          return s;
        }());
        CHECK_EQ(j2.toLatex(), "P\\left(A,C\\right)");
      }
    }

    static void testLatex_ConditionalSimplification() {
      auto m = gum::BayesNet< double >::fastPrototype("z2->x->z1->y;z2->z1;z2->z3->y");
      gum::Set< std::string > y, knw;
      y.insert("y");
      knw.insert("x");
      knw.insert("z1");
      knw.insert("z2");
      knw.insert("z3");
      gum::ASTposteriorProba< double > nod(m, y, knw);
      CHECK_EQ(nod.toLatex(), "P\\left(y\\mid z1,z3\\right)");

      gum::ASTposteriorProba< double > nod2(m, y, knw);
      CHECK_EQ(nod2.toLatex(), "P\\left(y\\mid z1,z3\\right)");
    }
  };


  GUM_TEST_ACTIF(Str_like_pyagrum)
  GUM_TEST_ACTIF(ConditionalSimplification)
  GUM_TEST_ACTIF(Eval_Posterior_CPT_Shortcut)
  GUM_TEST_ACTIF(Eval_Posterior_NoConditioning)
  GUM_TEST_ACTIF(Eval_Posterior_WithConditioning)
  GUM_TEST_ACTIF(Eval_Joint)
  GUM_TEST_ACTIF(Eval_Sum)
  GUM_TEST_ACTIF(Eval_BinaryOps_SameScope)
  GUM_TEST_ACTIF(Eval_ProductOfTrees)
  GUM_TEST_ACTIF(Throws_On_Empty_VarSets)
  GUM_TEST_ACTIF(Copy_DeepCopy)
  GUM_TEST_ACTIF(ProductOfTrees_EdgeCases)
  GUM_TEST_ACTIF(Eval_Compose_DifferentScopes)
  GUM_TEST_ACTIF(Eval_Sum_Flattening_Equivalence)
  GUM_TEST_ACTIF(Deterministic_Ordering)
  GUM_TEST_ACTIF(Latex_like_pyagrum)
  GUM_TEST_ACTIF(Latex_ConditionalSimplification)


}   // namespace gum_tests
