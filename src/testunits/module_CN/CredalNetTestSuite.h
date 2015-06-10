/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN and Christophe GONZALES  *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <string>

#include <cstdlib> // rand

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/CN/credalNet.h>
#include <agrum/CN/LpInterface.h>

#include <agrum/core/OMPThreads.h>

/**
 * @file
 * @brief Mono-threaded version
 * @author Matthieu HOURBRACQ and Pierre-Henri WUILLEMIN
 *
 * TestSuite operations are NOT thread safe (yet ?)
 */

namespace gum_tests {

  class CredalNetTestSuite : public CxxTest::TestSuite {

    private:
    protected:
    public:
    gum::credal::CredalNet<double> *cn;

    void initCNet() {
#ifdef _OPENMP
      gum::setNumberOfThreads(1);
#endif

      cn = new gum::credal::CredalNet<double>();
    }

    void clearCNet() { delete cn; }

    void LP() {
      gum::credal::lp::LpInterface<double> lp;

      auto c1 = lp.addCol();
      auto c2 = lp.addCol();
      auto c3 = lp.addCol();
      auto c4 = lp.addCol();
      auto c5 = lp.addCol();
      auto c6 = lp.addCol();
      auto c7 = lp.addCol();
      auto c8 = lp.addCol();

      gum::Timer tim;
      gum::Size tmp = 0;

      while (tim.step() < 10) {
        gum::credal::lp::LpExpr expr(c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8);
        lp.addRow(1 <= expr <= 1);
        lp.clearRows();
        tmp++;
      }

      tim.reset();
      tmp = 0;
      tim.resume();

      while (tim.step() < 10) {
        gum::credal::lp::LpExpr expr(c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8);
        lp.addRow(1 <= std::move(expr) <= 1);
        lp.clearRows();
        tmp++;
      }

      tim.reset();
      tmp = 0;
      tim.resume();

      while (tim.step() < 10) {
        gum::credal::lp::LpExpr expr(c1 + c2 + c3 + c4 + c5 + c6 + c7 + c8);
        lp.addRow(1 <= expr);
        lp.addRow(expr <= 1);
        lp.clearRows();
        tmp++;
      }
    };

    /// network : A --> C <-- B built manually
    void testCredalNetByLP() {
      initCNet();

      std::vector<gum::NodeId> ids;
      ids.push_back(cn->addNode("A", 3)); // id 0
      ids.push_back(cn->addNode("B", 3)); // id 1
      ids.push_back(cn->addNode("C", 3)); // id 2

      TS_ASSERT_EQUALS(ids[0], 0U);
      TS_ASSERT_EQUALS(ids[1], 1U);
      TS_ASSERT_EQUALS(ids[2], 2U);

      cn->addArc(ids[0], ids[2]);
      cn->addArc(ids[1], ids[2]);

      /// hardcoded lps to check solutions
      std::vector<std::vector<gum::credal::lp::LpInterface<double>>> lps(3);
      std::vector<std::vector<std::vector<std::vector<double>>>> lps_sols(3);

      lps[0] = std::vector<gum::credal::lp::LpInterface<double>>(1); // A lp
      lps_sols[0] = std::vector<std::vector<std::vector<double>>>(1);

      lps[1] = std::vector<gum::credal::lp::LpInterface<double>>(1); // B lp
      lps_sols[1] = std::vector<std::vector<std::vector<double>>>(1);

      lps[2] = std::vector<gum::credal::lp::LpInterface<double>>(9); // C lps
      lps_sols[2] = std::vector<std::vector<std::vector<double>>>(9);

      /// A - intervals ( not a linear - vacuous mixture, i.e. exists i : max Pxi +
      /// sum_(j != i) min Pxj != 1 )
      std::vector<gum::credal::lp::LpCol> A = lps[0][0].addCols(3);

      /// gum::credal::LpExpr expr = 0.1 <= A[ 0 ] <= 0.7;
      lps[0][0].addRow(0.1 <= A[0] <= 0.7);
      lps[0][0].addRow(0.1 <= A[1] <= 0.5);
      lps[0][0].addRow(0.1 <= A[2] <= 0.6);
      lps[0][0].addSumIsOne(); // positivity constraints are obviously redundant

      lps_sols[0][0] = {{7. / 10, 1. / 5, 1. / 10},
                        {7. / 10, 1. / 10, 1. / 5},
                        {3. / 10, 1. / 10, 3. / 5},
                        {2. / 5, 1. / 2, 1. / 10},
                        {1. / 10, 1. / 2, 2. / 5},
                        {1. / 10, 3. / 10, 3. / 5}}; // A lp solution

      /// B : x1 = x2, x0 >= x1
      A = lps[1][0].addCols(3);
      lps[1][0].addRow(A[1] <= A[2]);
      lps[1][0].addRow(A[2] <= A[1]);
      lps[1][0].addRow(A[1] <= A[0]);
      lps[1][0].addProba();

      lps_sols[1][0] = {{1. / 3, 1. / 3, 1. / 3}, {1, 0, 0}}; // B lp solution

      /// C
      /// C : ins idx = 0, A:0 B:0
      /// 2 * x0 <= x1, x1 <= x0 + x2
      A = lps[2][0].addCols(3);
      lps[2][0].addRow(2 * A[0] <= A[1]);
      lps[2][0].addRow(A[1] <= A[0] + A[2]);
      lps[2][0].addProba();

      lps_sols[2][0] = {{1. / 4, 1. / 2, 1. / 4},
                        {0, 0, 1},
                        {0, 1. / 2, 1. / 2}}; // C lp solution A:0 B:0

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
                        {0, 0, 1}}; // C lp solution A:1 B:0

