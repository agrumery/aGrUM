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


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/multidim/tensor.h>
#include <agrum/base/multidim/utils/operators/multiDimCombinationDefault.h>
#include <agrum/base/variables/labelizedVariable.h>

namespace gum_tests {

  class GUM_TEST_SUITE(MultiDimCombination) {
    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitP(gum::Tensor< double >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, rand() * 100000.0f / float(RAND_MAX));
    }   // namespace gum_tests

    // the function used to combine two tables
    static gum::Tensor< double > addTensor(const gum::Tensor< double >& t1,
                                           const gum::Tensor< double >& t2) {
      return t1 + t2;
    }

    // the function used to combine two tables
    static gum::Tensor< double > multTensor(const gum::Tensor< double >& t1,
                                            const gum::Tensor< double >& t2) {
      return t1 * t2;
    }

    public:
    GUM_ACTIVE_TEST(_op_multidimArray) {
      try {
        std::vector< gum::LabelizedVariable* > vars(10);

        for (gum::Idx i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i]       = new gum::LabelizedVariable(s, s, 4);
        }

        gum::Tensor< double > t1, t2, t3;

        t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
        t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
        t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);

        randomInitP(t1);
        randomInitP(t2);
        randomInitP(t3);

        gum::Tensor< double >*t4, *t5, *t6;
        t4 = new gum::Tensor< double >(t1 + t2);
        t5 = new gum::Tensor< double >(t3 + *t4);

        gum::Set< const gum::Tensor< double >* > set;
        set << &t1 << &t2 << &t3;

        gum::MultiDimCombinationDefault< gum::Tensor< double > > xxx(addTensor);
        t6 = xxx.execute(set);
        TS_ASSERT(t6)
        TS_ASSERT_EQUALS(*t6, *t5)

        delete t4;
        delete t5;
        delete t6;

        TS_ASSERT_EQUALS(xxx.nbOperations(set), 16641)
        auto yyy = xxx.memoryUsage(set);
        TS_ASSERT_EQUALS(yyy.first, 16640 * sizeof(double) + 2 * sizeof(gum::Tensor< double >))
        TS_ASSERT_EQUALS(yyy.second, 16384 * sizeof(double) + sizeof(gum::Tensor< double >))

        t4 = new gum::Tensor< double >(t1 * t2);
        t5 = new gum::Tensor< double >(t3 * (*t4));
        xxx.setCombinationFunction(multTensor);
        t6 = xxx.execute(set);
        TS_ASSERT(t6)
        TS_ASSERT_EQUALS(*t6, *t5)

        gum::Set< const gum::IScheduleMultiDim* >    sched_set;
        std::vector< const gum::IScheduleMultiDim* > sched_vect;
        for (const auto pot: set) {
          auto new_sched = new gum::ScheduleMultiDim< gum::Tensor< double > >(*pot, false);
          sched_set.insert(new_sched);
          sched_vect.push_back(new_sched);
        }

        const auto ops_plus_resS = xxx.operations(sched_set);
        TS_ASSERT(ops_plus_resS.first.size() == 3)
        TS_ASSERT(ops_plus_resS.second->variablesSequence().size() == 7)
        const auto ops_plus_resV = xxx.operations(sched_vect);
        TS_ASSERT(ops_plus_resV.first.size() == 3)
        TS_ASSERT(ops_plus_resV.second->variablesSequence().size() == 7)

        for (auto op: ops_plus_resS.first)
          delete op;
        for (auto op: ops_plus_resV.first)
          delete op;

        gum::Schedule scheduleS;
        for (const auto pot: sched_set)
          scheduleS.insertScheduleMultiDim(*pot);
        const auto ptrResS       = xxx.schedule(scheduleS, sched_set);
        bool       not_completed = true;
        do {
          auto avail_nodes = scheduleS.availableOperations();
          if (avail_nodes.empty()) not_completed = false;
          else {
            for (const auto node: avail_nodes) {
              auto& op = scheduleS.operation(node);
              const_cast< gum::ScheduleOperator& >(op).execute();
              std::vector< gum::NodeId > new_avail;
              scheduleS.updateAfterExecution(node, new_avail);
              break;
            }
          }
        } while (not_completed);
        TS_ASSERT(ptrResS->variablesSequence().size() == 7)

