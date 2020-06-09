
/**
 *
 *  Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>
#include <iostream>

#include <agrum/tools/stattests/idCondSet.h>

namespace gum_tests {

  class IdtCondSetTestSuite: public CxxTest::TestSuite {
    public:
    void test1() {
      gum::learning::IdCondSet<> set;
      TS_ASSERT(set.nbLHSIds() == std::size_t(0));

      gum::NodeId node0 = 0;
      gum::NodeId node1 = 1;
      gum::NodeId node2 = 2;
      gum::NodeId node3 = 3;
      gum::NodeId node4 = 4;
      gum::NodeId node5 = 5;
      gum::NodeId node6 = 6;

      std::vector< gum::NodeId > empty;

      gum::learning::IdCondSet<> set2(node0, empty);
      TS_ASSERT(set2.nbLHSIds() == std::size_t(1));
      TS_ASSERT(set2[0] == node0);
      TS_ASSERT(set2 != set);

      gum::learning::IdCondSet<> cond_set2 = set2.conditionalIdCondSet();
      TS_ASSERT(cond_set2.nbLHSIds() == std::size_t(0));
      TS_ASSERT(cond_set2.size() == std::size_t(0));

      gum::learning::IdCondSet<> set3(set2);
      TS_ASSERT(set3 == set2);
      TS_ASSERT(set3.nbLHSIds() == std::size_t(1));
      TS_ASSERT(set3[0] == node0);

      gum::learning::IdCondSet<> set4(set3, std::allocator< int >());
      TS_ASSERT(set4 == set2);
      TS_ASSERT(set4.nbLHSIds() == std::size_t(1));
      TS_ASSERT(set4[0] == node0);

      std::vector< gum::NodeId > seq1{node4, node3, node6, node5};
      gum::learning::IdCondSet<> set1(seq1, true, false);
      TS_ASSERT(set1.nbLHSIds() == std::size_t(0));
      TS_ASSERT(set1[0] == node3);
      TS_ASSERT(set1[1] == node4);
      TS_ASSERT(set1[2] == node5);
      TS_ASSERT(set1[3] == node6);

      gum::learning::IdCondSet<> cond_set1 = set1.conditionalIdCondSet();
      TS_ASSERT(cond_set1.nbLHSIds() == std::size_t(0));
      TS_ASSERT(cond_set1[0] == node3);
      TS_ASSERT(cond_set1[1] == node4);
      TS_ASSERT(cond_set1[2] == node5);
      TS_ASSERT(cond_set1[3] == node6);
      std::size_t cs1 = 0;
      for (const auto node: cond_set1)
        cs1 += node;
      TS_ASSERT(cs1 == std::size_t(18));

      gum::learning::IdCondSet<> set1b(empty, true, false);
      TS_ASSERT(set1b.nbLHSIds() == std::size_t(0));
      TS_ASSERT(set1b.size() == std::size_t(0));

      gum::learning::IdCondSet<> cond_set1b = set1b.conditionalIdCondSet();
      TS_ASSERT(cond_set1b.nbLHSIds() == std::size_t(0));
      TS_ASSERT(cond_set1b.size() == std::size_t(0));

      gum::learning::IdCondSet<> set5(node0, seq1, false, std::allocator< int >());
      TS_ASSERT(set5.nbLHSIds() == std::size_t(1));
      TS_ASSERT(set5[0] == node0);
      TS_ASSERT(set5[1] == node3);
      TS_ASSERT(set5[2] == node4);
      TS_ASSERT(set5[3] == node5);
      TS_ASSERT(set5[4] == node6);
      TS_ASSERT(set5 != set2);

      gum::learning::IdCondSet<> cond_set5 = set5.conditionalIdCondSet();
      TS_ASSERT(cond_set5.nbLHSIds() == std::size_t(0));
      TS_ASSERT(cond_set5[0] == node3);
      TS_ASSERT(cond_set5[1] == node4);
      TS_ASSERT(cond_set5[2] == node5);
      TS_ASSERT(cond_set5[3] == node6);


      gum::learning::IdCondSet<> set6(set5);
      TS_ASSERT(set6 == set5);
      TS_ASSERT(set6.nbLHSIds() == std::size_t(1));
      TS_ASSERT(set6[0] == node0);
      TS_ASSERT(set6[1] == node3);
      TS_ASSERT(set6[2] == node4);
      TS_ASSERT(set6[3] == node5);
      TS_ASSERT(set6[4] == node6);
      TS_ASSERT(set6 != set2);

      gum::learning::IdCondSet<> set7(set5, std::allocator< int >());
      TS_ASSERT(set7 == set6);

      gum::learning::IdCondSet<> set8(std::move(set2));
      TS_ASSERT(set8 == set3);
      TS_ASSERT(set8.nbLHSIds() == std::size_t(1));
      TS_ASSERT(set8[0] == node0);

      gum::learning::IdCondSet<> set9(std::move(set3), std::allocator< int >());
      TS_ASSERT(set9 == set8);
      TS_ASSERT(set9.nbLHSIds() == std::size_t(1));
      TS_ASSERT(set9[0] == node0);

      gum::learning::IdCondSet<> set10(std::move(set5), std::allocator< int >());
      TS_ASSERT(set10.nbLHSIds() == std::size_t(1));
      TS_ASSERT(set10[0] == node0);
      TS_ASSERT(set10[1] == node3);
      TS_ASSERT(set10[2] == node4);
      TS_ASSERT(set10[3] == node5);
      TS_ASSERT(set10[4] == node6);
      TS_ASSERT(set10 != set8);

      gum::learning::IdCondSet<> set11(std::move(set6));
      TS_ASSERT(set11 == set10);
      TS_ASSERT(set11.nbLHSIds() == std::size_t(1));
      TS_ASSERT(set11[0] == node0);
      TS_ASSERT(set11[1] == node3);
      TS_ASSERT(set11[2] == node4);
      TS_ASSERT(set11[3] == node5);
      TS_ASSERT(set11[4] == node6);


      gum::learning::IdCondSet<>* set12 = set10.clone(std::allocator< int >());
      TS_ASSERT(set12->nbLHSIds() == std::size_t(1));
      TS_ASSERT(set12->operator[](0) == node0);
      TS_ASSERT(set12->operator[](1) == node3);
      TS_ASSERT(set12->operator[](2) == node4);
      TS_ASSERT(set12->operator[](3) == node5);
      TS_ASSERT(set12->operator[](4) == node6);
      TS_ASSERT(*set12 != set8);

      gum::learning::IdCondSet<>* set13 = set10.clone();
      TS_ASSERT(set13->nbLHSIds() == std::size_t(1));
      TS_ASSERT(set13->operator[](0) == node0);
      TS_ASSERT(set13->operator[](1) == node3);
      TS_ASSERT(set13->operator[](2) == node4);
      TS_ASSERT(set13->operator[](3) == node5);
      TS_ASSERT(set13->operator[](4) == node6);
      TS_ASSERT(*set13 != set8);

      delete set12;
      delete set13;

      gum::learning::IdCondSet<> set14(
         node1, node0, empty, true, false, std::allocator< int >());
      TS_ASSERT(set14.nbLHSIds() == std::size_t(2));
      TS_ASSERT(set14[0] == node1);
      TS_ASSERT(set14[1] == node0);
      TS_ASSERT(set14 != set2);

      gum::learning::IdCondSet<> set15(node1, node0, empty, true);
      TS_ASSERT(set15.nbLHSIds() == std::size_t(2));
      TS_ASSERT(set15[0] == node1);
      TS_ASSERT(set15[1] == node0);
      TS_ASSERT(set15 != set2);

      gum::learning::IdCondSet<> set16(
         node1, node0, empty, false, false, std::allocator< int >());
      TS_ASSERT(set16.nbLHSIds() == std::size_t(2));
      TS_ASSERT(set16[0] == node0);
      TS_ASSERT(set16[1] == node1);
      TS_ASSERT(set16 != set2);

      gum::learning::IdCondSet<> set17(node1, node0, empty, false);
      TS_ASSERT(set17.nbLHSIds() == std::size_t(2));
      TS_ASSERT(set17[0] == node0);
      TS_ASSERT(set17[1] == node1);
      TS_ASSERT(set17 != set2);

      gum::learning::IdCondSet<> set18(
         node1, node0, seq1, true, false, std::allocator< int >());
      TS_ASSERT(set18.nbLHSIds() == std::size_t(2));
      TS_ASSERT(set18[0] == node1);
      TS_ASSERT(set18[1] == node0);
      TS_ASSERT(set18[2] == node3);
      TS_ASSERT(set18[3] == node4);
      TS_ASSERT(set18[4] == node5);
      TS_ASSERT(set18[5] == node6);
      TS_ASSERT(set18 != set2);

      gum::learning::IdCondSet<> set19(
         node1, node0, seq1, false, false, std::allocator< int >());
      TS_ASSERT(set19.nbLHSIds() == std::size_t(2));
      TS_ASSERT(set19[0] == node0);
      TS_ASSERT(set19[1] == node1);
      TS_ASSERT(set19[2] == node3);
      TS_ASSERT(set19[3] == node4);
      TS_ASSERT(set19[4] == node5);
      TS_ASSERT(set19[5] == node6);
      TS_ASSERT(set19 != set2);

      gum::learning::IdCondSet<> set20(node1, node0, seq1, true);
      TS_ASSERT(set20.nbLHSIds() == std::size_t(2));
      TS_ASSERT(set20[0] == node1);
      TS_ASSERT(set20[1] == node0);
      TS_ASSERT(set20[2] == node3);
      TS_ASSERT(set20[3] == node4);
      TS_ASSERT(set20[4] == node5);
      TS_ASSERT(set20[5] == node6);
      TS_ASSERT(set20 != set2);

      gum::learning::IdCondSet<> set21(node1, node0, seq1, false);
      TS_ASSERT(set21.nbLHSIds() == std::size_t(2));
      TS_ASSERT(set21[0] == node0);
      TS_ASSERT(set21[1] == node1);
      TS_ASSERT(set21[2] == node3);
      TS_ASSERT(set21[3] == node4);
      TS_ASSERT(set21[4] == node5);
      TS_ASSERT(set21[5] == node6);
      TS_ASSERT(set21 != set2);

      gum::learning::IdCondSet<> set22(node1, node0, node2, seq1, true);
      TS_ASSERT(set22.nbLHSIds() == std::size_t(3));
      TS_ASSERT(set22[0] == node1);
      TS_ASSERT(set22[1] == node0);
      TS_ASSERT(set22[2] == node2);
      TS_ASSERT(set22[3] == node3);
      TS_ASSERT(set22[4] == node4);
      TS_ASSERT(set22[5] == node5);
      TS_ASSERT(set22[6] == node6);
      TS_ASSERT(set22 != set2);

      gum::learning::IdCondSet<> set23(node1, node0, node2, seq1, false);
      TS_ASSERT(set23.nbLHSIds() == std::size_t(3));
      TS_ASSERT(set23[0] == node0);
      TS_ASSERT(set23[1] == node1);
      TS_ASSERT(set23[2] == node2);
      TS_ASSERT(set23[3] == node3);
      TS_ASSERT(set23[4] == node4);
      TS_ASSERT(set23[5] == node5);
      TS_ASSERT(set23[6] == node6);
      TS_ASSERT(set23 != set2);

      set22 = set23;
      TS_ASSERT(set23 == set22);
      TS_ASSERT(set22.nbLHSIds() == std::size_t(3));
      TS_ASSERT(set22[0] == node0);
      TS_ASSERT(set22[1] == node1);
      TS_ASSERT(set22[2] == node2);
      TS_ASSERT(set22[3] == node3);
      TS_ASSERT(set22[4] == node4);
      TS_ASSERT(set22[5] == node5);
      TS_ASSERT(set22[6] == node6);

      set4 = std::move(set23);
      TS_ASSERT(set22 == set4);
      TS_ASSERT(set4.nbLHSIds() == std::size_t(3));
      TS_ASSERT(set4[0] == node0);
      TS_ASSERT(set4[1] == node1);
      TS_ASSERT(set4[2] == node2);
      TS_ASSERT(set4[3] == node3);
      TS_ASSERT(set4[4] == node4);
      TS_ASSERT(set4[5] == node5);
      TS_ASSERT(set4[6] == node6);

      const auto& nodes = set4.ids();
      TS_ASSERT(nodes[0] == node0);
      TS_ASSERT(nodes[1] == node1);
      TS_ASSERT(nodes[2] == node2);
      TS_ASSERT(nodes[3] == node3);
      TS_ASSERT(nodes[4] == node4);
      TS_ASSERT(nodes[5] == node5);
      TS_ASSERT(nodes[6] == node6);

      std::string str = "{0 , 1 , 2 | 3 , 4 , 5 , 6}";
      TS_ASSERT(set4.toString() == str);

      gum::NodeId xnb = 0;
      for (auto iter = set4.begin(); iter != set4.end(); ++iter, ++xnb) {
        TS_ASSERT(*iter == xnb);
      }
      xnb = 0;
      for (auto iter = set4.beginSafe(); iter != set4.endSafe(); ++iter, ++xnb) {
        TS_ASSERT(*iter == xnb);
      }
      xnb = 0;
      for (auto nb: set4) {
        TS_ASSERT(nb == xnb);
        ++xnb;
      }

      auto                                          iter1 = set4.begin() + 2;
      auto                                          iter2 = set4.begin();
      typename gum::learning::IdCondSet<>::iterator iter3;
      TS_ASSERT(iter1 != iter2);
      TS_ASSERT(iter1 != iter3);
      iter2 += 2;
      TS_ASSERT(iter1 == iter2);
      iter3 = iter1;
      TS_ASSERT(iter3 == iter2);
      TS_ASSERT(iter3.pos() == 2);

      gum::learning::IdCondSet<> set25(node0, empty);
      TS_ASSERT(set4.contains(set25));
      TS_ASSERT(!set25.contains(set4));

      gum::NodeId                node7 = 7;
      gum::learning::IdCondSet<> set26(node7, seq1, false);
      TS_ASSERT(!set4.contains(set26));
      TS_ASSERT(!set26.contains(set4));

      gum::learning::IdCondSet<> set27;
      TS_ASSERT(!set26.empty());
      TS_ASSERT(set27.empty());
      TS_ASSERT(set4.size() == std::size_t(7));
      for (std::size_t i = std::size_t(0); i < std::size_t(7); ++i) {
        TS_ASSERT(set4.pos(i) == i);
        TS_ASSERT(set4.exists(i));
      }
      TS_ASSERT_THROWS(set4.pos(7), gum::NotFound);
      TS_ASSERT(!set4.exists(7));

      set4.clear();
      TS_ASSERT(set4.empty());
      TS_ASSERT(set4.size() == std::size_t(0));

      gum::learning::IdCondSet<> set28(0, std::vector< gum::NodeId >{2, 1}, true);
      xnb = 0;
      TS_ASSERT(set28.nbLHSIds() == 1);
      for (auto id: set28) {
        id++;
        xnb++;
      }
      TS_ASSERT(xnb == 3);
    }
  };


} /* namespace gum_tests */
