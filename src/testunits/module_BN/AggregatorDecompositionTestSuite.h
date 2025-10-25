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


#include <ctime>
#include <iostream>
#include <string>
// #include <experimental/filesystem>

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/BN/inference/lazyPropagation.h>
#include <agrum/BN/inference/tools/aggregatorDecomposition.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/BN/io/BIF/BIFWriter.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNReader.h>
#include <agrum/BN/io/BIFXML/BIFXMLBNWriter.h>
#include <agrum/PRM/inference/groundedInference.h>
#include <agrum/PRM/o3prm/O3prm.h>
#include <agrum/PRM/o3prm/O3prmBNReader.h>
#include <agrum/PRM/o3prm/O3prmReader.h>
#include <agrum/PRM/PRM.h>

#include <agrum/base/core/math/math_utils.h>

namespace gum_tests {
  class GUM_TEST_SUITE(AggregatorDecomposition) {
    public:
    GUM_ACTIVE_TEST(Decomposition) {
      gum::prm::PRM< double > prm;

      gum::prm::o3prm::O3prmReader< double > reader(prm);
      reader.readFile(GET_RESSOURCES_PATH("o3prm/watertanks.o3prm"));

      if (reader.errors() > 0) { GUM_ERROR(gum::FatalError, "could not load ressource file"); }

      reader.showElegantErrorsAndWarnings();

      gum::BayesNet< double > bn;
      gum::BayesNet< double > bn2;
      gum::NodeId             node = 0;

      auto factory  = gum::BayesNetFactory< double >(&bn);
      auto factory2 = gum::BayesNetFactory< double >(&bn2);

      TS_GUM_ASSERT_THROWS_NOTHING(prm.getSystem("aSys").groundedBN(factory))
      TS_GUM_ASSERT_THROWS_NOTHING(prm.getSystem("aSys").groundedBN(factory2))

      gum::AggregatorDecomposition< double > aggregatorDecomposition;
      TS_GUM_ASSERT_THROWS_NOTHING(aggregatorDecomposition.setMaximumArity(2))
      TS_GUM_ASSERT_THROWS_NOTHING(bn = aggregatorDecomposition.getDecomposedAggregator(bn))

      try {
        auto inf = gum::LazyPropagation< double >(&bn);
        inf.makeInference();

        auto inf2 = gum::LazyPropagation< double >(&bn2);
        inf2.makeInference();

        node = bn.idFromName("city.(total_quantity)waterlevel");

        gum::Instantiation inst(inf.posterior(node));
        gum::Instantiation inst2(inf2.posterior(node));

        inst.setFirst();
        inst2.setFirst();

        while (!inst.end()) {
          TS_ASSERT_DELTA(abs(inf.posterior(node).get(inst) - inf2.posterior(node).get(inst2)),
                          0,
                          TS_GUM_SMALL_ERROR);
          inst.inc();
          inst2.inc();
        }
      } catch (gum::Exception const& e) {
        GUM_SHOWERROR(e);
        TS_FAIL("An aGrUM's exception was thrown");
      }
    }   // namespace gum_tests
  };
}   // namespace gum_tests