        gum::Schedule scheduleV;
        for (const auto pot: sched_set)
          scheduleV.insertScheduleMultiDim(*pot);
        const auto ptrResV = xxx.schedule(scheduleV, sched_vect);
        not_completed      = true;
        do {
          auto avail_nodes = scheduleV.availableOperations();
          if (avail_nodes.empty()) not_completed = false;
          else {
            for (const auto node: avail_nodes) {
              auto& op = scheduleV.operation(node);
              const_cast< gum::ScheduleOperator& >(op).execute();
              std::vector< gum::NodeId > new_avail;
              scheduleV.updateAfterExecution(node, new_avail);
              break;
            }
          }
        } while (not_completed);
        TS_ASSERT(ptrResV->variablesSequence().size() == 7)

        for (const auto pot: sched_set)
          delete pot;

        delete t4;
        delete t5;
        delete t6;

        for (gum::Idx i = 0; i < vars.size(); ++i)
          delete vars[i];
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }

    GUM_ACTIVE_TEST(Constants) {
      gum::Tensor< double > t1, t2;
      gum::Instantiation    inst1(t1), inst2(t2);
      t1.set(inst1, 3.0);
      t2.set(inst2, 4.0);

      gum::MultiDimCombinationDefault< gum::Tensor< double > > xxx(multTensor);
      gum::Set< const gum::Tensor< double >* >                 set;
      set << &t1 << &t2;

      gum::Tensor< double >* t3 = xxx.execute(set);
      {
        gum::Instantiation inst3(t3);
        TS_ASSERT_EQUALS((*t3)[inst3], 12.0)
      }
      delete t3;
      t3 = nullptr;


      std::vector< gum::LabelizedVariable* > vars(2);

      for (gum::Idx i = 0; i < 2; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Tensor< double > t4;
      t4 << *(vars[0]) << *(vars[1]);
      randomInitP(t4);
      t4.normalize();
      for (gum::Instantiation inst4(t4); !inst4.end(); ++inst4)
        t4.set(inst4, t4[inst4] * 2);

      set.clear();
      set << &t1 << &t4;
      t3 = xxx.execute(set);
      {
        gum::Instantiation inst3(t3), inst4(t4);
        double             x = 0;
        for (inst3.setFirst(), inst4.setFirst(); !inst3.end(); ++inst3, ++inst4) {
          x += (*t3)[inst3];
          TS_ASSERT_DELTA((*t3)[inst3], t4[inst4] * 3.0, 0.001)
        }
        TS_ASSERT_DELTA(x, 6.0, 0.001)
      }
      delete t3;
      t3 = nullptr;

      set.clear();
      set << &t4 << &t1;
      t3 = xxx.execute(set);
      {
        gum::Instantiation inst3(t3), inst4(t4);
        double             x = 0;
        for (inst3.setFirst(), inst4.setFirst(); !inst3.end(); ++inst3, ++inst4) {
          x += (*t3)[inst3];
          TS_ASSERT_DELTA((*t3)[inst3], t4[inst4] * 3.0, 0.001)
        }
        TS_ASSERT_DELTA(x, 6.0, 0.001)
      }
      delete t3;
      t3 = nullptr;


      gum::Tensor< double > t5;
      t5 << *(vars[0]);
      randomInitP(t5);
      t5.normalize();
      for (gum::Instantiation inst5(t5); !inst5.end(); ++inst5)
        t5.set(inst5, t5[inst5] * 2);

      set.clear();
      set << &t1 << &t5;
      t3 = xxx.execute(set);
      {
        gum::Instantiation inst3(t3), inst5(t5);
        double             x = 0;
        for (inst3.setFirst(), inst5.setFirst(); !inst3.end(); ++inst3, ++inst5) {
          x += (*t3)[inst3];
          TS_ASSERT_DELTA((*t3)[inst3], t5[inst5] * 3.0, 0.001)
        }
        TS_ASSERT_DELTA(x, 6.0, 0.001)
      }
      delete t3;
      t3 = nullptr;

      set.clear();
      set << &t5 << &t1;
      t3 = xxx.execute(set);
      {
        gum::Instantiation inst3(t3), inst5(t5);
        double             x = 0;
        for (inst3.setFirst(), inst5.setFirst(); !inst3.end(); ++inst3, ++inst5) {
          x += (*t3)[inst3];
          TS_ASSERT_DELTA((*t3)[inst3], t5[inst5] * 3.0, 0.001)
        }
        TS_ASSERT_DELTA(x, 6.0, 0.001)
      }
      delete t3;
      t3 = nullptr;


      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_op_persitence) {
      try {
        std::vector< gum::LabelizedVariable* > vars(10);

        for (gum::Idx i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i]       = new gum::LabelizedVariable(s, s, 4);
        }

        gum::Tensor< double > t1, t2, t3;

        t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
        t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
        t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);

        randomInitP(t1);
        randomInitP(t2);
        randomInitP(t3);

        gum::Tensor< double >*t4, *t5, *t6;
        t4 = new gum::Tensor< double >(t1 + t2);
        t5 = new gum::Tensor< double >(t3 + *t4);

        gum::Set< const gum::Tensor< double >* > set;
        set << &t1 << &t2 << &t3;

        gum::MultiDimCombinationDefault< gum::Tensor< double > > xxx(addTensor);
        t6 = xxx.execute(set);
        TS_ASSERT(t6)
        TS_ASSERT_EQUALS(*t6, *t5)

        delete t4;
        delete t5;
        delete t6;

        TS_ASSERT_EQUALS(xxx.nbOperations(set), 16641)
        auto yyy = xxx.memoryUsage(set);
        TS_ASSERT_EQUALS(yyy.first, 16640 * sizeof(double) + 2 * sizeof(gum::Tensor< double >))
        TS_ASSERT_EQUALS(yyy.second, 16384 * sizeof(double) + sizeof(gum::Tensor< double >))

        t4 = new gum::Tensor< double >(t1 * t2);
        t5 = new gum::Tensor< double >(t3 * (*t4));
        xxx.setCombinationFunction(multTensor);
        t6 = xxx.execute(set);
        TS_ASSERT(t6)
        TS_ASSERT_EQUALS(*t6, *t5)

        gum::Set< const gum::IScheduleMultiDim* >    sched_set;
        std::vector< const gum::IScheduleMultiDim* > sched_vect;
        for (const auto pot: set) {
          auto new_sched = new gum::ScheduleMultiDim< gum::Tensor< double > >(*pot, false);
          sched_set.insert(new_sched);
          sched_vect.push_back(new_sched);
        }

        const auto ops_plus_resS = xxx.operations(sched_set, true);
        TS_ASSERT(ops_plus_resS.first.size() == 3)
        TS_ASSERT(ops_plus_resS.second->variablesSequence().size() == 7)
        const auto ops_plus_resV = xxx.operations(sched_vect, true);
        TS_ASSERT(ops_plus_resV.first.size() == 3)
        TS_ASSERT(ops_plus_resV.second->variablesSequence().size() == 7)

        for (auto op: ops_plus_resS.first)
          delete op;
        for (auto op: ops_plus_resV.first)
          delete op;
        delete ops_plus_resS.second;
        delete ops_plus_resV.second;

        gum::Schedule scheduleS;
        for (const auto pot: sched_set)
          scheduleS.insertScheduleMultiDim(*pot);
        const auto ptrResS       = xxx.schedule(scheduleS, sched_set, true);
        bool       not_completed = true;
        do {
          auto avail_nodes = scheduleS.availableOperations();
          if (avail_nodes.empty()) not_completed = false;
          else {
            for (const auto node: avail_nodes) {
              auto& op = scheduleS.operation(node);
              const_cast< gum::ScheduleOperator& >(op).execute();
              std::vector< gum::NodeId > new_avail;
              scheduleS.updateAfterExecution(node, new_avail);
              break;
            }
          }
        } while (not_completed);
        TS_ASSERT(ptrResS->variablesSequence().size() == 7)

        gum::Schedule scheduleV;
        for (const auto pot: sched_set)
          scheduleV.insertScheduleMultiDim(*pot);
        const auto ptrResV = xxx.schedule(scheduleV, sched_vect, true);
        not_completed      = true;
        do {
          auto avail_nodes = scheduleV.availableOperations();
          if (avail_nodes.empty()) not_completed = false;
          else {
            for (const auto node: avail_nodes) {
              auto& op = scheduleV.operation(node);
              const_cast< gum::ScheduleOperator& >(op).execute();
              std::vector< gum::NodeId > new_avail;
              scheduleV.updateAfterExecution(node, new_avail);
              break;
            }
          }
        } while (not_completed);
        TS_ASSERT(ptrResV->variablesSequence().size() == 7)

        for (const auto pot: sched_set)
          delete pot;

        delete ptrResS;
        delete ptrResV;

        delete t4;
        delete t5;
        delete t6;

        for (gum::Idx i = 0; i < vars.size(); ++i)
          delete vars[i];
      } catch (gum::Exception& e) { GUM_SHOWERROR(e); }
    }
  };

} /* namespace gum_tests */
