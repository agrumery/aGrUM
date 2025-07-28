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

#include <agrum/base/multidim/ICIModels/multiDimNoisyORCompound.h>
#include <agrum/base/multidim/tensor.h>
#include <agrum/base/variables/labelizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>

namespace gum_tests {

  class [[maybe_unused]] MultiDimNoisyORCompoundTestSuite: public CxxTest::TestSuite {
    public:
    GUM_ACTIVE_TEST(CreationNoisyOr) {
      gum::LabelizedVariable a("a", "", 2), b("b", "", 2), c("c", "", 2), d("d", "", 2);
      gum::MultiDimNoisyORCompound< double > p(0.2f);

      // trying to change weight for a non cause
      TS_ASSERT_THROWS(p.causalWeight(b, 0.4f), const gum::InvalidArgument&)
      TS_ASSERT_THROWS(p.causalWeight(d, 0.0f), const gum::InvalidArgument&)

      // adding causes
      TS_GUM_ASSERT_THROWS_NOTHING(p << a << b << c << d)

      // trying to set 0 for causal weight
      TS_ASSERT_THROWS(p.causalWeight(d, 0.0f), const gum::OutOfBounds&)

      // doing the right stuff :)
      p.causalWeight(b, 0.4f);
      p.causalWeight(d, 0.7f);
      TS_ASSERT_EQUALS(p.toString(),
                       "a:Labelized({0|1})=noisyORCompound([0.2], b:Labelized({0|1})[0.4], c:"
                       "Labelized({0|1})[1], d:Labelized({0|1})[0.7])");
      TS_ASSERT_EQUALS(p.realSize(), (gum::Size)4)

      gum::MultiDimNoisyORCompound< double > q(p);
      TS_ASSERT_EQUALS(q.toString(),
                       "a:Labelized({0|1})=noisyORCompound([0.2], b:Labelized({0|1})[0.4], c:"
                       "Labelized({0|1})[1], d:Labelized({0|1})[0.7])");
      TS_ASSERT_EQUALS(p.realSize(), (gum::Size)4)
    }

    GUM_ACTIVE_TEST(CompatibleWithHardOR) {
      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);

      gum::MultiDimNoisyORCompound< double > p(0.0f);
      p << fever << malaria << flu << cold;
      p.causalWeight(cold, 1.0f);
      p.causalWeight(flu, 1.0f);
      p.causalWeight(malaria, 1.0f);

      gum::Instantiation i(p);
      float              witness[] = {1.0f,
                                      0.0f,
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
                                      1.0f};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6)
      }
    }

    GUM_ACTIVE_TEST(ComputationInNoisyORCompound) {
      gum::LabelizedVariable cold("Cold", "", 2);
      gum::LabelizedVariable flu("Flu", "", 2);
      gum::LabelizedVariable malaria("Malaria", "", 2);
      gum::LabelizedVariable fever("Fever", "", 2);

      gum::MultiDimNoisyORCompound< double > p(0.0f);
      p << fever << malaria << flu << cold;
      p.causalWeight(cold, 0.4f);
      p.causalWeight(flu, 0.8f);
      p.causalWeight(malaria, 0.9f);

      gum::Instantiation i(p);
      float              witness[] = {1,
                                      0,
                                      0.1f,
                                      0.9f,
                                      0.2f,
                                      0.8f,
                                      0.02f,
                                      0.98f,
                                      0.6f,
                                      0.4f,
                                      0.06f,
                                      0.94f,
                                      0.12f,
                                      0.88f,
                                      0.012f,
                                      0.988f};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6)
      }

      gum::MultiDimNoisyORCompound< double > q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6)
      }
    }

    GUM_ACTIVE_TEST(ComputationInNoisyORCompound2) {
      gum::LabelizedVariable lazy("lazy", "", 2);
      gum::LabelizedVariable degree("degree", "", 2);
      gum::LabelizedVariable motivation("motivation", "", 2);
      gum::LabelizedVariable requirement("requirement", "", 2);
      gum::LabelizedVariable competition("competition", "", 2);
      gum::LabelizedVariable unemployment("unemployment", "", 2);

      gum::MultiDimNoisyORCompound< double > p(0.0001f);
      p << unemployment << competition << requirement << motivation << degree << lazy;
      p.causalWeight(lazy, 0.1f);
      p.causalWeight(degree, 0.3f);
      p.causalWeight(motivation, 0.5f);
      p.causalWeight(requirement, 0.7f);
      p.causalWeight(competition, 0.9f);

      gum::Instantiation i(p);
      float              witness[] = {
          0.9999f,   0.0001f,   0.1f,      0.9f,      0.3f,      0.7f,      0.030003f, 0.969997f,
          0.5f,      0.5f,      0.050005f, 0.949995f, 0.150015f, 0.849985f, 0.015003f, 0.984997f,
          0.7f,      0.3f,      0.070007f, 0.929993f, 0.210021f, 0.789979f, 0.021004f, 0.978996f,
          0.350035f, 0.649965f, 0.035007f, 0.964993f, 0.105021f, 0.894979f, 0.010503f, 0.989496f,
          0.9f,      0.1f,      0.090009f, 0.909991f, 0.270027f, 0.729973f, 0.027005f, 0.972995f,
          0.450045f, 0.549955f, 0.045009f, 0.954991f, 0.135027f, 0.864973f, 0.013504f, 0.986496f,
          0.630063f, 0.369937f, 0.063013f, 0.936987f, 0.189038f, 0.810962f, 0.018906f, 0.981094f,
          0.315063f, 0.684937f, 0.031509f, 0.968491f, 0.094528f, 0.905472f, 0.009454f, 0.990546f};

      int j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(p[i], witness[j], 1e-6)
      }

      gum::MultiDimNoisyORCompound< double > q(p);

      j = 0;

      for (i.setFirst(); !i.end(); ++i, j++) {
        TS_ASSERT_DELTA(q[i], witness[j], 1e-6)
      }
    }
  };
}   // namespace gum_tests
