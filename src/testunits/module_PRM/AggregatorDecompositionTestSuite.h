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


#include <algorithm>
#include <ctime>
#include <iostream>
#include <string>
// #include <experimental/filesystem>

#include <agrum/base/variables/discretizedVariable.h>
#include <agrum/base/variables/rangeVariable.h>
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
#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {
  struct AggregatorDecompositionTestSuite {
    public:
    // namespace gum_tests
  };

  GUM_TEST(Decomposition) {
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

    GUM_CHECK_ASSERT_THROWS_NOTHING(prm.getSystem("aSys").groundedBN(factory));
    GUM_CHECK_ASSERT_THROWS_NOTHING(prm.getSystem("aSys").groundedBN(factory2));

    gum::AggregatorDecomposition< double > aggregatorDecomposition;
    GUM_CHECK_ASSERT_THROWS_NOTHING(aggregatorDecomposition.setMaximumArity(2));
    GUM_CHECK_ASSERT_THROWS_NOTHING(bn = aggregatorDecomposition.getDecomposedAggregator(bn));

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
        CHECK((abs(inf.posterior(node).get(inst) - inf2.posterior(node).get(inst2)))
              == doctest::Approx(0).epsilon(GUM_SMALL_ERROR));
        inst.inc();
        inst2.inc();
      }
    } catch (gum::Exception const& e) {
      GUM_SHOWERROR(e);
      FAIL("An aGrUM's exception was thrown");
    }
  }

  // Y = AGG(A, B, C, D) with A, B in [0, 1] and C, D in [10, 11]
  static gum::BayesNet< double > heterogeneousRangeAggregator(const std::string& agg) {
    gum::BayesNet< double > bn;
    gum::RangeVariable a("A", "", 0, 1), b("B", "", 0, 1), c("C", "", 10, 11), d("D", "", 10, 11);
    gum::NodeId        y;
    if (agg == "min") y = bn.addMIN(gum::RangeVariable("Y", "", 0, 11));
    else if (agg == "max") y = bn.addMAX(gum::RangeVariable("Y", "", 0, 11));
    else y = bn.addSUM(gum::RangeVariable("Y", "", 0, 24));
    for (const auto& v: {a, b, c, d})
      bn.addArc(bn.add(v), y);
    bn.cpt("A").fillWith({0.3, 0.7});
    bn.cpt("B").fillWith({0.6, 0.4});
    bn.cpt("C").fillWith({0.2, 0.8});
    bn.cpt("D").fillWith({0.9, 0.1});
    return bn;
  }

  GUM_TEST(DecompositionMinMaxIntermediateRanges) {
    // issue #198: the range of an intermediate MIN (resp. MAX) node must be [min of the parents'
    // lower bounds, min of the parents' upper bounds] (resp. with max), not the sum of the bounds
    for (const std::string agg: {"min", "max"}) {
      auto                                   bn = heterogeneousRangeAggregator(agg);
      gum::AggregatorDecomposition< double > decomposition;
      decomposition.setMaximumArity(2);
      decomposition.getDecomposedAggregator(bn);

      gum::Size nb_intermediates = 0;
      for (const auto node: bn.topologicalOrder()) {
        const auto& var = bn.variable(node);
        if (var.name().rfind("Y_", 0) != 0) continue;
        ++nb_intermediates;
        const auto& range = static_cast< const gum::RangeVariable& >(var);
        bool        first = true;
        long        lo = 0, hi = 0;
        for (const auto parent: bn.parents(node)) {
          const auto& prange = static_cast< const gum::RangeVariable& >(bn.variable(parent));
          if (first) {
            lo    = prange.minVal();
            hi    = prange.maxVal();
            first = false;
          } else if (agg == "min") {
            lo = std::min(lo, prange.minVal());
            hi = std::min(hi, prange.maxVal());
          } else {
            lo = std::max(lo, prange.minVal());
            hi = std::max(hi, prange.maxVal());
          }
        }
        CHECK_MESSAGE(range.minVal() == lo, agg << " " << var.name());
        CHECK_MESSAGE(range.maxVal() == hi, agg << " " << var.name());
      }
      CHECK_GT(nb_intermediates, gum::Size(0));
    }
  }

  GUM_TEST(DecompositionHeterogeneousRangesKeepsPosterior) {
    // issue #198: the decomposed aggregator must give the same distribution as the original one
    for (const std::string agg: {"min", "max", "sum"}) {
      auto                                   original   = heterogeneousRangeAggregator(agg);
      auto                                   decomposed = heterogeneousRangeAggregator(agg);
      gum::AggregatorDecomposition< double > decomposition;
      decomposition.setMaximumArity(2);
      decomposition.getDecomposedAggregator(decomposed);

      gum::LazyPropagation< double > ie_original(&original);
      ie_original.makeInference();
      gum::LazyPropagation< double > ie_decomposed(&decomposed);
      ie_decomposed.makeInference();

      const auto&        p1 = ie_original.posterior("Y");
      const auto&        p2 = ie_decomposed.posterior("Y");
      gum::Instantiation i1(p1), i2(p2);
      for (i1.setFirst(), i2.setFirst(); !i1.end(); i1.inc(), i2.inc())
        CHECK_MESSAGE(p1.get(i1) == doctest::Approx(p2.get(i2)).epsilon(GUM_SMALL_ERROR),
                      agg << " " << i1);
    }
  }
}   // namespace gum_tests
