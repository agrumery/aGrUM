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
#include <agrum/base/multidim/utils/operators/completeProjections4MultiDim.h>
#include <agrum/base/multidim/utils/operators/multiDimCompleteProjection.h>
#include <agrum/base/variables/labelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  MultiDimCompleteProjection
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct MultiDimCompleteProjectionTestSuite {
    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    static void randomInit(gum::MultiDimArray< double >* t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, rand() * 100000.0f / float(RAND_MAX));
    }   // namespace gum_tests

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    static void randomInitPointer(gum::MultiDimArray< double* >* t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, new double(rand() * 100000.0f / float(RAND_MAX)));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    static void randomInitP(gum::Tensor< double >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, rand() * 100000.0f / float(RAND_MAX));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    static void randomInitPPointer(gum::Tensor< double* >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, new double(rand() * 100000.0f / float(RAND_MAX)));
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

    static double local_projmax(const gum::MultiDimArray< double >& table,
                                gum::Instantiation&                 instantiation) {
      gum::Instantiation inst(table);
      double             result = table[inst];
      gum::Size          offset = 0, i = 0;

      for (inst.setFirst(); !inst.end(); ++inst, ++i) {
        double xxx = table[inst];

        if (xxx > result) {
          result = xxx;
          offset = i;
        }
      }

      table.fromOffset(instantiation, offset);
      return result;
    }

    static double local_projmax(const gum::Tensor< double >& table,
                                gum::Instantiation&          instantiation) {
      const gum::MultiDimArray< double >& impl
          = dynamic_cast< const gum::MultiDimArray< double >& >(*(table.content()));
      return local_projmax(impl, instantiation);
    }

    static double* local_projmax(const gum::MultiDimArray< double* >& table,
                                 gum::Instantiation&                  instantiation) {
      gum::Instantiation inst(table);
      double*            result = table[inst];
      gum::Size          offset = 0, i = 0;

      for (inst.setFirst(); !inst.end(); ++inst, ++i) {
        double* xxx = table[inst];

        if (*xxx > *result) {
          result = xxx;
          offset = i;
        }
      }

      table.fromOffset(instantiation, offset);
      return result;
    }

    static double* local_projmax(const gum::Tensor< double* >& table,
                                 gum::Instantiation&           instantiation) {
      const gum::MultiDimArray< double* >& impl
          = dynamic_cast< const gum::MultiDimArray< double* >& >(*(table.content()));
      return local_projmax(impl, instantiation);
    }

    static double local_projsum(const gum::MultiDimArray< double >& table) {
      gum::Instantiation inst(table);
      double             result = 0;

      for (inst.setFirst(); !inst.end(); ++inst)
        result += table[inst];

      return result;
    }

    static bool local_equal(const gum::Instantiation& inst1, const gum::Instantiation& inst2) {
      const gum::Sequence< const gum::DiscreteVariable* >& vars = inst1.variablesSequence();

      for (const auto var: vars)
        if (inst1.val(*var) != inst2.val(*var)) return false;

      return true;
    }

    public:
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

      gum::Instantiation inst(t1);
      double             t2 = projectMaxMultiDimArray(&t1);
      double             t3 = local_projmax(t1, inst);

      CHECK((t2) == (t3));

      gum::Instantiation inst2(t1);
      t2 = projectMaxMultiDimArray(&t1, &inst2);
      CHECK(local_equal(inst2, inst));

      t2 = projectSumMultiDimArray(&t1);
      t3 = local_projsum(t1);
      CHECK((t2) == (t3));

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

      gum::Instantiation inst(t1);
      double             t2 = projectMaxMultiDimArray(&t1);
      double             t3 = local_projmax(tt1, inst);

      CHECK((t2) == (t3));

      gum::Instantiation inst2(t1);
      t2 = projectMaxMultiDimImplementation(&t1, &inst2);
      CHECK(local_equal(inst2, inst));

      t2 = projectMaxMultiDimImplementation(&t1);
      CHECK((t2) == (t3));

      gum::Instantiation inst3(t1);
      t2 = projectMaxMultiDimImplementation(&t1, &inst3);
      CHECK(local_equal(inst3, inst));

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

      gum::Instantiation inst(t1);
      double*            t2 = projectMaxMultiDimArray4Pointers(t1);
      double*            t3 = local_projmax(*t1, inst);

      CHECK((*t2) == (*t3));
      CHECK((t2) == (t3));

      gum::Instantiation inst2(t1);
      t2 = projectMaxMultiDimArray4Pointers(t1, &inst2);
      CHECK(local_equal(inst2, inst));
      CHECK((t2) == (t3));

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

      gum::Instantiation inst(*t1);
      double*            t2 = projectMaxMultiDimArray4Pointers(t1);
      double*            t3 = local_projmax(*tt1, inst);

      CHECK((*t2) == (*t3));
      CHECK((t2) == (t3));

      gum::Instantiation inst2(*t1);
      t2 = projectMaxMultiDimArray4Pointers(t1, &inst2);
      CHECK(local_equal(inst2, inst));
      CHECK((t2) == (t3));

      pointerDelete(tt1);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_projections_init() {
      gum::completeProjections4MultiDimInit< double >();
      gum::completeProjections4MultiDimInit< double >();
      gum::completeProjections4MultiDimInit< double >();

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

      gum::Instantiation inst(t1);
      double             t2 = projectMax(t1);
      double             t3 = local_projmax(t1, inst);
      CHECK((t2) == (t3));

      gum::Instantiation inst2(t1);
      t2 = projectMax(t1, &inst2);
      CHECK((t2) == (t3));
      CHECK(local_equal(inst2, inst));

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

      gum::Instantiation inst(t1);
      double             t2 = t1.max();
      double             t3 = local_projmax(t1, inst);
      CHECK((t2) == (t3));

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static void test_Pointer_init() {
      gum::pointerCompleteProjections4MultiDimInit< double >();
      gum::pointerCompleteProjections4MultiDimInit< double >();
      gum::pointerCompleteProjections4MultiDimInit< double >();

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

      gum::Instantiation inst(t1);
      double*            t2 = projectMax(*t1);
      double*            t3 = local_projmax(*t1, inst);

      CHECK((*t2) == (*t3));
      CHECK((t2) == (t3));

      gum::Instantiation inst2(t1);
      t2 = projectMax(*t1, &inst2);
      CHECK(local_equal(inst2, inst));
      CHECK((t2) == (t3));

      pointerDelete(t1);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static double myMax(const gum::Tensor< double >& table, gum::Instantiation* inst) {
      return table.max();
    }

    static void test_MultiDimCompleteProjection() {
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
      gum::MultiDimCompleteProjection< double, gum::Tensor > Proj(myMax);

      gum::Instantiation inst(t1);
      double             t2 = Proj.project(t1);
      double             t3 = local_projmax(t1, inst);

      CHECK((t2) == (t3));

      gum::Instantiation inst2(t1);
      t2 = Proj.project(t1, &inst2);
      CHECK((t2) == (t3));

      Proj.setProjectFunction(myMax);
      t2 = Proj.project(t1);
      CHECK((t2) == (t3));

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

  GUM_TEST_ACTIF(_MultiDimArray)
  GUM_TEST_ACTIF(_MultiDimImplementation)
  GUM_TEST_ACTIF(_MultiDimArrayPointer)
  GUM_TEST_ACTIF(_MultiDimImplementationPointer)
  GUM_TEST_ACTIF(_projections_init)
  GUM_TEST_ACTIF(_tensors)
  GUM_TEST_ACTIF(_Pointer_init)
  GUM_TEST_ACTIF(_MultiDimCompleteProjection)

} /* namespace gum_tests */
