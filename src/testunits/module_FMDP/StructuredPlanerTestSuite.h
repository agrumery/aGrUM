
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


// ==============================================================================
#include <iostream>
#include <string>
// ==============================================================================
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// ==============================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>
#include <agrum/FMDP/planning/structuredPlaner.h>
// ==============================================================================

namespace gum_tests {

  class StructuredPlanerTestSuite : public CxxTest::TestSuite {
    private:
    std::string file;

    void run() {
      gum::FMDP< double >              fmdp(true);
      gum::StructuredPlaner< double >* planer = nullptr;
      TS_GUM_ASSERT_THROWS_NOTHING(
         planer =
            gum::StructuredPlaner< double >::spumddInstance(0.9, 0.01, false));

      gum::FMDPDatReader< double > reader(&fmdp, file);
      TS_GUM_ASSERT_THROWS_NOTHING(reader.trace(false));
      TS_GUM_ASSERT_THROWS_NOTHING(reader.proceed());

      TS_GUM_ASSERT_THROWS_NOTHING(planer->initialize(&fmdp));
      TS_GUM_ASSERT_THROWS_NOTHING(planer->makePlanning(10));


      std::ofstream __traceAlgoSaveFile;
      __traceAlgoSaveFile.open(GET_RESSOURCES_PATH("FMDP/PlannedPolicy.dot"),
                               std::ios::out | std::ios::trunc);
      if (!__traceAlgoSaveFile) return;
      TS_GUM_ASSERT_THROWS_NOTHING(__traceAlgoSaveFile << fmdp.toString());
      __traceAlgoSaveFile.close();

      int deletedFile = std::remove(GET_RESSOURCES_PATH("FMDP/PlannedPolicy.dot"));
      if (deletedFile != 0)
        std::cout << "Couldn't delete output file." << std::endl;

      TS_GUM_ASSERT_THROWS_NOTHING(delete planer);
    }

    public:
    void testPlanningCoffee() {
      file = GET_RESSOURCES_PATH("FMDP/coffee/coffee.dat");
      run();
    }

    void testPlanningTinyFactory() {
      file = GET_RESSOURCES_PATH("FMDP/factory/tiny-factory.dat");
      run();
    }

    void testPlanningTaxi() {
      file = GET_RESSOURCES_PATH("FMDP/taxi/taxi.dat");
      run();
    }
  };
}   // namespace gum_tests
