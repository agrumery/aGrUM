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

// ==============================================================================
#include <iostream>
#include <string>
// ==============================================================================
#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>
// ==============================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
#include <agrum/FMDP/planning/spumdd.h>
#include <agrum/FMDP/planning/pspumdd.h>
#include <agrum/FMDP/planning/svi.h>
// ==============================================================================

namespace gum_tests {

  class SPUDDPlanningTestSuite: public CxxTest::TestSuite {

    private :
      std::string file;

      void run ( const std::string showSaveFile ) {

        gum::FactoredMarkovDecisionProcess<double> fmdp(true);
        gum::SPUMDD<double> planer ( &fmdp );//, 10 ); // Epsilon is set high, indeed we just want ot check that the algorithm works fine.
//        gum::SVI<double> planer(&fmdp);

        gum::FMDPDatReader<double> reader ( &fmdp, file );
        TS_GUM_ASSERT_THROWS_NOTHING ( reader.trace ( false ) );
        TS_GUM_ASSERT_THROWS_NOTHING ( reader.proceed( ) );

        TS_GUM_ASSERT_THROWS_NOTHING ( planer.initialize() );
        TS_GUM_ASSERT_THROWS_NOTHING ( planer.makePlanning(10000) );

        std::cout << fmdp.size() << "\t" << planer.vFunction()->realSize() << "\t" << planer.optimalPolicy()->realSize() << std::endl;
        std::cout << planer.optimalPolicy2String() << std::endl;
      }

    public:

      void test_Coffee() {
        file = GET_PATH_STR ( "FMDP/coffee/coffee.dat" );
        run ( "Coffee" );
      }

      void est_FactoryS() {
        file = GET_PATH_STR ( "FMDP/factory/tiny-factory.dat" );
        run ( "TinyFactory" );
      }

      void est_Maze() {
        file = GET_PATH_STR ( "FMDP/labyrinth/maze.dat" );
        run ( "maze" );
      }


      void est_MazeB() {
        file = GET_PATH_STR ( "FMDP/labyrinth/mazeb.dat" );
        run ( "mazeb" );
      }


      void est_Factory() {
        file = GET_PATH_STR ( "FMDP/factory/factory.dat" );
        run ( "Factory" );
      }

      void est_FactoryB() {
        file = GET_PATH_STR ( "FMDP/factory/factoryB.dat" );
        run ( "FactoryB" );
      }


      void est_Factory0() {
        file = GET_PATH_STR ( "FMDP/factory/factory0.dat" );
        run ( "Factory0" );
      }

      void est_Factory0B() {
        file = GET_PATH_STR ( "FMDP/factory/factory0B.dat" );
        run ( "Factory0B" );
      }


      void est_Factory1() {
        file = GET_PATH_STR ( "FMDP/factory/factory1.dat" );
        run ( "Factory1" );
      }

      void est_Factory1B() {
        file = GET_PATH_STR ( "FMDP/factory/factory1B.dat" );
        run ( "Factory1B" );
      }


      void est_Factory2() {
        file = GET_PATH_STR ( "FMDP/factory/factory2.dat" );
        run ( "Factory2" );
      }

      void est_Factory2B() {
        file = GET_PATH_STR ( "FMDP/factory/factory2B.dat" );
        run ( "Factory2B" );
      }


      void est_Factory3() {
        file = GET_PATH_STR ( "FMDP/factory/factory3.dat" );
        run ( "Factory3" );
      }

      void est_Factory3B() {
        file = GET_PATH_STR ( "FMDP/factory/factory3B.dat" );
        run ( "Factory3B" );
      }

  };
}
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
