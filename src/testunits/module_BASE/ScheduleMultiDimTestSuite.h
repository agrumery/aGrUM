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


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/graphicalModels/inference/scheduler/scheduleMultiDim.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  ScheduleMultiDim
#define GUM_CURRENT_MODULE GUMBASE

namespace gum_tests {

  struct ScheduleMultiDimTestSuite {
    public:
    static void test_construct() {
      // reset the ids of the ScheduleMultiDim to avoid conflicts with other
      // testunits
      gum::IScheduleMultiDim::resetIdGenerator();

      std::vector< gum::LabelizedVariable* > vars(10);

      for (unsigned int i = 0; i < 10; ++i) {
        std::stringstream str;
        str << "x" << i;
        std::string s = str.str();
        vars[i]       = new gum::LabelizedVariable(s, s, 10);
      }

      gum::Sequence< const gum::DiscreteVariable* > seq;
      seq << vars[0] << vars[2] << vars[4];

      gum::ScheduleMultiDim< gum::Tensor< double > > f1(seq);
      CHECK(f1.isAbstract());
      CHECK_THROWS_AS(f1.multiDim(), const gum::NullElement&);
      CHECK(f1.id() == gum::Idx(1));

      std::string       s1 = f1.toString();
      std::stringstream s2;
      s2 << "<id: " << f1.id() << ", table: -->";
      CHECK(s2.str() == s1);

      gum::ScheduleMultiDim< gum::Tensor< double > > f1bis(seq, 10);
      CHECK_THROWS_AS(f1bis.multiDim(), const gum::NullElement&);
      CHECK(f1.variablesSequence() == f1bis.variablesSequence());
      CHECK(f1bis.id() == gum::Idx(10));
      CHECK(f1bis.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Tensor< double > > f1ter(seq, 10);
      CHECK(f1.variablesSequence() == f1ter.variablesSequence());
      CHECK(f1ter.id() == gum::Idx(10));
      CHECK(f1ter.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Tensor< double > > f1quart(seq);
      CHECK(f1.variablesSequence() == f1quart.variablesSequence());
      CHECK(f1quart.id() == gum::Idx(11));
      CHECK(f1quart.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Tensor< double > > f2(f1);
      CHECK(f2.isAbstract());
      CHECK(f2.containsMultiDim() == false);
      CHECK_THROWS_AS(f2.multiDim(), const gum::NullElement&);
      CHECK(f1 == f2);
      CHECK(!(f1 != f2));
      CHECK(f1 == dynamic_cast< gum::IScheduleMultiDim& >(f2));
      CHECK(!(f1 != dynamic_cast< gum::IScheduleMultiDim& >(f2)));
      CHECK(f1.id() == f2.id());
      CHECK(f1.variablesSequence() == f2.variablesSequence());
      CHECK(f2.domainSize() == gum::Size(1000));

      f2.makeAbstract();
      CHECK(f2.isAbstract());
      CHECK(f2.containsMultiDim() == false);
      CHECK(f2.domainSize() == gum::Size(1000));
      CHECK(f2.variablesSequence() == seq);

      gum::ScheduleMultiDim< gum::Tensor< double > > f3(seq);
      CHECK(f1 != f3);
      CHECK(f1.hasSameVariables(f3));
      CHECK(f1.hasSameContent(f3));
      CHECK(f1.hasSameVariables(dynamic_cast< gum::IScheduleMultiDim& >(f3)));
      CHECK(f1.hasSameContent(dynamic_cast< gum::IScheduleMultiDim& >(f3)));
      CHECK(f1.variablesSequence() == f3.variablesSequence());
      CHECK(f3.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Tensor< double > > f3b(std::move(f3));
      CHECK(f1 != f3b);
      CHECK(f1.hasSameVariables(f3b));
      CHECK(f1.hasSameContent(f3b));
      CHECK(f1.variablesSequence() == f3b.variablesSequence());

      gum::Tensor< double > pot;
      pot << *(vars[0]) << *(vars[2]) << *(vars[4]);
      const std::vector< double > cpt(1000, 10);
      pot.fillWith(cpt);

      gum::ScheduleMultiDim< gum::Tensor< double > > f4(pot, true, 5);
      CHECK(!f4.isAbstract());
      CHECK(f1 != f4);
      CHECK(f1.hasSameVariables(f4));
      CHECK(!f1.hasSameContent(f4));
      const gum::Tensor< double >& res_pot = f4.multiDim();
      CHECK(pot == res_pot);
      CHECK(f4.containsMultiDim());
      CHECK(f4.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Tensor< double > > f4bis(pot, true);
      CHECK(!f4bis.isAbstract());
      CHECK(f4.hasSameVariables(f4bis));
      CHECK(f4.hasSameContent(f4bis));
      CHECK(f4 != f4bis);
      CHECK(!(f4 == f4bis));
      const gum::Tensor< double >& res_pot_bis = f4bis.multiDim();
      CHECK(res_pot == res_pot_bis);
      CHECK(f4bis.id() == gum::Idx(13));
      CHECK(f4bis.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Tensor< double > > f4ter(pot, true, 2);
      CHECK(!f4ter.isAbstract());
      const gum::Tensor< double >& res_pot_ter = f4ter.multiDim();
      CHECK(res_pot == res_pot_ter);
      CHECK(f4ter.id() == gum::Idx(2));
      CHECK(f4ter.domainSize() == gum::Size(1000));

      gum::ScheduleMultiDim< gum::Tensor< double > > f4b(pot, false);
      CHECK(!f4b.isAbstract());
      CHECK(f4.multiDim() == f4b.multiDim());
      CHECK(f4 != f4b);
      CHECK(f4.hasSameVariables(f4b));
      CHECK(!f4b.containsMultiDim());
      CHECK(f4b.id() == gum::Idx(14));
      CHECK(f4b.domainSize() == gum::Size(1000));

      gum::Tensor< double >                          xxpot = res_pot;
      gum::ScheduleMultiDim< gum::Tensor< double > > f4bt(xxpot, false, 5);
      CHECK(!f4bt.isAbstract());
      CHECK(f4bt.multiDim() == f4.multiDim());
      CHECK(f4ter.hasSameVariables(f4bt));
      CHECK(!f4bt.containsMultiDim());
      CHECK(f4bt.id() == gum::Idx(5));
      CHECK(f4bt.domainSize() == gum::Size(1000));

      gum::Tensor< double >                          potx = res_pot;
      gum::ScheduleMultiDim< gum::Tensor< double > > f4c(std::move(potx));
      CHECK(!f4c.isAbstract());
      CHECK(f4c.containsMultiDim());
      CHECK(f4.multiDim() == f4c.multiDim());
      CHECK(f4.hasSameVariables(f4c));
      CHECK(f4c.id() == gum::Idx(15));
      CHECK(f4c.domainSize() == gum::Size(1000));

      gum::Tensor< double >                          potxx = res_pot;
      gum::ScheduleMultiDim< gum::Tensor< double > > f4d(std::move(potxx), 10);
      CHECK(!f4d.isAbstract());
      CHECK(f4d.containsMultiDim());
      CHECK(f4.multiDim() == f4d.multiDim());
      CHECK(f4.hasSameVariables(f4d));
      CHECK(f4 != f4d);
      CHECK(f4d.id() == gum::Idx(10));

      f4d.makeAbstract();
      CHECK(f4d.isAbstract());
      CHECK(f4d.containsMultiDim() == false);
      CHECK(f4d.domainSize() == gum::Size(1000));
      CHECK(f4d.variablesSequence() == seq);

      gum::ScheduleMultiDim< gum::Tensor< double > > f5(f4);
      CHECK(!f5.isAbstract());
      CHECK(f5 == f4);
      CHECK(f5.hasSameContent(f4));
      CHECK(f4.id() == f5.id());
      CHECK(f4.multiDim() == res_pot);

      std::string       s5 = f5.toString();
      std::stringstream s6;
      s6 << "<id: 5, table: " << f5.multiDim().content() << ">";
      CHECK(s6.str() == s5);

      gum::Tensor< double > pot2;
      pot2 << *(vars[1]) << *(vars[2]) << *(vars[4]);
      const std::vector< double > cpt2(1000, 42);
      pot2.fillWith(cpt2);
      f4.setMultiDim(pot2, true);
      gum::ScheduleMultiDim< gum::Tensor< double > > f6(f4);
      CHECK(f6.multiDim() == pot2);
      f6.setMultiDim(pot, true);
      CHECK(f6.multiDim() == pot);
      CHECK(f6.multiDim() != f4.multiDim());

      CHECK(f4.containsMultiDim());
      f6 = f4;
      CHECK(f6 == f4);
      CHECK(f6.hasSameVariables(f4));
      CHECK(f6.hasSameContent(f4));

      gum::ScheduleMultiDim< gum::Tensor< double > > f7(pot, false, 5);
      f7 = f4;
      CHECK(f7 == f4);

      gum::ScheduleMultiDim< gum::Tensor< double > > f8(pot, false, 8);
      gum::ScheduleMultiDim< gum::Tensor< double > > f9(pot2, true, 9);
      CHECK(f9.multiDim() != f8.multiDim());
      f9 = f8;
      CHECK(!f8.containsMultiDim());
      CHECK(!f9.containsMultiDim());
      CHECK(f8 == f9);
      CHECK(f8.hasSameVariables(f9));
      CHECK(f8.hasSameContent(f9));

      gum::ScheduleMultiDim< gum::Tensor< double > > f10(pot, false, 10);
      gum::ScheduleMultiDim< gum::Tensor< double > > f11(pot2, true, 10);
      f10 = std::move(f11);
      CHECK(f10.containsMultiDim());
      CHECK(f10.variablesSequence() == pot2.variablesSequence());

      auto pot3 = f10.exportMultiDim();
      CHECK(f10.isAbstract());
      CHECK(!f10.containsMultiDim());
      CHECK(*pot3 == pot2);
      f10.makeAbstract();
      CHECK(f10.isAbstract());
      delete pot3;

      const auto pot4 = pot2;
      f10.setMultiDim(std::move(pot2));
      CHECK(f10.containsMultiDim());
      CHECK(f10.multiDim() == pot4);

      gum::ScheduleMultiDim< gum::Tensor< double > >* f12 = f10.clone();
      CHECK(*f12 == f10);
      CHECK(f10.hasSameVariables(*f12));
      CHECK(f10.hasSameContent(*f12));

      delete f12;

      for (unsigned int i = 0; i < vars.size(); ++i)
        delete vars[i];
    }   // namespace gum_tests

    static void testConstants() {
      gum::Tensor< double > pot;
      pot.fillWith({42.42});

      gum::ScheduleMultiDim< gum::Tensor< double > > f1(pot, false);
      CHECK(!f1.containsMultiDim());
      CHECK(f1.domainSize() == 1);

      gum::ScheduleMultiDim< gum::Tensor< double > > f2(pot, true);
      CHECK(f2.containsMultiDim());
      CHECK(f2.domainSize() == 1);
      gum::Tensor< double >* pot2 = f2.exportMultiDim();
      CHECK(pot2->max() == pot2->min());
      CHECK(pot2->min() == 42.42);
      delete pot2;

      gum::Sequence< const gum::DiscreteVariable* >  seq;
      gum::ScheduleMultiDim< gum::Tensor< double > > f3(seq, 0);
      CHECK(!f3.containsMultiDim());
      CHECK(f3.domainSize() == 1);
      CHECK(f3.isAbstract());
    }
  };

  GUM_TEST_ACTIF(_construct)
  GUM_TEST_ACTIF(Constants)

} /* namespace gum_tests */
