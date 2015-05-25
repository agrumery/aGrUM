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

#include <agrum/core/debug.h>
#include <agrum/core/set.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/partialInstantiation4MultiDim.h>
#include <agrum/multidim/multiDimPartialInstantiation.h>

namespace gum_tests {

  class MultiDimPartialInstantiationTestSuite : public CxxTest::TestSuite {
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
    void randomInitP(gum::Potential<double> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, (int)(((float)rand() / RAND_MAX) * 100000));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPPointer(gum::Potential<double *> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, new double((int)(((float)rand() / RAND_MAX) * 100000)));
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

    // ==========================================================================
    // ==========================================================================
    template <typename T>
    gum::Potential<T> *manual_instantiate(
        const gum::Potential<T> &t_in,
        const gum::HashTable<const gum::DiscreteVariable *, gum::Idx> &inst_vars) {

      // construction of the output table
      gum::Potential<T> *t_out = new gum::Potential<T>;
      gum::Instantiation partial_inst;
      const gum::Sequence<const gum::DiscreteVariable *> &tin_vars =
          t_in.variablesSequence();
      t_out->beginMultipleChanges();

      for (const auto var : tin_vars)
        if (!inst_vars.exists(var))
          t_out->add(*var);
        else
          partial_inst.add(*var);

      t_out->endMultipleChanges();

      for (const auto &elt : inst_vars)
        partial_inst.chgVal(elt.first, elt.second);

      // fill the table
      gum::Instantiation inst_in(t_in);
      gum::Instantiation inst_out(t_out);
      inst_in.setFirst();
      inst_in.setVals(partial_inst);
      inst_out.setVals(inst_in);

      while (!inst_in.end()) {
        t_out->set(inst_out, t_in[inst_in]);
        inst_in.incOut(partial_inst);
        inst_out.setVals(inst_in);
      }

      return t_out;
    }

    // ==========================================================================
    // ==========================================================================
    template <typename T>
    gum::Potential<T *> *manual_instantiate(
        const gum::Potential<T *> &t_in,
        const gum::HashTable<const gum::DiscreteVariable *, gum::Idx> &inst_vars) {

      // construction of the output table
      gum::Potential<T *> *t_out = new gum::Potential<T *>;
      gum::Instantiation partial_inst;
      const gum::Sequence<const gum::DiscreteVariable *> &tin_vars =
          t_in.variablesSequence();
      t_out->beginMultipleChanges();

      for (const auto var : tin_vars)
        if (!inst_vars.exists(var))
          t_out->add(*var);
        else
          partial_inst.add(*var);

      t_out->endMultipleChanges();

      for (const auto &elt : inst_vars)
        partial_inst.chgVal(elt.first, elt.second);

      // fill the table
      gum::Instantiation inst_in(t_in);
      gum::Instantiation inst_out(t_out);
      inst_in.setFirst();
      inst_in.setVals(partial_inst);
      inst_out.setVals(inst_in);

      while (!inst_in.end()) {
        t_out->set(inst_out, new T(*(t_in[inst_in])));
        inst_in.incOut(partial_inst);
        inst_out.setVals(inst_in);
      }

      return t_out;
    }

    // ==========================================================================
    // ==========================================================================
    template <typename T>
    gum::MultiDimArray<T> *manual_instantiate(
        const gum::MultiDimArray<T> &t_in,
        const gum::HashTable<const gum::DiscreteVariable *, gum::Idx> &inst_vars) {

      // construction of the output table
      gum::MultiDimArray<T> *t_out = new gum::MultiDimArray<T>;
      gum::Instantiation partial_inst;
      const gum::Sequence<const gum::DiscreteVariable *> &tin_vars =
          t_in.variablesSequence();
      t_out->beginMultipleChanges();

      for (const auto var : tin_vars)
        if (!inst_vars.exists(var))
          t_out->add(*var);
        else
          partial_inst.add(*var);

      t_out->endMultipleChanges();

      for (const auto &elt : inst_vars)
        partial_inst.chgVal(elt.first, elt.second);

      // fill the table
      gum::Instantiation inst_in(t_in);
      gum::Instantiation inst_out(t_out);
      inst_in.setFirst();
      inst_in.setVals(partial_inst);
      inst_out.setVals(inst_in);

      while (!inst_in.end()) {
        t_out->set(inst_out, t_in[inst_in]);
        inst_in.incOut(partial_inst);
        inst_out.setVals(inst_in);
      }

      return t_out;
    }

