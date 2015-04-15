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
#include <vector>
#include "../../agrum/core/set.h"

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/core/set.h>
#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/multidim/operators/multiDimCombinationDefault.h>
#include <agrum/multidim/operators/multiDimProjection.h>
#include <agrum/multidim/operators/multiDimCombineAndProjectDefault.h>

namespace gum_tests {

  class MultiDimCombineAndProjectTestSuite : public CxxTest::TestSuite {
    public:
    void test1() {
      std::vector<gum::LabelizedVariable *> vars(11);

      for (unsigned int i = 0; i < 11; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 4);
      }

      gum::Potential<float> t1, t2, t3, t4, t5, t6;
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

      gum::MultiDimCombineAndProjectDefault<float, gum::Potential> projcomb(multPot,
                                                                            mySum);

      gum::Set<const gum::Potential<float> *> to_comb;
      to_comb << &t1 << &t2 << &t3 << &t4 << &t5 << &t6;
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars << vars[1] << vars[4] << vars[5] << vars[6] << vars[9] << vars[10];

      try {

        gum::Set<const gum::Potential<float> *> res =
            projcomb.combineAndProject(to_comb, del_vars);

        float nb_ops = projcomb.nbOperations(to_comb, del_vars);

        std::pair<long, long> yyy = projcomb.memoryUsage(to_comb, del_vars);

        TS_ASSERT(yyy.first == 244);
        TS_ASSERT(yyy.second == 228);

        TS_ASSERT(nb_ops == 416);
        TS_ASSERT(res.size() == 3);

        gum::Set<const gum::Potential<float> *>::const_iterator iter = res.begin();
        const gum::Potential<float> *res1 = *iter;
        ++iter;
        const gum::Potential<float> *res2 = *iter;
        ++iter;
        const gum::Potential<float> *res3 = *iter;

        gum::MultiDimCombinationDefault<float, gum::Potential> comb(multPot);
        gum::MultiDimProjection<float, gum::Potential> proj(mySum);

        gum::Set<const gum::Potential<float> *> comb_set;
        comb_set << &t1 << &t2;
        gum::Set<const gum::DiscreteVariable *> del_vars2;
        del_vars2 << vars[1];
        gum::Potential<float> *tt1 = comb.combine(comb_set);
        gum::Potential<float> *tt2 = proj.project(*tt1, del_vars2);
        delete tt1;
        TS_ASSERT((*tt2 == *res1) || (*tt2 == *res2) || (*tt2 == *res3));

        delete tt2;
        comb_set.clear();
        del_vars2.clear();
        comb_set << &t3 << &t4 << &t5;
        del_vars2 << vars[4] << vars[5] << vars[6];
        tt1 = comb.combine(comb_set);
        tt2 = proj.project(*tt1, del_vars2);
        delete tt1;
        TS_ASSERT((*tt2 == *res1) || (*tt2 == *res2) || (*tt2 == *res3));

        delete tt2;
        del_vars2.clear();
        del_vars2 << vars[9] << vars[10];
        tt2 = proj.project(t6, del_vars2);
        TS_ASSERT((*tt2 == *res1) || (*tt2 == *res2) || (*tt2 == *res3));
        delete tt2;

        gum::MultiDimCombineAndProjectDefault<float, gum::Potential> projcomb2 =
            projcomb;
        projcomb2.setCombineFunction(addPot);
        projcomb2.setProjectFunction(myMax);

        gum::MultiDimCombineAndProjectDefault<float, gum::Potential> *projcomb3 =
            projcomb.newFactory();

        delete projcomb3;

        for (const auto pot : res)
          delete pot;

        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];

      } catch (gum::NotFound &e) {
        GUM_SHOWERROR(e);
        TS_ASSERT(false);
      }
    }

    private:
    // =========================================================================
    /// initialize randomly a table
    // =========================================================================
    void randomInitP(gum::Potential<float> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, 1 + (int)(((float)rand() / RAND_MAX) * 50));
    }

    // the function used to combine two tables
    static gum::Potential<float> *addPot(const gum::Potential<float> &t1,
                                         const gum::Potential<float> &t2) {
      return new gum::Potential<float>(t1 + t2);
    }

    // the function used to combine two tables
    static gum::Potential<float> *multPot(const gum::Potential<float> &t1,
                                          const gum::Potential<float> &t2) {
      return new gum::Potential<float>(t1 * t2);
    }

    static gum::Potential<float> *
    mySum(const gum::Potential<float> &table,
          const gum::Set<const gum::DiscreteVariable *> &del_vars) {
      return new gum::Potential<float>(projectSum(table, del_vars));
    }

    static gum::Potential<float> *
    myMax(const gum::Potential<float> &table,
          const gum::Set<const gum::DiscreteVariable *> &del_vars) {
      return new gum::Potential<float>(projectMax(table, del_vars));
    }
  };

} /* namespace gum_tests */
