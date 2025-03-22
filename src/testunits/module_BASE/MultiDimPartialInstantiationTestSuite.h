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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/debug.h>
#include <agrum/base/core/set.h>
#include <agrum/base/multidim/implementations/multiDimArray.h>
#include <agrum/base/multidim/implementations/multiDimPartialInstantiation.h>
#include <agrum/base/multidim/instantiation.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/multidim/utils/partialInstantiation4MultiDim.h>
#include <agrum/base/variables/labelizedVariable.h>

#include <agrum/base/core/utils_random.h>

namespace gum_tests {

  class [[maybe_unused]] MultiDimPartialInstantiationTestSuite: public CxxTest::TestSuite {
    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInit(gum::MultiDimArray< double >* t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, double(gum::randomProba()));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPointer(gum::MultiDimArray< double* >* t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, new double(double(gum::randomProba())));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitP(gum::Tensor< double >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, gum::randomProba());
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPPointer(gum::Tensor< double* >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, new double(gum::randomProba()));
    }

    template < typename T >
    void pointerDelete(gum::MultiDimArray< T* >* t) {
      if (t->variablesSequence().size()) {
        gum::Instantiation i(t);

        for (i.setFirst(); !i.end(); ++i) {
          delete t->get(i);
        }
      }

      delete t;
    }

    template < typename T >
    void pointerDelete(gum::MultiDimImplementation< T* >* t) {
      if (t->variablesSequence().size()) {
        gum::Instantiation i(t);

        for (i.setFirst(); !i.end(); ++i) {
          delete t->get(i);
        }
      }

      delete t;
    }