      /// C : ins idx = 2, A:2 B:0
      /// 0.5 * x0 + 0.5 * x1 = x2 ( i.e. x0 + x1 = 2 * x2 )
      A = lps[2][2].addCols(3);
      lps[2][2].addRow(0.5 * A[0] + 0.5 * A[1] <= A[2]);
      lps[2][2].addRow(A[2] <= 0.5 * A[0] + 0.5 * A[1]);
      lps[2][2].addProba();

      lps_sols[2][2] = {{2. / 3, 0, 1. / 3},
                        {0, 2. / 3, 1. / 3}}; // C lp solution A:2 B:0

      /// C : ins idx = 3, A:0 B:1
      /// vacuous
      A = lps[2][3].addCols(3);
      lps[2][3].addProba();

      lps_sols[2][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; // C lp solution A:0 B:1

      /// C : ins idx = 4, A:1 B:1
      /// x0 <= x1 <= x2
      /// x0 = 2 * x1 ( => x0 = x1 = 0 with above )
      A = lps[2][4].addCols(3);
      lps[2][4].addRow(A[0] <= A[1] <= A[2]);
      lps[2][4].addRow(A[0] <= 2 * A[1]);
      lps[2][4].addRow(2 * A[1] <= A[0]);
      lps[2][4].addProba();

      lps_sols[2][4] = {{0, 0, 1}}; // C lp solution A:1 B:1

      /// C : ins idx = 5, A:2 B:1
      /// at least 2 times x2 than x1 and 2 times x1 than x0
      /// 2 * x1 <= x2
      /// 2 * x0 <= x1
      A = lps[2][5].addCols(3);
      lps[2][5].addRow(2 * A[1] <= A[2]);
      lps[2][5].addRow(2 * A[0] <= A[1]);
      lps[2][5].addProba();

      lps_sols[2][5] = {{1. / 7, 2. / 7, 4. / 7},
                        {0, 1. / 3, 2. / 3},
                        {0, 0, 1}}; // C lp solution A:2 B:1

      /// C : ins idx = 6, A:0 B:2
      /// x2 - x1 <= x0
      A = lps[2][6].addCols(3);
      lps[2][6].addRow(A[2] - A[1] <= A[0]);
      lps[2][6].addProba();

      lps_sols[2][6] = {{1, 0, 0},
                        {1. / 2, 0, 1. / 2},
                        {0, 1. / 2, 1. / 2},
                        {0, 1, 0}}; // C lp solution A:0 B:2

      /// C : ins idx = 7, A:1 B:2
      /// vacuous
      A = lps[2][7].addCols(3);
      lps[2][7].addProba();

      lps_sols[2][7] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; // C lp solution A:1 B:2

      /// C : ins idx = 8, A:2 B:2
      /// x2 = 1 / 3 * x0 + 2 / 3 * x1
      A = lps[2][8].addCols(3);
      lps[2][8].addRow(A[2] <= 1. / 3 * A[0] + 2. / 3 * A[1]);
      lps[2][8].addRow(1. / 3 * A[0] + 2. / 3 * A[1] <= A[2]);
      lps[2][8].addProba();

      lps_sols[2][8] = {{3. / 4, 0, 1. / 4},
                        {0, 3. / 5, 2. / 5}}; // C lp solution A:2 B:2

      /// compute solutions, check bijection

      for (const auto &id : ids) {
        gum::Instantiation ins(cn->instantiation(id));
        ins.setFirst();

        unsigned long int entry(0);

        while (!ins.end()) {
          std::vector<std::vector<double>> vertices(
              lps[id][entry].solve()); // we solve the lp

          gum::Size sols_size(lps_sols[id][entry].size());
          TS_ASSERT_EQUALS(vertices.size(), sols_size);

          std::vector<bool> checked(sols_size, false);

          for (const auto &vertex : vertices) {
            for (gum::Size sol = 0; sol < sols_size; sol++) {
              bool eq = true;

              for (gum::Size i = 0, end = cn->domainSize(id); i < end; i++) {
                if (fabs(vertex[i] - lps_sols[id][entry][sol][i]) > 1e-6) {
                  eq = false;
                  break;
                }
              }

              if (eq) {
                TS_ASSERT(!checked[sol]);
                checked[sol] = true;
                break;
              }
            }
          }

          bool r = true;

          for (const auto b : checked) {
            r = r && b;
          }

          TS_ASSERT(r);

          cn->setCPT(id, ins, vertices);

          ins += cn->domainSize(id); // next instantiation without taking into
                                     // account the head variable; ins increased by
                                     // cardinality of head variable
          entry++;
        }
      }

      clearCNet();
    }

  }; // end of class CredalNetTestSuite

} // end of namespace gum_tests
