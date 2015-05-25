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

#include <agrum/core/utils_random.h>
#include "../../agrum/core/sequence.h"

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/potential.h>

namespace gum_tests {

  class MultiDimOperatorsTestSuite : public CxxTest::TestSuite {
    public:
    void test_op_multidimArray() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float> t1, t2, t3, *t4, *t5, *t6, *t7;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);
      randomInit(&t1);
      randomInit(&t2);
      randomInit(&t3);

      t4 = add2MultiDimArrays(&t1, &t2);
      t5 = add_test_arrays(&t1, &t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;
      t4 = add2MultiDimArrays(&t3, &t2);
      t5 = add_test_arrays(&t3, &t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      t4 = add2MultiDimArrays(&t2, &t3);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;

      t4 = subtract2MultiDimArrays(&t1, &t2);
      t5 = sub_test_arrays(&t1, &t2);
      TS_ASSERT(*t4 == *t5);
      t6 = subtract2MultiDimArrays(&t2, &t1);
      t7 = sub_test_arrays(&t2, &t1);
      TS_ASSERT(*t6 == *t7);
      TS_ASSERT(*t5 != *t7);
      TS_ASSERT(*t6 != *t4);
      delete t4;
      delete t5;
      delete t6;
      delete t7;

      t4 = multiply2MultiDimArrays(&t1, &t2);
      t5 = mult_test_arrays(&t1, &t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;
      t4 = multiply2MultiDimArrays(&t3, &t2);
      t5 = mult_test_arrays(&t3, &t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      t4 = multiply2MultiDimArrays(&t2, &t3);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;

      t4 = divide2MultiDimArrays(&t1, &t2);
      t5 = div_test_arrays(&t1, &t2);
      TS_ASSERT(*t4 == *t5);
      t6 = divide2MultiDimArrays(&t2, &t1);
      t7 = div_test_arrays(&t2, &t1);
      TS_ASSERT(*t6 == *t7);
      TS_ASSERT(*t5 != *t7);
      TS_ASSERT(*t6 != *t4);
      delete t4;
      delete t5;
      delete t6;
      delete t7;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_op_multidimImplementationOfMultiDimArray() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float> tt1, tt2, tt3;

      gum::MultiDimImplementation<float> *t4, *t5, *t6, *t7;
      tt1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      tt2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      tt3 << *(vars[6]) << *(vars[4]) << *(vars[3]);
      randomInit(&tt1);
      randomInit(&tt2);
      randomInit(&tt3);
      gum::MultiDimImplementation<float> &t1 = tt1;
      gum::MultiDimImplementation<float> &t2 = tt2;
      gum::MultiDimImplementation<float> &t3 = tt3;

      t4 = add2MultiDimArrays(&t1, &t2);
      t5 = add_test_arrays(&tt1, &tt2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;
      t4 = add2MultiDimArrays(&t3, &t2);
      t5 = add_test_arrays(&tt3, &tt2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      t4 = add2MultiDimArrays(&t2, &t3);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;

      t4 = subtract2MultiDimArrays(&t1, &t2);
      t5 = sub_test_arrays(&tt1, &tt2);
      TS_ASSERT(*t4 == *t5);
      t6 = subtract2MultiDimArrays(&t2, &t1);
      t7 = sub_test_arrays(&tt2, &tt1);
      TS_ASSERT(*t6 == *t7);
      TS_ASSERT(*t5 != *t7);
      TS_ASSERT(*t6 != *t4);
      delete t4;
      delete t5;
      delete t6;
      delete t7;

      t4 = multiply2MultiDimArrays(&t1, &t2);
      t5 = mult_test_arrays(&tt1, &tt2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;
      t4 = multiply2MultiDimArrays(&t3, &t2);
      t5 = mult_test_arrays(&tt3, &tt2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      t4 = multiply2MultiDimArrays(&t2, &t3);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;

      t4 = divide2MultiDimArrays(&t1, &t2);
      t5 = div_test_arrays(&tt1, &tt2);
      TS_ASSERT(*t4 == *t5);
      t6 = divide2MultiDimArrays(&t2, &t1);
      t7 = div_test_arrays(&tt2, &tt1);
      TS_ASSERT(*t6 == *t7);
      TS_ASSERT(*t5 != *t7);
      TS_ASSERT(*t6 != *t4);
      delete t4;
      delete t5;
      delete t6;
      delete t7;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_operators_init() {
      gum::operators4MultiDimInit<float>();
      // why 3 times ?
      // gum::operators4MultiDimInit<float> ();
      // gum::operators4MultiDimInit<float> ();

      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float> t1, t2, t3;

      gum::MultiDimImplementation<float> *t4, *t5;
      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);
      randomInit(&t1);
      randomInit(&t2);
      randomInit(&t3);

      t4 = t1 + t2;
      t5 = add_test_arrays(&t1, &t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;
      t4 = t3 + t2;
      t5 = add_test_arrays(&t3, &t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      t4 = t2 + t3;
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_potentials() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential<double> t1, t2, t3;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);

      randomInitP(t1);
      randomInitP(t2);
      randomInitP(t3);

      gum::Potential<double> *t4, *t6;
      gum::Potential<double> *t5, *t7;

      t4 = new gum::Potential<double>(t1 + t2);
      t5 = add_test_potentials(t1, t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;
      t4 = new gum::Potential<double>(t3 + t2);
      t5 = add_test_potentials(t3, t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      t4 = new gum::Potential<double>(t2 + t3);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;

      t4 = new gum::Potential<double>(t1 - t2);
      t5 = sub_test_potentials(t1, t2);
      TS_ASSERT(*t4 == *t5);
      t6 = new gum::Potential<double>(t2 - t1);
      t7 = sub_test_potentials(t2, t1);
      TS_ASSERT(*t6 == *t7);
      TS_ASSERT(*t5 != *t7);
      TS_ASSERT(*t6 != *t4);
      delete t4;
      delete t5;
      delete t6;
      delete t7;

      t4 = new gum::Potential<double>(t1 * t2);
      t5 = mult_test_potentials(t1, t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;
      t4 = new gum::Potential<double>(t3 * t2);
      t5 = mult_test_potentials(t3, t2);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      t4 = new gum::Potential<double>(t2 * t3);
      TS_ASSERT(*t4 == *t5);
      delete t4;
      delete t5;

      t4 = new gum::Potential<double>(t1 / t2);
      t5 = div_test_potentials(t1, t2);
      TS_ASSERT(*t4 == *t5);
      t6 = new gum::Potential<double>(t2 / t1);
      t7 = div_test_potentials(t2, t1);
      TS_ASSERT(*t6 == *t7);
      TS_ASSERT(*t5 != *t7);
      TS_ASSERT(*t6 != *t4);
      delete t4;
      delete t5;
      delete t6;
      delete t7;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_op_multidimArray4Pointers() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float *> t1, t2, t3, *t4, *t5, *t6, *t7;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);
      randomInitPointer(&t1);
      randomInitPointer(&t2);
      randomInitPointer(&t3);

      t4 = add2MultiDimArrays4Pointers(&t1, &t2);
      t5 = add_test_arrays4Pointers(&t1, &t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);
      t4 = add2MultiDimArrays4Pointers(&t3, &t2);
      t5 = add_test_arrays4Pointers(&t3, &t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      t4 = add2MultiDimArrays4Pointers(&t2, &t3);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);

      t4 = subtract2MultiDimArrays4Pointers(&t1, &t2);
      t5 = sub_test_arrays4Pointers(&t1, &t2);
      TS_ASSERT(equal(*t4, *t5));
      t6 = subtract2MultiDimArrays4Pointers(&t2, &t1);
      t7 = sub_test_arrays4Pointers(&t2, &t1);
      TS_ASSERT(equal(*t6, *t7));
      TS_ASSERT(!equal(*t5, *t7));
      TS_ASSERT(!equal(*t6, *t4));
      mydelete(t4);
      mydelete(t5);
      mydelete(t6);
      mydelete(t7);

      t4 = multiply2MultiDimArrays4Pointers(&t1, &t2);
      t5 = mult_test_arrays4Pointers(&t1, &t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);
      t4 = multiply2MultiDimArrays4Pointers(&t3, &t2);
      t5 = mult_test_arrays4Pointers(&t3, &t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      t4 = multiply2MultiDimArrays4Pointers(&t2, &t3);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);

      t4 = divide2MultiDimArrays4Pointers(&t1, &t2);
      t5 = div_test_arrays4Pointers(&t1, &t2);
      TS_ASSERT(equal(*t4, *t5));
      t6 = divide2MultiDimArrays4Pointers(&t2, &t1);
      t7 = div_test_arrays4Pointers(&t2, &t1);
      TS_ASSERT(equal(*t6, *t7));
      TS_ASSERT(!equal(*t5, *t7));
      TS_ASSERT(!equal(*t6, *t4));
      mydelete(t4);
      mydelete(t5);
      mydelete(t6);
      mydelete(t7);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_op_multidimImplementationOfMultiDimArray4Pointers() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float *> tt1, tt2, tt3;

      gum::MultiDimImplementation<float *> *t4, *t5, *t6, *t7;
      tt1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      tt2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      tt3 << *(vars[6]) << *(vars[4]) << *(vars[3]);
      randomInitPointer(&tt1);
      randomInitPointer(&tt2);
      randomInitPointer(&tt3);
      gum::MultiDimImplementation<float *> &t1 = tt1;
      gum::MultiDimImplementation<float *> &t2 = tt2;
      gum::MultiDimImplementation<float *> &t3 = tt3;

      t4 = add2MultiDimArrays4Pointers(&t1, &t2);
      t5 = add_test_arrays4Pointers(&tt1, &tt2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);
      t4 = add2MultiDimArrays4Pointers(&t3, &t2);
      t5 = add_test_arrays4Pointers(&tt3, &tt2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      t4 = add2MultiDimArrays4Pointers(&t2, &t3);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);

      t4 = subtract2MultiDimArrays4Pointers(&t1, &t2);
      t5 = sub_test_arrays4Pointers(&tt1, &tt2);
      TS_ASSERT(equal(*t4, *t5));
      t6 = subtract2MultiDimArrays4Pointers(&t2, &t1);
      t7 = sub_test_arrays4Pointers(&tt2, &tt1);
      TS_ASSERT(equal(*t6, *t7));
      TS_ASSERT(!equal(*t5, *t7));
      TS_ASSERT(!equal(*t6, *t4));
      mydelete(t4);
      mydelete(t5);
      mydelete(t6);
      mydelete(t7);

      t4 = multiply2MultiDimArrays4Pointers(&t1, &t2);
      t5 = mult_test_arrays4Pointers(&tt1, &tt2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);
      t4 = multiply2MultiDimArrays4Pointers(&t3, &t2);
      t5 = mult_test_arrays4Pointers(&tt3, &tt2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      t4 = multiply2MultiDimArrays4Pointers(&t2, &t3);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);

      t4 = divide2MultiDimArrays4Pointers(&t1, &t2);
      t5 = div_test_arrays4Pointers(&tt1, &tt2);
      TS_ASSERT(equal(*t4, *t5));
      t6 = divide2MultiDimArrays4Pointers(&t2, &t1);
      t7 = div_test_arrays4Pointers(&tt2, &tt1);
      TS_ASSERT(equal(*t6, *t7));
      TS_ASSERT(!equal(*t5, *t7));
      TS_ASSERT(!equal(*t6, *t4));
      mydelete(t4);
      mydelete(t5);
      mydelete(t6);
      mydelete(t7);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_operators_init4Pointers() {
      gum::pointerOperators4MultiDimInit<float>();
      // why 3 times ?
      // gum::pointerOperators4MultiDimInit<float> ();
      // gum::pointerOperators4MultiDimInit<float> ();

      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float *> t1, t2, t3;

      gum::MultiDimImplementation<float *> *t4, *t5;
      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);
      randomInitPointer(&t1);
      randomInitPointer(&t2);
      randomInitPointer(&t3);

      t4 = t1 + t2;
      t5 = add_test_arrays4Pointers(&t1, &t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);
      t4 = t3 + t2;
      t5 = add_test_arrays4Pointers(&t3, &t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      t4 = t2 + t3;
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_potentials4Pointers() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential<double *> t1, t2, t3;

      t1 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      t2 << *(vars[0]) << *(vars[1]) << *(vars[5]);
      t3 << *(vars[6]) << *(vars[4]) << *(vars[3]);

      randomInitPPointer(t1);
      randomInitPPointer(t2);
      randomInitPPointer(t3);

      gum::Potential<double *> *t4, *t6;
      gum::Potential<double *> *t5, *t7;

      t4 = new gum::Potential<double *>(t1 + t2);
      t5 = add_test_potentials4Pointers(t1, t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);
      t4 = new gum::Potential<double *>(t3 + t2);
      t5 = add_test_potentials4Pointers(t3, t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      t4 = new gum::Potential<double *>(t2 + t3);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);

      t4 = new gum::Potential<double *>(t1 - t2);
      t5 = sub_test_potentials4Pointers(t1, t2);
      TS_ASSERT(equal(*t4, *t5));
      t6 = new gum::Potential<double *>(t2 - t1);
      t7 = sub_test_potentials4Pointers(t2, t1);
      TS_ASSERT(equal(*t6, *t7));
      TS_ASSERT(!equal(*t5, *t7));
      TS_ASSERT(!equal(*t6, *t4));
      mydelete(t4);
      mydelete(t5);
      mydelete(t6);
      mydelete(t7);

      t4 = new gum::Potential<double *>(t1 * t2);
      t5 = mult_test_potentials4Pointers(t1, t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);
      t4 = new gum::Potential<double *>(t3 * t2);
      t5 = mult_test_potentials4Pointers(t3, t2);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      t4 = new gum::Potential<double *>(t2 * t3);
      TS_ASSERT(equal(*t4, *t5));
      mydelete(t4);
      mydelete(t5);

      t4 = new gum::Potential<double *>(t1 / t2);
      t5 = div_test_potentials4Pointers(t1, t2);
      TS_ASSERT(equal(*t4, *t5));
      t6 = new gum::Potential<double *>(t2 / t1);
      t7 = div_test_potentials4Pointers(t2, t1);
      TS_ASSERT(equal(*t6, *t7));
      TS_ASSERT(!equal(*t5, *t7));
      TS_ASSERT(!equal(*t6, *t4));
      mydelete(t4);
      mydelete(t5);
      mydelete(t6);
      mydelete(t7);

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_op_optimizeArrays() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float> t1, t2, t3, t4, *t5, *t6;

      t1 << *(vars[0]) << *(vars[2]) << *(vars[1]);
      t2 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      t3 << *(vars[6]) << *(vars[0]) << *(vars[2]);
      t4 << *(vars[6]) << *(vars[0]) << *(vars[2]);
      randomInit(&t1);
      randomInit(&t2);
      randomInit(&t3);
      randomInit(&t4);
      t5 = add2MultiDimArrays(&t1, &t2);
      t6 = add_test_arrays(&t1, &t2);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t3, &t4);
      t6 = add_test_arrays(&t3, &t4);
      TS_ASSERT(*t6 == *t5);
      delete t6;
      t6 = add2MultiDimArrays(&t4, &t3);
      TS_ASSERT(*t6 == *t5);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t1, &t2);
      t6 = add_test_arrays(&t1, &t2);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t3, &t2);
      t6 = add_test_arrays(&t3, &t2);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      gum::MultiDimArray<float> t7, t8, t9;
      t7 << *(vars[0]) << *(vars[2]) << *(vars[1]);
      t8 << *(vars[0]) << *(vars[2]);
      t9 << *(vars[0]);
      randomInit(&t7);
      randomInit(&t8);
      randomInit(&t9);

      t5 = add2MultiDimArrays(&t7, &t8);
      t6 = add_test_arrays(&t7, &t8);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t8, &t7);
      t6 = add_test_arrays(&t8, &t7);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      t5 = add2MultiDimArrays(&t7, &t9);
      t6 = add_test_arrays(&t7, &t9);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t9, &t7);
      t6 = add_test_arrays(&t9, &t7);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      t5 = add2MultiDimArrays(&t8, &t9);
      t6 = add_test_arrays(&t8, &t9);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t9, &t8);
      t6 = add_test_arrays(&t9, &t8);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      gum::MultiDimArray<float> t10, t11, t12;
      t10 << *(vars[0]) << *(vars[2]) << *(vars[1]);
      t11 << *(vars[1]) << *(vars[0]) << *(vars[2]);
      t12 << *(vars[0]) << *(vars[3]) << *(vars[1]);
      randomInit(&t10);
      randomInit(&t11);
      randomInit(&t12);

      t5 = add2MultiDimArrays(&t10, &t11);
      t6 = add_test_arrays(&t10, &t11);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t10, &t12);
      t6 = add_test_arrays(&t10, &t12);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t11, &t12);
      t6 = add_test_arrays(&t11, &t12);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    void test_op_optimizeMultiDimImplementations() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::MultiDimArray<float> tt1, tt2, tt3, tt4;

      gum::MultiDimImplementation<float> *t5, *t6;
      tt1 << *(vars[0]) << *(vars[2]) << *(vars[1]);
      tt2 << *(vars[0]) << *(vars[1]) << *(vars[2]);
      tt3 << *(vars[6]) << *(vars[0]) << *(vars[2]);
      tt4 << *(vars[6]) << *(vars[0]) << *(vars[2]);
      randomInit(&tt1);
      randomInit(&tt2);
      randomInit(&tt3);
      randomInit(&tt4);
      gum::MultiDimImplementation<float> &t1 = tt1;
      gum::MultiDimImplementation<float> &t2 = tt2;
      gum::MultiDimImplementation<float> &t3 = tt3;
      gum::MultiDimImplementation<float> &t4 = tt4;

      t5 = add2MultiDimArrays(&t1, &t2);
      t6 = add_test_arrays(&tt1, &tt2);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t3, &t4);
      t6 = add_test_arrays(&tt3, &tt4);
      TS_ASSERT(*t6 == *t5);
      delete t6;
      t6 = add2MultiDimArrays(&t4, &t3);
      TS_ASSERT(*t6 == *t5);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t1, &t2);
      t6 = add_test_arrays(&tt1, &tt2);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t3, &t2);
      t6 = add_test_arrays(&tt3, &tt2);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      gum::MultiDimArray<float> tt7, tt8, tt9;
      tt7 << *(vars[0]) << *(vars[2]) << *(vars[1]);
      tt8 << *(vars[0]) << *(vars[2]);
      tt9 << *(vars[0]);
      randomInit(&tt7);
      randomInit(&tt8);
      randomInit(&tt9);
      gum::MultiDimImplementation<float> &t7 = tt7;
      gum::MultiDimImplementation<float> &t8 = tt8;
      gum::MultiDimImplementation<float> &t9 = tt9;

      t5 = add2MultiDimArrays(&t7, &t8);
      t6 = add_test_arrays(&tt7, &tt8);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t8, &t7);
      t6 = add_test_arrays(&tt8, &tt7);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      t5 = add2MultiDimArrays(&t7, &t9);
      t6 = add_test_arrays(&tt7, &tt9);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t9, &t7);
      t6 = add_test_arrays(&tt9, &tt7);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      t5 = add2MultiDimArrays(&t8, &t9);
      t6 = add_test_arrays(&tt8, &tt9);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t9, &t8);
      t6 = add_test_arrays(&tt9, &tt8);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      gum::MultiDimArray<float> tt10, tt11, tt12;
      tt10 << *(vars[0]) << *(vars[2]) << *(vars[1]);
      tt11 << *(vars[1]) << *(vars[0]) << *(vars[2]);
      tt12 << *(vars[0]) << *(vars[3]) << *(vars[1]);
      randomInit(&tt10);
      randomInit(&tt11);
      randomInit(&tt12);
      gum::MultiDimImplementation<float> &t10 = tt10;
      gum::MultiDimImplementation<float> &t11 = tt11;
      gum::MultiDimImplementation<float> &t12 = tt12;

      t5 = add2MultiDimArrays(&t10, &t11);
      t6 = add_test_arrays(&tt10, &tt11);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t10, &t12);
      t6 = add_test_arrays(&tt10, &tt12);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;
      t5 = add2MultiDimArrays(&t11, &t12);
      t6 = add_test_arrays(&tt11, &tt12);
      TS_ASSERT(*t5 == *t6);
      delete t6;
      delete t5;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    // ==========================================================================
    // ==========================================================================
    template <typename T> void mydelete(gum::MultiDimImplementation<T *> *t) {
      gum::Instantiation inst(*t);

      for (inst.setFirst(); !inst.end(); ++inst) {
        delete (*t)[inst];
      }

      delete t;
    }

    // ==========================================================================
    // ==========================================================================
    template <typename T> void mydelete(gum::Potential<T *> *t) {
      gum::Instantiation inst(*t);

      for (inst.setFirst(); !inst.end(); ++inst) {
        delete (*t)[inst];
      }

      delete t;
    }

    // ==========================================================================
    // ==========================================================================
    template <typename T>
    bool equal(const gum::MultiDimImplementation<T *> &t1,
               const gum::MultiDimImplementation<T *> &t2) {
      if ((t1.nbrDim() == t2.nbrDim()) && (t1.domainSize() == t2.domainSize())) {
        for (const auto var : t1.variablesSequence())
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
        for (const auto var : t1.variablesSequence())
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
    void randomInitP(gum::Potential<double> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, (int)(((double)rand() / RAND_MAX) * 100000));
    }

    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInitPPointer(gum::Potential<double *> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, new double((int)(((double)rand() / RAND_MAX) * 100000)));
    }

    // ==========================================================================
    // ==========================================================================
    gum::MultiDimArray<float> *add_test_arrays(const gum::MultiDimArray<float> *t1,
                                               const gum::MultiDimArray<float> *t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1->variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2->variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::MultiDimArray<float> *result = new gum::MultiDimArray<float>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, (*t1)[inst] + (*t2)[inst]);
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::MultiDimArray<float> *sub_test_arrays(const gum::MultiDimArray<float> *t1,
                                               const gum::MultiDimArray<float> *t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1->variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2->variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::MultiDimArray<float> *result = new gum::MultiDimArray<float>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, (*t1)[inst] - (*t2)[inst]);
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::MultiDimArray<float> *
    mult_test_arrays(const gum::MultiDimArray<float> *t1,
                     const gum::MultiDimArray<float> *t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1->variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2->variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::MultiDimArray<float> *result = new gum::MultiDimArray<float>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, (*t1)[inst] * (*t2)[inst]);
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::MultiDimArray<float> *div_test_arrays(const gum::MultiDimArray<float> *t1,
                                               const gum::MultiDimArray<float> *t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1->variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2->variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::MultiDimArray<float> *result = new gum::MultiDimArray<float>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, (*t1)[inst] / (*t2)[inst]);
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::MultiDimArray<float *> *
    add_test_arrays4Pointers(const gum::MultiDimArray<float *> *t1,
                             const gum::MultiDimArray<float *> *t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1->variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2->variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::MultiDimArray<float *> *result = new gum::MultiDimArray<float *>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, new float(*((*t1)[inst]) + *((*t2)[inst])));
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::MultiDimArray<float *> *
    sub_test_arrays4Pointers(const gum::MultiDimArray<float *> *t1,
                             const gum::MultiDimArray<float *> *t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1->variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2->variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::MultiDimArray<float *> *result = new gum::MultiDimArray<float *>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, new float(*((*t1)[inst]) - *((*t2)[inst])));
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::MultiDimArray<float *> *
    mult_test_arrays4Pointers(const gum::MultiDimArray<float *> *t1,
                              const gum::MultiDimArray<float *> *t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1->variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2->variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::MultiDimArray<float *> *result = new gum::MultiDimArray<float *>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, new float(*((*t1)[inst]) * *((*t2)[inst])));
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::MultiDimArray<float *> *
    div_test_arrays4Pointers(const gum::MultiDimArray<float *> *t1,
                             const gum::MultiDimArray<float *> *t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1->variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2->variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::MultiDimArray<float *> *result = new gum::MultiDimArray<float *>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, new float(*((*t1)[inst]) / *((*t2)[inst])));
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::Potential<double> *add_test_potentials(const gum::Potential<double> &t1,
                                                const gum::Potential<double> &t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1.variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2.variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::Potential<double> *result = new gum::Potential<double>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, t1[inst] + t2[inst]);
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::Potential<double> *sub_test_potentials(const gum::Potential<double> &t1,
                                                const gum::Potential<double> &t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1.variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2.variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::Potential<double> *result = new gum::Potential<double>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, t1[inst] - t2[inst]);
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::Potential<double> *mult_test_potentials(const gum::Potential<double> &t1,
                                                 const gum::Potential<double> &t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1.variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2.variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::Potential<double> *result = new gum::Potential<double>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, t1[inst] * t2[inst]);
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::Potential<double> *div_test_potentials(const gum::Potential<double> &t1,
                                                const gum::Potential<double> &t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1.variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2.variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::Potential<double> *result = new gum::Potential<double>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, t1[inst] / t2[inst]);
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::Potential<double *> *
    add_test_potentials4Pointers(const gum::Potential<double *> &t1,
                                 const gum::Potential<double *> &t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1.variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2.variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::Potential<double *> *result = new gum::Potential<double *>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, new double(*(t1[inst]) + *(t2[inst])));
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::Potential<double *> *
    sub_test_potentials4Pointers(const gum::Potential<double *> &t1,
                                 const gum::Potential<double *> &t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1.variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2.variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::Potential<double *> *result = new gum::Potential<double *>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, new double(*(t1[inst]) - *(t2[inst])));
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::Potential<double *> *
    mult_test_potentials4Pointers(const gum::Potential<double *> &t1,
                                  const gum::Potential<double *> &t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1.variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2.variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::Potential<double *> *result = new gum::Potential<double *>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, new double(*(t1[inst]) * *(t2[inst])));
      }

      return result;
    }

    // ==========================================================================
    // ==========================================================================
    gum::Potential<double *> *
    div_test_potentials4Pointers(const gum::Potential<double *> &t1,
                                 const gum::Potential<double *> &t2) {
      // creation of the resulting variable list
      gum::Sequence<const gum::DiscreteVariable *> seq = t1.variablesSequence();
      const gum::Sequence<const gum::DiscreteVariable *> &seq2 =
          t2.variablesSequence();

      for (const auto var : seq2)
        if (!seq.exists(var))
          seq << var;

      // creation of the resulting table
      gum::Potential<double *> *result = new gum::Potential<double *>;

      result->beginMultipleChanges();

      for (const auto var : seq)
        *result << *var;

      result->endMultipleChanges();

      gum::Instantiation inst(*result);

      // compute the addition

      for (inst.setFirst(); !inst.end(); ++inst) {
        result->set(inst, new double(*(t1[inst]) / *(t2[inst])));
      }

      return result;
    }
  };

} /* namespace gum_tests */
