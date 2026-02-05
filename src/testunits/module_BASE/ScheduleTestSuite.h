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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphicalModels/inference/scheduler/schedule.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  Schedule
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct ScheduleTestSuite {
    public:
    static void test_construct1() {
      // reset the ids of the ScheduleMultiDim to avoid conflicts with other
      // testunits
      gum::IScheduleMultiDim::resetIdGenerator();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Tensor< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      pot1.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f1(pot1, false);

      gum::Tensor< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      pot2.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f2(pot2, false);

      gum::Tensor< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      pot3.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f3(pot3, false);

      gum::Tensor< double > pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      pot4.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f4(pot4, false);

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
                                                            comb1(f1, f2, myadd);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result1 = comb1.result();

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
                                                            comb2(f2, f3, myadd);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result2 = comb2.result();

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
                                                            comb3(result2, f4, myadd);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result3 = comb3.result();

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
                                                            comb4(result1, result3, myadd);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result4 = comb4.result();

      gum::ScheduleDeletion< gum::Tensor< double > > del1(result1);
      gum::ScheduleDeletion< gum::Tensor< double > > del2(result2);
      gum::ScheduleDeletion< gum::Tensor< double > > del3(result3);

      gum::Schedule schedule;
      const auto    xf1 = schedule.insertScheduleMultiDim(f1);
      CHECK(*xf1 == f1);
      CHECK_THROWS_AS(schedule.insertScheduleMultiDim(f1), const gum::DuplicateScheduleMultiDim&);
      CHECK_THROWS_AS(schedule.insertTable(pot1, true, 1), const gum::DuplicateScheduleMultiDim&);

      gum::Sequence< const gum::DiscreteVariable* > seq;
      seq << vars[1] << vars[2] << vars[3];
      gum::ScheduleMultiDim< gum::Tensor< double > > abstract_f(seq);
      CHECK_THROWS_AS(schedule.insertScheduleMultiDim(abstract_f),
                      const gum::AbstractScheduleMultiDim&);

      CHECK_THROWS_AS(schedule.insertOperation(comb1), const gum::UnknownScheduleMultiDim&);

      const auto xf2 = schedule.insertScheduleMultiDim(f2);
      CHECK(!f2.isAbstract());
      CHECK(!xf2->isAbstract());

      const auto&  xcomb1               = schedule.insertOperation(comb1);
      gum::NodeSet available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 1);
      CHECK(available_operations.contains(gum::NodeId(1)));
      CHECK(schedule.nodeId(xcomb1) == gum::NodeId(1));
      const gum::DAG& dag = schedule.dag();
      CHECK(dag.sizeNodes() == 1);

      const auto xf3 = schedule.insertScheduleMultiDim(f3);
      CHECK(!f3.isAbstract());
      CHECK(!xf3->isAbstract());
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(comb2));
      const auto& xcomb2 = schedule.operation(gum::NodeId(2));
      CHECK(!xcomb2.isExecuted());
      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 2);

      const auto xf4 = schedule.insertScheduleMultiDim(f4);
      CHECK(!f4.isAbstract());
      CHECK(!xf4->isAbstract());
      const auto& xr2 = xcomb2.results();
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(comb3));
      const auto& xcomb3 = schedule.operation(gum::NodeId(3));
      const auto& arg3   = xcomb3.args();
      CHECK(arg3.size() == 2);
      CHECK(arg3[0] == xr2[0]);
      CHECK(arg3[1] == xf4);
      CHECK(dag.sizeNodes() == 3);
      CHECK(dag.sizeArcs() == 1);
      CHECK(dag.existsArc(2, 3));
      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 2);

      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(del2));
      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(dag.sizeNodes() == 4);
      CHECK(dag.sizeArcs() == 3);
      CHECK(dag.existsArc(2, 3));
      CHECK(dag.existsArc(2, 4));
      CHECK(dag.existsArc(3, 4));

      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(del1));
      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(dag.sizeNodes() == 5);
      CHECK(dag.sizeArcs() == 4);
      CHECK(dag.existsArc(1, 5));

      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.emplaceBinaryCombination(result1, result3, myadd));
      const auto& xcomb4 = schedule.operation(6);
      const auto& args4  = xcomb4.args();
      CHECK(result1 == *args4[0]);
      CHECK(result3 == *args4[1]);
      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(dag.sizeNodes() == 6);
      CHECK(dag.sizeArcs() == 7);
      CHECK(dag.existsArc(1, 5));
      CHECK(dag.existsArc(1, 6));
      CHECK(dag.existsArc(2, 3));
      CHECK(dag.existsArc(2, 4));
      CHECK(dag.existsArc(3, 4));
      CHECK(dag.existsArc(3, 6));
      CHECK(dag.existsArc(6, 5));

      const auto& xxcomb4
          = dynamic_cast< const gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                                                gum::Tensor< double >,
                                                                gum::Tensor< double > >& >(xcomb4);
      const gum::ScheduleMultiDim< gum::Tensor< double > >&      xres4 = xxcomb4.result();
      std::vector< gum::Tensor< double > >                       vect4;
      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store4(xres4, vect4);
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(store4));
      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(dag.sizeNodes() == 7);
      CHECK(dag.sizeArcs() == 8);
      CHECK(dag.existsArc(6, 7));

      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(del3));
      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(dag.sizeNodes() == 8);
      CHECK(dag.sizeArcs() == 10);
      CHECK(dag.existsArc(3, 8));
      CHECK(dag.existsArc(6, 8));

      gum::Schedule   schedule2(schedule);
      const gum::DAG& dag2 = schedule2.dag();
      available_operations = schedule2.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(dag2.sizeNodes() == 8);
      CHECK(dag2.sizeArcs() == 10);
      CHECK(dag2.existsArc(1, 5));
      CHECK(dag2.existsArc(1, 6));
      CHECK(dag2.existsArc(2, 3));
      CHECK(dag2.existsArc(2, 4));
      CHECK(dag2.existsArc(3, 4));
      CHECK(dag2.existsArc(3, 6));
      CHECK(dag2.existsArc(6, 5));
      CHECK(dag2.existsArc(6, 7));
      CHECK(dag2.existsArc(3, 8));
      CHECK(dag2.existsArc(6, 8));

      gum::Schedule   schedule3(std::move(schedule2));
      const gum::DAG& dag3 = schedule3.dag();
      available_operations = schedule3.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(dag3.sizeNodes() == 8);
      CHECK(dag3.sizeArcs() == 10);
      CHECK(dag3.existsArc(1, 5));
      CHECK(dag3.existsArc(1, 6));
      CHECK(dag3.existsArc(2, 3));
      CHECK(dag3.existsArc(2, 4));
      CHECK(dag3.existsArc(3, 4));
      CHECK(dag3.existsArc(3, 6));
      CHECK(dag3.existsArc(6, 5));
      CHECK(dag3.existsArc(6, 7));
      CHECK(dag3.existsArc(3, 8));
      CHECK(dag3.existsArc(6, 8));

      CHECK(schedule3 == schedule);
      CHECK(schedule2 != schedule);

      gum::Schedule*  schedule4 = schedule3.clone();
      const gum::DAG& dag4      = schedule4->dag();
      available_operations      = schedule4->availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(dag4.sizeNodes() == 8);
      CHECK(dag4.sizeArcs() == 10);
      CHECK(dag4.existsArc(1, 5));
      CHECK(dag4.existsArc(1, 6));
      CHECK(dag4.existsArc(2, 3));
      CHECK(dag4.existsArc(2, 4));
      CHECK(dag4.existsArc(3, 4));
      CHECK(dag4.existsArc(3, 6));
      CHECK(dag4.existsArc(6, 5));
      CHECK(dag4.existsArc(6, 7));
      CHECK(dag4.existsArc(3, 8));
      CHECK(dag4.existsArc(6, 8));
      CHECK(*schedule4 == schedule);
      delete schedule4;

      auto& op2 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(2)));
      op2.execute();
      CHECK(op2.isExecuted());
      std::vector< gum::NodeId > available_nodes;
      schedule3.updateAfterExecution(op2, available_nodes, true);
      CHECK(available_nodes.size() == 1);
      CHECK(available_nodes[0] == gum::NodeId(3));

      available_operations = schedule3.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(available_operations.contains(gum::NodeId(1)));
      CHECK(available_operations.contains(gum::NodeId(3)));

      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(available_operations.contains(gum::NodeId(1)));
      CHECK(available_operations.contains(gum::NodeId(2)));

      comb2.execute();
      const auto& op2_res = dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >& >(
          *op2.results()[0]);
      CHECK(result2.hasSameVariables(op2_res));
      CHECK(result2.hasSameContent(op2_res));
      CHECK(!op2_res.isAbstract());
      CHECK(!result2.isAbstract());

      auto& op1 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(1)));
      op1.execute();
      schedule3.updateAfterExecution(gum::NodeId(1), available_nodes, true);
      CHECK(available_nodes.size() == 0);
      available_operations = schedule3.availableOperations();
      CHECK(available_operations.size() == 1);
      CHECK(available_operations.contains(gum::NodeId(3)));

      CHECK_THROWS_AS(schedule3.updateAfterExecution(gum::NodeId(3), available_nodes, true),
                      const gum::UnexecutedScheduleOperation&);
      CHECK_THROWS_AS(schedule3.updateAfterExecution(gum::NodeId(1), available_nodes, true),
                      const gum::UnknownScheduleOperation&);
      CHECK_THROWS_AS(schedule3.updateAfterExecution(gum::NodeId(4), available_nodes, true),
                      const gum::UnavailableScheduleOperation&);

      comb1.execute();
      const auto& op1_res = dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >& >(
          *op1.results()[0]);
      CHECK(result1.hasSameVariables(op1_res));
      CHECK(result1.hasSameContent(op1_res));

      auto& op3 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(3)));
      op3.execute();
      schedule3.updateAfterExecution(gum::NodeId(3), available_nodes, true);
      CHECK(available_nodes.size() == 2);
      CHECK(((available_nodes[0] == gum::NodeId(4)) || (available_nodes[0] == gum::NodeId(6))));
      CHECK(((available_nodes[1] == gum::NodeId(4)) || (available_nodes[1] == gum::NodeId(6))));
      available_operations = schedule3.availableOperations();
      CHECK(available_operations.size() == 2);
      CHECK(available_operations.contains(gum::NodeId(4)));
      CHECK(available_operations.contains(gum::NodeId(6)));

      comb3.execute();
      const auto& op3_res = dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >& >(
          *op3.results()[0]);
      CHECK(result3.hasSameVariables(op3_res));
      CHECK(result3.hasSameContent(op3_res));

      auto& op4 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(4)));
      CHECK(!op2_res.isAbstract());
      op4.execute();
      CHECK(op2_res.isAbstract());
      schedule3.updateAfterExecution(gum::NodeId(4), available_nodes, true);
      CHECK(available_nodes.size() == 0);
      available_operations = schedule3.availableOperations();
      CHECK(available_operations.size() == 1);
      CHECK(available_operations.contains(gum::NodeId(6)));

      auto& op6 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(6)));
      op6.execute();
      schedule3.updateAfterExecution(gum::NodeId(6), available_nodes, true);
      CHECK(available_nodes.size() == 3);
      available_operations = schedule3.availableOperations();
      CHECK(available_operations.size() == 3);
      CHECK(available_operations.contains(gum::NodeId(5)));
      CHECK(available_operations.contains(gum::NodeId(7)));
      CHECK(available_operations.contains(gum::NodeId(8)));

      comb4.execute();
      const auto& op6_res = dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >& >(
          *op6.results()[0]);
      CHECK(result4.hasSameVariables(op6_res));
      CHECK(result4.hasSameContent(op6_res));
      CHECK(!result4.isAbstract());
      CHECK(!op6_res.isAbstract());

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }   // namespace gum_tests

    static void test_del_store() {
      // reset the ids of the ScheduleMultiDim to avoid conflicts with other
      // testunits
      gum::IScheduleMultiDim::resetIdGenerator();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Tensor< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      pot1.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f1(pot1, false);

      gum::Tensor< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      pot2.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f2(pot2, false);

      gum::Tensor< double > pot3;
      pot3 << *(vars[4]) << *(vars[6]) << *(vars[3]);
      pot3.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f3(pot3, false);

      gum::ScheduleBinaryCombination< gum::Tensor< double >,
                                      gum::Tensor< double >,
                                      gum::Tensor< double > >
                                                            comb(f1, f2, myadd);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result1 = comb.result();

      gum::ScheduleDeletion< gum::Tensor< double > > del(result1);

      gum::VariableSet del_vars;
      del_vars << vars[2] << vars[1];
      gum::ScheduleProjection< gum::Tensor< double > >      myproj(result1, del_vars, myProjectSum);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& result2 = myproj.result();

      std::vector< gum::Tensor< double > >                       vect;
      gum::ScheduleStorage< gum::Tensor< double >, std::vector > store(result2, vect);

      gum::Schedule schedule;
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertScheduleMultiDim(f1));
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertScheduleMultiDim(f2));
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertScheduleMultiDim(f3));
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(comb));
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(myproj));
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(del));
      GUM_CHECK_ASSERT_THROWS_NOTHING(schedule.insertOperation(store));

      gum::NodeSet               available_operations = schedule.availableOperations();
      std::vector< gum::NodeId > available_nodes;
      CHECK(available_operations.size() == 1);
      gum::ScheduleOperator& op1
          = const_cast< gum::ScheduleOperator& >(schedule.operation(*available_operations.begin()));
      op1.execute();
      CHECK(op1.isExecuted());
      schedule.updateAfterExecution(op1, available_nodes, true);

      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 1);
      gum::ScheduleOperator& op2
          = const_cast< gum::ScheduleOperator& >(schedule.operation(*available_operations.begin()));
      op2.execute();
      CHECK(op2.isExecuted());
      schedule.updateAfterExecution(op2, available_nodes, false);

      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 2);
      gum::ScheduleOperator& op3
          = const_cast< gum::ScheduleOperator& >(schedule.operation(*available_operations.begin()));
      op3.execute();
      CHECK(op3.isExecuted());
      schedule.updateAfterExecution(op3, available_nodes, false);

      available_operations = schedule.availableOperations();
      CHECK(available_operations.size() == 1);
      gum::ScheduleOperator& op4
          = const_cast< gum::ScheduleOperator& >(schedule.operation(*available_operations.begin()));
      op4.execute();
      CHECK(op4.isExecuted());

      CHECK(vect.size() == 1);
      comb.execute();
      myproj.execute();
      del.execute();
      CHECK(result2.multiDim() == vect[0]);

      gum::Schedule schedule2;
      schedule2.insertScheduleMultiDim(f1);
      schedule2.insertScheduleMultiDim(f2);
      schedule2.insertScheduleMultiDim(f3);
      const auto& xcomb = schedule2.emplaceBinaryCombination(f1, f2, myadd);
      const auto  xres1 = static_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >* >(
          xcomb.results()[0]);
      const auto& xproj = schedule2.emplaceProjection(*xres1, del_vars, myProjectSum);
      const auto  xres2 = static_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >* >(
          xproj.results()[0]);
      schedule2.emplaceDeletion(*xres1);
      schedule2.emplaceStorage(*xres2, vect);

      CHECK(schedule2.dag().sizeNodes() == 4);

      available_operations = schedule2.availableOperations();
      CHECK(available_operations.size() == 1);
      gum::ScheduleOperator& xop1 = const_cast< gum::ScheduleOperator& >(
          schedule2.operation(*available_operations.begin()));
      xop1.execute();
      CHECK(xop1.isExecuted());
      schedule2.updateAfterExecution(xop1, available_nodes, true);

      available_operations = schedule2.availableOperations();
      CHECK(available_operations.size() == 1);
      gum::ScheduleOperator& xop2 = const_cast< gum::ScheduleOperator& >(
          schedule2.operation(*available_operations.begin()));
      xop2.execute();
      CHECK(xop2.isExecuted());
      schedule2.updateAfterExecution(xop2, available_nodes, false);

      available_operations = schedule2.availableOperations();
      CHECK(available_operations.size() == 2);
      gum::ScheduleOperator& xop3 = const_cast< gum::ScheduleOperator& >(
          schedule2.operation(*available_operations.begin()));
      xop3.execute();
      CHECK(xop3.isExecuted());
      schedule2.updateAfterExecution(xop3, available_nodes, false);

      available_operations = schedule2.availableOperations();
      CHECK(available_operations.size() == 1);
      gum::ScheduleOperator& xop4 = const_cast< gum::ScheduleOperator& >(
          schedule2.operation(*available_operations.begin()));
      xop4.execute();
      CHECK(xop4.isExecuted());

      CHECK(vect.size() == 2);
      CHECK(result2.multiDim() == vect[0]);
      CHECK(result2.multiDim() == vect[1]);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Tensor< double > myadd(const gum::Tensor< double >& f1,
                                       const gum::Tensor< double >& f2) {
      return f1 + f2;
    }

    static gum::Tensor< double > myProjectSum(const gum::Tensor< double >& pot,
                                              const gum::VariableSet&      del_vars) {
      return gum::Tensor< double >(gum::projectSum(*(pot.content()), del_vars));
    }
  };

  GUM_TEST_ACTIF(_construct1)
  GUM_TEST_ACTIF(_del_store)

} /* namespace gum_tests */
