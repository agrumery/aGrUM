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


#include <iostream>
#include <string>

#include <agrum/base/multidim/implementations/multiDimFunctionGraph.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/simulation/fmdpSimulator.h>

#include <testunits/gumtest/AgrumTestSuite.h>

namespace gum_tests {

  struct FMDPSimulatorTestSuite {
    // P(x'|x) = [x' == x]
    static gum::MultiDimFunctionGraph< double >* identity(const gum::DiscreteVariable* x,
                                                          const gum::DiscreteVariable* xp) {
      auto* fg = gum::MultiDimFunctionGraph< double >::getReducedAndOrderedInstance();
      fg->add(*x);
      fg->add(*xp);
      const auto root = fg->manager()->addInternalNode(x);
      fg->manager()->setRootNode(root);
      const auto zero = fg->manager()->addTerminalNode(0.0);
      const auto one  = fg->manager()->addTerminalNode(1.0);
      for (gum::Idx m = 0; m < x->domainSize(); ++m) {
        const auto nxp = fg->manager()->addInternalNode(xp);
        fg->manager()->setSon(root, m, nxp);
        for (gum::Idx mp = 0; mp < xp->domainSize(); ++mp)
          fg->manager()->setSon(nxp, mp, (m == mp) ? one : zero);
      }
      return fg;
    }
  };

  GUM_TEST(PerformKeepsFrameVariables) {
    // f has no transition for action "go": it is a frame variable and keeps its value
    gum::FMDP< double > fmdp(true);
    auto*               s = new gum::LabelizedVariable("s", "", 2);
    auto*               f = new gum::LabelizedVariable("f", "", 2);
    fmdp.addVariable(s);
    fmdp.addVariable(f);
    fmdp.addAction(1, "go");
    fmdp.addTransitionForAction(1, s, identity(s, fmdp.main2prime(s)));

    gum::FMDPSimulator sim(&fmdp);
    gum::Instantiation init;
    init.add(*s);
    init.add(*f);
    init.chgVal(*s, 1);
    init.chgVal(*f, 1);
    sim.setInitialState(init);

    for (int i = 0; i < 5; ++i) {
      GUM_CHECK_ASSERT_THROWS_NOTHING(sim.perform(1));
      CHECK_EQ(sim.currentState().val(*s), static_cast< gum::Idx >(1));
      CHECK_EQ(sim.currentState().val(*f), static_cast< gum::Idx >(1));
    }
  }
}   // namespace gum_tests
