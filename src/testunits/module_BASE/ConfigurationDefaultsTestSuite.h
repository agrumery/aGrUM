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


#include <cmath>
#include <cstdint>
#include <vector>

#include <agrum/base/core/configuration/configuration.h>

#include <testunits/gumtest/AgrumTestSuite.h>
#include <testunits/gumtest/utils.h>

namespace gum_tests {

  // The default values of the keys of gum::Configuration that replaced constants of the code
  // (numerical tolerances, parameters of the approximation schemes, of the learning algorithms and
  // of the display of the tensors) are the values that were written in the code before.
  //
  // This test is meant to fail if a default value of defaults.ini is changed: changing a default
  // is a change of behavior of aGrUM, which must be intended and written in the CHANGELOG. In this
  // case, update the table below together with the CHANGELOG.
  struct ConfigurationDefaultsTestSuite {
    public:
    template < typename VALUE >
    struct Expected {
      const char* section;
      const char* key;
      VALUE       value;
    };
  };

  GUM_TEST(DefaultsAreTheHistoricalValues) {
    auto& conf = gum::Configuration::instance();

    // the defaults of the schema, whatever agrum.ini or the current state of the configuration
    ConfigurationGuard guard;

    for (const auto& e: std::vector< ConfigurationDefaultsTestSuite::Expected< std::int64_t > >{
             {"core", "maxNumberOfThreads", -1},
             {"core", "jgumIndent", -1},
             {"approximation", "maxIter", 10000},
             {"approximation", "burnIn", 0},
             {"approximation", "periodSize", 1},
             {"sampling", "maxIter", 10000000},
             {"sampling", "periodSize", 100},
             {"gibbs", "burnIn", 300},
             {"gibbs", "percentDrawnSample", 50},
             {"lbp", "maxIter", 100},
             {"lbp", "periodSize", 1},
             {"loopySampling", "virtualLBPSize", 5000},
             {"gibbsKL", "maxIter", 10000000},
             {"gibbsKL", "periodSize", 200},
             {"gibbsKL", "burnIn", 2000},
             {"gibbsKL", "percentDrawnSample", 10},
             {"mcKL", "maxIter", 10000000},
             {"mcKL", "periodSize", 200},
             {"learning", "tabuListSize", 100},
             {"learning", "tabuNbDecrease", 2},
             {"tensor", "maxRowsConsole", 6},
             {"tensor", "columnWidthConsole", 9},
             {"notebook", "junctionTreeSeparatorFontSize", 8},
             {"notebook", "junctionTreeCliqueFontSize", 10},
         }) {
      INFO(e.section << "." << e.key);
      CHECK_EQ(conf.typed< std::int64_t >(e.section, e.key), e.value);
    }

    for (const auto& e: std::vector< ConfigurationDefaultsTestSuite::Expected< double > >{
             {"core", "epsilonCoarse", 1e-2},
             {"core", "epsilonStandard", 1e-6},
             {"core", "epsilonFine", 1e-10},
             {"approximation", "epsilon", 0.05},
             {"approximation", "minEpsilonRate", 0.01},
             {"approximation", "maxTime", 1.0},
             {"sampling", "epsilon", 0.01},
             {"sampling", "maxTime", 6000.0},
             {"sampling", "minEpsilonRate", 1e-5},
             {"gibbs", "epsilon", std::exp(-1.6)},
             {"gibbs", "minEpsilonRate", std::exp(-5)},
             {"lbp", "epsilon", 1e-8},
             {"lbp", "minEpsilonRate", 1e-10},
             {"gibbsKL", "epsilon", 1e-10},
             {"gibbsKL", "minEpsilonRate", 1e-10},
             {"gibbsKL", "maxTime", 6000.0},
             {"mcKL", "epsilon", 1e-10},
             {"mcKL", "minEpsilonRate", 1e-10},
             {"mcKL", "maxTime", 6000.0},
             {"learning", "alpha", 0.05},
             {"learning", "priorWeight", 1.0},
             {"learning", "emNoise", 0.1},
             {"notebook", "junctionTreeMapCliqueScale", 0.3},
             {"notebook", "junctionTreeMapSepScale", 0.1},
             {"notebook", "junctionTreeMapEdgeLen", 1.0},
             {"factorgraph", "edgeLength", 0.7},
         }) {
      INFO(e.section << "." << e.key);
      // a tolerance : some values are written as decimal literals of a computed constant
      CHECK_LT(std::abs(conf.typed< double >(e.section, e.key) - e.value), 1e-15 * (1. + e.value));
    }

    for (const auto& e: std::vector< ConfigurationDefaultsTestSuite::Expected< bool > >{
             {"core", "allowModificationWhenSaving", false},
             {"approximation", "verbosity", false},
             {"sampling", "verbosity", false},
             {"gibbs", "drawnAtRandom", true},
             {"lbp", "verbosity", false},
             {"gibbsKL", "verbosity", false},
             {"gibbsKL", "drawnAtRandom", true},
             {"mcKL", "verbosity", false},
             {"notebook", "junctionTreeWithNames", true},
             {"causal", "showLatentNames", false},
         }) {
      INFO(e.section << "." << e.key);
      CHECK_EQ(conf.typed< bool >(e.section, e.key), e.value);
    }

    for (const auto& e: std::vector< ConfigurationDefaultsTestSuite::Expected< std::string > >{
             {"learning", "BNLearnerPreferredAlgo", "MIIC"},
             // look of the graphs (toDot): the values that were in the defaults of pyAgrum
             {"notebook", "arcColor", "#4A4A4A"},
             {"notebook", "nodeBgColor", "#404040"},
             {"notebook", "nodeFgColor", "adaptive"},
             {"notebook", "graphSize", "5"},
             {"notebook", "graphRankDir", "TB"},
             {"notebook", "graphLayout", "dot"},
             {"notebook", "markovRandomFieldView", "factorgraph"},
             {"notebook", "junctionTreeGraphSize", "10"},
             {"notebook", "junctionTreeSeparatorBgColor", "palegreen"},
             {"notebook", "junctionTreeSeparatorFgColor", "adaptive"},
             {"notebook", "junctionTreeCliqueBgColor", "burlywood"},
             {"notebook", "junctionTreeCliqueFgColor", "adaptive"},
             {"notebook", "junctionTreeMapSize", "10"},
             {"factorgraph", "nodeBgColor", "coral"},
             {"factorgraph", "nodeFgColor", "adaptive"},
             {"factorgraph", "factorBgColor", "burlywood"},
             {"factorgraph", "graphLayout", "neato"},
             {"dynamicBN", "graphSize", "6"},
             {"dynamicBN", "timeSliceBgColor", "#DDDDDD"},
             {"dynamicBN", "replicatedSliceBgColor", "lightcyan"},
             {"dynamicBN", "atemporalBgColor", "lightyellow"},
             {"influenceDiagram", "graphSize", "6"},
             {"influenceDiagram", "chanceBgColor", "#808080"},
             {"influenceDiagram", "chanceFgColor", "adaptive"},
             {"influenceDiagram", "utilityBgColor", "#50508A"},
             {"influenceDiagram", "utilityFgColor", "adaptive"},
             {"influenceDiagram", "decisionBgColor", "#9A5050"},
             {"influenceDiagram", "decisionFgColor", "adaptive"},
             {"influenceDiagram", "chanceShape", "ellipse"},
             {"influenceDiagram", "utilityShape", "hexagon"},
             {"influenceDiagram", "decisionShape", "box"},
             {"influenceDiagram", "decisionArcStyle", "tapered, bold, dotted"},
             {"influenceDiagram", "utilityArcStyle", "dashed"},
             {"credalnet", "nodeBgColor", "#404040"},
             {"credalnet", "nodeFgColor", "adaptive"},
             {"causal", "graphSize", "2.5"},
             {"causal", "nodeBgColor", "#404040"},
             {"causal", "nodeFgColor", "adaptive"},
             {"causal", "latentBgColor", "#A08080"},
             {"causal", "latentFgColor", "adaptive"},
         }) {
      INFO(e.section << "." << e.key);
      CHECK_EQ(conf.typed< std::string >(e.section, e.key), e.value);
    }
  }
}   // namespace gum_tests
