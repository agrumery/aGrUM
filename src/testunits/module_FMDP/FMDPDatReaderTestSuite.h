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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/





// =====================================================================
#include <iostream>
#include <string>
// =====================================================================
#include <utils.h>

#include <gumtest/AgrumTestSuite.h>
// =====================================================================
#include <agrum/base/core/set.h>
// =====================================================================
#include <agrum/base/variables/labelizedVariable.h>
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
