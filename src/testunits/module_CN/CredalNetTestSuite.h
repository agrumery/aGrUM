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
#pragma once


#include <cstdlib>   // rand
#include <iostream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/threads/threadExecutor.h>
#include <agrum/base/core/threads/threads.h>
#include <agrum/CN/credalNet.h>
#include <agrum/CN/polytope/LpInterface.h>

/**
 * @file
 * @brief Mono-threaded version
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN(_at_LIP6)
 *
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {

  class GUM_TEST_SUITE(CredalNet) {
    private:

    protected:

    public:
    gum::credal::CredalNet< double >* cn;

    void initCNet() {
#ifdef _OPENMP
      gum::threadsOMP::setNumberOfThreads(1);
#endif

      cn = new gum::credal::CredalNet< double >();
    }

    void clearCNet() { delete cn; }

    void LP() {
      gum::credal::lp::LpInterface< double > lp;

      auto c1 = lp.addCol();
      auto c2 = lp.addCol();
      auto c3 = lp.addCol();
      auto c4 = lp.addCol();
      auto c5 = lp.addCol();
      auto c6 = lp.addCol();
      auto c7 = lp.addCol();
      auto c8 = lp.addCol();

      gum::Timer tim;

      while (tim.step() < 10) {
        gum::credal::lp::LpExpr expr(c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8);
        lp.addRow(1 <= expr <= 1);
        lp.clearRows();
      }

      tim.reset();
      tim.resume();

      while (tim.step() < 10) {
        gum::credal::lp::LpExpr expr(c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8);
        lp.addRow(1 <= std::move(expr) <= 1);
        lp.clearRows();
      }

      tim.reset();
      tim.resume();

      while (tim.step() < 10) {
        gum::credal::lp::LpExpr expr(c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8);
        lp.addRow(1 <= expr);
        lp.addRow(expr <= 1);
        lp.clearRows();
      }
    };

    /// network : A --> C <-- B built manually
    GUM_ACTIVE_TEST(CredalNetByLP) {
      initCNet();

      std::vector< gum::NodeId > ids;
      ids.push_back(cn->addVariable("A", 3));   // id 0
      ids.push_back(cn->addVariable("B", 3));   // id 1
      ids.push_back(cn->addVariable("C", 3));   // id 2

      TS_ASSERT_EQUALS(ids[0], 0U)
      TS_ASSERT_EQUALS(ids[1], 1U)
      TS_ASSERT_EQUALS(ids[2], 2U)

      cn->addArc(ids[0], ids[2]);
      cn->addArc(ids[1], ids[2]);

      /// hardcoded lps to check solutions
      std::vector< std::vector< gum::credal::lp::LpInterface< double > > > lps(3);
      std::vector< std::vector< std::vector< std::vector< double > > > >   lps_sols(3);

      lps[0]      = std::vector< gum::credal::lp::LpInterface< double > >(1);   // A lp
      lps_sols[0] = std::vector< std::vector< std::vector< double > > >(1);

      lps[1]      = std::vector< gum::credal::lp::LpInterface< double > >(1);   // B lp
      lps_sols[1] = std::vector< std::vector< std::vector< double > > >(1);

      lps[2]      = std::vector< gum::credal::lp::LpInterface< double > >(9);   // C lps
      lps_sols[2] = std::vector< std::vector< std::vector< double > > >(9);

      /// A - intervals ( not a linear - vacuous mixture, i.e. exists i : max
      /// Pxi +
      /// sum_(j != i) min Pxj != 1 )
      std::vector< gum::credal::lp::LpCol > A = lps[0][0].addCols(3);

      /// gum::credal::LpExpr expr = 0.1 <= A[ 0 ] <= 0.7;
      lps[0][0].addRow(0.1 <= A[0] <= 0.7);
      lps[0][0].addRow(0.1 <= A[1] <= 0.5);
      lps[0][0].addRow(0.1 <= A[2] <= 0.6);
      lps[0][0].addSumIsOne();   // positivity constraints are obviously redundant

      lps_sols[0][0] = {{7. / 10, 1. / 5, 1. / 10},
                        {7. / 10, 1. / 10, 1. / 5},
                        {3. / 10, 1. / 10, 3. / 5},
                        {2. / 5, 1. / 2, 1. / 10},
                        {1. / 10, 1. / 2, 2. / 5},
                        {1. / 10, 3. / 10, 3. / 5}};   // A lp solution

      /// B : x1 = x2, x0 >= x1
      A = lps[1][0].addCols(3);
      lps[1][0].addRow(A[1] <= A[2]);
      lps[1][0].addRow(A[2] <= A[1]);
      lps[1][0].addRow(A[1] <= A[0]);
      lps[1][0].addProba();

      lps_sols[1][0] = {{1. / 3, 1. / 3, 1. / 3}, {1, 0, 0}};   // B lp solution

      /// C
      /// C : ins idx = 0, A:0 B:0
      /// 2 * x0 <= x1, x1 <= x0 + x2
      A = lps[2][0].addCols(3);
      lps[2][0].addRow(2 * A[0] <= A[1]);
      lps[2][0].addRow(A[1] <= A[0] + A[2]);
      lps[2][0].addProba();

      lps_sols[2][0]
          = {{1. / 4, 1. / 2, 1. / 4}, {0, 0, 1}, {0, 1. / 2, 1. / 2}};   // C lp solution A:0 B:0

      /// C : ins idx = 1, A:1 B:0
      /// 2 * x0 + x1 <= x2 written as x0 + x1 <= x2 - x0
      /// x0 + 0.4 <= x2
      A = lps[2][1].addCols(3);
      lps[2][1].addRow(A[0] + A[1] <= A[2] - A[0]);
      lps[2][1].addRow(A[0] + 0.4 <= A[2]);
      lps[2][1].addProba();

      lps_sols[2][1] = {{0, 1. / 2, 1. / 2},
                        {1. / 5, 1. / 5, 3. / 5},
                        {3. / 10, 0, 7. / 10},
                        {0, 0, 1}};   // C lp solution A:1 B:0

      /// C : ins idx = 2, A:2 B:0
      /// 0.5 * x0 + 0.5 * x1 = x2 ( i.e. x0 + x1 = 2 * x2 )
      A = lps[2][2].addCols(3);
      lps[2][2].addRow(0.5 * A[0] + 0.5 * A[1] <= A[2]);
      lps[2][2].addRow(A[2] <= 0.5 * A[0] + 0.5 * A[1]);
      lps[2][2].addProba();

      lps_sols[2][2] = {{2. / 3, 0, 1. / 3}, {0, 2. / 3, 1. / 3}};   // C lp solution A:2 B:0

      /// C : ins idx = 3, A:0 B:1
      /// vacuous
      A = lps[2][3].addCols(3);
      lps[2][3].addProba();

      lps_sols[2][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};   // C lp solution A:0 B:1

      /// C : ins idx = 4, A:1 B:1
      /// x0 <= x1 <= x2
      /// x0 = 2 * x1 ( => x0 = x1 = 0 with above )
      A = lps[2][4].addCols(3);
      lps[2][4].addRow(A[0] <= A[1] <= A[2]);
      lps[2][4].addRow(A[0] <= 2 * A[1]);
      lps[2][4].addRow(2 * A[1] <= A[0]);
      lps[2][4].addProba();

      lps_sols[2][4] = {{0, 0, 1}};   // C lp solution A:1 B:1

      /// C : ins idx = 5, A:2 B:1
      /// at least 2 times x2 than x1 and 2 times x1 than x0
      /// 2 * x1 <= x2
      /// 2 * x0 <= x1
      A = lps[2][5].addCols(3);
      lps[2][5].addRow(2 * A[1] <= A[2]);
      lps[2][5].addRow(2 * A[0] <= A[1]);
      lps[2][5].addProba();

      lps_sols[2][5]
          = {{1. / 7, 2. / 7, 4. / 7}, {0, 1. / 3, 2. / 3}, {0, 0, 1}};   // C lp solution A:2 B:1

      /// C : ins idx = 6, A:0 B:2
      /// x2 - x1 <= x0
      A = lps[2][6].addCols(3);
      lps[2][6].addRow(A[2] - A[1] <= A[0]);
      lps[2][6].addProba();

      lps_sols[2][6] = {{1, 0, 0},
                        {1. / 2, 0, 1. / 2},
                        {0, 1. / 2, 1. / 2},
                        {0, 1, 0}};   // C lp solution A:0 B:2

      /// C : ins idx = 7, A:1 B:2
      /// vacuous
      A = lps[2][7].addCols(3);
      lps[2][7].addProba();

      lps_sols[2][7] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};   // C lp solution A:1 B:2

      /// C : ins idx = 8, A:2 B:2
      /// x2 = 1 / 3 * x0 + 2 / 3 * x1
      A = lps[2][8].addCols(3);
      lps[2][8].addRow(A[2] <= 1. / 3 * A[0] + 2. / 3 * A[1]);
      lps[2][8].addRow(1. / 3 * A[0] + 2. / 3 * A[1] <= A[2]);
      lps[2][8].addProba();

      lps_sols[2][8] = {{3. / 4, 0, 1. / 4}, {0, 3. / 5, 2. / 5}};   // C lp solution A:2 B:2

      /// compute solutions, check bijection
      for (const auto& id: ids) {
        gum::Instantiation ins(cn->instantiation(id));
        ins.setFirst();

        gum::Size entry(0);

        while (!ins.end()) {
          std::vector< std::vector< double > > vertices(
              lps[id][entry].solve());   // we solve the lp

          gum::Size sols_size = gum::Size(lps_sols[id][entry].size());
          TS_ASSERT_EQUALS(vertices.size(), sols_size)

          std::vector< bool > checked(sols_size, false);

          for (const auto& vertex: vertices) {
            for (gum::Size sol = 0; sol < sols_size; sol++) {
              bool eq = true;

              for (gum::Size i = 0, end = cn->domainSize(id); i < end; i++) {
                if (fabs(vertex[i] - lps_sols[id][entry][sol][i]) > 1e-6) {
                  eq = false;
                  break;
                }
              }

              if (eq) {
                TS_ASSERT(!checked[sol])
                checked[sol] = true;
                break;
              }
            }
          }

          bool r = true;

          for (const auto b: checked) {
            r = r && b;
          }

          TS_ASSERT(r)

          cn->setCPT(id, ins, vertices);

          ins += cn->domainSize(id);   // next instantiation without taking into
          // account the head variable; ins increased by
          // cardinality of head variable
          entry++;
        }
      }

      clearCNet();
    }

    GUM_ACTIVE_TEST(Binarization) {
      auto bn = gum::BayesNet< double >::fastPrototype("A[2]->B[3]");
      for (const auto nod: bn.nodes())
        bn.cpt(nod).translate(1).scale(0.25).normalizeAsCPT();

      auto bnmin = gum::BayesNet< double >(bn);
      auto bnmax = gum::BayesNet< double >(bn);

      for (const auto nod: bn.nodes()) {
        bnmin.cpt(nod).scale(0.9);
        bnmax.cpt(nod).scale(1.1);
      }

      gum::credal::CredalNet< double > cnet(bnmin, bnmax);
      cnet.intervalToCredal();
      cnet.approximatedBinarization();
      cnet.computeBinaryCPTMinMax();

      const auto& current = cnet.current_bn();
      const auto& valsmin = cnet.get_binaryCPT_min();
      const auto& valsmax = cnet.get_binaryCPT_max();

      for (gum::Idx i = 0; i < 4; i++) {
        TS_ASSERT_EQUALS(valsmin[current.idFromName("B-v0")][i], (i == 0 ? 1.0 : 0.0))
        TS_ASSERT_EQUALS(valsmax[current.idFromName("B-v0")][i], (i == 0 ? 1.0 : 0.0))
        TS_ASSERT_EQUALS(valsmin[current.idFromName("B-v1")][i], (i == 1 ? 1.0 : 0.0))
        TS_ASSERT_EQUALS(valsmax[current.idFromName("B-v1")][i], (i == 1 ? 1.0 : 0.0))
        TS_ASSERT_EQUALS(valsmin[current.idFromName("B-v2")][i], (i == 2 ? 1.0 : 0.0))
        TS_ASSERT_EQUALS(valsmax[current.idFromName("B-v2")][i], (i == 2 ? 1.0 : 0.0))
      }
    }

    GUM_ACTIVE_TEST(BadMinMaxFile) {
      gum::BayesNet< double >  monBNa;
      gum::BIFReader< double > readera(
          &monBNa,
          GET_RESSOURCES_PATH("cn/errorBN_min.bif"));   // not well defined
      readera.proceed();

      gum::BayesNet< double >  monBNb;
      gum::BIFReader< double > readerb(
          &monBNb,
          GET_RESSOURCES_PATH("cn/errorBN_max.bif"));   // not well defined
      readerb.proceed();

      gum::credal::CredalNet< double > cn(monBNa, monBNb);
      TS_ASSERT_THROWS(cn.intervalToCredal(), const gum::CPTError&)
    }
  };   // end of class CredalNetTestSuite

}   // end of namespace gum_tests
