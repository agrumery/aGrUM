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
      CHECK((set.nbLHSIds()) == (std::size_t(0)));

      gum::NodeId node0 = 0;
      gum::NodeId node1 = 1;
      gum::NodeId node2 = 2;
      gum::NodeId node3 = 3;
      gum::NodeId node4 = 4;
      gum::NodeId node5 = 5;
      gum::NodeId node6 = 6;

      std::vector< gum::NodeId > empty;

      gum::learning::IdCondSet set2(node0, empty);
      CHECK((set2.nbLHSIds()) == (std::size_t(1)));
      CHECK((set2[0]) == (node0));
      CHECK((set2) != (set));

      gum::learning::IdCondSet cond_set2 = set2.conditionalIdCondSet();
      CHECK((cond_set2.nbLHSIds()) == (std::size_t(0)));
      CHECK((cond_set2.size()) == (std::size_t(0)));

      gum::learning::IdCondSet set3(set2);
      CHECK((set3) == (set2));
      CHECK((set3.nbLHSIds()) == (std::size_t(1)));
      CHECK((set3[0]) == (node0));

      gum::learning::IdCondSet set4(set3);
      CHECK((set4) == (set2));
      CHECK((set4.nbLHSIds()) == (std::size_t(1)));
      CHECK((set4[0]) == (node0));

      std::vector< gum::NodeId > seq1{node4, node3, node6, node5};
      gum::learning::IdCondSet   set1(seq1, true, false);
      CHECK((set1.nbLHSIds()) == (std::size_t(0)));
      CHECK((set1[0]) == (node3));
      CHECK((set1[1]) == (node4));
      CHECK((set1[2]) == (node5));
      CHECK((set1[3]) == (node6));

      gum::learning::IdCondSet cond_set1 = set1.conditionalIdCondSet();
      CHECK((cond_set1.nbLHSIds()) == (std::size_t(0)));
      CHECK((cond_set1[0]) == (node3));
      CHECK((cond_set1[1]) == (node4));
      CHECK((cond_set1[2]) == (node5));
      CHECK((cond_set1[3]) == (node6));
      std::size_t cs1 = 0;
      for (const auto node: cond_set1)
        cs1 += node;
      CHECK((cs1) == (std::size_t(18)));

      gum::learning::IdCondSet set1b(empty, true, false);
      CHECK((set1b.nbLHSIds()) == (std::size_t(0)));
      CHECK((set1b.size()) == (std::size_t(0)));

      gum::learning::IdCondSet cond_set1b = set1b.conditionalIdCondSet();
      CHECK((cond_set1b.nbLHSIds()) == (std::size_t(0)));
      CHECK((cond_set1b.size()) == (std::size_t(0)));

      gum::learning::IdCondSet set5(node0, seq1, false);
      CHECK((set5.nbLHSIds()) == (std::size_t(1)));
      CHECK((set5[0]) == (node0));
      CHECK((set5[1]) == (node3));
      CHECK((set5[2]) == (node4));
      CHECK((set5[3]) == (node5));
      CHECK((set5[4]) == (node6));
      CHECK((set5) != (set2));

      gum::learning::IdCondSet cond_set5 = set5.conditionalIdCondSet();
      CHECK((cond_set5.nbLHSIds()) == (std::size_t(0)));
      CHECK((cond_set5[0]) == (node3));
      CHECK((cond_set5[1]) == (node4));
      CHECK((cond_set5[2]) == (node5));
      CHECK((cond_set5[3]) == (node6));


      gum::learning::IdCondSet set6(set5);
      CHECK((set6) == (set5));
      CHECK((set6.nbLHSIds()) == (std::size_t(1)));
      CHECK((set6[0]) == (node0));
      CHECK((set6[1]) == (node3));
      CHECK((set6[2]) == (node4));
      CHECK((set6[3]) == (node5));
      CHECK((set6[4]) == (node6));
      CHECK((set6) != (set2));

      gum::learning::IdCondSet set7(set5);
      CHECK((set7) == (set6));

      gum::learning::IdCondSet set8(std::move(set2));
      CHECK((set8) == (set3));
      CHECK((set8.nbLHSIds()) == (std::size_t(1)));
      CHECK((set8[0]) == (node0));

      gum::learning::IdCondSet set9(std::move(set3));
      CHECK((set9) == (set8));
      CHECK((set9.nbLHSIds()) == (std::size_t(1)));
      CHECK((set9[0]) == (node0));

      gum::learning::IdCondSet set10(std::move(set5));
      CHECK((set10.nbLHSIds()) == (std::size_t(1)));
      CHECK((set10[0]) == (node0));
      CHECK((set10[1]) == (node3));
      CHECK((set10[2]) == (node4));
      CHECK((set10[3]) == (node5));
      CHECK((set10[4]) == (node6));
      CHECK((set10) != (set8));

      gum::learning::IdCondSet set11(std::move(set6));
      CHECK((set11) == (set10));
      CHECK((set11.nbLHSIds()) == (std::size_t(1)));
      CHECK((set11[0]) == (node0));
      CHECK((set11[1]) == (node3));
      CHECK((set11[2]) == (node4));
      CHECK((set11[3]) == (node5));
      CHECK((set11[4]) == (node6));


      gum::learning::IdCondSet* set12 = set10.clone();
      CHECK((set12->nbLHSIds()) == (std::size_t(1)));
      CHECK((set12->operator[](0)) == (node0));
      CHECK((set12->operator[](1)) == (node3));
      CHECK((set12->operator[](2)) == (node4));
      CHECK((set12->operator[](3)) == (node5));
      CHECK((set12->operator[](4)) == (node6));
      CHECK((*set12) != (set8));

      gum::learning::IdCondSet* set13 = set10.clone();
      CHECK((set13->nbLHSIds()) == (std::size_t(1)));
      CHECK((set13->operator[](0)) == (node0));
      CHECK((set13->operator[](1)) == (node3));
      CHECK((set13->operator[](2)) == (node4));
      CHECK((set13->operator[](3)) == (node5));
      CHECK((set13->operator[](4)) == (node6));
      CHECK((*set13) != (set8));

      delete set12;
      delete set13;

      gum::learning::IdCondSet set14(node1, node0, empty, true, false);
      CHECK((set14.nbLHSIds()) == (std::size_t(2)));
      CHECK((set14[0]) == (node1));
      CHECK((set14[1]) == (node0));
      CHECK((set14) != (set2));

      gum::learning::IdCondSet set15(node1, node0, empty, true);
      CHECK((set15.nbLHSIds()) == (std::size_t(2)));
      CHECK((set15[0]) == (node1));
      CHECK((set15[1]) == (node0));
      CHECK((set15) != (set2));

      gum::learning::IdCondSet set16(node1, node0, empty, false, false);
      CHECK((set16.nbLHSIds()) == (std::size_t(2)));
      CHECK((set16[0]) == (node0));
      CHECK((set16[1]) == (node1));
      CHECK((set16) != (set2));

      gum::learning::IdCondSet set17(node1, node0, empty, false);
      CHECK((set17.nbLHSIds()) == (std::size_t(2)));
      CHECK((set17[0]) == (node0));
      CHECK((set17[1]) == (node1));
      CHECK((set17) != (set2));

      gum::learning::IdCondSet set18(node1, node0, seq1, true, false);
      CHECK((set18.nbLHSIds()) == (std::size_t(2)));
      CHECK((set18[0]) == (node1));
      CHECK((set18[1]) == (node0));
      CHECK((set18[2]) == (node3));
      CHECK((set18[3]) == (node4));
      CHECK((set18[4]) == (node5));
      CHECK((set18[5]) == (node6));
      CHECK((set18) != (set2));

      gum::learning::IdCondSet set19(node1, node0, seq1, false, false);
      CHECK((set19.nbLHSIds()) == (std::size_t(2)));
      CHECK((set19[0]) == (node0));
      CHECK((set19[1]) == (node1));
      CHECK((set19[2]) == (node3));
      CHECK((set19[3]) == (node4));
      CHECK((set19[4]) == (node5));
      CHECK((set19[5]) == (node6));
      CHECK((set19) != (set2));

      gum::learning::IdCondSet set20(node1, node0, seq1, true);
      CHECK((set20.nbLHSIds()) == (std::size_t(2)));
      CHECK((set20[0]) == (node1));
      CHECK((set20[1]) == (node0));
      CHECK((set20[2]) == (node3));
      CHECK((set20[3]) == (node4));
      CHECK((set20[4]) == (node5));
      CHECK((set20[5]) == (node6));
      CHECK((set20) != (set2));

      gum::learning::IdCondSet set21(node1, node0, seq1, false);
      CHECK((set21.nbLHSIds()) == (std::size_t(2)));
      CHECK((set21[0]) == (node0));
      CHECK((set21[1]) == (node1));
      CHECK((set21[2]) == (node3));
      CHECK((set21[3]) == (node4));
      CHECK((set21[4]) == (node5));
      CHECK((set21[5]) == (node6));
      CHECK((set21) != (set2));

      gum::learning::IdCondSet set22(node1, node0, node2, seq1, true);
      CHECK((set22.nbLHSIds()) == (std::size_t(3)));
      CHECK((set22[0]) == (node1));
      CHECK((set22[1]) == (node0));
      CHECK((set22[2]) == (node2));
      CHECK((set22[3]) == (node3));
      CHECK((set22[4]) == (node4));
      CHECK((set22[5]) == (node5));
      CHECK((set22[6]) == (node6));
      CHECK((set22) != (set2));

      gum::learning::IdCondSet set23(node1, node0, node2, seq1, false);
      CHECK((set23.nbLHSIds()) == (std::size_t(3)));
      CHECK((set23[0]) == (node0));
      CHECK((set23[1]) == (node1));
      CHECK((set23[2]) == (node2));
      CHECK((set23[3]) == (node3));
      CHECK((set23[4]) == (node4));
      CHECK((set23[5]) == (node5));
      CHECK((set23[6]) == (node6));
      CHECK((set23) != (set2));

      set22 = set23;
      CHECK((set23) == (set22));
      CHECK((set22.nbLHSIds()) == (std::size_t(3)));
      CHECK((set22[0]) == (node0));
      CHECK((set22[1]) == (node1));
      CHECK((set22[2]) == (node2));
      CHECK((set22[3]) == (node3));
      CHECK((set22[4]) == (node4));
      CHECK((set22[5]) == (node5));
      CHECK((set22[6]) == (node6));

      set4 = std::move(set23);
      CHECK((set22) == (set4));
      CHECK((set4.nbLHSIds()) == (std::size_t(3)));
      CHECK((set4[0]) == (node0));
      CHECK((set4[1]) == (node1));
      CHECK((set4[2]) == (node2));
      CHECK((set4[3]) == (node3));
      CHECK((set4[4]) == (node4));
      CHECK((set4[5]) == (node5));
      CHECK((set4[6]) == (node6));

      const auto& nodes = set4.ids();
      CHECK((nodes[0]) == (node0));
      CHECK((nodes[1]) == (node1));
      CHECK((nodes[2]) == (node2));
      CHECK((nodes[3]) == (node3));
      CHECK((nodes[4]) == (node4));
      CHECK((nodes[5]) == (node5));
      CHECK((nodes[6]) == (node6));

      std::string str = "{0 , 1 , 2 | 3 , 4 , 5 , 6}";
      CHECK((set4.toString()) == (str));

      auto [l, r]    = set4.toNodeSets();
      gum::NodeSet s = gum::NodeSet{0, 1, 2};
      CHECK((l) == (s));
      s = gum::NodeSet{3, 4, 5, 6};
      CHECK((r) == (s));

      gum::NodeId xnb = 0;
      for (auto iter = set4.begin(); iter != set4.end(); ++iter, ++xnb) {
        CHECK((*iter) == (xnb));
      }
      xnb = 0;
      for (auto iter = set4.beginSafe(); iter != set4.endSafe(); ++iter, ++xnb) {
        CHECK((*iter) == (xnb));
      }
      xnb = 0;
      for (auto nb: set4) {
        CHECK((nb) == (xnb));
        ++xnb;
      }

      auto                                        iter1 = set4.begin() + 2;
      auto                                        iter2 = set4.begin();
      typename gum::learning::IdCondSet::iterator iter3;
      CHECK((iter1) != (iter2));
      CHECK((iter1) != (iter3));
      iter2 += 2;
      CHECK((iter1) == (iter2));
      iter3 = iter1;
      CHECK((iter3) == (iter2));
      CHECK((iter3.pos()) == ((std::size_t)2));

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
      CHECK((set4.size()) == (std::size_t(7)));
      for (std::size_t i = std::size_t(0); i < std::size_t(7); ++i) {
        CHECK((set4.pos(i)) == ((std::size_t)i));
        CHECK(set4.exists(i));
      }
      CHECK_THROWS_AS(set4.pos(7), const gum::NotFound&);
      CHECK(!set4.exists(7));

      set4.clear();
      CHECK(set4.empty());
      CHECK((set4.size()) == (std::size_t(0)));

      gum::learning::IdCondSet set28(0, std::vector< gum::NodeId >{2, 1}, true);
      xnb = 0;
      CHECK((set28.nbLHSIds()) == ((std::size_t)1));
      for ([[maybe_unused]] auto id: set28) {
        id++;
        xnb++;
      }
      CHECK((xnb) == (gum::NodeId(3)));
    }   // namespace gum_tests
  };

  GUM_TEST_ACTIF(1)


} /* namespace gum_tests */
