/**
 *
 *   Copyright (c) 2005-2023 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#include <gumtest/AgrumTestSuite.h>

#include <utils.h>
// =====================================================================
#include <agrum/tools/core/set.h>
// =====================================================================
#include <agrum/tools/variables/labelizedVariable.h>
// =====================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>

// =====================================================================


namespace gum_tests {

  class [[maybe_unused]] FMDPDatReaderTestSuite: public CxxTest::TestSuite {
    private:
    std::string _file_;

    void _run_() {
      gum::FMDP< double >          fmdp(true);
      gum::FMDPDatReader< double > reader(&fmdp, _file_);

      reader.trace(false);
      auto nbrErr = (gum::Size)0;
      TS_GUM_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed())

      TS_ASSERT_EQUALS(nbrErr, (gum::Size)0)
      TS_ASSERT_EQUALS(reader.warnings(), (gum::Size)0)
      TS_ASSERT_EQUALS(reader.errors(), (gum::Size)0)
      reader.showElegantErrorsAndWarnings();


      std::ofstream _traceAlgoSaveFile_;
      _traceAlgoSaveFile_.open(GET_RESSOURCES_PATH("outputs/FMDPDatRead.dot"),
                               std::ios::out | std::ios::trunc);
      if (!_traceAlgoSaveFile_) return;
      TS_GUM_ASSERT_THROWS_NOTHING(_traceAlgoSaveFile_ << fmdp.toString())
      _traceAlgoSaveFile_.close();

      int deletedFile = std::remove(GET_RESSOURCES_PATH("outputs/FMDPDatRead.dot"));
      if (deletedFile != 0) std::cout << "Couldn't delete output _file_." << std::endl;


      // Only way to ensure diagrams are read correctly
      for (gum::SequenceIteratorSafe< gum::Idx > actIter = fmdp.beginActions();
           actIter != fmdp.endActions();
           ++actIter)
        for (gum::SequenceIteratorSafe< const gum::DiscreteVariable* > varIter
             = fmdp.beginVariables();
             varIter != fmdp.endVariables();
             ++varIter) {
          gum::MultiDimFunctionGraph< double >* hey = new gum::MultiDimFunctionGraph< double >(
             *static_cast< const gum::MultiDimFunctionGraph< double >* >(
                fmdp.transition(*actIter, *varIter)));
          delete hey;
        }
    }

    public:
    GUM_ACTIVE_TEST(Constuctor) {
      std::string _file_ = GET_RESSOURCES_PATH("");

      gum::FMDP< double >           fmdp(true);
      gum::FMDPDatReader< double >* reader = nullptr;

      TS_GUM_ASSERT_THROWS_NOTHING(reader = new gum::FMDPDatReader< double >(&fmdp, _file_))

      TS_GUM_ASSERT_THROWS_NOTHING(if (reader != nullptr) delete reader)
    }

    GUM_ACTIVE_TEST(ReadFileCoffeeRobot) {
      _file_ = GET_RESSOURCES_PATH("FMDP/coffee/coffee.dat");
      _run_();
    }

    GUM_ACTIVE_TEST(ReadFileTinyFactory) {
      _file_ = GET_RESSOURCES_PATH("FMDP/factory/tiny-factory.dat");
      _run_();
    }

    GUM_ACTIVE_TEST(ReadFileFactory) {
      _file_ = GET_RESSOURCES_PATH("FMDP/factory/factory.dat");
      _run_();
    }

    GUM_ACTIVE_TEST(ReadFileTaxi) {
      _file_ = GET_RESSOURCES_PATH("FMDP/taxi/taxi.dat");
      _run_();
    }
  };
}   // namespace gum_tests
