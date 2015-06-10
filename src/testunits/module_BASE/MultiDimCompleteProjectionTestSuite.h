/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/set.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/operators/completeProjections4MultiDim.h>
#include <agrum/multidim/operators/multiDimCompleteProjection.h>

namespace gum_tests {

  class MultiDimCompleteProjectionTestSuite : public CxxTest::TestSuite {
    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInit(gum::MultiDimArray<float> *t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, (int)(((float)rand() / RAND_MAX) * 100000));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPointer(gum::MultiDimArray<float *> *t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, new float((int)(((float)rand() / RAND_MAX) * 100000)));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitP(gum::Potential<float> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, (int)(((float)rand() / RAND_MAX) * 100000));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPPointer(gum::Potential<float *> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, new float((int)(((float)rand() / RAND_MAX) * 100000)));
    }

    template <typename T> void pointerDelete(gum::MultiDimArray<T *> *t) {
      if (t->variablesSequence().size()) {
        gum::Instantiation i(t);

        for (i.setFirst(); !i.end(); ++i) {
          delete t->get(i);
        }
      }

      delete t;
    }

    template <typename T> void pointerDelete(gum::MultiDimImplementation<T *> *t) {
      if (t->variablesSequence().size()) {
        gum::Instantiation i(t);

        for (i.setFirst(); !i.end(); ++i) {
          delete t->get(i);
        }
      }

      delete t;
    }

    template <typename T> void pointerDelete(gum::Potential<T *> *t) {
      if (t->variablesSequence().size()) {
        gum::Instantiation i(t);

        for (i.setFirst(); !i.end(); ++i) {
          delete t->get(i);
        }
      }

      delete t;
    }

    float projmax(const gum::MultiDimArray<float> &table,
                  gum::Instantiation &instantiation) {
      gum::Instantiation inst(table);
      float result = table[inst];
      gum::Size offset = 0, i = 0;

      for (inst.setFirst(); !inst.end(); ++inst, ++i) {
        float xxx = table[inst];

        if (xxx > result) {
          result = xxx;
          offset = i;
        }
      }

      table.fromOffset(instantiation, offset);
      return result;
    }

    float projmax(const gum::Potential<float> &table,
                  gum::Instantiation &instantiation) {
      const gum::MultiDimArray<float> &impl =
          dynamic_cast<const gum::MultiDimArray<float> &>(*(table.content()));
      return projmax(impl, instantiation);
    }

    float *projmax(const gum::MultiDimArray<float *> &table,
                   gum::Instantiation &instantiation) {
      gum::Instantiation inst(table);
      float *result = table[inst];
      gum::Size offset = 0, i = 0;

      for (inst.setFirst(); !inst.end(); ++inst, ++i) {
        float *xxx = table[inst];

        if (*xxx > *result) {
          result = xxx;
          offset = i;
        }
      }

      table.fromOffset(instantiation, offset);
      return result;
    }

    float *projmax(const gum::Potential<float *> &table,
                   gum::Instantiation &instantiation) {
      const gum::MultiDimArray<float *> &impl =
          dynamic_cast<const gum::MultiDimArray<float *> &>(*(table.content()));
      return projmax(impl, instantiation);
    }

    float projsum(const gum::MultiDimArray<float> &table) {
      gum::Instantiation inst(table);
      float result = 0;

      for (inst.setFirst(); !inst.end(); ++inst)
        result += table[inst];

      return result;
    }

    bool equal(const gum::Instantiation &inst1, const gum::Instantiation &inst2) {
      const gum::Sequence<const gum::DiscreteVariable *> &vars =
          inst1.variablesSequence();

      for (const auto var : vars)
        if (inst1.val(*var) != inst2.val(*var))
          return false;

      return true;
    }

