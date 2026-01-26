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


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/set.h>
#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/multidim/utils/operators/multiDimProjection.h>
#include <agrum/base/multidim/utils/operators/projections4MultiDim.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <agrum/base/core/utils_random.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MultiDimProjection
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct MultiDimProjectionTestSuite {
    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    static void randomInit(gum::MultiDimArray< double >* t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, 100000.0f * double(gum::randomProba()));
    }   // namespace gum_tests

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    static void randomInitPointer(gum::MultiDimArray< double* >* t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, new double(100000.0f * double(gum::randomProba())));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    static void randomInitP(gum::Tensor< double >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, 100000.0f * double(gum::randomProba()));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    static void randomInitPPointer(gum::Tensor< double* >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, new double(100000.0f * double(gum::randomProba())));
    }

    template < typename T >
    static void pointerDelete(gum::MultiDimArray< T* >* t) {
      if (t->variablesSequence().size()) {
        gum::Instantiation i(t);

        for (i.setFirst(); !i.end(); ++i) {
          delete t->get(i);
        }
      }

      delete t;
    }

    template < typename T >
    static void pointerDelete(gum::MultiDimImplementation< T* >* t) {
      if (t->variablesSequence().size()) {
        gum::Instantiation i(t);

        for (i.setFirst(); !i.end(); ++i) {
          delete t->get(i);
        }
      }

      delete t;
    }

    template < typename T >
    static void pointerDelete(gum::Tensor< T* >* t) {
      if (t->variablesSequence().size()) {
        gum::Instantiation i(t);

        for (i.setFirst(); !i.end(); ++i) {
          delete t->get(i);
        }
      }

      delete t;
    }

    static gum::Tensor< double > myMax(const gum::Tensor< double >& table,
                                       const gum::VariableSet&      del_vars) {
      return table.maxOut(del_vars);
    }

    static gum::Tensor< double > mySum(const gum::Tensor< double >& table,
                                       const gum::VariableSet&      del_vars) {
      return table.sumOut(del_vars);
    }

    // ==========================================================================
    // ==========================================================================
    template < typename T >
    static bool equal(const gum::MultiDimImplementation< T* >& t1,
                      const gum::MultiDimImplementation< T* >& t2) {
      if ((t1.nbrDim() == t2.nbrDim()) && (t1.domainSize() == t2.domainSize())) {
        for (const auto var: t1.variablesSequence())
          if (!t2.variablesSequence().exists(var)) return false;

        gum::Instantiation i(t1);

        for (i.setFirst(); !i.end(); ++i) {
          if (*(t1.get(i)) != *(t2.get(i))) { return false; }
        }

        return true;
      } else {
        return false;
      }
    }

    // ==========================================================================
    // ==========================================================================
    template < typename T >
    static bool equal(const gum::Tensor< T* >& t1, const gum::Tensor< T* >& t2) {
      if ((t1.nbrDim() == t2.nbrDim()) && (t1.domainSize() == t2.domainSize())) {
        for (const auto var: t1.variablesSequence())
          if (!t2.variablesSequence().exists(var)) return false;

        gum::Instantiation i(t1);

        for (i.setFirst(); !i.end(); ++i) {
          if (*(t1.get(i)) != *(t2.get(i))) { return false; }
        }

        return true;
      } else {
        return false;
      }
    }

    // the function used to combine two tables
    static gum::Tensor< double >* addTensor(const gum::Tensor< double >& t1,
                                            const gum::Tensor< double >& t2) {
      return new gum::Tensor< double >(t1 + t2);
    }

    // the function used to combine two tables
    static gum::Tensor< double >* multTensor(const gum::Tensor< double >& t1,
                                             const gum::Tensor< double >& t2) {
      return new gum::Tensor< double >(t1 * t2);
    }

    // projection of a table over a set
    static gum::MultiDimArray< double >* proj(const gum::MultiDimArray< double >& table,
                                              const gum::VariableSet&             del_vars,
                                              double                              neutral_elt) {
      gum::MultiDimArray< double >* result = new gum::MultiDimArray< double >;

      const gum::Sequence< const gum::DiscreteVariable* >& vars = table.variablesSequence();
      result->beginMultipleChanges();

      for (const auto var: vars)
        if (!del_vars.exists(var)) *result << *var;

      result->endMultipleChanges();

      result->fill(neutral_elt);

      gum::Instantiation inst(table);

      for (inst.setFirst(); !inst.end(); ++inst) {
        double xxx = result->get(inst);
        double yyy = table[inst];
        result->set(inst, xxx > yyy ? xxx : yyy);
      }

      return result;
    }

    // projection of a table over a set
    static gum::MultiDimArray< double* >* proj(const gum::MultiDimArray< double* >& table,
                                               const gum::VariableSet&              del_vars,
                                               double                               neutral_elt) {
      gum::MultiDimArray< double* >* result                     = new gum::MultiDimArray< double* >;
      const gum::Sequence< const gum::DiscreteVariable* >& vars = table.variablesSequence();
      result->beginMultipleChanges();

      for (const auto var: vars)
        if (!del_vars.exists(var)) *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst1(result);

      for (inst1.setFirst(); !inst1.end(); ++inst1) {
        result->set(inst1, new double(neutral_elt));
      }

      gum::Instantiation inst2(table);

      for (inst2.setFirst(); !inst2.end(); ++inst2) {
        double* xxx = result->get(inst2);
        double* yyy = table[inst2];

        if (*xxx < *yyy) *xxx = *yyy;
      }

      return result;
    }

    // projection of a table over a set
    static gum::Tensor< double >* proj(const gum::Tensor< double >& table,
                                       const gum::VariableSet&      del_vars,
                                       double                       neutral_elt) {
      gum::Tensor< double >*                               result = new gum::Tensor< double >;
      const gum::Sequence< const gum::DiscreteVariable* >& vars   = table.variablesSequence();
      result->beginMultipleChanges();

      for (const auto var: vars)
        if (!del_vars.exists(var)) *result << *var;

      result->endMultipleChanges();

      result->fill(neutral_elt);

      gum::Instantiation inst(table);

      for (inst.setFirst(); !inst.end(); ++inst) {
        double xxx = result->get(inst);
        double yyy = table[inst];
        result->set(inst, xxx > yyy ? xxx : yyy);
      }

      return result;
    }

    // projection of a table over a set
    static gum::Tensor< double* >* proj(const gum::Tensor< double* >& table,
                                        const gum::VariableSet&       del_vars,
                                        double                        neutral_elt) {
      gum::Tensor< double* >*                              result = new gum::Tensor< double* >;
      const gum::Sequence< const gum::DiscreteVariable* >& vars   = table.variablesSequence();
      result->beginMultipleChanges();

      for (const auto var: vars)
        if (!del_vars.exists(var)) *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst1(result);

      for (inst1.setFirst(); !inst1.end(); ++inst1) {
        result->set(inst1, new double(neutral_elt));
      }

      gum::Instantiation inst2(table);

      for (inst2.setFirst(); !inst2.end(); ++inst2) {
        double* xxx = result->get(inst2);
        double* yyy = table[inst2];

        if (*xxx < *yyy) *xxx = *yyy;
      }

      return result;
    }

    public:
    static void test_MultiDimArray_time() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimArray< double >* t2 = projectSumMultiDimArray(&t1, del_vars);
      CHECK(t2->variablesSequence().exists(vars[2]));
      CHECK(t2->variablesSequence().exists(vars[3]));
      CHECK(t2->variablesSequence().exists(vars[6]));
      CHECK(t2->variablesSequence().exists(vars[7]));
      CHECK(t2->variablesSequence().exists(vars[4]));
      CHECK(t2->variablesSequence().exists(vars[5]));
      CHECK(t2->variablesSequence().exists(vars[8]));
      CHECK(!t2->variablesSequence().exists(vars[0]));
      CHECK(!t2->variablesSequence().exists(vars[9]));
      CHECK(!t2->variablesSequence().exists(vars[1]));
      delete t2;

      t2 = projectMinMultiDimArray(&t1, proj_set);
      delete t2;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectProductMultiDimArray(&t1, proj_set);
      delete t2;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_MultiDimArray() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimArray< double >* t2 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray< double >* t3 = proj(t1, del_vars, 0.0f);
      CHECK(*t2 == *t3);

      delete t2;
      delete t3;

      t2 = projectMaxMultiDimArray(&t1, proj_set);
      t3 = proj(t1, proj_set, 0.0f);
      CHECK(*t2 == *t3);

      delete t2;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray(&t1, proj_set);
      delete t2;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_MultiDimArrayDeb() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[3]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[8]);
      proj_set.insert(vars[9]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[1]);
      del_vars.insert(vars[0]);
      del_vars.insert(vars[2]);

      gum::MultiDimArray< double >* t2 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray< double >* t3 = proj(t1, del_vars, 0.0f);
      CHECK(*t2 == *t3);

      delete t2;
      delete t3;

      t2 = projectMaxMultiDimArray(&t1, proj_set);
      t3 = proj(t1, proj_set, 0.0f);
      CHECK(*t2 == *t3);

      delete t2;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray(&t1, proj_set);
      delete t2;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_MultiDimArrayEnd() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[0]);
      proj_set.insert(vars[1]);
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[6]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[8]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[7]);

      gum::MultiDimArray< double >* t2 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray< double >* t3 = proj(t1, del_vars, 0.0f);
      CHECK(*t2 == *t3);

      delete t2;
      delete t3;

      t2 = projectMaxMultiDimArray(&t1, proj_set);
      t3 = proj(t1, proj_set, 0.0f);
      CHECK(*t2 == *t3);

      delete t2;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray(&t1, proj_set);
      delete t2;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_MultiDimImplementation() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > tt1;

      tt1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
          << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&tt1);

      gum::MultiDimImplementation< double >& t1 = tt1;
      gum::VariableSet                       proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation< double >* t4 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray< double >*          t3 = proj(tt1, del_vars, 0.0f);
      CHECK(*t4 == *t3);

      delete t3;
      delete t4;

      t4 = projectMaxMultiDimArray(&t1, proj_set);
      t3 = proj(tt1, proj_set, 0.0f);
      CHECK(*t4 == *t3);

      delete t4;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t4 = projectMaxMultiDimArray(&t1, proj_set);
      delete t4;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_MultiDimArrayPointer() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double* >* t1 = new gum::MultiDimArray< double* >;

      *t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
          << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimArray< double* >* t2 = projectMaxMultiDimArray4Pointers(t1, del_vars);
      gum::MultiDimArray< double* >* t3 = proj(*t1, del_vars, 0.0f);

      CHECK(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      t2 = projectMaxMultiDimArray4Pointers(t1, proj_set);
      t3 = proj(*t1, proj_set, 0.0f);
      CHECK(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray4Pointers(t1, proj_set);
      pointerDelete(t2);

      pointerDelete(t1);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_MultiDimImplementationPointer() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double* >* tt1 = new gum::MultiDimArray< double* >;

      *tt1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
           << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(tt1);
      gum::MultiDimImplementation< double* >* t1 = tt1;
      gum::VariableSet                        proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation< double* >* t2 = projectMaxMultiDimArray4Pointers(t1, del_vars);
      gum::MultiDimArray< double* >*          t3 = proj(*tt1, del_vars, 0.0f);

      CHECK(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      t2 = projectMaxMultiDimArray4Pointers(t1, proj_set);
      t3 = proj(*tt1, proj_set, 0.0f);
      CHECK(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray4Pointers(t1, proj_set);
      pointerDelete(t2);

      pointerDelete(tt1);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_projections_init() {
      gum::projections4MultiDimInit< double >();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation< double >* t2 = projectMax(t1, del_vars);
      gum::MultiDimArray< double >*          t3 = proj(t1, del_vars, 0.0f);
      CHECK(*t2 == *t3);

      delete t2;
      delete t3;

      t2 = projectMax(t1, proj_set);
      t3 = proj(t1, proj_set, 0.0f);
      CHECK(*t2 == *t3);

      delete t2;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMax(t1, proj_set);
      delete t2;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_tensors() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::Tensor< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::Tensor< double >* t2 = new gum::Tensor< double >(t1.maxOut(del_vars));
      gum::Tensor< double >* t3 = proj(t1, del_vars, 0.0f);
      CHECK(*t2 == *t3);

      delete t2;
      delete t3;

      gum::Tensor< double >* t4 = new gum::Tensor< double >(t1.maxOut(proj_set));
      t3                        = proj(t1, proj_set, 0.0f);
      CHECK(*t4 == *t3);

      delete t4;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Tensor< double >* t5 = new gum::Tensor< double >(t1.maxOut(proj_set));
      delete t5;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_Pointer_init() {
      gum::pointerProjections4MultiDimInit< double >();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double* >* t1 = new gum::MultiDimArray< double* >;

      *t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
          << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation< double* >* t2 = projectMax(*t1, del_vars);
      gum::MultiDimArray< double* >*          t3 = proj(*t1, del_vars, 0.0f);

      CHECK(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      t2 = projectMax(*t1, proj_set);
      t3 = proj(*t1, proj_set, 0.0f);
      CHECK(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMax(*t1, proj_set);
      pointerDelete(t2);

      pointerDelete(t1);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_Pointer_tensor() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Tensor< double* >* t1 = new gum::Tensor< double* >;

      *t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
          << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPPointer(*t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::Tensor< double* >* t2 = new gum::Tensor< double* >(t1->maxOut(del_vars));
      gum::Tensor< double* >* t3 = proj(*t1, del_vars, 0.0f);
      CHECK(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      gum::Tensor< double* >* t4 = new gum::Tensor< double* >(t1->maxOut(proj_set));
      t3                         = proj(*t1, proj_set, 0.0f);
      CHECK(equal(*t4, *t3));

      pointerDelete(t4);
      pointerDelete(t3);

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Tensor< double* >* t5 = new gum::Tensor< double* >(t1->maxOut(proj_set));
      pointerDelete(t5);

      pointerDelete(t1);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_MultiDimProjection() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::Tensor< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);
      gum::MultiDimProjection< gum::Tensor< double > > Proj(myMax);

      {
        auto t2 = t1.maxOut(del_vars);
        auto t3 = Proj.execute(t1, del_vars);
        CHECK((t2) == (*t3));
        delete t3;
      }
      {
        auto t2 = t1.maxOut(proj_set);
        auto t3 = Proj.execute(t1, proj_set);
        CHECK((t2) == (*t3));
        delete (t3);
      }
      {
        gum::Tensor< double > t3;
        auto                  t2 = t1.maxOut(proj_set);
        Proj.execute(t3, t1, proj_set);
        CHECK((t2) == (t3));
      }

      {
        gum::Schedule schedule;
        auto          xt1    = schedule.insertTable(t1, false);
        const auto    ptrRes = Proj.schedule(schedule, xt1, proj_set);

        auto       avail_nodes = schedule.availableOperations();
        const auto node        = *(avail_nodes.begin());
        auto&      op          = schedule.operation(node);
        const_cast< gum::ScheduleOperator& >(op).execute();

        auto t2 = t1.maxOut(proj_set);
        CHECK(dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >* >(ptrRes)
                  ->multiDim()
              == t2);
      }

      {
        gum::ScheduleMultiDim< gum::Tensor< double > > xt1(t1, false);
        const auto ops_plus_res = Proj.operations(&xt1, proj_set);
        ops_plus_res.first->execute();
        auto t2 = t1.maxOut(proj_set);
        CHECK(dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >* >(
                  ops_plus_res.second)
                  ->multiDim()
              == t2);
        delete ops_plus_res.first;
      }

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Tensor< double >* t5 = Proj.execute(t1, proj_set);
      delete t5;

      CHECK((Proj.nbOperations(t1, proj_set)) == (59049));
      CHECK((Proj.nbOperations(t1.variablesSequence(), proj_set)) == (59049));

      std::pair< double, double > yyy = Proj.memoryUsage(t1, del_vars);
      CHECK((yyy.first) == (2187 * sizeof(double) + sizeof(gum::Tensor< double >)));
      yyy = Proj.memoryUsage(t1.variablesSequence(), del_vars);

      gum::Tensor< double > t6;
      randomInitP(t6);
      CHECK(t6.domainSize() == 1);
      CHECK(Proj.nbOperations(t6, del_vars) == 1.0);
      CHECK(Proj.nbOperations(t6.variablesSequence(), del_vars) == 1.0);
      auto mem_usage = Proj.memoryUsage(t6.variablesSequence(), del_vars);
      CHECK(mem_usage.first == 1.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      CHECK(mem_usage.second == 1.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      gum::ScheduleMultiDim< gum::Tensor< double > > t6multi(t6, false);
      auto                                           xxx1 = Proj.operations(&t6multi, del_vars);
      delete xxx1.first;

      gum::Schedule schedule;
      schedule.insertScheduleMultiDim(t6multi);

      const auto t6sched = Proj.schedule(schedule, &t6multi, del_vars);

      gum::Set< gum::NodeId > avail = schedule.availableOperations();
      // here, there is no operation available because the projections of constants
      // are performed immediately
      CHECK(avail.size() == 0);
      const auto& xt6sched
          = dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >& >(*t6sched);
      CHECK(xt6sched.multiDim() == t6multi.multiDim());
      CHECK(schedule.dag().sizeNodes() == 0);

      gum::Tensor< double > t7;
      t7 << *(vars[2]);
      randomInitP(t7);
      CHECK(t7.domainSize() == 3);
      CHECK(Proj.nbOperations(t7, del_vars) == 3.0);
      CHECK(Proj.nbOperations(t7.variablesSequence(), del_vars) == 3.0);
      gum::ScheduleMultiDim< gum::Tensor< double > > t7multi(t7, false);
      mem_usage = Proj.memoryUsage(t7, del_vars);
      CHECK(mem_usage.first == 3.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      CHECK(mem_usage.second == 3.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      auto xxx2 = Proj.operations(&t7multi, del_vars);
      delete xxx2.first;

      schedule.insertScheduleMultiDim(t7multi);
      const auto t7sched = Proj.schedule(schedule, &t7multi, del_vars);
      CHECK(t7sched->isAbstract());
      avail = schedule.availableOperations();
      CHECK(avail.size() == 1);
      const gum::NodeId node     = *avail.begin();
      auto&             proj_ops = const_cast< gum::ScheduleOperator& >(schedule.operation(node));
      proj_ops.execute();
      std::vector< gum::NodeId > xavail;
      schedule.updateAfterExecution(node, xavail, false);
      CHECK(schedule.dag().sizeNodes() == 0);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& xt7sched
          = dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >& >(*t7sched);
      CHECK(xt7sched.multiDim() == t7);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void testConstants() {
      gum::Tensor< double > t1;
      gum::Instantiation    inst1(t1);
      t1.set(inst1, 3.0);

      gum::Tensor< double > t2;
      gum::Instantiation    inst2(t2);
      t2.set(inst2, 4.0);

      gum::MultiDimProjection< gum::Tensor< double > > proj(mySum);
      gum::VariableSet                                 del_vars;

      {
        gum::Tensor< double >* t3 = proj.execute(t1, del_vars);
        auto                   t2 = t1.sumOut(del_vars);
        CHECK((t2) == (*t3));
        CHECK((t1) == (*t3));
        gum::Instantiation inst(t3);
        CHECK(t3->get(inst) == 3.0);
        delete (t3);
      }


      std::vector< gum::LabelizedVariable* > vars(5);

      for (gum::Idx i = 0; i < 5; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      del_vars.insert(vars[0]);
      {
        gum::Tensor< double >* t3 = proj.execute(t1, del_vars);
        auto                   t2 = t1.maxOut(del_vars);
        CHECK((t2) == (*t3));
        CHECK((t1) == (*t3));
        gum::Instantiation inst(t3);
        CHECK(t3->get(inst) == 3.0);
        delete (t3);
      }

      del_vars.insert(vars[1]);
      {
        gum::Tensor< double >* t3 = proj.execute(t1, del_vars);
        auto                   t2 = t1.maxOut(del_vars);
        CHECK((t2) == (*t3));
        CHECK((t1) == (*t3));
        gum::Instantiation inst(t3);
        CHECK(t3->get(inst) == 3.0);
        delete (t3);
      }


      t1 << *(vars[0]);
      randomInitP(t1);
      t1.normalize();
      for (gum::Instantiation inst1(t1); !inst1.end(); ++inst1)
        t1.set(inst1, t1[inst1] * 2);
      del_vars.clear();
      del_vars.insert(vars[0]);
      {
        gum::Tensor< double >* t3 = proj.execute(t1, del_vars);
        auto                   t2 = t1.sumOut(del_vars);
        CHECK((t2) == (*t3));
        CHECK((t3->variablesSequence().size()) == (gum::Size(0)));
        gum::Instantiation inst3(t3);
        CHECK(((*t3)[inst3]) == doctest::Approx(2.0).epsilon(0.001));
        delete (t3);
      }

      t1 << *(vars[1]);
      CHECK(t1.variablesSequence().size() == 2);
      randomInitP(t1);
      t1.normalize();
      for (gum::Instantiation inst1(t1); !inst1.end(); ++inst1)
        t1.set(inst1, t1[inst1] * 5);
      del_vars.insert(vars[1]);
      {
        gum::Tensor< double >* t3 = proj.execute(t1, del_vars);
        auto                   t2 = t1.sumOut(del_vars);
        CHECK((t2) == (*t3));
        CHECK((t3->variablesSequence().size()) == (gum::Size(0)));
        gum::Instantiation inst3(t3);
        CHECK(((*t3)[inst3]) == doctest::Approx(5.0).epsilon(0.001));
        delete (t3);
      }

      for (gum::Idx i = 0; i < 5; ++i)
        delete (vars[i]);
    }

    static void test_persistence() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::Tensor< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);
      gum::MultiDimProjection< gum::Tensor< double > > Proj(myMax);

      {
        gum::Schedule schedule;
        auto          xt1    = schedule.insertTable(t1, false);
        const auto    ptrRes = Proj.schedule(schedule, xt1, proj_set, true);

        auto       avail_nodes = schedule.availableOperations();
        const auto node        = *(avail_nodes.begin());
        auto&      op          = schedule.operation(node);
        const_cast< gum::ScheduleOperator& >(op).execute();

        auto t2 = t1.maxOut(proj_set);
        CHECK(dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >* >(ptrRes)
                  ->multiDim()
              == t2);

        delete ptrRes;
      }

      {
        gum::ScheduleMultiDim< gum::Tensor< double > > xt1(t1, false);
        const auto ops_plus_res = Proj.operations(&xt1, proj_set, true);
        ops_plus_res.first->execute();
        auto t2 = t1.maxOut(proj_set);
        CHECK(dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >* >(
                  ops_plus_res.second)
                  ->multiDim()
              == t2);
        delete ops_plus_res.first;
        delete ops_plus_res.second;
      }

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Tensor< double >* t5 = Proj.execute(t1, proj_set);
      delete t5;

      CHECK((Proj.nbOperations(t1, proj_set)) == (59049));
      CHECK((Proj.nbOperations(t1.variablesSequence(), proj_set)) == (59049));

      std::pair< double, double > yyy = Proj.memoryUsage(t1, del_vars);
      CHECK((yyy.first) == (2187 * sizeof(double) + sizeof(gum::Tensor< double >)));
      yyy = Proj.memoryUsage(t1.variablesSequence(), del_vars);

      gum::Tensor< double > t6;
      randomInitP(t6);
      CHECK(t6.domainSize() == 1);
      CHECK(Proj.nbOperations(t6, del_vars) == 1.0);
      CHECK(Proj.nbOperations(t6.variablesSequence(), del_vars) == 1.0);
      auto mem_usage = Proj.memoryUsage(t6.variablesSequence(), del_vars);
      CHECK(mem_usage.first == 1.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      CHECK(mem_usage.second == 1.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      gum::ScheduleMultiDim< gum::Tensor< double > > t6multi(t6, false);
      auto xxx1 = Proj.operations(&t6multi, del_vars, true);
      delete xxx1.first;
      delete xxx1.second;

      gum::Schedule schedule;
      schedule.insertScheduleMultiDim(t6multi);

      const auto t6sched = Proj.schedule(schedule, &t6multi, del_vars, true);

      gum::Set< gum::NodeId > avail = schedule.availableOperations();
      // here, there is no operation available because the projections of constants
      // are performed immediately
      CHECK(avail.size() == 0);
      const auto& xt6sched
          = dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >& >(*t6sched);
      CHECK(xt6sched.multiDim() == t6multi.multiDim());
      CHECK(schedule.dag().sizeNodes() == 0);

      gum::Tensor< double > t7;
      t7 << *(vars[2]);
      randomInitP(t7);
      CHECK(t7.domainSize() == 3);
      CHECK(Proj.nbOperations(t7, del_vars) == 3.0);
      CHECK(Proj.nbOperations(t7.variablesSequence(), del_vars) == 3.0);
      gum::ScheduleMultiDim< gum::Tensor< double > > t7multi(t7, false);
      mem_usage = Proj.memoryUsage(t7, del_vars);
      CHECK(mem_usage.first == 3.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      CHECK(mem_usage.second == 3.0 * sizeof(double) + sizeof(gum::Tensor< double >));
      auto xxx2 = Proj.operations(&t7multi, del_vars, true);
      delete xxx2.first;
      delete xxx2.second;

      schedule.insertScheduleMultiDim(t7multi);
      const auto t7sched = Proj.schedule(schedule, &t7multi, del_vars, true);
      CHECK(t7sched->isAbstract());
      avail = schedule.availableOperations();
      CHECK(avail.size() == 1);
      const gum::NodeId node     = *avail.begin();
      auto&             proj_ops = const_cast< gum::ScheduleOperator& >(schedule.operation(node));
      proj_ops.execute();
      std::vector< gum::NodeId > xavail;
      schedule.updateAfterExecution(node, xavail, false);
      CHECK(schedule.dag().sizeNodes() == 0);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& xt7sched
          = dynamic_cast< const gum::ScheduleMultiDim< gum::Tensor< double > >& >(*t7sched);
      CHECK(xt7sched.multiDim() == t7);

      delete t6sched;
      delete t7sched;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_MultiDimSumProjection() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::Tensor< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);
      t1.normalize();
      gum::VariableSet proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::VariableSet del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);
      gum::MultiDimProjection< gum::Tensor< double > > Proj(mySum);
      {
        auto t2 = t1.sumOut(del_vars);
        auto t3 = Proj.execute(t1, del_vars);
        CHECK((t2) == (*t3));
        delete t3;
      }
      {
        auto t2 = t1.sumOut(proj_set);
        auto t3 = Proj.execute(t1, proj_set);
        CHECK((t2) == (*t3));
        delete t3;
      }

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Tensor< double >* t5 = Proj.execute(t1, proj_set);
      {
        auto t2 = t1.sumOut(proj_set);
        CHECK((t2) == (*t5));

        gum::Instantiation I5(*t5);
        CHECK((1.0) == doctest::Approx((*t5)[I5]).epsilon(0.0001));
      }
      delete t5;

      CHECK((Proj.nbOperations(t1, proj_set)) == (59049));
      CHECK((Proj.nbOperations(t1.variablesSequence(), proj_set)) == (59049));

      std::pair< double, double > yyy = Proj.memoryUsage(t1, del_vars);
      CHECK((yyy.first) == (2187 * sizeof(double) + sizeof(gum::Tensor< double >)));
      yyy = Proj.memoryUsage(t1.variablesSequence(), del_vars);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

  GUM_TEST_ACTIF(_MultiDimArray_time)
  GUM_TEST_ACTIF(_MultiDimArray)
  GUM_TEST_ACTIF(_MultiDimArrayDeb)
  GUM_TEST_ACTIF(_MultiDimArrayEnd)
  GUM_TEST_ACTIF(_MultiDimImplementation)
  GUM_TEST_ACTIF(_MultiDimArrayPointer)
  GUM_TEST_ACTIF(_MultiDimImplementationPointer)
  GUM_TEST_ACTIF(_projections_init)
  GUM_TEST_ACTIF(_tensors)
  GUM_TEST_ACTIF(_Pointer_init)
  GUM_TEST_ACTIF(_Pointer_tensor)
  GUM_TEST_ACTIF(_MultiDimProjection)
  GUM_TEST_ACTIF(Constants)
  GUM_TEST_ACTIF(_persistence)
  GUM_TEST_ACTIF(_MultiDimSumProjection)

} /* namespace gum_tests */
