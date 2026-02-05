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

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  FMDPDatReader
#define GUM_CURRENT_MODULE FMDP

// =====================================================================


namespace gum_tests {

  struct FMDPDatReaderTestSuite {
    private:
    std::string _file_;

    void _run_() {
      gum::FMDP< double >          fmdp(true);
      gum::FMDPDatReader< double > reader(&fmdp, _file_);

      reader.trace(false);
      auto nbrErr = static_cast< gum::Size >(0);
      GUM_CHECK_ASSERT_THROWS_NOTHING(nbrErr = reader.proceed());

      CHECK((nbrErr) == (static_cast< gum::Size >(0)));
      CHECK((reader.warnings()) == (static_cast< gum::Size >(0)));
      CHECK((reader.errors()) == (static_cast< gum::Size >(0)));
      reader.showElegantErrorsAndWarnings();


      std::ofstream _traceAlgoSaveFile_;
      _traceAlgoSaveFile_.open(GET_RESSOURCES_PATH("outputs/FMDPDatRead.dot"),
                               std::ios::out | std::ios::trunc);
      if (!_traceAlgoSaveFile_) return;
      GUM_CHECK_ASSERT_THROWS_NOTHING(_traceAlgoSaveFile_ << fmdp.toString());
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
    void testConstuctor() {
      std::string _file_ = GET_RESSOURCES_PATH("");

      gum::FMDP< double >           fmdp(true);
      gum::FMDPDatReader< double >* reader = nullptr;

      GUM_CHECK_ASSERT_THROWS_NOTHING(reader = new gum::FMDPDatReader< double >(&fmdp, _file_));

      GUM_CHECK_ASSERT_THROWS_NOTHING(if (reader != nullptr) delete reader);
    }

    void testReadFileCoffeeRobot() {
      _file_ = GET_RESSOURCES_PATH("FMDP/coffee/coffee.dat");
      _run_();
    }

    void testReadFileTinyFactory() {
      _file_ = GET_RESSOURCES_PATH("FMDP/factory/tiny-factory.dat");
      _run_();
    }

    void testReadFileFactory() {
      _file_ = GET_RESSOURCES_PATH("FMDP/factory/factory.dat");
      _run_();
    }

    void testReadFileTaxi() {
      _file_ = GET_RESSOURCES_PATH("FMDP/taxi/taxi.dat");
      _run_();
    }
  };

  GUM_TEST_ACTIF(Constuctor)
  GUM_TEST_ACTIF(ReadFileCoffeeRobot)
  GUM_TEST_ACTIF(ReadFileTinyFactory)
  GUM_TEST_ACTIF(ReadFileFactory)
  GUM_TEST_ACTIF(ReadFileTaxi)
}   // namespace gum_tests