    template < typename T >
    void pointerDelete(gum::Tensor< T* >* t) {
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
    template < typename T >
    bool equal(const gum::MultiDimImplementation< T* >& t1,
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
    bool equal(const gum::Tensor< T* >& t1, const gum::Tensor< T* >& t2) {
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
    gum::Tensor< T >* manual_instantiate(
        const gum::Tensor< T >&                                         t_in,
        const gum::HashTable< const gum::DiscreteVariable*, gum::Idx >& inst_vars) {
      // construction of the output table
      gum::Tensor< T >*                                    t_out = new gum::Tensor< T >;
      gum::Instantiation                                   partial_inst;
      const gum::Sequence< const gum::DiscreteVariable* >& tin_vars = t_in.variablesSequence();
      t_out->beginMultipleChanges();

      for (const auto var: tin_vars)
        if (!inst_vars.exists(var)) t_out->add(*var);
        else partial_inst.add(*var);

      t_out->endMultipleChanges();

      for (const auto& elt: inst_vars)
        partial_inst.chgVal(*elt.first, elt.second);

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
    template < typename T >
    gum::Tensor< T* >* manual_instantiate(
        const gum::Tensor< T* >&                                        t_in,
        const gum::HashTable< const gum::DiscreteVariable*, gum::Idx >& inst_vars) {
      // construction of the output table
      gum::Tensor< T* >*                                   t_out = new gum::Tensor< T* >;
      gum::Instantiation                                   partial_inst;
      const gum::Sequence< const gum::DiscreteVariable* >& tin_vars = t_in.variablesSequence();
      t_out->beginMultipleChanges();

      for (const auto var: tin_vars)
        if (!inst_vars.exists(var)) t_out->add(*var);
        else partial_inst.add(*var);

      t_out->endMultipleChanges();

      for (const auto& elt: inst_vars)
        partial_inst.chgVal(*elt.first, elt.second);

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
    template < typename T >
    gum::MultiDimArray< T >* manual_instantiate(
        const gum::MultiDimArray< T >&                                  t_in,
        const gum::HashTable< const gum::DiscreteVariable*, gum::Idx >& inst_vars) {
      // construction of the output table
      gum::MultiDimArray< T >*                             t_out = new gum::MultiDimArray< T >;
      gum::Instantiation                                   partial_inst;
      const gum::Sequence< const gum::DiscreteVariable* >& tin_vars = t_in.variablesSequence();
      t_out->beginMultipleChanges();

      for (const auto var: tin_vars)
        if (!inst_vars.exists(var)) t_out->add(*var);
        else partial_inst.add(*var);

      t_out->endMultipleChanges();

      for (const auto& elt: inst_vars)
        partial_inst.chgVal(*elt.first, elt.second);

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
    template < typename T >
    gum::MultiDimArray< T* >* manual_instantiate(
        const gum::MultiDimArray< T* >&                                 t_in,
        const gum::HashTable< const gum::DiscreteVariable*, gum::Idx >& inst_vars) {
      // construction of the output table
      gum::MultiDimArray< T* >*                            t_out = new gum::MultiDimArray< T* >;
      gum::Instantiation                                   partial_inst;
      const gum::Sequence< const gum::DiscreteVariable* >& tin_vars = t_in.variablesSequence();
      t_out->beginMultipleChanges();

      for (const auto var: tin_vars)
        if (!inst_vars.exists(var)) t_out->add(*var);
        else partial_inst.add(*var);

      t_out->endMultipleChanges();

      for (const auto& elt: inst_vars)
        partial_inst.chgVal(*elt.first, elt.second);

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
    GUM_ACTIVE_TEST(_MultiDimArray) {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);

      gum::HashTable< const gum::DiscreteVariable*, gum::Idx > inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimArray< double >* t2 = partialInstantiationMultiDimArray(&t1, inst_set);

      gum::MultiDimArray< double >* t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiationMultiDimArray(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiationMultiDimArray(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_MultiDimImplementation) {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);

      gum::HashTable< const gum::DiscreteVariable*, gum::Idx > inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimImplementation< double >* t2
          = partialInstantiationMultiDimImplementation(&t1, inst_set);

      gum::MultiDimArray< double >* t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiationMultiDimImplementation(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiationMultiDimImplementation(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_MultiDimArrayPointer) {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray< double* > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(&t1);

      gum::HashTable< const gum::DiscreteVariable*, gum::Idx > inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimArray< double* >* t2 = partialInstantiationMultiDimArray4Pointers(&t1, inst_set);

      gum::MultiDimArray< double* >* t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiationMultiDimArray4Pointers(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiationMultiDimArray4Pointers(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_MultiDimImplementationPointer) {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray< double* > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(&t1);

      gum::HashTable< const gum::DiscreteVariable*, gum::Idx > inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimImplementation< double* >* t2
          = partialInstantiationMultiDimImplementation4Pointers(&t1, inst_set);

      gum::MultiDimArray< double* >* t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiationMultiDimImplementation4Pointers(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiationMultiDimImplementation4Pointers(&t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_partialInstantiation_init) {
      gum::partialInstantiation4MultiDimInit< double >();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);

      gum::HashTable< const gum::DiscreteVariable*, gum::Idx > inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimImplementation< double >* t2 = partialInstantiation(t1, inst_set);
      gum::MultiDimArray< double >*          t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiation(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiation(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_partialInstantiationPointer_init) {
      gum::pointerPartialInstantiation4MultiDimInit< double >();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray< double* > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(&t1);

      gum::HashTable< const gum::DiscreteVariable*, gum::Idx > inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::MultiDimImplementation< double* >* t2 = partialInstantiation(t1, inst_set);
      gum::MultiDimArray< double* >*          t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = partialInstantiation(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = partialInstantiation(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_tensor) {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Tensor< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);

      gum::HashTable< const gum::DiscreteVariable*, gum::Idx > inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::Tensor< double >* t2 = new gum::Tensor< double >(partialInstantiation(t1, inst_set));
      gum::Tensor< double >* t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = new gum::Tensor< double >(partialInstantiation(t1, inst_set));
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = new gum::Tensor< double >(partialInstantiation(t1, inst_set));
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_tensor_pointer) {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Tensor< double* > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPPointer(t1);

      gum::HashTable< const gum::DiscreteVariable*, gum::Idx > inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::Tensor< double* >* t2 = new gum::Tensor< double* >(partialInstantiation(t1, inst_set));
      gum::Tensor< double* >* t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = new gum::Tensor< double* >(partialInstantiation(t1, inst_set));
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = new gum::Tensor< double* >(partialInstantiation(t1, inst_set));
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(equal(*t2, *t3))
      pointerDelete(t2);
      pointerDelete(t3);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    GUM_ACTIVE_TEST(_multidimPartialInstantiation) {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimPartialInstantiation< double, gum::Tensor > MDPI;

      gum::Tensor< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4]) << *(vars[5])
         << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);

      gum::HashTable< const gum::DiscreteVariable*, gum::Idx > inst_set;
      inst_set.insert(vars[2], 0);
      inst_set.insert(vars[3], 2);
      inst_set.insert(vars[5], 1);
      inst_set.insert(vars[8], 3);
      inst_set.insert(vars[9], 2);

      gum::Tensor< double >* t2 = MDPI.instantiate(t1, inst_set);
      gum::Tensor< double >* t3 = manual_instantiate(t1, inst_set);

      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[6], 1);
      inst_set.insert(vars[7], 0);
      inst_set.insert(vars[8], 2);
      inst_set.insert(vars[9], 3);
      t2 = MDPI.instantiate(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      inst_set.clear();
      inst_set.insert(vars[0], 1);
      inst_set.insert(vars[1], 0);
      inst_set.insert(vars[2], 2);
      inst_set.insert(vars[3], 3);
      t2 = MDPI.instantiate(t1, inst_set);
      t3 = manual_instantiate(t1, inst_set);
      TS_ASSERT(*t2 == *t3)
      delete t2;
      delete t3;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

} /* namespace gum_tests */
