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
#pragma once


#include <sstream>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/core/exceptions.h>
#include <agrum/base/multidim/ICIModels/multiDimNoisyAND.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>

namespace gum_tests {

  class GUM_TEST_SUITE(MultiDimNoisyAND) {
    public:
    GUM_ACTIVE_TEST(CreationNoisyOr) {
      gum::LabelizedVariable          a("a", "", 2), b("b", "", 2), c("c", "", 2), d("d", "", 2);
      gum::MultiDimNoisyAND< double > p(0.2f);

      // trying to change weight for a non cause
      TS_ASSERT_THROWS(p.causalWeight(b, 0.4f), const gum::InvalidArgument&)
      TS_ASSERT_THROWS(p.causalWeight(d, 0.0f), const gum::InvalidArgument&)

      // adding causes
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d)

      // trying to set 0 for causal weight
      TS_ASSERT_THROWS(p.causalWeight(d, 0.0f), const gum::OutOfBounds&)

      // doing the right stuf :)
      TS_GUM_ASSERT_THROWS_NOTHING(p.causalWeight(b, 0.4f))
      TS_GUM_ASSERT_THROWS_NOTHING(p.causalWeight(d, 0.7f))
      TS_ASSERT_EQUALS(
          p.toString(),
          "a:Labelized({0|1})=noisyAND([0.2], b:Labelized({0|1})[0.4], c:Labelized({0|1})"
          "[1], d:Labelized({0|1})[0.7])");
      TS_ASSERT_EQUALS(p.realSize(), static_cast< gum::Size >(4))

      gum::MultiDimNoisyAND< double > q(p);
      TS_ASSERT_EQUALS(
          q.toString(),
          "a:Labelized({0|1})=noisyAND([0.2], b:Labelized({0|1})[0.4], c:Labelized({0|1})"
          "[1], d:Labelized({0|1})[0.7])");
      TS_ASSERT_EQUALS(p.realSize(), static_cast< gum::Size >(4))

      // trying a noisyAND with 0 as external weight (not allowed)
      TS_ASSERT_THROWS(gum::MultiDimNoisyAND< double > qq(0.0f), const gum::InvalidArgument&)
    }   // namespace gum_tests

    GUM_ACTIVE_TEST(CompatibleWithHardAND) {
      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);

      gum::MultiDimNoisyAND< double > p(1.0f);
      p << fever << malaria << flu << cold;
      p.causalWeight(cold, 1.0f);
      p.causalWeight(flu, 1.0f);
      p.causalWeight(malaria, 1.0f);

      gum::Instantiation i(p);
      float              witness[] = {1.0f,
                                      0.0f,
                                      1.0f,
                                      0.0f,
                                      1.0f,
                                      0.0f,
                                      1.0f,
                                      0.0f,
                                      1.0f,
                                      0.0f,
                                      1.0f,
                                      0.0f,
                                      1.0f,
                                      0.0f,
                                      0.0f,
                                      1.0f};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6)
      }
    }

    GUM_ACTIVE_TEST(ComputationInNoisyAND) {
      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);

      gum::MultiDimNoisyAND< double > p(1.0f);
      p << fever << malaria << flu << cold;   // malaria, flu and cold are causes of fever
      p.causalWeight(cold, 0.4f);
      p.causalWeight(flu, 0.8f);
      p.causalWeight(malaria, 0.9f);

      gum::Instantiation i(p);
      float              witness[] = {0.988f,
                                      0.012f,
                                      0.892f,
                                      0.108f,
                                      0.952f,
                                      0.048f,
                                      0.568f,
                                      0.432f,
                                      0.992f,
                                      0.008f,
                                      0.928f,
                                      0.072f,
                                      0.968f,
                                      0.032f,
                                      0.712f,
                                      0.288f};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6)
      }

      gum::MultiDimNoisyAND< double > q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6)
      }
    }

    GUM_ACTIVE_TEST(ComputationInNoisyAND2) {
      gum::LabelizedVariable lazy("lazy", "", 2);
      gum::LabelizedVariable degree("degree", "", 2);
      gum::LabelizedVariable motivation("motivation", "", 2);
      gum::LabelizedVariable requirement("requirement", "", 2);
      gum::LabelizedVariable competition("competition", "", 2);
      gum::LabelizedVariable unemployment("unemployment", "", 2);

      gum::MultiDimNoisyAND< double > p(1 - 0.0001f);
      p << unemployment << competition << requirement << motivation << degree << lazy;
      p.causalWeight(lazy, 0.8f);
      p.causalWeight(degree, 0.7f);
      p.causalWeight(motivation, 0.9f);
      p.causalWeight(requirement, 0.8f);
      p.causalWeight(competition, 0.9f);

      gum::Instantiation i(p);
      float              witness[]
          = {0.999880012f, 0.000119988f, 0.998920108f, 0.001079892f, 0.999520048f, 0.000479952f,
             0.995680432f, 0.004319568f, 0.998920108f, 0.001079892f, 0.990280972f, 0.009719028f,
             0.995680432f, 0.004319568f, 0.961123888f, 0.038876112f, 0.999720028f, 0.000279972f,
             0.997480252f, 0.002519748f, 0.998880112f, 0.001119888f, 0.989921008f, 0.010078992f,
             0.997480252f, 0.002519748f, 0.977322268f, 0.022677732f, 0.989921008f, 0.010078992f,
             0.909289072f, 0.090710928f, 0.999520048f, 0.000479952f, 0.995680432f, 0.004319568f,
             0.998080192f, 0.001919808f, 0.982721728f, 0.017278272f, 0.995680432f, 0.004319568f,
             0.961123888f, 0.038876112f, 0.982721728f, 0.017278272f, 0.844495552f, 0.155504448f,
             0.998880112f, 0.001119888f, 0.989921008f, 0.010078992f, 0.995520448f, 0.004479552f,
             0.959684032f, 0.040315968f, 0.989921008f, 0.010078992f, 0.909289072f, 0.090710928f,
             0.959684032f, 0.040315968f, 0.637156288f, 0.362843712f};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6)
      }

      gum::MultiDimNoisyAND< double > q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6)
      }
    }
  };
}   // namespace gum_tests
