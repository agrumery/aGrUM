/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <sstream>

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/variables/labelizedVariable.h>
#include <agrum/multidim/potential.h>
#include <agrum/graphicalModels/inference/scheduleProject.h>

namespace gum_tests {

  class ScheduleProjectTestSuite : public CxxTest::TestSuite {
    public:
    void test_construct() {
      std::vector<gum::LabelizedVariable *> vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i] = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Potential<float> pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      randomInit(pot1);
      gum::ScheduleMultiDim<float> f1(pot1);
      gum::Set<const gum::DiscreteVariable *> del_vars;
      del_vars << vars[0] << vars[3];

      gum::ScheduleProject<float> myproj(f1, del_vars, gum::projectMax);
      const gum::ScheduleMultiDim<float> &res = myproj.result();
      TS_ASSERT(res.isAbstract());

      TS_ASSERT(myproj.nbOperations() == 16);
      std::pair<long, long> xxx = myproj.memoryUsage();
      TS_ASSERT(xxx.first == 4);

      gum::Sequence<const gum::ScheduleMultiDim<float> *> multidims =
          myproj.multiDimArgs();
      TS_ASSERT(multidims.size() == 1);
      TS_ASSERT(*(multidims.atPos(0)) == f1);

      std::stringstream s1;
      s1 << res.toString() << " = project ( " << f1.toString() << " , "
         << del_vars.toString() << " )";
      TS_ASSERT(s1.str() == myproj.toString());

      gum::ScheduleProject<float> myproj2 = myproj;
      TS_ASSERT(myproj2.result().isAbstract());
      TS_ASSERT(myproj2 == myproj);
      TS_ASSERT(!(myproj2 != myproj));

      myproj.execute();
      TS_ASSERT(!res.isAbstract());
      TS_ASSERT(!myproj2.result().isAbstract());
      gum::Potential<float> *res2 = proj(pot1, del_vars, 0);
      TS_ASSERT(*(res2->content()) == res.multiDim());

      gum::ScheduleProject<float> myproj3(f1, del_vars, gum::projectMin);
      TS_ASSERT(myproj3 != myproj);
      const gum::ScheduleMultiDim<float> &res3 = myproj3.result();
      TS_ASSERT(res3.isAbstract());
      myproj3 = myproj;
      myproj3.execute();
      TS_ASSERT(res3.multiDim() == res.multiDim());

      gum::ScheduleProject<float> *myproj4 = myproj3.newFactory();
      TS_ASSERT(*myproj4 == myproj3);
      delete myproj4;

      delete res2;
      delete &(res.multiDim());

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    // ==========================================================================
    /// initialize randomly a table
    // ==========================================================================
    void randomInit(gum::Potential<float> &t) {
      gum::Instantiation i(t);

      for (i.setFirst(); !i.end(); ++i)
        t.set(i, (int)(((float)rand() / RAND_MAX) * 100000));
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
  };

} /* namespace gum_tests */
