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
#include <agrum/FMDP/FMDP.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
// =====================================================================


namespace gum_tests {

  class FMDPDatReaderTestSuite: public CxxTest::TestSuite {

    private :

      std::string file;

      void run( ) {

        gum::FMDP<float> fmdp(true);
        gum::FMDPDatReader<float> reader ( &fmdp, file ) ;

        reader.trace ( false );
        int nbrErr = 0;
        TS_GUM_ASSERT_THROWS_NOTHING ( nbrErr = reader.proceed() );

        TS_ASSERT ( nbrErr == 0 );
        TS_ASSERT_EQUALS ( reader.warnings(), ( gum::Size ) 0 );
        TS_ASSERT_EQUALS ( reader.errors(), ( gum::Size ) 0 );
        reader.showElegantErrorsAndWarnings();


        std::ofstream __traceAlgoSaveFile;
        __traceAlgoSaveFile.open ( GET_PATH_STR ( "test.dot" ), std::ios::out | std::ios::trunc );
        if ( !__traceAlgoSaveFile )  return;
        TS_GUM_ASSERT_THROWS_NOTHING ( __traceAlgoSaveFile << fmdp.show() );
//        std::cout << fmdp.show();
        __traceAlgoSaveFile.close();

      }

    public:


      void testConstuctor() {
        std::string file = GET_PATH_STR ( "FMDP/coffee/coffee.dat" );

        gum::FMDP<float> fmdp;

        gum::FMDPDatReader<float>* reader = 0;

        TS_GUM_ASSERT_THROWS_NOTHING ( reader = new gum::FMDPDatReader<float> ( &fmdp, file ) );

        TS_GUM_ASSERT_THROWS_NOTHING ( delete reader );

      }

      void test_Coffee() {
        file = GET_PATH_STR ( "FMDP/coffee/coffee.dat" );
        run ();
      }

      void test_FactoryS() {
        file = GET_PATH_STR ( "FMDP/factory/tiny-factory.dat" );
        run ();
      }

      void est_Maze() {
        file = GET_PATH_STR ( "FMDP/labyrinth/maze.dat" );
        run ();
      }


      void est_MazeB() {
        file = GET_PATH_STR ( "FMDP/labyrinth/mazeb.dat" );
        run ();
      }


      void test_Factory() {
        file = GET_PATH_STR ( "FMDP/factory/factory.dat" );
        run ();
      }

      void test_FactoryB() {
        file = GET_PATH_STR ( "FMDP/factory/factoryB.dat" );
        run ();
      }


      void test_Factory0() {
        file = GET_PATH_STR ( "FMDP/factory/factory0.dat" );
        run ();
      }

      void test_Factory0B() {
        file = GET_PATH_STR ( "FMDP/factory/factory0B.dat" );
        run ();
      }


      void test_Factory1() {
        file = GET_PATH_STR ( "FMDP/factory/factory1.dat" );
        run ();
      }

      void test_Factory1B() {
        file = GET_PATH_STR ( "FMDP/factory/factory1B.dat" );
        run ();
      }


      void test_Factory2() {
        file = GET_PATH_STR ( "FMDP/factory/factory2.dat" );
        run ();
      }

      void test_Factory2B() {
        file = GET_PATH_STR ( "FMDP/factory/factory2B.dat" );
        run ();
      }


      void test_Factory3() {
        file = GET_PATH_STR ( "FMDP/factory/factory3.dat" );
        run ();
      }

      void test_Factory3B() {
        file = GET_PATH_STR ( "FMDP/factory/factory3B.dat" );
        run ();
      }

  };
}
