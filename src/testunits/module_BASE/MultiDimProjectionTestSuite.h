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
#include <agrum/core/utils_random.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/operators/projections4MultiDim.h>
#include <agrum/multidim/operators/multiDimProjection.h>

namespace gum_tests {

  class MultiDimProjectionTestSuite : public CxxTest::TestSuite {
    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInit(gum::MultiDimArray<float> *t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, (int)(gum::randomProba() * 100000));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPointer(gum::MultiDimArray<float *> *t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, new float((int)(gum::randomProba() * 100000)));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitP(gum::Potential<float> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, (int)(gum::randomProba() * 100000));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPPointer(gum::Potential<float *> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, new float((int)(gum::randomProba() * 100000)));
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

    // ==========================================================================
    // ==========================================================================
    template <typename T>
    bool equal(const gum::MultiDimImplementation<T *> &t1,
               const gum::MultiDimImplementation<T *> &t2) {
      if ((t1.nbrDim() == t2.nbrDim()) && (t1.domainSize() == t2.domainSize())) {
        for (const auto var : t1)
          if (!t2.variablesSequence().exists(var))
            return false;

        gum::Instantiation i(t1);

        for (i.setFirst(); !i.end(); ++i) {
          if (*(t1.get(i)) != *(t2.get(i))) {
            return false;
          }
        }

        return true;
      } else {
        return false;
      }
    }

    // ==========================================================================
    // ==========================================================================
    template <typename T>
    bool equal(const gum::Potential<T *> &t1, const gum::Potential<T *> &t2) {
      if ((t1.nbrDim() == t2.nbrDim()) && (t1.domainSize() == t2.domainSize())) {
        for (const auto var : t1)
          if (!t2.variablesSequence().exists(var))
            return false;

        gum::Instantiation i(t1);

        for (i.setFirst(); !i.end(); ++i) {
          if (*(t1.get(i)) != *(t2.get(i))) {
            return false;
          }
        }

        return true;
      } else {
        return false;
      }
    }

    // the function used to combine two tables
    static gum::Potential<float> *addPotential(const gum::Potential<float> &t1,
                                               const gum::Potential<float> &t2) {
      return new gum::Potential<float>(t1 + t2);
    }

    // the function used to combine two tables
    static gum::Potential<float> *multPotential(const gum::Potential<float> &t1,
                                                const gum::Potential<float> &t2) {
      return new gum::Potential<float>(t1 * t2);
    }

    // projection of a table over a set
    gum::MultiDimArray<float> *
    proj(const gum::MultiDimArray<float> &table,
         const gum::Set<const gum::DiscreteVariable *> &del_vars,
         float neutral_elt) {
      gum::MultiDimArray<float> *result = new gum::MultiDimArray<float>;
      const gum::Sequence<const gum::DiscreteVariable *> &vars =
          table.variablesSequence();
      result->beginMultipleChanges();

      for (const auto var : vars)
        if (!del_vars.exists(var))
          *result << *var;

      result->endMultipleChanges();

      result->fill(neutral_elt);

      gum::Instantiation inst(table);

      for (inst.setFirst(); !inst.end(); ++inst) {
        float xxx = result->get(inst);
        float yyy = table[inst];
        result->set(inst, xxx > yyy ? xxx : yyy);
      }

      return result;
    }

    // projection of a table over a set
    gum::MultiDimArray<float *> *
    proj(const gum::MultiDimArray<float *> &table,
         const gum::Set<const gum::DiscreteVariable *> &del_vars,
         float neutral_elt) {
      gum::MultiDimArray<float *> *result = new gum::MultiDimArray<float *>;
      const gum::Sequence<const gum::DiscreteVariable *> &vars =
          table.variablesSequence();
      result->beginMultipleChanges();

      for (const auto var : vars)
        if (!del_vars.exists(var))
          *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst1(result);

      for (inst1.setFirst(); !inst1.end(); ++inst1) {
        result->set(inst1, new float(neutral_elt));
      }

      gum::Instantiation inst2(table);

      for (inst2.setFirst(); !inst2.end(); ++inst2) {
        float *xxx = result->get(inst2);
        float *yyy = table[inst2];

        if (*xxx < *yyy)
          *xxx = *yyy;
      }

      return result;
    }

