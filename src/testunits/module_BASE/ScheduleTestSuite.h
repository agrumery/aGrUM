/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/tools/graphicalModels/inference/scheduler/schedule.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class [[maybe_unused]] ScheduleTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_construct1) {
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

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      pot1.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f1(pot1, false);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      pot2.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f2(pot2, false);

      gum::Potential< double > pot3;
      pot3 << *(vars[0]) << *(vars[3]) << *(vars[5]);
      pot3.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f3(pot3, false);

      gum::Potential< double > pot4;
      pot4 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      pot4.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f4(pot4, false);

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > >
                                                               comb1(f1, f2, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result1 = comb1.result();

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > >
                                                               comb2(f2, f3, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result2 = comb2.result();

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > >
                                                               comb3(result2, f4, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result3 = comb3.result();

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > >
                                                               comb4(result1, result3, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result4 = comb4.result();

      gum::ScheduleDeletion< gum::Potential< double > > del1(result1);
      gum::ScheduleDeletion< gum::Potential< double > > del2(result2);
      gum::ScheduleDeletion< gum::Potential< double > > del3(result3);

      gum::Schedule schedule;
      const auto    xf1 = schedule.insertScheduleMultiDim(f1);
      TS_ASSERT(*xf1 == f1);
      TS_ASSERT_THROWS(schedule.insertScheduleMultiDim(f1), const gum::DuplicateScheduleMultiDim&)
      TS_ASSERT_THROWS(schedule.insertTable(pot1, true, 1), const gum::DuplicateScheduleMultiDim&)

      gum::Sequence< const gum::DiscreteVariable* > seq;
      seq << vars[1] << vars[2] << vars[3];
      gum::ScheduleMultiDim< gum::Potential< double > > abstract_f(seq);
      TS_ASSERT_THROWS(schedule.insertScheduleMultiDim(abstract_f),
                       const gum::AbstractScheduleMultiDim&)

      TS_ASSERT_THROWS(schedule.insertOperation(comb1), const gum::UnknownScheduleMultiDim&)

      const auto xf2 = schedule.insertScheduleMultiDim(f2);
      TS_ASSERT(!f2.isAbstract());
      TS_ASSERT(!xf2->isAbstract());

      const auto&  xcomb1               = schedule.insertOperation(comb1);
      gum::NodeSet available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 1);
      TS_ASSERT(available_operations.contains(gum::NodeId(1)));
      TS_ASSERT(schedule.nodeId(xcomb1) == gum::NodeId(1));
      const gum::DAG& dag = schedule.dag();
      TS_ASSERT(dag.sizeNodes() == 1);

      const auto xf3 = schedule.insertScheduleMultiDim(f3);
      TS_ASSERT(!f3.isAbstract());
      TS_ASSERT(!xf3->isAbstract());
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(comb2))
      const auto& xcomb2 = schedule.operation(gum::NodeId(2));
      TS_ASSERT(!xcomb2.isExecuted());
      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 2);

      const auto xf4 = schedule.insertScheduleMultiDim(f4);
      TS_ASSERT(!f4.isAbstract());
      TS_ASSERT(!xf4->isAbstract());
      const auto& xr2 = xcomb2.results();
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(comb3))
      const auto& xcomb3 = schedule.operation(gum::NodeId(3));
      const auto& arg3   = xcomb3.args();
      TS_ASSERT(arg3.size() == 2);
      TS_ASSERT(arg3[0] == xr2[0]);
      TS_ASSERT(arg3[1] == xf4);
      TS_ASSERT(dag.sizeNodes() == 3);
      TS_ASSERT(dag.sizeArcs() == 1);
      TS_ASSERT(dag.existsArc(2, 3));
      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 2);

      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(del2))
      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 2);
      TS_ASSERT(dag.sizeNodes() == 4);
      TS_ASSERT(dag.sizeArcs() == 3);
      TS_ASSERT(dag.existsArc(2, 3));
      TS_ASSERT(dag.existsArc(2, 4));
      TS_ASSERT(dag.existsArc(3, 4));

      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(del1))
      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 2);
      TS_ASSERT(dag.sizeNodes() == 5);
      TS_ASSERT(dag.sizeArcs() == 4);
      TS_ASSERT(dag.existsArc(1, 5));

      TS_GUM_ASSERT_THROWS_NOTHING(schedule.emplaceBinaryCombination(result1, result3, myadd))
      const auto& xcomb4 = schedule.operation(6);
      const auto& args4  = xcomb4.args();
      TS_ASSERT(result1 == *args4[0])
      TS_ASSERT(result3 == *args4[1])
      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      TS_ASSERT(dag.sizeNodes() == 6)
      TS_ASSERT(dag.sizeArcs() == 7)
      TS_ASSERT(dag.existsArc(1, 5))
      TS_ASSERT(dag.existsArc(1, 6))
      TS_ASSERT(dag.existsArc(2, 3))
      TS_ASSERT(dag.existsArc(2, 4))
      TS_ASSERT(dag.existsArc(3, 4))
      TS_ASSERT(dag.existsArc(3, 6))
      TS_ASSERT(dag.existsArc(6, 5))

      const auto& xxcomb4
         = dynamic_cast< const gum::ScheduleBinaryCombination< gum::Potential< double >,
                                                               gum::Potential< double >,
                                                               gum::Potential< double > >& >(
            xcomb4);
      const gum::ScheduleMultiDim< gum::Potential< double > >&      xres4 = xxcomb4.result();
      std::vector< gum::Potential< double > >                       vect4;
      gum::ScheduleStorage< gum::Potential< double >, std::vector > store4(xres4, vect4);
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(store4))
      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      TS_ASSERT(dag.sizeNodes() == 7)
      TS_ASSERT(dag.sizeArcs() == 8)
      TS_ASSERT(dag.existsArc(6, 7))

      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(del3))
      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      TS_ASSERT(dag.sizeNodes() == 8)
      TS_ASSERT(dag.sizeArcs() == 10)
      TS_ASSERT(dag.existsArc(3, 8))
      TS_ASSERT(dag.existsArc(6, 8))

      gum::Schedule   schedule2(schedule);
      const gum::DAG& dag2 = schedule2.dag();
      available_operations = schedule2.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      TS_ASSERT(dag2.sizeNodes() == 8)
      TS_ASSERT(dag2.sizeArcs() == 10)
      TS_ASSERT(dag2.existsArc(1, 5))
      TS_ASSERT(dag2.existsArc(1, 6))
      TS_ASSERT(dag2.existsArc(2, 3))
      TS_ASSERT(dag2.existsArc(2, 4))
      TS_ASSERT(dag2.existsArc(3, 4))
      TS_ASSERT(dag2.existsArc(3, 6))
      TS_ASSERT(dag2.existsArc(6, 5))
      TS_ASSERT(dag2.existsArc(6, 7))
      TS_ASSERT(dag2.existsArc(3, 8))
      TS_ASSERT(dag2.existsArc(6, 8))

      gum::Schedule   schedule3(std::move(schedule2));
      const gum::DAG& dag3 = schedule3.dag();
      available_operations = schedule3.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      TS_ASSERT(dag3.sizeNodes() == 8)
      TS_ASSERT(dag3.sizeArcs() == 10)
      TS_ASSERT(dag3.existsArc(1, 5))
      TS_ASSERT(dag3.existsArc(1, 6))
      TS_ASSERT(dag3.existsArc(2, 3))
      TS_ASSERT(dag3.existsArc(2, 4))
      TS_ASSERT(dag3.existsArc(3, 4))
      TS_ASSERT(dag3.existsArc(3, 6))
      TS_ASSERT(dag3.existsArc(6, 5))
      TS_ASSERT(dag3.existsArc(6, 7))
      TS_ASSERT(dag3.existsArc(3, 8))
      TS_ASSERT(dag3.existsArc(6, 8))

      TS_ASSERT(schedule3 == schedule)
      TS_ASSERT(schedule2 != schedule)

      gum::Schedule*  schedule4 = schedule3.clone();
      const gum::DAG& dag4      = schedule4->dag();
      available_operations      = schedule4->availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      TS_ASSERT(dag4.sizeNodes() == 8)
      TS_ASSERT(dag4.sizeArcs() == 10)
      TS_ASSERT(dag4.existsArc(1, 5))
      TS_ASSERT(dag4.existsArc(1, 6))
      TS_ASSERT(dag4.existsArc(2, 3))
      TS_ASSERT(dag4.existsArc(2, 4))
      TS_ASSERT(dag4.existsArc(3, 4))
      TS_ASSERT(dag4.existsArc(3, 6))
      TS_ASSERT(dag4.existsArc(6, 5))
      TS_ASSERT(dag4.existsArc(6, 7))
      TS_ASSERT(dag4.existsArc(3, 8))
      TS_ASSERT(dag4.existsArc(6, 8))
      TS_ASSERT(*schedule4 == schedule)
      delete schedule4;

      auto& op2 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(2)));
      op2.execute();
      TS_ASSERT(op2.isExecuted());
      std::vector< gum::NodeId > available_nodes;
      schedule3.updateAfterExecution(op2, available_nodes, true);
      TS_ASSERT(available_nodes.size() == 1)
      TS_ASSERT(available_nodes[0] == gum::NodeId(3))

      available_operations = schedule3.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      TS_ASSERT(available_operations.contains(gum::NodeId(1)))
      TS_ASSERT(available_operations.contains(gum::NodeId(3)))

      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      TS_ASSERT(available_operations.contains(gum::NodeId(1)))
      TS_ASSERT(available_operations.contains(gum::NodeId(2)))

      comb2.execute();
      const auto& op2_res
         = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >& >(
            *op2.results()[0]);
      TS_ASSERT(result2.hasSameVariables(op2_res))
      TS_ASSERT(result2.hasSameContent(op2_res))
      TS_ASSERT(!op2_res.isAbstract())
      TS_ASSERT(!result2.isAbstract())

      auto& op1 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(1)));
      op1.execute();
      schedule3.updateAfterExecution(gum::NodeId(1), available_nodes, true);
      TS_ASSERT(available_nodes.size() == 0)
      available_operations = schedule3.availableOperations();
      TS_ASSERT(available_operations.size() == 1)
      TS_ASSERT(available_operations.contains(gum::NodeId(3)));

      TS_ASSERT_THROWS(schedule3.updateAfterExecution(gum::NodeId(3), available_nodes, true),
                       const gum::UnexecutedScheduleOperation&)
      TS_ASSERT_THROWS(schedule3.updateAfterExecution(gum::NodeId(1), available_nodes, true),
                       const gum::UnknownScheduleOperation&)
      TS_ASSERT_THROWS(schedule3.updateAfterExecution(gum::NodeId(4), available_nodes, true),
                       const gum::UnavailableScheduleOperation&)

      comb1.execute();
      const auto& op1_res
         = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >& >(
            *op1.results()[0]);
      TS_ASSERT(result1.hasSameVariables(op1_res))
      TS_ASSERT(result1.hasSameContent(op1_res))

      auto& op3 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(3)));
      op3.execute();
      schedule3.updateAfterExecution(gum::NodeId(3), available_nodes, true);
      TS_ASSERT(available_nodes.size() == 2)
      TS_ASSERT((available_nodes[0] == gum::NodeId(4)) || (available_nodes[0] == gum::NodeId(6)))
      TS_ASSERT((available_nodes[1] == gum::NodeId(4)) || (available_nodes[1] == gum::NodeId(6)))
      available_operations = schedule3.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      TS_ASSERT(available_operations.contains(gum::NodeId(4)))
      TS_ASSERT(available_operations.contains(gum::NodeId(6)))

      comb3.execute();
      const auto& op3_res
         = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >& >(
            *op3.results()[0]);
      TS_ASSERT(result3.hasSameVariables(op3_res))
      TS_ASSERT(result3.hasSameContent(op3_res))

      auto& op4 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(4)));
      TS_ASSERT(!op2_res.isAbstract())
      op4.execute();
      TS_ASSERT(op2_res.isAbstract())
      schedule3.updateAfterExecution(gum::NodeId(4), available_nodes, true);
      TS_ASSERT(available_nodes.size() == 0)
      available_operations = schedule3.availableOperations();
      TS_ASSERT(available_operations.size() == 1)
      TS_ASSERT(available_operations.contains(gum::NodeId(6)))

      auto& op6 = const_cast< gum::ScheduleOperator& >(schedule3.operation(gum::NodeId(6)));
      op6.execute();
      schedule3.updateAfterExecution(gum::NodeId(6), available_nodes, true);
      TS_ASSERT(available_nodes.size() == 3)
      available_operations = schedule3.availableOperations();
      TS_ASSERT(available_operations.size() == 3)
      TS_ASSERT(available_operations.contains(gum::NodeId(5)))
      TS_ASSERT(available_operations.contains(gum::NodeId(7)))
      TS_ASSERT(available_operations.contains(gum::NodeId(8)))

      comb4.execute();
      const auto& op6_res
         = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >& >(
            *op6.results()[0]);
      TS_ASSERT(result4.hasSameVariables(op6_res))
      TS_ASSERT(result4.hasSameContent(op6_res))
      TS_ASSERT(!result4.isAbstract())
      TS_ASSERT(!op6_res.isAbstract())

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_del_store) {
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

      gum::Potential< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[4]);
      pot1.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f1(pot1, false);

      gum::Potential< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[3]);
      pot2.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f2(pot2, false);

      gum::Potential< double > pot3;
      pot3 << *(vars[4]) << *(vars[6]) << *(vars[3]);
      pot3.random();
      gum::ScheduleMultiDim< gum::Potential< double > > f3(pot3, false);

      gum::ScheduleBinaryCombination< gum::Potential< double >,
                                      gum::Potential< double >,
                                      gum::Potential< double > >
                                                               comb(f1, f2, myadd);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result1 = comb.result();

      gum::ScheduleDeletion< gum::Potential< double > > del(result1);

      gum::VariableSet del_vars;
      del_vars << vars[2] << vars[1];
      gum::ScheduleProjection< gum::Potential< double > > myproj(result1, del_vars, myProjectSum);
      const gum::ScheduleMultiDim< gum::Potential< double > >& result2 = myproj.result();

      std::vector< gum::Potential< double > >                       vect;
      gum::ScheduleStorage< gum::Potential< double >, std::vector > store(result2, vect);

      gum::Schedule schedule;
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertScheduleMultiDim(f1))
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertScheduleMultiDim(f2))
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertScheduleMultiDim(f3))
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(comb))
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(myproj))
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(del))
      TS_GUM_ASSERT_THROWS_NOTHING(schedule.insertOperation(store))

      gum::NodeSet               available_operations = schedule.availableOperations();
      std::vector< gum::NodeId > available_nodes;
      TS_ASSERT(available_operations.size() == 1)
      gum::ScheduleOperator& op1
         = const_cast< gum::ScheduleOperator& >(schedule.operation(*available_operations.begin()));
      op1.execute();
      TS_ASSERT(op1.isExecuted())
      schedule.updateAfterExecution(op1, available_nodes, true);

      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 1)
      gum::ScheduleOperator& op2
         = const_cast< gum::ScheduleOperator& >(schedule.operation(*available_operations.begin()));
      op2.execute();
      TS_ASSERT(op2.isExecuted())
      schedule.updateAfterExecution(op2, available_nodes, false);

      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      gum::ScheduleOperator& op3
         = const_cast< gum::ScheduleOperator& >(schedule.operation(*available_operations.begin()));
      op3.execute();
      TS_ASSERT(op3.isExecuted())
      schedule.updateAfterExecution(op3, available_nodes, false);

      available_operations = schedule.availableOperations();
      TS_ASSERT(available_operations.size() == 1);
      gum::ScheduleOperator& op4
         = const_cast< gum::ScheduleOperator& >(schedule.operation(*available_operations.begin()));
      op4.execute();
      TS_ASSERT(op4.isExecuted())

      TS_ASSERT(vect.size() == 1)
      comb.execute();
      myproj.execute();
      del.execute();
      TS_ASSERT(result2.multiDim() == vect[0])

      gum::Schedule schedule2;
      schedule2.insertScheduleMultiDim(f1);
      schedule2.insertScheduleMultiDim(f2);
      schedule2.insertScheduleMultiDim(f3);
      const auto& xcomb = schedule2.emplaceBinaryCombination(f1, f2, myadd);
      const auto  xres1 = static_cast< const gum::ScheduleMultiDim< gum::Potential< double > >* >(
         xcomb.results()[0]);
      const auto& xproj = schedule2.emplaceProjection(*xres1, del_vars, myProjectSum);
      const auto  xres2 = static_cast< const gum::ScheduleMultiDim< gum::Potential< double > >* >(
         xproj.results()[0]);
      schedule2.emplaceDeletion(*xres1);
      schedule2.emplaceStorage(*xres2, vect);

      TS_ASSERT(schedule2.dag().sizeNodes() == 4)

      available_operations = schedule2.availableOperations();
      TS_ASSERT(available_operations.size() == 1)
      gum::ScheduleOperator& xop1
         = const_cast< gum::ScheduleOperator& >(schedule2.operation(*available_operations.begin()));
      xop1.execute();
      TS_ASSERT(xop1.isExecuted())
      schedule2.updateAfterExecution(xop1, available_nodes, true);

      available_operations = schedule2.availableOperations();
      TS_ASSERT(available_operations.size() == 1)
      gum::ScheduleOperator& xop2
         = const_cast< gum::ScheduleOperator& >(schedule2.operation(*available_operations.begin()));
      xop2.execute();
      TS_ASSERT(xop2.isExecuted())
      schedule2.updateAfterExecution(xop2, available_nodes, false);

      available_operations = schedule2.availableOperations();
      TS_ASSERT(available_operations.size() == 2)
      gum::ScheduleOperator& xop3
         = const_cast< gum::ScheduleOperator& >(schedule2.operation(*available_operations.begin()));
      xop3.execute();
      TS_ASSERT(xop3.isExecuted())
      schedule2.updateAfterExecution(xop3, available_nodes, false);

      available_operations = schedule2.availableOperations();
      TS_ASSERT(available_operations.size() == 1);
      gum::ScheduleOperator& xop4
         = const_cast< gum::ScheduleOperator& >(schedule2.operation(*available_operations.begin()));
      xop4.execute();
      TS_ASSERT(xop4.isExecuted())

      TS_ASSERT(vect.size() == 2)
      TS_ASSERT(result2.multiDim() == vect[0])
      TS_ASSERT(result2.multiDim() == vect[1])

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Potential< double > myadd(const gum::Potential< double >& f1,
                                          const gum::Potential< double >& f2) {
      return f1 + f2;
    }

    static gum::Potential< double > myProjectSum(const gum::Potential< double >& pot,
                                                 const gum::VariableSet&         del_vars) {
      return gum::Potential< double >(gum::projectSum(*(pot.content()), del_vars));
    }
  };

} /* namespace gum_tests */
