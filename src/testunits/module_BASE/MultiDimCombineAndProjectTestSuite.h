/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/core/set.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/multidim/utils/operators/multiDimCombinationDefault.h>
#include <agrum/tools/multidim/utils/operators/multiDimCombineAndProjectDefault.h>
#include <agrum/tools/multidim/utils/operators/multiDimProjection.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class MultiDimCombineAndProjectTestSuite: public CxxTest::TestSuite {
    public:
    void testDouble() {
      gum::IScheduleMultiDim::resetIdGenerator();
      std::vector< gum::LabelizedVariable* > vars(11);

      for (gum::Idx i = 0; i < 11; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential< double > t1, t2, t3, t4, t5, t6;
      t1 << *(vars[0]) << *(vars[1]);
      t2 << *(vars[1]) << *(vars[2]);
      t3 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      t4 << *(vars[4]) << *(vars[7]);
      t5 << *(vars[5]) << *(vars[6]);
      t6 << *(vars[8]) << *(vars[9]);

      randomInitP(t1);
      randomInitP(t2);
      randomInitP(t3);
      randomInitP(t4);
      randomInitP(t5);
      randomInitP(t6);

      gum::MultiDimCombineAndProjectDefault< gum::Potential< double > > projcomb(multPot, mySum);

      gum::Set< const gum::Potential< double >* > to_comb;
      to_comb << &t1 << &t2 << &t3 << &t4 << &t5 << &t6;
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars << vars[1] << vars[4] << vars[5] << vars[6] << vars[9] << vars[10];

      gum::Set< const gum::Potential< double >* > res = projcomb.execute(to_comb, del_vars);

      double nb_ops = projcomb.nbOperations(to_comb, del_vars);

      auto yyy = projcomb.memoryUsage(to_comb, del_vars);

      TS_ASSERT_EQUALS(yyy.first, 116 * sizeof(double) + 5 * sizeof(gum::Potential< double >))
      TS_ASSERT_EQUALS(yyy.second, 36 * sizeof(double) + 3 * sizeof(gum::Potential< double >))

      TS_ASSERT_EQUALS(nb_ops, 421)   // combinations + projections + deletions
      TS_ASSERT_EQUALS(res.size(), (gum::Size)3)

      gum::Set< const gum::Potential< double >* >::const_iterator iter = res.begin();
      const gum::Potential< double >*                             res1 = *iter;
      ++iter;
      const gum::Potential< double >* res2 = *iter;
      ++iter;
      const gum::Potential< double >* res3 = *iter;

      gum::MultiDimCombinationDefault< gum::Potential< double > > comb(multPot);
      gum::MultiDimProjection< gum::Potential< double > >         proj(mySum);

      gum::Set< const gum::Potential< double >* > comb_set;
      comb_set << &t1 << &t2;
      gum::Set< const gum::DiscreteVariable* > del_vars2;
      del_vars2 << vars[1];
      gum::Potential< double >* tt1 = comb.execute(comb_set);
      gum::Potential< double >* tt2 = proj.execute(*tt1, del_vars2);
      delete tt1;
      TS_ASSERT((*tt2 == *res1) || (*tt2 == *res2) || (*tt2 == *res3))

      delete tt2;
      comb_set.clear();
      del_vars2.clear();
      comb_set << &t3 << &t4 << &t5;
      del_vars2 << vars[4] << vars[5] << vars[6];
      tt1 = comb.execute(comb_set);
      tt2 = proj.execute(*tt1, del_vars2);
      delete tt1;
      TS_ASSERT((*tt2 == *res1) || (*tt2 == *res2) || (*tt2 == *res3))

      delete tt2;
      del_vars2.clear();
      del_vars2 << vars[9] << vars[10];
      tt2 = proj.execute(t6, del_vars2);
      TS_ASSERT((*tt2 == *res1) || (*tt2 == *res2) || (*tt2 == *res3))
      delete tt2;

      gum::MultiDimCombineAndProjectDefault< gum::Potential< double > > projcomb2 = projcomb;
      projcomb2.setCombinationFunction(addPot);
      projcomb2.setProjectionFunction(myMax);

      gum::MultiDimCombineAndProjectDefault< gum::Potential< double > >* projcomb3
         = projcomb.clone();

      delete projcomb3;

      for (const auto pot: res)
        delete pot;

      gum::IScheduleMultiDim::resetIdGenerator();
      gum::ScheduleMultiDim< gum::Potential< double > > xt1(t1, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt2(t2, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt3(t3, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt4(t4, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt5(t5, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt6(t6, false);
      gum::Set< const gum::IScheduleMultiDim* > sched_to_comb{&xt1, &xt2, &xt3, &xt4, &xt5, &xt6};

      auto ops_plus_res = projcomb.operations(sched_to_comb, del_vars);
      for (auto op: ops_plus_res.first) {
        const_cast< gum::ScheduleOperator* >(op)->execute();
      }

      gum::Potential< double > result1;
      if (ops_plus_res.second.size() > 1) {
        auto comb_ops_plus_res = comb.operations(ops_plus_res.second);
        for (auto op: comb_ops_plus_res.first) {
          const_cast< gum::ScheduleOperator* >(op)->execute();
        }
        result1 = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >* >(
                     comb_ops_plus_res.second)
                     ->multiDim();
        for (auto op: comb_ops_plus_res.first)
          delete op;
      } else {
        result1 = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >* >(
                     *(ops_plus_res.second.begin()))
                     ->multiDim();
      }

      gum::Potential< double >* result2a = comb.execute(to_comb);
      gum::Potential< double >* result2  = proj.execute(*result2a, del_vars);
      TS_ASSERT(result1 == *result2);

      delete result2;
      delete result2a;

      for (auto op: ops_plus_res.first)
        delete op;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void testSchedules() {
      gum::IScheduleMultiDim::resetIdGenerator();
      std::vector< gum::LabelizedVariable* > vars(11);

      for (gum::Idx i = 0; i < 11; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential< double > t1, t2, t3, t4, t5, t6, t7, t8;
      t1 << *(vars[0]) << *(vars[1]);
      t2 << *(vars[1]) << *(vars[2]);
      t3 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      t4 << *(vars[4]) << *(vars[7]);
      t5 << *(vars[5]) << *(vars[6]);
      t6 << *(vars[8]) << *(vars[9]);
      t7 << *(vars[8]);
      t8 << *(vars[2]);

      randomInitP(t1);
      randomInitP(t2);
      randomInitP(t3);
      randomInitP(t4);
      randomInitP(t5);
      randomInitP(t6);
      randomInitP(t7);
      randomInitP(t8);

      gum::MultiDimCombineAndProjectDefault< gum::Potential< double > > projcomb(multPot, mySum);

      gum::Set< const gum::Potential< double >* > to_comb;
      to_comb << &t1 << &t2 << &t3 << &t4 << &t5 << &t6 << &t7 << &t8;
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars << vars[1] << vars[4] << vars[5] << vars[6] << vars[9] << vars[10];

      gum::ScheduleMultiDim< gum::Potential< double > > xt1(t1, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt2(t2, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt3(t3, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt4(t4, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt5(t5, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt6(t6, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt7(t7, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt8(t8, false);
      gum::Set< const gum::IScheduleMultiDim* >
         sched_to_comb{&xt1, &xt2, &xt3, &xt4, &xt5, &xt6, &xt7, &xt8};

      gum::Schedule schedule;
      schedule.insertScheduleMultiDim(xt1);
      schedule.insertScheduleMultiDim(xt2);
      schedule.insertScheduleMultiDim(xt3);
      schedule.insertScheduleMultiDim(xt4);
      schedule.insertScheduleMultiDim(xt5);
      schedule.insertScheduleMultiDim(xt6);
      schedule.insertScheduleMultiDim(xt7);
      schedule.insertScheduleMultiDim(xt8);

      auto                       xxres = projcomb.schedule(schedule, sched_to_comb, del_vars);
      gum::NodeSet               available_operations = schedule.availableOperations();
      std::vector< gum::NodeId > available_nodes;
      gum::NodeSet               results{1, 2, 3, 4, 5, 6, 7, 8};
      if (!available_operations.empty()) {
        do {
          gum::NodeId            node = *available_operations.begin();
          gum::ScheduleOperator& op
             = const_cast< gum::ScheduleOperator& >(schedule.operation(node));
          op.execute();

          for (const auto res: op.results())
            results.insert(res->id());
          for (const auto arg: op.args())
            results.erase(arg->id());

          schedule.updateAfterExecution(op, available_nodes, false);
          available_operations = schedule.availableOperations();
        } while (!available_operations.empty());
      }

      TS_ASSERT(results.size() == xxres.size())
      int nb = 0;
      for (const auto table: xxres) {
        TS_ASSERT(results.exists(table->id()))
        TS_ASSERT(!table->isAbstract())
        const auto& xtable
           = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >& >(*table);
        const gum::Potential< double >* pot = &xtable.multiDim();
        if ((pot == &t7) || (pot == &t8)) nb++;
      }
      TS_ASSERT(nb == 2)

      gum::Set< const gum::Potential< double >* > result_tables;
      for (const auto node: results) {
        const auto table = static_cast< const gum::ScheduleMultiDim< gum::Potential< double > >* >(
           schedule.scheduleMultiDim(node));
        TS_ASSERT(!table->isAbstract())
        result_tables.insert(&table->multiDim());
      }

      gum::MultiDimCombinationDefault< gum::Potential< double > > fast_comb(multPot);
      gum::Potential< double >* all_res = fast_comb.execute(result_tables);

      gum::Potential< double >*                           big_table = fast_comb.execute(to_comb);
      gum::MultiDimProjection< gum::Potential< double > > fast_proj(mySum);
      gum::Potential< double >* base_res = fast_proj.execute(*big_table, del_vars);

      TS_ASSERT(*all_res == *base_res)

      delete all_res;
      delete big_table;
      delete base_res;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void testOpPersistence() {
      gum::IScheduleMultiDim::resetIdGenerator();
      std::vector< gum::LabelizedVariable* > vars(11);

      for (gum::Idx i = 0; i < 11; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential< double > t1, t2, t3, t4, t5, t6;
      t1 << *(vars[0]) << *(vars[1]);
      t2 << *(vars[1]) << *(vars[2]);
      t3 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      t4 << *(vars[4]) << *(vars[7]);
      t5 << *(vars[5]) << *(vars[6]);
      t6 << *(vars[8]) << *(vars[9]);

      randomInitP(t1);
      randomInitP(t2);
      randomInitP(t3);
      randomInitP(t4);
      randomInitP(t5);
      randomInitP(t6);

      gum::MultiDimCombineAndProjectDefault< gum::Potential< double > > projcomb(multPot, mySum);

      gum::Set< const gum::Potential< double >* > to_comb;
      to_comb << &t1 << &t2 << &t3 << &t4 << &t5 << &t6;
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars << vars[1] << vars[4] << vars[5] << vars[6] << vars[9] << vars[10];

      gum::MultiDimCombinationDefault< gum::Potential< double > > comb(multPot);
      gum::MultiDimProjection< gum::Potential< double > >         proj(mySum);

      gum::IScheduleMultiDim::resetIdGenerator();
      gum::ScheduleMultiDim< gum::Potential< double > > xt1(t1, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt2(t2, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt3(t3, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt4(t4, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt5(t5, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt6(t6, false);
      gum::Set< const gum::IScheduleMultiDim* > sched_to_comb{&xt1, &xt2, &xt3, &xt4, &xt5, &xt6};

      auto ops_plus_res = projcomb.operations(sched_to_comb, del_vars, true);
      for (auto op: ops_plus_res.first) {
        const_cast< gum::ScheduleOperator* >(op)->execute();
      }

      gum::Potential< double > result1;
      if (ops_plus_res.second.size() > 1) {
        auto comb_ops_plus_res = comb.operations(ops_plus_res.second);
        for (auto op: comb_ops_plus_res.first) {
          const_cast< gum::ScheduleOperator* >(op)->execute();
        }
        result1 = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >* >(
                     comb_ops_plus_res.second)
                     ->multiDim();
        for (auto op: comb_ops_plus_res.first)
          delete op;
      } else {
        result1 = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >* >(
                     *(ops_plus_res.second.begin()))
                     ->multiDim();
      }

      gum::Potential< double >* result2a = comb.execute(to_comb);
      gum::Potential< double >* result2  = proj.execute(*result2a, del_vars);
      TS_ASSERT(result1 == *result2);

      for (const auto pot: ops_plus_res.second) {
        if (!sched_to_comb.exists(pot)) delete pot;
      }

      delete result2;
      delete result2a;

      for (auto op: ops_plus_res.first)
        delete op;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void testSchedulesPersistence() {
      gum::IScheduleMultiDim::resetIdGenerator();
      std::vector< gum::LabelizedVariable* > vars(11);

      for (gum::Idx i = 0; i < 11; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential< double > t1, t2, t3, t4, t5, t6, t7, t8;
      t1 << *(vars[0]) << *(vars[1]);
      t2 << *(vars[1]) << *(vars[2]);
      t3 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      t4 << *(vars[4]) << *(vars[7]);
      t5 << *(vars[5]) << *(vars[6]);
      t6 << *(vars[8]) << *(vars[9]);
      t7 << *(vars[8]);
      t8 << *(vars[2]);

      randomInitP(t1);
      randomInitP(t2);
      randomInitP(t3);
      randomInitP(t4);
      randomInitP(t5);
      randomInitP(t6);
      randomInitP(t7);
      randomInitP(t8);

      gum::MultiDimCombineAndProjectDefault< gum::Potential< double > > projcomb(multPot, mySum);

      gum::Set< const gum::Potential< double >* > to_comb;
      to_comb << &t1 << &t2 << &t3 << &t4 << &t5 << &t6 << &t7 << &t8;
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars << vars[1] << vars[4] << vars[5] << vars[6] << vars[9] << vars[10];

      gum::ScheduleMultiDim< gum::Potential< double > > xt1(t1, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt2(t2, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt3(t3, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt4(t4, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt5(t5, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt6(t6, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt7(t7, false);
      gum::ScheduleMultiDim< gum::Potential< double > > xt8(t8, false);
      gum::Set< const gum::IScheduleMultiDim* >
         sched_to_comb{&xt1, &xt2, &xt3, &xt4, &xt5, &xt6, &xt7, &xt8};

      gum::Schedule schedule;
      auto          xxt1 = schedule.insertScheduleMultiDim(xt1);
      auto          xxt2 = schedule.insertScheduleMultiDim(xt2);
      auto          xxt3 = schedule.insertScheduleMultiDim(xt3);
      auto          xxt4 = schedule.insertScheduleMultiDim(xt4);
      auto          xxt5 = schedule.insertScheduleMultiDim(xt5);
      auto          xxt6 = schedule.insertScheduleMultiDim(xt6);
      auto          xxt7 = schedule.insertScheduleMultiDim(xt7);
      auto          xxt8 = schedule.insertScheduleMultiDim(xt8);
      gum::Set< const gum::IScheduleMultiDim* >
         xxsched_to_comb{xxt1, xxt2, xxt3, xxt4, xxt5, xxt6, xxt7, xxt8};

      auto                       xxres = projcomb.schedule(schedule, sched_to_comb, del_vars, true);
      gum::NodeSet               available_operations = schedule.availableOperations();
      std::vector< gum::NodeId > available_nodes;
      gum::NodeSet               results{1, 2, 3, 4, 5, 6, 7, 8};
      if (!available_operations.empty()) {
        do {
          gum::NodeId            node = *available_operations.begin();
          gum::ScheduleOperator& op
             = const_cast< gum::ScheduleOperator& >(schedule.operation(node));
          op.execute();

          for (const auto res: op.results())
            results.insert(res->id());
          for (const auto arg: op.args())
            results.erase(arg->id());

          schedule.updateAfterExecution(op, available_nodes, false);
          available_operations = schedule.availableOperations();
        } while (!available_operations.empty());
      }

      TS_ASSERT(results.size() == xxres.size())
      int nb = 0;
      for (const auto table: xxres) {
        TS_ASSERT(results.exists(table->id()))
        TS_ASSERT(!table->isAbstract())
        const auto& xtable
           = dynamic_cast< const gum::ScheduleMultiDim< gum::Potential< double > >& >(*table);
        const gum::Potential< double >* pot = &xtable.multiDim();
        if ((pot == &t7) || (pot == &t8)) nb++;
      }
      TS_ASSERT(nb == 2)

      gum::Set< const gum::Potential< double >* > result_tables;
      for (const auto node: results) {
        const auto table = static_cast< const gum::ScheduleMultiDim< gum::Potential< double > >* >(
           schedule.scheduleMultiDim(node));
        TS_ASSERT(!table->isAbstract())
        result_tables.insert(&table->multiDim());
      }

      gum::MultiDimCombinationDefault< gum::Potential< double > > fast_comb(multPot);
      gum::Potential< double >* all_res = fast_comb.execute(result_tables);

      gum::Potential< double >*                           big_table = fast_comb.execute(to_comb);
      gum::MultiDimProjection< gum::Potential< double > > fast_proj(mySum);
      gum::Potential< double >* base_res = fast_proj.execute(*big_table, del_vars);

      TS_ASSERT(*all_res == *base_res)

      for (const auto pot: xxres) {
        if (!xxsched_to_comb.exists(pot)) delete pot;
      }

      delete all_res;
      delete big_table;
      delete base_res;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void testFloat() {
      std::vector< gum::LabelizedVariable* > vars(11);

      for (gum::Idx i = 0; i < 11; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential< float > t1, t2, t3, t4, t5, t6;
      t1 << *(vars[0]) << *(vars[1]);
      t2 << *(vars[1]) << *(vars[2]);
      t3 << *(vars[3]) << *(vars[4]) << *(vars[5]);
      t4 << *(vars[4]) << *(vars[7]);
      t5 << *(vars[5]) << *(vars[6]);
      t6 << *(vars[8]) << *(vars[9]);

      randomInitP(t1);
      randomInitP(t2);
      randomInitP(t3);
      randomInitP(t4);
      randomInitP(t5);
      randomInitP(t6);

      gum::MultiDimCombineAndProjectDefault< gum::Potential< float > > projcomb(multPot, mySum);

      gum::Set< const gum::Potential< float >* > to_comb;
      to_comb << &t1 << &t2 << &t3 << &t4 << &t5 << &t6;
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars << vars[1] << vars[4] << vars[5] << vars[6] << vars[9] << vars[10];

      gum::Set< const gum::Potential< float >* > res = projcomb.execute(to_comb, del_vars);

      auto nb_ops = projcomb.nbOperations(to_comb, del_vars);

      auto yyy = projcomb.memoryUsage(to_comb, del_vars);

      TS_ASSERT_EQUALS(yyy.first, 116 * sizeof(float) + 5 * sizeof(gum::Potential< float >))
      TS_ASSERT_EQUALS(yyy.second, 36 * sizeof(float) + 3 * sizeof(gum::Potential< float >))

      TS_ASSERT_EQUALS(nb_ops, 421)
      TS_ASSERT_EQUALS(res.size(), (gum::Size)3)

      gum::Set< const gum::Potential< float >* >::const_iterator iter = res.begin();
      const gum::Potential< float >*                             res1 = *iter;
      ++iter;
      const gum::Potential< float >* res2 = *iter;
      ++iter;
      const gum::Potential< float >* res3 = *iter;

      gum::MultiDimCombinationDefault< gum::Potential< float > > comb(multPot);
      gum::MultiDimProjection< gum::Potential< float > >         proj(mySum);

      gum::Set< const gum::Potential< float >* > comb_set;
      comb_set << &t1 << &t2;
      gum::Set< const gum::DiscreteVariable* > del_vars2;
      del_vars2 << vars[1];
      gum::Potential< float >* tt1 = comb.execute(comb_set);
      gum::Potential< float >* tt2 = proj.execute(*tt1, del_vars2);
      delete tt1;
      TS_ASSERT((*tt2 == *res1) || (*tt2 == *res2) || (*tt2 == *res3))

      delete tt2;
      comb_set.clear();
      del_vars2.clear();
      comb_set << &t3 << &t4 << &t5;
      del_vars2 << vars[4] << vars[5] << vars[6];
      tt1 = comb.execute(comb_set);
      tt2 = proj.execute(*tt1, del_vars2);
      delete tt1;

      TS_ASSERT((*tt2 == *res1) || (*tt2 == *res2) || (*tt2 == *res3))

      delete tt2;
      del_vars2.clear();
      del_vars2 << vars[9] << vars[10];
      tt2 = proj.execute(t6, del_vars2);
      TS_ASSERT((*tt2 == *res1) || (*tt2 == *res2) || (*tt2 == *res3))
      delete tt2;

      gum::MultiDimCombineAndProjectDefault< gum::Potential< float > > projcomb2 = projcomb;
      projcomb2.setCombinationFunction(addPot);
      projcomb2.setProjectionFunction(myMax);

      gum::MultiDimCombineAndProjectDefault< gum::Potential< float > >* projcomb3
         = projcomb.clone();

      delete projcomb3;

      for (const auto pot: res)
        delete pot;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }


    void testConstants() {
      gum::MultiDimCombineAndProjectDefault< gum::Potential< float > > projcomb(multPot, mySum);

      gum::Potential< float > t1, t2;
      gum::Instantiation      inst1(t1), inst2(t2);
      t1.set(inst1, 3.0);
      t2.set(inst2, 4.0);

      gum::Set< const gum::Potential< float >* > to_comb;
      to_comb << &t1 << &t2;
      gum::Set< const gum::DiscreteVariable* > del_vars;

      {
        gum::Set< const gum::Potential< float >* > res = projcomb.execute(to_comb, del_vars);
        TS_ASSERT(res.size() == 2);
        auto t3   = t1 * t2;
        auto iter = res.begin();
        auto t4   = *iter;
        ++iter;
        auto t5 = *iter;
        TS_ASSERT_EQUALS(t3, *t4 * (*t5))
        gum::Instantiation inst(t3);
        TS_ASSERT(t3.get(inst) == 12.0)

        for (const auto pot: res)
          delete pot;
      }

      std::vector< gum::LabelizedVariable* > vars(3);

      for (gum::Idx i = 0; i < 3; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      t1 << *(vars[0]);
      randomInitP(t1);
      t1.normalize();
      for (gum::Instantiation xinst1(t1); !xinst1.end(); ++xinst1)
        t1.set(xinst1, t1[xinst1] * 2);

      gum::Potential< float > t3;
      gum::Instantiation      I2(t2), I3(t3);
      t2.set(I2, 3.0);
      t3.set(I3, 5.0);

      to_comb << &t3;
      del_vars << vars[0];

      {
        gum::Set< const gum::Potential< float >* > res = projcomb.execute(to_comb, del_vars);
        TS_ASSERT_EQUALS(res.size(), (gum::Size)3)

        int   nb_empty = 0;
        float prod     = 1;
        for (const auto ptrPot: res) {
          if (ptrPot->nbrDim() == 0) {
            gum::Instantiation I(*ptrPot);
            prod *= (*ptrPot)[I];
            nb_empty++;
          }
          delete ptrPot;
        }

        TS_ASSERT_EQUALS(nb_empty, 3)
        TS_ASSERT(prod > 0.0)   // wtf ?
      }

      del_vars << vars[2];
      {
        gum::Set< const gum::Potential< float >* > res = projcomb.execute(to_comb, del_vars);
        TS_ASSERT_EQUALS(res.size(), (gum::Size)3)

        int   nb_empty = 0;
        float prod     = 1;
        for (const auto ptrPot: res) {
          if (ptrPot->nbrDim() == 0) {
            gum::Instantiation I(*ptrPot);
            prod *= (*ptrPot)[I];
            nb_empty++;
          }
          delete ptrPot;
        }

        TS_ASSERT_EQUALS(nb_empty, 3)
        TS_ASSERT(prod = 30.0)
      }


      t1 << *(vars[1]) << *(vars[2]);
      randomInitP(t1);
      t1.normalize();
      for (gum::Instantiation xinst1(t1); !xinst1.end(); ++xinst1)
        t1.set(xinst1, t1[xinst1] * 4);

      del_vars << vars[1];
      {
        gum::Set< const gum::Potential< float >* > res = projcomb.execute(to_comb, del_vars);

        TS_ASSERT_EQUALS(res.size(), (gum::Size)3)

        int   nb_empty = 0;
        float prod     = 1;
        for (const auto ptrPot: res) {
          if (ptrPot->nbrDim() == 0) {
            gum::Instantiation I(*ptrPot);
            prod *= (*ptrPot)[I];
            nb_empty++;
          }
          delete ptrPot;
        }

        TS_ASSERT_EQUALS(nb_empty, 3)
        TS_ASSERT_DELTA(prod, 60.0, 1e-3)
      }

      t2 << *(vars[1]) << *(vars[2]);
      randomInitP(t2);
      t2.normalize();
      for (gum::Instantiation xinst2(t2); !xinst2.end(); ++xinst2)
        t2.set(xinst2, t2[xinst2] * 6);

      {
        gum::Set< const gum::Potential< float >* > res = projcomb.execute(to_comb, del_vars);

        TS_ASSERT_EQUALS(res.size(), (gum::Size)2)

        auto t4 = t1 * t2;

        int nb_empty = 0;
        for (const auto ptrPot: res) {
          if (ptrPot->nbrDim() == 0) {
            gum::Instantiation I(*ptrPot);
            nb_empty++;
          } else {
            TS_ASSERT_EQUALS(t4, *ptrPot)
          }
          delete ptrPot;
        }
        TS_ASSERT_EQUALS(nb_empty, 2)
      }

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }


    private:
    // =========================================================================
    /// initialize randomly a table
    // =========================================================================
    void randomInitP(gum::Potential< double >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, 1.0f + rand() * 50.0f / float(RAND_MAX));
    }

    // the function used to combine two tables
    static gum::Potential< double > addPot(const gum::Potential< double >& t1,
                                           const gum::Potential< double >& t2) {
      return t1 + t2;
    }

    // the function used to combine two tables
    static gum::Potential< double > multPot(const gum::Potential< double >& t1,
                                            const gum::Potential< double >& t2) {
      return t1 * t2;
    }

    static gum::Potential< double >
       mySum(const gum::Potential< double >&                 table,
             const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return table.margSumOut(del_vars);
    }

    static gum::Potential< double >
       myMax(const gum::Potential< double >&                 table,
             const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return table.margMaxOut(del_vars);
    }

    void randomInitP(gum::Potential< float >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, float(1.0f + rand() * 50.0f / float(RAND_MAX)));
    }

    // the function used to combine two tables
    static gum::Potential< float > addPot(const gum::Potential< float >& t1,
                                          const gum::Potential< float >& t2) {
      return t1 + t2;
    }

    // the function used to combine two tables
    static gum::Potential< float > multPot(const gum::Potential< float >& t1,
                                           const gum::Potential< float >& t2) {
      return t1 * t2;
    }

    static gum::Potential< float > mySum(const gum::Potential< float >&                  table,
                                         const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return table.margSumOut(del_vars);
    }

    static gum::Potential< float > myMax(const gum::Potential< float >&                  table,
                                         const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return table.margMaxOut(del_vars);
    }
  };

} /* namespace gum_tests */
