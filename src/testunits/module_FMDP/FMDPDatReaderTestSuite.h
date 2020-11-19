/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *   info_at_agrum_dot_org
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


// =====================================================================
#include <iostream>
#include <string>
// =====================================================================
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// =====================================================================
#include <agrum/tools/core/set.h>
// =====================================================================
#include <agrum/tools/variables/labelizedVariable.h>
// =====================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>
// =====================================================================


namespace gum_tests {

  class FMDPDatReaderTestSuite: public CxxTest::TestSuite {
    private:
    std::string file;

    void run() {
      gum::FMDP< double >          fmdp(true);
      gum::FMDPDatReader< double > reader(&fmdp, file);

      reader.trace(false);
      auto nbrErr = gum::Size(0);
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      TS_ASSERT(nbrErr == 0);
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0);
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0);
      reader.showElegantErrorsAndWarnings();


      std::ofstream traceAlgoSaveFile__;
      traceAlgoSaveFile__.open(GET_RESSOURCES_PATH("outputs/FMDPDatRead.dot"),
                               std::ios::out | std::ios::trunc);
      if (!traceAlgoSaveFile__) return;
      TS_GUM_ASSERT_THROWS_NOTHING(traceAlgoSaveFile__ << fmdp.toString());
      traceAlgoSaveFile__.close();

      int deletedFile =
         std::remove(GET_RESSOURCES_PATH("outputs/FMDPDatRead.dot"));
      if (deletedFile != 0)
        std::cout << "Couldn't delete output file." << std::endl;


      // Only way to ensure diagrams are read correctly
      for (gum::SequenceIteratorSafe< gum::Idx > actIter = fmdp.beginActions();
           actIter != fmdp.endActions();
           ++actIter)
        for (gum::SequenceIteratorSafe< const gum::DiscreteVariable* > varIter =
                fmdp.beginVariables();
             varIter != fmdp.endVariables();
             ++varIter) {
          gum::MultiDimFunctionGraph< double >* hey =
             new gum::MultiDimFunctionGraph< double >(
                *static_cast< const gum::MultiDimFunctionGraph< double >* >(
                   fmdp.transition(*actIter, *varIter)));
          delete hey;
        }
    }

    public:
    void testConstuctor() {
      std::string file = GET_RESSOURCES_PATH("");

      gum::FMDP< double >           fmdp(true);
      gum::FMDPDatReader< double >* reader = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(
         reader = new gum::FMDPDatReader< double >(&fmdp, file));

      TS_GUM_ASSERT_THROWS_NOTHING(if (reader != nullptr) delete reader);
    }

    void testReadFileCoffeeRobot() {
      file = GET_RESSOURCES_PATH("FMDP/coffee/coffee.dat");
      run();
    }

    void testReadFileTinyFactory() {
      file = GET_RESSOURCES_PATH("FMDP/factory/tiny-factory.dat");
      run();
    }

    void testReadFileFactory() {
      file = GET_RESSOURCES_PATH("FMDP/factory/factory.dat");
      run();
    }

    void testReadFileTaxi() {
      file = GET_RESSOURCES_PATH("FMDP/taxi/taxi.dat");
      run();
    }
  };
}   // namespace gum_tests
