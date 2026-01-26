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


#include <iostream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/stattests/idCondSet.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  IdtCondSet
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct IdtCondSetTestSuite {
    public:
    static void test1() {
      gum::learning::IdCondSet set;
      GUM_CHECK_EQ(set.nbLHSIds(), std::size_t(0));

      gum::NodeId node0 = 0;
      gum::NodeId node1 = 1;
      gum::NodeId node2 = 2;
      gum::NodeId node3 = 3;
      gum::NodeId node4 = 4;
      gum::NodeId node5 = 5;
      gum::NodeId node6 = 6;

      std::vector< gum::NodeId > empty;

      gum::learning::IdCondSet set2(node0, empty);
      GUM_CHECK_EQ(set2.nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set2[0], node0);
      GUM_CHECK_NE(set2, set);

      gum::learning::IdCondSet cond_set2 = set2.conditionalIdCondSet();
      GUM_CHECK_EQ(cond_set2.nbLHSIds(), std::size_t(0));
      GUM_CHECK_EQ(cond_set2.size(), std::size_t(0));

      gum::learning::IdCondSet set3(set2);
      GUM_CHECK_EQ(set3, set2);
      GUM_CHECK_EQ(set3.nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set3[0], node0);

      gum::learning::IdCondSet set4(set3);
      GUM_CHECK_EQ(set4, set2);
      GUM_CHECK_EQ(set4.nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set4[0], node0);

      std::vector< gum::NodeId > seq1{node4, node3, node6, node5};
      gum::learning::IdCondSet   set1(seq1, true, false);
      GUM_CHECK_EQ(set1.nbLHSIds(), std::size_t(0));
      GUM_CHECK_EQ(set1[0], node3);
      GUM_CHECK_EQ(set1[1], node4);
      GUM_CHECK_EQ(set1[2], node5);
      GUM_CHECK_EQ(set1[3], node6);

      gum::learning::IdCondSet cond_set1 = set1.conditionalIdCondSet();
      GUM_CHECK_EQ(cond_set1.nbLHSIds(), std::size_t(0));
      GUM_CHECK_EQ(cond_set1[0], node3);
      GUM_CHECK_EQ(cond_set1[1], node4);
      GUM_CHECK_EQ(cond_set1[2], node5);
      GUM_CHECK_EQ(cond_set1[3], node6);
      std::size_t cs1 = 0;
      for (const auto node: cond_set1)
        cs1 += node;
      GUM_CHECK_EQ(cs1, std::size_t(18));

      gum::learning::IdCondSet set1b(empty, true, false);
      GUM_CHECK_EQ(set1b.nbLHSIds(), std::size_t(0));
      GUM_CHECK_EQ(set1b.size(), std::size_t(0));

      gum::learning::IdCondSet cond_set1b = set1b.conditionalIdCondSet();
      GUM_CHECK_EQ(cond_set1b.nbLHSIds(), std::size_t(0));
      GUM_CHECK_EQ(cond_set1b.size(), std::size_t(0));

      gum::learning::IdCondSet set5(node0, seq1, false);
      GUM_CHECK_EQ(set5.nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set5[0], node0);
      GUM_CHECK_EQ(set5[1], node3);
      GUM_CHECK_EQ(set5[2], node4);
      GUM_CHECK_EQ(set5[3], node5);
      GUM_CHECK_EQ(set5[4], node6);
      GUM_CHECK_NE(set5, set2);

      gum::learning::IdCondSet cond_set5 = set5.conditionalIdCondSet();
      GUM_CHECK_EQ(cond_set5.nbLHSIds(), std::size_t(0));
      GUM_CHECK_EQ(cond_set5[0], node3);
      GUM_CHECK_EQ(cond_set5[1], node4);
      GUM_CHECK_EQ(cond_set5[2], node5);
      GUM_CHECK_EQ(cond_set5[3], node6);


      gum::learning::IdCondSet set6(set5);
      GUM_CHECK_EQ(set6, set5);
      GUM_CHECK_EQ(set6.nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set6[0], node0);
      GUM_CHECK_EQ(set6[1], node3);
      GUM_CHECK_EQ(set6[2], node4);
      GUM_CHECK_EQ(set6[3], node5);
      GUM_CHECK_EQ(set6[4], node6);
      GUM_CHECK_NE(set6, set2);

      gum::learning::IdCondSet set7(set5);
      GUM_CHECK_EQ(set7, set6);

      gum::learning::IdCondSet set8(std::move(set2));
      GUM_CHECK_EQ(set8, set3);
      GUM_CHECK_EQ(set8.nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set8[0], node0);

      gum::learning::IdCondSet set9(std::move(set3));
      GUM_CHECK_EQ(set9, set8);
      GUM_CHECK_EQ(set9.nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set9[0], node0);

      gum::learning::IdCondSet set10(std::move(set5));
      GUM_CHECK_EQ(set10.nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set10[0], node0);
      GUM_CHECK_EQ(set10[1], node3);
      GUM_CHECK_EQ(set10[2], node4);
      GUM_CHECK_EQ(set10[3], node5);
      GUM_CHECK_EQ(set10[4], node6);
      GUM_CHECK_NE(set10, set8);

      gum::learning::IdCondSet set11(std::move(set6));
      GUM_CHECK_EQ(set11, set10);
      GUM_CHECK_EQ(set11.nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set11[0], node0);
      GUM_CHECK_EQ(set11[1], node3);
      GUM_CHECK_EQ(set11[2], node4);
      GUM_CHECK_EQ(set11[3], node5);
      GUM_CHECK_EQ(set11[4], node6);


      gum::learning::IdCondSet* set12 = set10.clone();
      GUM_CHECK_EQ(set12->nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set12->operator[](0), node0);
      GUM_CHECK_EQ(set12->operator[](1), node3);
      GUM_CHECK_EQ(set12->operator[](2), node4);
      GUM_CHECK_EQ(set12->operator[](3), node5);
      GUM_CHECK_EQ(set12->operator[](4), node6);
      GUM_CHECK_NE(*set12, set8);

      gum::learning::IdCondSet* set13 = set10.clone();
      GUM_CHECK_EQ(set13->nbLHSIds(), std::size_t(1));
      GUM_CHECK_EQ(set13->operator[](0), node0);
      GUM_CHECK_EQ(set13->operator[](1), node3);
      GUM_CHECK_EQ(set13->operator[](2), node4);
      GUM_CHECK_EQ(set13->operator[](3), node5);
      GUM_CHECK_EQ(set13->operator[](4), node6);
      GUM_CHECK_NE(*set13, set8);

      delete set12;
      delete set13;

      gum::learning::IdCondSet set14(node1, node0, empty, true, false);
      GUM_CHECK_EQ(set14.nbLHSIds(), std::size_t(2));
      GUM_CHECK_EQ(set14[0], node1);
      GUM_CHECK_EQ(set14[1], node0);
      GUM_CHECK_NE(set14, set2);

      gum::learning::IdCondSet set15(node1, node0, empty, true);
      GUM_CHECK_EQ(set15.nbLHSIds(), std::size_t(2));
      GUM_CHECK_EQ(set15[0], node1);
      GUM_CHECK_EQ(set15[1], node0);
      GUM_CHECK_NE(set15, set2);

      gum::learning::IdCondSet set16(node1, node0, empty, false, false);
      GUM_CHECK_EQ(set16.nbLHSIds(), std::size_t(2));
      GUM_CHECK_EQ(set16[0], node0);
      GUM_CHECK_EQ(set16[1], node1);
      GUM_CHECK_NE(set16, set2);

      gum::learning::IdCondSet set17(node1, node0, empty, false);
      GUM_CHECK_EQ(set17.nbLHSIds(), std::size_t(2));
      GUM_CHECK_EQ(set17[0], node0);
      GUM_CHECK_EQ(set17[1], node1);
      GUM_CHECK_NE(set17, set2);

      gum::learning::IdCondSet set18(node1, node0, seq1, true, false);
      GUM_CHECK_EQ(set18.nbLHSIds(), std::size_t(2));
      GUM_CHECK_EQ(set18[0], node1);
      GUM_CHECK_EQ(set18[1], node0);
      GUM_CHECK_EQ(set18[2], node3);
      GUM_CHECK_EQ(set18[3], node4);
      GUM_CHECK_EQ(set18[4], node5);
      GUM_CHECK_EQ(set18[5], node6);
      GUM_CHECK_NE(set18, set2);

      gum::learning::IdCondSet set19(node1, node0, seq1, false, false);
      GUM_CHECK_EQ(set19.nbLHSIds(), std::size_t(2));
      GUM_CHECK_EQ(set19[0], node0);
      GUM_CHECK_EQ(set19[1], node1);
      GUM_CHECK_EQ(set19[2], node3);
      GUM_CHECK_EQ(set19[3], node4);
      GUM_CHECK_EQ(set19[4], node5);
      GUM_CHECK_EQ(set19[5], node6);
      GUM_CHECK_NE(set19, set2);

      gum::learning::IdCondSet set20(node1, node0, seq1, true);
      GUM_CHECK_EQ(set20.nbLHSIds(), std::size_t(2));
      GUM_CHECK_EQ(set20[0], node1);
      GUM_CHECK_EQ(set20[1], node0);
      GUM_CHECK_EQ(set20[2], node3);
      GUM_CHECK_EQ(set20[3], node4);
      GUM_CHECK_EQ(set20[4], node5);
      GUM_CHECK_EQ(set20[5], node6);
      GUM_CHECK_NE(set20, set2);

      gum::learning::IdCondSet set21(node1, node0, seq1, false);
      GUM_CHECK_EQ(set21.nbLHSIds(), std::size_t(2));
      GUM_CHECK_EQ(set21[0], node0);
      GUM_CHECK_EQ(set21[1], node1);
      GUM_CHECK_EQ(set21[2], node3);
      GUM_CHECK_EQ(set21[3], node4);
      GUM_CHECK_EQ(set21[4], node5);
      GUM_CHECK_EQ(set21[5], node6);
      GUM_CHECK_NE(set21, set2);

      gum::learning::IdCondSet set22(node1, node0, node2, seq1, true);
      GUM_CHECK_EQ(set22.nbLHSIds(), std::size_t(3));
      GUM_CHECK_EQ(set22[0], node1);
      GUM_CHECK_EQ(set22[1], node0);
      GUM_CHECK_EQ(set22[2], node2);
      GUM_CHECK_EQ(set22[3], node3);
      GUM_CHECK_EQ(set22[4], node4);
      GUM_CHECK_EQ(set22[5], node5);
      GUM_CHECK_EQ(set22[6], node6);
      GUM_CHECK_NE(set22, set2);

      gum::learning::IdCondSet set23(node1, node0, node2, seq1, false);
      GUM_CHECK_EQ(set23.nbLHSIds(), std::size_t(3));
      GUM_CHECK_EQ(set23[0], node0);
      GUM_CHECK_EQ(set23[1], node1);
      GUM_CHECK_EQ(set23[2], node2);
      GUM_CHECK_EQ(set23[3], node3);
      GUM_CHECK_EQ(set23[4], node4);
      GUM_CHECK_EQ(set23[5], node5);
      GUM_CHECK_EQ(set23[6], node6);
      GUM_CHECK_NE(set23, set2);

      set22 = set23;
      GUM_CHECK_EQ(set23, set22);
      GUM_CHECK_EQ(set22.nbLHSIds(), std::size_t(3));
      GUM_CHECK_EQ(set22[0], node0);
      GUM_CHECK_EQ(set22[1], node1);
      GUM_CHECK_EQ(set22[2], node2);
      GUM_CHECK_EQ(set22[3], node3);
      GUM_CHECK_EQ(set22[4], node4);
      GUM_CHECK_EQ(set22[5], node5);
      GUM_CHECK_EQ(set22[6], node6);

      set4 = std::move(set23);
      GUM_CHECK_EQ(set22, set4);
      GUM_CHECK_EQ(set4.nbLHSIds(), std::size_t(3));
      GUM_CHECK_EQ(set4[0], node0);
      GUM_CHECK_EQ(set4[1], node1);
      GUM_CHECK_EQ(set4[2], node2);
      GUM_CHECK_EQ(set4[3], node3);
      GUM_CHECK_EQ(set4[4], node4);
      GUM_CHECK_EQ(set4[5], node5);
      GUM_CHECK_EQ(set4[6], node6);

      const auto& nodes = set4.ids();
      GUM_CHECK_EQ(nodes[0], node0);
      GUM_CHECK_EQ(nodes[1], node1);
      GUM_CHECK_EQ(nodes[2], node2);
      GUM_CHECK_EQ(nodes[3], node3);
      GUM_CHECK_EQ(nodes[4], node4);
      GUM_CHECK_EQ(nodes[5], node5);
      GUM_CHECK_EQ(nodes[6], node6);

      std::string str = "{0 , 1 , 2 | 3 , 4 , 5 , 6}";
      GUM_CHECK_EQ(set4.toString(), str);

      auto [l, r]    = set4.toNodeSets();
      gum::NodeSet s = gum::NodeSet{0, 1, 2};
      GUM_CHECK_EQ(l, s);
      s = gum::NodeSet{3, 4, 5, 6};
      GUM_CHECK_EQ(r, s);

      gum::NodeId xnb = 0;
      for (auto iter = set4.begin(); iter != set4.end(); ++iter, ++xnb) {
        GUM_CHECK_EQ(*iter, xnb);
      }
      xnb = 0;
      for (auto iter = set4.beginSafe(); iter != set4.endSafe(); ++iter, ++xnb) {
        GUM_CHECK_EQ(*iter, xnb);
      }
      xnb = 0;
      for (auto nb: set4) {
        GUM_CHECK_EQ(nb, xnb);
        ++xnb;
      }

      auto                                        iter1 = set4.begin() + 2;
      auto                                        iter2 = set4.begin();
      typename gum::learning::IdCondSet::iterator iter3;
      GUM_CHECK_NE(iter1, iter2);
      GUM_CHECK_NE(iter1, iter3);
      iter2 += 2;
      GUM_CHECK_EQ(iter1, iter2);
      iter3 = iter1;
      GUM_CHECK_EQ(iter3, iter2);
      GUM_CHECK_EQ(iter3.pos(), (std::size_t)2);

      gum::learning::IdCondSet set25(node0, empty);
      CHECK(set4.contains(set25));
      CHECK(!set25.contains(set4));

      gum::NodeId              node7 = 7;
      gum::learning::IdCondSet set26(node7, seq1, false);
      CHECK(!set4.contains(set26));
      CHECK(!set26.contains(set4));

      gum::learning::IdCondSet set27;
      CHECK(!set26.empty());
      CHECK(set27.empty());
      GUM_CHECK_EQ(set4.size(), std::size_t(7));
      for (std::size_t i = std::size_t(0); i < std::size_t(7); ++i) {
        GUM_CHECK_EQ(set4.pos(i), (std::size_t)i);
        CHECK(set4.exists(i));
      }
      CHECK_THROWS_AS(set4.pos(7), const gum::NotFound&);
      CHECK(!set4.exists(7));

      set4.clear();
      CHECK(set4.empty());
      GUM_CHECK_EQ(set4.size(), std::size_t(0));

      gum::learning::IdCondSet set28(0, std::vector< gum::NodeId >{2, 1}, true);
      xnb = 0;
      GUM_CHECK_EQ(set28.nbLHSIds(), (std::size_t)1);
      for ([[maybe_unused]] auto id: set28) {
        id++;
        xnb++;
      }
      GUM_CHECK_EQ(xnb, gum::NodeId(3));
    }   // namespace gum_tests
  };

  GUM_TEST_ACTIF(1)


} /* namespace gum_tests */
