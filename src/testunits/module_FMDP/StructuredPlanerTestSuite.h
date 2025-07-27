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


// ==============================================================================
#include <iostream>
#include <string>
// ==============================================================================
#include <utils.h>

#include <gumtest/AgrumTestSuite.h>
// ==============================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>
#include <agrum/FMDP/planning/structuredPlaner.h>

// ==============================================================================

namespace gum_tests {

  class [[maybe_unused]] StructuredPlanerTestSuite: public CxxTest::TestSuite {
    private:
    std::string file;

    void run() {
      gum::FMDP< double >              fmdp(true);
      gum::StructuredPlaner< double >* planer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
          planer = gum::StructuredPlaner< double >::spumddInstance(0.9, 0.01, false));

      gum::FMDPDatReader< double > reader(&fmdp, file);
      TS_GUM_ASSERT_THROWS_NOTHING(reader.trace(false))
      TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed())

      TS_GUM_ASSERT_THROWS_NOTHING(planer->initialize(&fmdp))
      TS_GUM_ASSERT_THROWS_NOTHING(planer->makePlanning(10))


      std::ofstream _traceAlgoSaveFile_;
      _traceAlgoSaveFile_.open(GET_RESSOURCES_PATH("outputs/PlannedPolicy.dot"),
                               std::ios::out | std::ios::trunc);
      if (!_traceAlgoSaveFile_) return;
      TS_GUM_ASSERT_THROWS_NOTHING(_traceAlgoSaveFile_ << fmdp.toString())
      _traceAlgoSaveFile_.close();

      int deletedFile = std::remove(GET_RESSOURCES_PATH("outputs/PlannedPolicy.dot"));
      if (deletedFile != 0) std::cout << "Couldn't delete output file." << std::endl;

      TS_GUM_ASSERT_THROWS_NOTHING(delete planer)
    }

    public:
    GUM_ACTIVE_TEST(PlanningCoffee) {
      file = GET_RESSOURCES_PATH("FMDP/coffee/coffee.dat");
      run();
    }

    GUM_ACTIVE_TEST(PlanningTinyFactory) {
      file = GET_RESSOURCES_PATH("FMDP/factory/tiny-factory.dat");
      run();
    }

    GUM_ACTIVE_TEST(PlanningTaxi) {
      file = GET_RESSOURCES_PATH("FMDP/taxi/taxi.dat");
      run();
    }
  };
}   // namespace gum_tests