    public:
    void test_MultiDimArray() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray<float> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);

      gum::Instantiation inst(t1);
      float t2 = projectMaxMultiDimArray(&t1);
      float t3 = projmax(t1, inst);

      TS_ASSERT(t2 == t3);

      gum::Instantiation inst2(t1);
      t2 = projectMaxMultiDimArray(&t1, &inst2);
      TS_ASSERT(equal(inst2, inst));

      t2 = projectSumMultiDimArray(&t1);
      t3 = projsum(t1);
      TS_ASSERT(t2 == t3);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimImplementation() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray<float> tt1;

      tt1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
          << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&tt1);

      gum::MultiDimImplementation<float> &t1 = tt1;

      gum::Instantiation inst(t1);
      float t2 = projectMaxMultiDimArray(&t1);
      float t3 = projmax(tt1, inst);

      TS_ASSERT(t2 == t3);

      gum::Instantiation inst2(t1);
      t2 = projectMaxMultiDimImplementation(&t1, &inst2);
      TS_ASSERT(equal(inst2, inst));

      t2 = projectMaxMultiDimImplementation(&t1);
      TS_ASSERT(t2 == t3);

      gum::Instantiation inst3(t1);
      t2 = projectMaxMultiDimImplementation(&t1, &inst3);
      TS_ASSERT(equal(inst3, inst));

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimArrayPointer() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray<float *> *t1 = new gum::MultiDimArray<float *>;

      *t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
          << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(t1);

      gum::Instantiation inst(t1);
      float *t2 = projectMaxMultiDimArray4Pointers(t1);
      float *t3 = projmax(*t1, inst);

      TS_ASSERT(*t2 == *t3);
      TS_ASSERT(t2 == t3);

      gum::Instantiation inst2(t1);
      t2 = projectMaxMultiDimArray4Pointers(t1, &inst2);
      TS_ASSERT(equal(inst2, inst));
      TS_ASSERT(t2 == t3);

      pointerDelete(t1);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimImplementationPointer() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray<float *> *tt1 = new gum::MultiDimArray<float *>;

      *tt1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
           << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(tt1);
      gum::MultiDimImplementation<float *> *t1 = tt1;

      gum::Instantiation inst(*t1);
      float *t2 = projectMaxMultiDimArray4Pointers(t1);
      float *t3 = projmax(*tt1, inst);

      TS_ASSERT(*t2 == *t3);
      TS_ASSERT(t2 == t3);

      gum::Instantiation inst2(*t1);
      t2 = projectMaxMultiDimArray4Pointers(t1, &inst2);
      TS_ASSERT(equal(inst2, inst));
      TS_ASSERT(t2 == t3);

      pointerDelete(tt1);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_projections_init() {
      gum::completeProjections4MultiDimInit<float>();
      gum::completeProjections4MultiDimInit<float>();
      gum::completeProjections4MultiDimInit<float>();

      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray<float> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);

      gum::Instantiation inst(t1);
      float t2 = projectMax(t1);
      float t3 = projmax(t1, inst);
      TS_ASSERT(t2 == t3);

      gum::Instantiation inst2(t1);
      t2 = projectMax(t1, &inst2);
      TS_ASSERT(t2 == t3);
      TS_ASSERT(equal(inst2, inst));

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_potentials() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 3);
      }

      gum::Potential<float> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);

      gum::Instantiation inst(t1);
      float t2 = projectMax(t1);
      float t3 = projmax(t1, inst);
      TS_ASSERT(t2 == t3);

      gum::Instantiation inst2(t1);
      t2 = projectMax(t1, &inst2);
      TS_ASSERT(t2 == t3);
      TS_ASSERT(equal(inst2, inst));

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_Pointer_init() {
      gum::pointerCompleteProjections4MultiDimInit<float>();
      gum::pointerCompleteProjections4MultiDimInit<float>();
      gum::pointerCompleteProjections4MultiDimInit<float>();

      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray<float *> *t1 = new gum::MultiDimArray<float *>;

      *t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
          << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(t1);

      gum::Instantiation inst(t1);
      float *t2 = projectMax(*t1);
      float *t3 = projmax(*t1, inst);

      TS_ASSERT(*t2 == *t3);
      TS_ASSERT(t2 == t3);

      gum::Instantiation inst2(t1);
      t2 = projectMax(*t1, &inst2);
      TS_ASSERT(equal(inst2, inst));
      TS_ASSERT(t2 == t3);

      pointerDelete(t1);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_Pointer_potential() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential<float *> *t1 = new gum::Potential<float *>;

      *t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
          << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPPointer(*t1);

      gum::Instantiation inst(t1);
      float *t2 = projectMax(*t1);
      float *t3 = projmax(*t1, inst);

      TS_ASSERT(*t2 == *t3);
      TS_ASSERT(t2 == t3);

      gum::Instantiation inst2(t1);
      t2 = projectMax(*t1, &inst2);
      TS_ASSERT(equal(inst2, inst));
      TS_ASSERT(t2 == t3);

      pointerDelete(t1);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static float myMax(const gum::Potential<float> &table,
                       gum::Instantiation *inst) {
      return projectMax(table, inst);
    }

    void test_MultiDimCompleteProjection() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 3);
      }

      gum::Potential<float> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);
      gum::MultiDimCompleteProjection<float, gum::Potential> Proj(myMax);

      gum::Instantiation inst(t1);
      float t2 = Proj.project(t1);
      float t3 = projmax(t1, inst);

      TS_ASSERT(t2 == t3);

      gum::Instantiation inst2(t1);
      t2 = Proj.project(t1, &inst2);
      TS_ASSERT(t2 == t3);
      TS_ASSERT(equal(inst2, inst));

      Proj.setProjectFunction(myMax);
      t2 = Proj.project(t1);
      TS_ASSERT(t2 == t3);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

} /* namespace gum_tests */
