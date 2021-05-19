/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/testsuite_utils.h>

#include <agrum/tools/core/set.h>
#include <agrum/tools/core/utils_random.h>
#include <agrum/tools/multidim/implementations/multiDimArray.h>
#include <agrum/tools/multidim/potential.h>
#include <agrum/tools/multidim/utils/operators/multiDimProjection.h>
#include <agrum/tools/multidim/utils/operators/projections4MultiDim.h>
#include <agrum/tools/variables/labelizedVariable.h>

namespace gum_tests {

  class MultiDimProjectionTestSuite: public CxxTest::TestSuite {
    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInit(gum::MultiDimArray< double >* t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, 100000.0f * double(gum::randomProba()));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPointer(gum::MultiDimArray< double* >* t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t->set(i, new double(100000.0f * double(gum::randomProba())));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitP(gum::Potential< double >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, 100000.0f * double(gum::randomProba()));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPPointer(gum::Potential< double* >& t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, new double(100000.0f * double(gum::randomProba())));
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
    void pointerDelete(gum::Potential< T* >* t) {
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
    bool equal(const gum::Potential< T* >& t1, const gum::Potential< T* >& t2) {
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
    static gum::Potential< double >*
       addPotential(const gum::Potential< double >& t1,
                    const gum::Potential< double >& t2) {
      return new gum::Potential< double >(t1 + t2);
    }

    // the function used to combine two tables
    static gum::Potential< double >*
       multPotential(const gum::Potential< double >& t1,
                     const gum::Potential< double >& t2) {
      return new gum::Potential< double >(t1 * t2);
    }

    // projection of a table over a set
    gum::MultiDimArray< double >*
       proj(const gum::MultiDimArray< double >&             table,
            const gum::Set< const gum::DiscreteVariable* >& del_vars,
            double                                          neutral_elt) {
      gum::MultiDimArray< double >* result = new gum::MultiDimArray< double >;

      const gum::Sequence< const gum::DiscreteVariable* >& vars
         = table.variablesSequence();
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
    gum::MultiDimArray< double* >*
       proj(const gum::MultiDimArray< double* >&            table,
            const gum::Set< const gum::DiscreteVariable* >& del_vars,
            double                                          neutral_elt) {
      gum::MultiDimArray< double* >* result = new gum::MultiDimArray< double* >;
      const gum::Sequence< const gum::DiscreteVariable* >& vars
         = table.variablesSequence();
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
    gum::Potential< double >*
       proj(const gum::Potential< double >&                 table,
            const gum::Set< const gum::DiscreteVariable* >& del_vars,
            double                                          neutral_elt) {
      gum::Potential< double >* result = new gum::Potential< double >;
      const gum::Sequence< const gum::DiscreteVariable* >& vars
         = table.variablesSequence();
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
    gum::Potential< double* >*
       proj(const gum::Potential< double* >&                table,
            const gum::Set< const gum::DiscreteVariable* >& del_vars,
            double                                          neutral_elt) {
      gum::Potential< double* >* result = new gum::Potential< double* >;
      const gum::Sequence< const gum::DiscreteVariable* >& vars
         = table.variablesSequence();
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
    void test_MultiDimArray_time() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimArray< double >* t2 = projectSumMultiDimArray(&t1, del_vars);
      TS_ASSERT(t2->variablesSequence().exists(vars[2]));
      TS_ASSERT(t2->variablesSequence().exists(vars[3]));
      TS_ASSERT(t2->variablesSequence().exists(vars[6]));
      TS_ASSERT(t2->variablesSequence().exists(vars[7]));
      TS_ASSERT(t2->variablesSequence().exists(vars[4]));
      TS_ASSERT(t2->variablesSequence().exists(vars[5]));
      TS_ASSERT(t2->variablesSequence().exists(vars[8]));
      TS_ASSERT(!t2->variablesSequence().exists(vars[0]));
      TS_ASSERT(!t2->variablesSequence().exists(vars[9]));
      TS_ASSERT(!t2->variablesSequence().exists(vars[1]));
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

    void test_MultiDimArray() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimArray< double >* t2 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray< double >* t3 = proj(t1, del_vars, 0.0f);
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

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimArrayDeb() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[3]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[8]);
      proj_set.insert(vars[9]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[1]);
      del_vars.insert(vars[0]);
      del_vars.insert(vars[2]);

      gum::MultiDimArray< double >* t2 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray< double >* t3 = proj(t1, del_vars, 0.0f);
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

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimArrayEnd() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[0]);
      proj_set.insert(vars[1]);
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[6]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[8]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[7]);

      gum::MultiDimArray< double >* t2 = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray< double >* t3 = proj(t1, del_vars, 0.0f);
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

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimImplementation() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > tt1;

      tt1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
          << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&tt1);

      gum::MultiDimImplementation< double >&   t1 = tt1;
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation< double >* t4
         = projectMaxMultiDimArray(&t1, del_vars);
      gum::MultiDimArray< double >* t3 = proj(tt1, del_vars, 0.0f);
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

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimArrayPointer() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double* >* t1 = new gum::MultiDimArray< double* >;

      *t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
          << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimArray< double* >* t2
         = projectMaxMultiDimArray4Pointers(t1, del_vars);
      gum::MultiDimArray< double* >* t3 = proj(*t1, del_vars, 0.0f);

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

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_MultiDimImplementationPointer() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double* >* tt1 = new gum::MultiDimArray< double* >;

      *tt1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
           << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(tt1);
      gum::MultiDimImplementation< double* >*  t1 = tt1;
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation< double* >* t2
         = projectMaxMultiDimArray4Pointers(t1, del_vars);
      gum::MultiDimArray< double* >* t3 = proj(*tt1, del_vars, 0.0f);

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

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_projections_init() {
      gum::projections4MultiDimInit< double >();
      // why 3 times ?
      // projections4MultiDimInit<double> ();
      // projections4MultiDimInit<double> ();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInit(&t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation< double >* t2 = projectMax(t1, del_vars);
      gum::MultiDimArray< double >*          t3 = proj(t1, del_vars, 0.0f);
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

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_potentials() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::Potential< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::Potential< double >* t2
         = new gum::Potential< double >(t1.margMaxOut(del_vars));
      gum::Potential< double >* t3 = proj(t1, del_vars, 0.0f);
      TS_ASSERT(*t2 == *t3);

      delete t2;
      delete t3;

      gum::Potential< double >* t4
         = new gum::Potential< double >(t1.margMaxOut(proj_set));
      t3 = proj(t1, proj_set, 0.0f);
      TS_ASSERT(*t4 == *t3);

      delete t4;
      delete t3;

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Potential< double >* t5
         = new gum::Potential< double >(t1.margMaxOut(proj_set));
      delete t5;

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_Pointer_init() {
      gum::pointerProjections4MultiDimInit< double >();
      // why 3 times  ?
      // pointerProjections4MultiDimInit<double> ();
      // pointerProjections4MultiDimInit<double> ();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::MultiDimArray< double* >* t1 = new gum::MultiDimArray< double* >;

      *t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
          << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPointer(t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::MultiDimImplementation< double* >* t2 = projectMax(*t1, del_vars);
      gum::MultiDimArray< double* >*          t3 = proj(*t1, del_vars, 0.0f);

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

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_Pointer_potential() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential< double* >* t1 = new gum::Potential< double* >;

      *t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
          << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitPPointer(*t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);

      gum::Potential< double* >* t2
         = new gum::Potential< double* >(t1->margMaxOut(del_vars));
      gum::Potential< double* >* t3 = proj(*t1, del_vars, 0.0f);
      TS_ASSERT(equal(*t2, *t3));

      pointerDelete(t2);
      pointerDelete(t3);

      gum::Potential< double* >* t4
         = new gum::Potential< double* >(t1->margMaxOut(proj_set));
      t3 = proj(*t1, proj_set, 0.0f);
      TS_ASSERT(equal(*t4, *t3));

      pointerDelete(t4);
      pointerDelete(t3);

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Potential< double* >* t5
         = new gum::Potential< double* >(t1->margMaxOut(proj_set));
      pointerDelete(t5);

      pointerDelete(t1);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    static gum::Potential< double >*
       myMax(const gum::Potential< double >&                 table,
             const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return new gum::Potential< double >(table.margMaxOut(del_vars));
    }

    void test_MultiDimProjection() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::Potential< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);
      gum::MultiDimProjection< double, gum::Potential > Proj(myMax);

      {
        auto t2 = t1.margMaxOut(del_vars);
        auto t3 = Proj.project(t1, del_vars);
        TS_ASSERT(t2 == *t3);
        delete t3;
      }
      {
        auto t2 = t1.margMaxOut(proj_set);
        auto t3 = Proj.project(t1, proj_set);
        TS_ASSERT(t2 == *t3);
        delete (t3);
      }

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Potential< double >* t5 = Proj.project(t1, proj_set);
      delete t5;

      TS_ASSERT(Proj.nbOperations(t1, proj_set) == 59049);
      TS_ASSERT(Proj.nbOperations(t1.variablesSequence(), proj_set) == 59049);

      std::pair< long, long > yyy = Proj.memoryUsage(t1, del_vars);
      TS_ASSERT(yyy.first == 2187);
      yyy = Proj.memoryUsage(t1.variablesSequence(), del_vars);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }


    static gum::Potential< double >*
       mySum(const gum::Potential< double >&                 table,
             const gum::Set< const gum::DiscreteVariable* >& del_vars) {
      return new gum::Potential< double >(table.margSumOut(del_vars));
    }

    void test_MultiDimSumProjection() {
      std::vector< gum::LabelizedVariable* > vars(10);

      for (gum::Idx i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 3);
      }

      gum::Potential< double > t1;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]) << *(vars[3]) << *(vars[4])
         << *(vars[5]) << *(vars[6]) << *(vars[7]) << *(vars[8]) << *(vars[9]);
      randomInitP(t1);
      gum::Set< const gum::DiscreteVariable* > proj_set;
      proj_set.insert(vars[2]);
      proj_set.insert(vars[3]);
      proj_set.insert(vars[6]);
      proj_set.insert(vars[7]);
      proj_set.insert(vars[4]);
      proj_set.insert(vars[5]);
      proj_set.insert(vars[8]);
      gum::Set< const gum::DiscreteVariable* > del_vars;
      del_vars.insert(vars[0]);
      del_vars.insert(vars[9]);
      del_vars.insert(vars[1]);
      gum::MultiDimProjection< double, gum::Potential > Proj(mySum);
      {
        auto t2 = t1.margSumOut(del_vars);
        auto t3 = Proj.project(t1, del_vars);
        TS_ASSERT(t2 == *t3);
        delete t3;
      }
      {
        auto t2 = t1.margSumOut(proj_set);
        auto t3 = Proj.project(t1, proj_set);
        TS_ASSERT(t2 == *t3);
        delete (t3);
      }

      proj_set.insert(vars[0]);
      proj_set.insert(vars[9]);
      proj_set.insert(vars[1]);
      gum::Potential< double >* t5 = Proj.project(t1, proj_set);
      {
        auto t2 = t1.margSumOut(proj_set);
        TS_ASSERT(t2 == *t5);

        gum::Instantiation I5(*t5);
        TS_ASSERT(1.0 == (*t5)[I5]);
      }
      delete t5;

      TS_ASSERT(Proj.nbOperations(t1, proj_set) == 59049);
      TS_ASSERT(Proj.nbOperations(t1.variablesSequence(), proj_set) == 59049);

      std::pair< long, long > yyy = Proj.memoryUsage(t1, del_vars);
      TS_ASSERT(yyy.first == 2187);
      yyy = Proj.memoryUsage(t1.variablesSequence(), del_vars);

      for (gum::Idx i = 0; i < vars.size(); ++i)
        delete vars[i];
    }
  };

} /* namespace gum_tests */
