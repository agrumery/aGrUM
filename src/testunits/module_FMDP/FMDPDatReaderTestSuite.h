/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// =====================================================================
#include <iostream>
#include <string>
// =====================================================================
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// =====================================================================
#include <agrum/core/set.h>
// =====================================================================
#include <agrum/variables/labelizedVariable.h>
// =====================================================================
#include <agrum/FMDP/fmdp.h>
#include <agrum/FMDP/io/dat/fmdpDatReader.h>
// =====================================================================


namespace gum_tests {

  class FMDPDatReaderTestSuite : public CxxTest::TestSuite {

    private:
    std::string file;

    void run() {

      gum::FMDP<float> fmdp( true );
      gum::FMDPDatReader<float> reader( &fmdp, file );

      reader.trace( false );
      int nbrErr = 0;
      TS_GUM_ASSERT_THROWS_NOTHING( nbrErr = reader.proceed() );

      TS_ASSERT( nbrErr == 0 );
      TS_ASSERT_EQUALS( reader.warnings(), (gum::Size)0 );
      TS_ASSERT_EQUALS( reader.errors(), (gum::Size)0 );
      reader.showElegantErrorsAndWarnings();


      std::ofstream __traceAlgoSaveFile;
      __traceAlgoSaveFile.open( GET_RESSOURCES_PATH( "FMDP/FMDPDatRead.dot" ),
                                std::ios::out | std::ios::trunc );
      if ( !__traceAlgoSaveFile ) return;
      TS_GUM_ASSERT_THROWS_NOTHING( __traceAlgoSaveFile << fmdp.toString() );
      __traceAlgoSaveFile.close();

      int deletedFile =
          std::remove( GET_RESSOURCES_PATH( "FMDP/FMDPDatRead.dot" ) );
      if ( deletedFile != 0 )
        std::cout << "Couldn't delete output file." << std::endl;


      // Only way to ensure diagrams are read correctly
      for ( gum::SequenceIteratorSafe<gum::Idx> actIter = fmdp.beginActions();
            actIter != fmdp.endActions();
            ++actIter )
        for ( gum::SequenceIteratorSafe<const gum::DiscreteVariable*> varIter =
                  fmdp.beginVariables();
              varIter != fmdp.endVariables();
              ++varIter ) {
          gum::MultiDimFunctionGraph<float>* hey =
              new gum::MultiDimFunctionGraph<float>(
                  *static_cast<const gum::MultiDimFunctionGraph<float>*>(
                      fmdp.transition( *actIter, *varIter ) ) );
          delete hey;
        }
    }

    public:
    void testConstuctor() {
      std::string file = GET_RESSOURCES_PATH( "" );

      gum::FMDP<float> fmdp( true );
      gum::FMDPDatReader<float>* reader;

      TS_GUM_ASSERT_THROWS_NOTHING(
          reader = new gum::FMDPDatReader<float>( &fmdp, file ) );

      TS_GUM_ASSERT_THROWS_NOTHING( delete reader );
    }

    void testReadFileCoffeeRobot() {
      file = GET_RESSOURCES_PATH( "FMDP/coffee/coffee.dat" );
      run();
    }

    void testReadFileTinyFactory() {
      file = GET_RESSOURCES_PATH( "FMDP/factory/tiny-factory.dat" );
      run();
    }

    void testReadFileFactory() {
      file = GET_RESSOURCES_PATH( "FMDP/factory/factory.dat" );
      run();
    }

    void testReadFileTaxi() {
      file = GET_RESSOURCES_PATH( "FMDP/taxi/taxi.dat" );
      run();
    }
  };
}
