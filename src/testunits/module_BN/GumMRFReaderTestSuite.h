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

#include <string>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

#include <agrum/MRF/MarkovRandomField.h>
#include <agrum/MRF/io/GUM/GumMRFReader.h>

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  GumMRFReader
#define GUM_CURRENT_MODULE BN

namespace gum_tests {
  struct GumMRFReaderTestSuite {
    public:
    static void testBuildingMRFFromJson() {
      // Read minimal.mrf.jgum: nodes A[2], B[2], C[2], factors {A,B} and {B,C}
      const std::string                filename = GET_RESSOURCES_PATH("jsonGum/minimal.mrf.jgum");
      gum::MarkovRandomField< double > mrf;
      auto reader = gum::GumMRFReader< double >(&mrf, filename);
      CHECK_EQ(reader.proceed(), 0u);

      CHECK_EQ(mrf.size(), 3u);
      CHECK_EQ(mrf.factors().size(), 2u);

      // verify nodes exist (idFromName throws if not found)
      CHECK_NOTHROW(mrf.idFromName("A"));
      CHECK_NOTHROW(mrf.idFromName("B"));
      CHECK_NOTHROW(mrf.idFromName("C"));

      CHECK_EQ(mrf.variable("A").domainSize(), 2u);
      CHECK_EQ(mrf.variable("B").domainSize(), 2u);
      CHECK_EQ(mrf.variable("C").domainSize(), 2u);

      CHECK(mrf.existsEdge(mrf.idFromName("A"), mrf.idFromName("B")));
      CHECK(mrf.existsEdge(mrf.idFromName("B"), mrf.idFromName("C")));

      // factor {A,B}: [0.5, 0.2, 0.3, 0.8]
      const auto& factorAB = mrf.factor({"A", "B"});
      CHECK_EQ(factorAB.domainSize(), 4u);

      // factor {B,C}: [0.9, 0.1, 0.4, 0.6]
      const auto& factorBC = mrf.factor({"B", "C"});
      CHECK_EQ(factorBC.domainSize(), 4u);

      // properties from file
      CHECK(mrf.existsProperty("software"));
      CHECK_EQ(mrf.property("software"), "aGrUM test");
    }
  };

  GUM_TEST_ACTIF(BuildingMRFFromJson)
}   // namespace gum_tests
