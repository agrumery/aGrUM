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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphicalModels/inference/scheduler/scheduleProjection.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

namespace gum_tests {

  class [[maybe_unused]] ScheduleOperationTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(_construct) {
      // reset the ids of the ScheduleMultiDim to avoid conflicts with other
      // testunits
      gum::IScheduleMultiDim::resetIdGenerator();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        const std::string& s = str.str();
        vars[i]              = new gum::LabelizedVariable(s, s, 2);
      }

      gum::Tensor< double > pot1;
      pot1 << *(vars[0]) << *(vars[2]) << *(vars[3]) << *(vars[4]);
      pot1.random();
      gum::ScheduleMultiDim< gum::Tensor< double > > f1(pot1, true);
      gum::VariableSet                               del_vars;
      del_vars << vars[0] << vars[3];

      gum::ScheduleProjection< gum::Tensor< double > >      real_myproj(f1, del_vars, myProjectMax);
      const gum::ScheduleMultiDim< gum::Tensor< double > >& res    = real_myproj.result();
      gum::ScheduleOperator&                                myproj = real_myproj;

      const gum::Sequence< const gum::IScheduleMultiDim* >& multidims = myproj.args();
      TS_ASSERT(multidims.size() == 1);
      TS_ASSERT(*(multidims.atPos(0)) == f1);

      std::stringstream s1;
      s1 << res.toString() << " = project ( " << f1.toString() << " , " << del_vars.toString()
         << " )";
      TS_ASSERT(s1.str() == myproj.toString());

      gum::ScheduleProjection< gum::Tensor< double > > real_myproj2 = real_myproj;
      gum::ScheduleOperator&                           myproj2      = real_myproj2;
      TS_ASSERT(real_myproj2.result().isAbstract());
      TS_ASSERT(myproj2 == myproj);
      TS_ASSERT(!(myproj2 != myproj));

      myproj.execute();
      TS_ASSERT(!res.isAbstract());
      TS_ASSERT(real_myproj2.result().isAbstract());
      gum::Tensor< double >* res2 = proj(pot1, del_vars, 0);
      TS_ASSERT(*(res2->content()) == res.multiDim());

      delete res2;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }

    private:
    static gum::Tensor< double > myProjectMax(const gum::Tensor< double >& pot,
                                              const gum::VariableSet&      del_vars) {
      return gum::Tensor< double >(gum::projectMax(*(pot.content()), del_vars));
    }

    // projection of a table over a set
    gum::Tensor< double >* proj(const gum::Tensor< double >& table,
                                const gum::VariableSet&      del_vars,
                                double                       neutral_elt) {
      gum::Tensor< double >* result = new gum::Tensor< double >;

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
  };

} /* namespace gum_tests */