    // projection of a table over a set
    gum::Potential<float> *
    proj(const gum::Potential<float> &table,
         const gum::Set<const gum::DiscreteVariable *> &del_vars,
         float neutral_elt) {
      gum::Potential<float> *result = new gum::Potential<float>;
      const gum::Sequence<const gum::DiscreteVariable *> &vars =
          table.variablesSequence();
      result->beginMultipleChanges();

      for (const auto var : vars)
        if (!del_vars.exists(var))
          *result << *var;

      result->endMultipleChanges();

      result->fill(neutral_elt);

      gum::Instantiation inst(table);

      for (inst.setFirst(); !inst.end(); ++inst) {
        float xxx = result->get(inst);
        float yyy = table[inst];
        result->set(inst, xxx > yyy ? xxx : yyy);
      }

      return result;
    }

    // projection of a table over a set
    gum::Potential<float *> *
    proj(const gum::Potential<float *> &table,
         const gum::Set<const gum::DiscreteVariable *> &del_vars,
         float neutral_elt) {
      gum::Potential<float *> *result = new gum::Potential<float *>;
      const gum::Sequence<const gum::DiscreteVariable *> &vars =
          table.variablesSequence();
      result->beginMultipleChanges();

      for (const auto var : vars)
        if (!del_vars.exists(var))
          *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst1(result);

      for (inst1.setFirst(); !inst1.end(); ++inst1) {
        result->set(inst1, new float(neutral_elt));
      }

      gum::Instantiation inst2(table);

      for (inst2.setFirst(); !inst2.end(); ++inst2) {
        float *xxx = result->get(inst2);
        float *yyy = table[inst2];

        if (*xxx < *yyy)
          *xxx = *yyy;
      }

      return result;
    }