    // ==========================================================================
    // ==========================================================================
    template <typename T>
    gum::MultiDimArray<T *> *manual_instantiate(
        const gum::MultiDimArray<T *> &t_in,
        const gum::HashTable<const gum::DiscreteVariable *, gum::Idx> &inst_vars) {

      // construction of the output table
      gum::MultiDimArray<T *> *t_out = new gum::MultiDimArray<T *>;
      gum::Instantiation partial_inst;
      const gum::Sequence<const gum::DiscreteVariable *> &tin_vars =
          t_in.variablesSequence();
      t_out->beginMultipleChanges();

      for (const auto var : tin_vars)
        if (!inst_vars.exists(var))
          t_out->add(*var);
        else
          partial_inst.add(*var);

      t_out->endMultipleChanges();

      for (const auto &elt : inst_vars)
        partial_inst.chgVal(elt.first, elt.second);

      // fill the table
      gum::Instantiation inst_in(t_in);
      gum::Instantiation inst_out(t_out);
      inst_in.setFirst();
      inst_in.setVals(partial_inst);
      inst_out.setVals(inst_in);

      while (!inst_in.end()) {
        t_out->set(inst_out, new T(*(t_in[inst_in])));
        inst_in.incOut(partial_inst);
        inst_out.setVals(inst_in);
      }

      return t_out;
    }

    public:
    void test_MultiDimArray() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);

      gum::HashTable<const gum::DiscreteVariable *, gum::Idx> inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimArray<float> *t2 =
          partialInstantiationMultiDimArray(&t1, inst_set);

      gum::MultiDimArray<float> *t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiationMultiDimArray(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiationMultiDimArray(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimImplementation() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);

      gum::HashTable<const gum::DiscreteVariable *, gum::Idx> inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimImplementation<float> *t2 =
          partialInstantiationMultiDimImplementation(&t1, inst_set);

      gum::MultiDimArray<float> *t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiationMultiDimImplementation(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiationMultiDimImplementation(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimArrayPointer() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float *> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(&t1);

      gum::HashTable<const gum::DiscreteVariable *, gum::Idx> inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimArray<float *> *t2 =
          partialInstantiationMultiDimArray4Pointers(&t1, inst_set);

      gum::MultiDimArray<float *> *t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiationMultiDimArray4Pointers(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiationMultiDimArray4Pointers(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimImplementationPointer() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float *> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(&t1);

      gum::HashTable<const gum::DiscreteVariable *, gum::Idx> inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimImplementation<float *> *t2 =
          partialInstantiationMultiDimImplementation4Pointers(&t1, inst_set);

      gum::MultiDimArray<float *> *t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiationMultiDimImplementation4Pointers(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiationMultiDimImplementation4Pointers(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_partialInstantiation_init() {
      gum::partialInstantiation4MultiDimInit<float>();

      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);

      gum::HashTable<const gum::DiscreteVariable *, gum::Idx> inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimImplementation<float> *t2 = partialInstantiation(t1, inst_set);
      gum::MultiDimArray<float> *t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiation(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiation(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_partialInstantiationPointer_init() {
      gum::pointerPartialInstantiation4MultiDimInit<float>();

      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float *> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(&t1);

      gum::HashTable<const gum::DiscreteVariable *, gum::Idx> inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimImplementation<float *> *t2 = partialInstantiation(t1, inst_set);
      gum::MultiDimArray<float *> *t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiation(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiation(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_potential() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential<double> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);

      gum::HashTable<const gum::DiscreteVariable *, gum::Idx> inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::Potential<double> *t2 =
          new gum::Potential<double>(partialInstantiation(t1, inst_set));
      gum::Potential<double> *t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = new gum::Potential<double>(partialInstantiation(t1, inst_set));
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = new gum::Potential<double>(partialInstantiation(t1, inst_set));
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_potential_pointer() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential<double *> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPPointer(t1);

      gum::HashTable<const gum::DiscreteVariable *, gum::Idx> inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::Potential<double *> *t2 =
          new gum::Potential<double *>(partialInstantiation(t1, inst_set));
      gum::Potential<double *> *t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = new gum::Potential<double *>(partialInstantiation(t1, inst_set));
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = new gum::Potential<double *>(partialInstantiation(t1, inst_set));
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3));
      pointerDelete(t2);
      pointerDelete(t3);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_multidimPartialInstantiation() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimPartialInstantiation<double, gum::Potential> MDPI;

      gum::Potential<double> t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);

      gum::HashTable<const gum::DiscreteVariable *, gum::Idx> inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::Potential<double> *t2 = MDPI.instantiate(t1, inst_set);
      gum::Potential<double> *t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = MDPI.instantiate(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = MDPI.instantiate(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3);
      delete t2;
      delete t3;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

} /* namespace gum_tests */