    public:
    void test_MultiDimArray_time() {
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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimArray<float> *t2 = projectSumMultiDimArray(&t1, del_vars);
      TS_ASSERT(t2->variablesSequence().exists(vars[2]));
      TS_ASSERT(t2->variablesSequence().exists(vars[3]));
      TS_ASSERT(t2->variablesSequence().exists(vars[6]));
      TS_ASSERT(t2->variablesSequence().exists(vars[7]));
      TS_ASSERT(t2->variablesSequence().exists(vars[4]));
      TS_ASSERT(t2->variablesSequence().exists(vars[5]));
      TS_ASSERT(t2->variablesSequence().exists(vars[8]));
      TS_ASSERT(not t2->variablesSequence().exists(vars[0]));
      TS_ASSERT(not t2->variablesSequence().exists(vars[9]));
      TS_ASSERT(not t2->variablesSequence().exists(vars[1]));
      delete t2;

      t2 = projectMinMultiDimArray(&t1, proj_set);
      delete t2;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectProductMultiDimArray(&t1, proj_set);
      delete t2;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimArray<float> *t2 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray<float> *t3 = proj(t1, del_vars, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      t2 = projectMaxMultiDimArray(&t1, proj_set);
      t3 = proj(t1, proj_set, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray(&t1, proj_set);
      delete t2;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimArrayDeb() {
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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[3]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[8]);
      proj_set.insert(vars[9]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[1]);
      del_vars.insert(vars[0]);
      del_vars.insert(vars[2]);

      gum::MultiDimArray<float> *t2 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray<float> *t3 = proj(t1, del_vars, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      t2 = projectMaxMultiDimArray(&t1, proj_set);
      t3 = proj(t1, proj_set, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray(&t1, proj_set);
      delete t2;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimArrayEnd() {
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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[0]);
      proj_set.insert(vars[1]);
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[6]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[8]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[7]);

      gum::MultiDimArray<float> *t2 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray<float> *t3 = proj(t1, del_vars, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      t2 = projectMaxMultiDimArray(&t1, proj_set);
      t3 = proj(t1, proj_set, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray(&t1, proj_set);
      delete t2;

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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation<float> *t4 =
          projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray<float> *t3 = proj(tt1, del_vars, 0.0f);
      TS_ASSERT(*t4 == *t3);

      delete t3;
      delete t4;

      t4 = projectMaxMultiDimArray(&t1, proj_set);
      t3 = proj(tt1, proj_set, 0.0f);
      TS_ASSERT(*t4 == *t3);

      delete t4;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t4 = projectMaxMultiDimArray(&t1, proj_set);
      delete t4;

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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimArray<float *> *t2 =
          projectMaxMultiDimArray4Pointers(t1, del_vars);
      gum::MultiDimArray<float *> *t3 = proj(*t1, del_vars, 0.0f);

      TS_ASSERT(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      t2 = projectMaxMultiDimArray4Pointers(t1, proj_set);
      t3 = proj(*t1, proj_set, 0.0f);
      TS_ASSERT(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray4Pointers(t1, proj_set);
      pointerDelete(t2);

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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation<float *> *t2 =
          projectMaxMultiDimArray4Pointers(t1, del_vars);
      gum::MultiDimArray<float *> *t3 = proj(*tt1, del_vars, 0.0f);

      TS_ASSERT(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      t2 = projectMaxMultiDimArray4Pointers(t1, proj_set);
      t3 = proj(*tt1, proj_set, 0.0f);
      TS_ASSERT(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMaxMultiDimArray4Pointers(t1, proj_set);
      pointerDelete(t2);

      pointerDelete(tt1);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_projections_init() {
      gum::projections4MultiDimInit<float>();
      // why 3 times ?
      // projections4MultiDimInit<float> ();
      // projections4MultiDimInit<float> ();

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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation<float> *t2 = projectMax(t1, del_vars);
      gum::MultiDimArray<float> *t3 = proj(t1, del_vars, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      t2 = projectMax(t1, proj_set);
      t3 = proj(t1, proj_set, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMax(t1, proj_set);
      delete t2;

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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::Potential<float> *t2 =
          new gum::Potential<float>(projectMax(t1, del_vars));
      gum::Potential<float> *t3 = proj(t1, del_vars, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      gum::Potential<float> *t4 =
          new gum::Potential<float>(projectMax(t1, proj_set));
      t3 = proj(t1, proj_set, 0.0f);
      TS_ASSERT(*t4 == *t3);

      delete t4;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Potential<float> *t5 =
          new gum::Potential<float>(projectMax(t1, proj_set));
      delete t5;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_Pointer_init() {
      gum::pointerProjections4MultiDimInit<float>();
      // why 3 times  ?
      // pointerProjections4MultiDimInit<float> ();
      // pointerProjections4MultiDimInit<float> ();

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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation<float *> *t2 = projectMax(*t1, del_vars);
      gum::MultiDimArray<float *> *t3 = proj(*t1, del_vars, 0.0f);

      TS_ASSERT(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      t2 = projectMax(*t1, proj_set);
      t3 = proj(*t1, proj_set, 0.0f);
      TS_ASSERT(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      t2 = projectMax(*t1, proj_set);
      pointerDelete(t2);

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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::Potential<float *> *t2 =
          new gum::Potential<float *>(projectMax(*t1, del_vars));
      gum::Potential<float *> *t3 = proj(*t1, del_vars, 0.0f);
      TS_ASSERT(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      gum::Potential<float *> *t4 =
          new gum::Potential<float *>(projectMax(*t1, proj_set));
      t3 = proj(*t1, proj_set, 0.0f);
      TS_ASSERT(equal(*t4, *t3));

      pointerDelete(t4);
      pointerDelete(t3);

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Potential<float *> *t5 =
          new gum::Potential<float *>(projectMax(*t1, proj_set));
      pointerDelete(t5);

      pointerDelete(t1);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static gum::Potential<float> *
    myMax(const gum::Potential<float> &table,
          const gum::Set<const gum::DiscreteVariable *> &del_vars) {
      return new gum::Potential<float>(projectMax(table, del_vars));
    }

    void test_MultiDimProjection() {
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
      gum::Set<const gum::DiscreteVariable *> proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);
      gum::MultiDimProjection<float, gum::Potential> Proj(myMax);

      gum::Potential<float> *t2 =
          new gum::Potential<float>(projectMax(t1, del_vars));
      gum::Potential<float> *t3 = Proj.project(t1, del_vars);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      t2 = new gum::Potential<float>(projectMax(t1, proj_set));
      t3 = Proj.project(t1, proj_set);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Potential<float> *t5 = Proj.project(t1, proj_set);
      delete t5;

      TS_ASSERT(Proj.nbOperations(t1, proj_set) == 59049);
      TS_ASSERT(Proj.nbOperations(t1.variablesSequence(), proj_set) == 59049);

      std::pair<long, long> yyy = Proj.memoryUsage(t1, del_vars);
      TS_ASSERT(yyy.first == 2187);
      yyy = Proj.memoryUsage(t1.variablesSequence(), del_vars);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

} /* namespace gum_tests */
