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
#include "testsuite_utils.h"
// =====================================================================
#include <agrum/core/approximationPolicy/approximationPolicy.h>
#include <agrum/core/approximationPolicy/linearApproximationPolicy.h>
// =====================================================================
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/multiDimDecisionDiagramFactory.h>
// =====================================================================
#include <agrum/FMDP/FactoredMarkovDecisionProcess.h>
#include <agrum/FMDP/io/dat/FMDPDatReader.h>
#include <agrum/FMDP/planning/SPUDDPlanning.h>
// =====================================================================

namespace gum {

  namespace tests {

    class SPUDDPlanningTestSuite: public CxxTest::TestSuite {
      
      private :
        std::string file;
        
        void run( const std::string showSaveFile ){

          gum::FactoredMarkovDecisionProcess<double> fmdp;
          gum::MultiDimDecisionDiagramFactory<double> ddf;
          // gum::MultiDimDecisionDiagramFactory<double,LinearApproximationPolicy> ddf;
          // ddf.setEpsilon( 0.0001 );
          // ddf.setLowLimit( -10 );
          // ddf.setHighLimit( 10 );
          SPUDDPlanning<double> inf( &fmdp, showSaveFile );

          gum::FMDPDatReader<double> reader( &fmdp, file, &ddf );

          reader.trace( false );

          TS_GUM_ASSERT_THROWS_NOTHING( reader.proceed() );
          
          
        
//           ofstream fichier(showSaveFile.c_str(), std::ios::out | std::ios::app);  //déclaration du flux et ouverture du fichier
//           
//           if(!fichier)  // si l'ouverture a réussi
//           {
//               std::cerr << "Erreur à l'ouverture !" << std::endl;
//               return;
//           }
//           fichier << fmdp.show();
//           fichier.close();
          
          TS_GUM_ASSERT_THROWS_NOTHING( inf.makePlanning() );
        }
        
      public:
        
        void test_Coffee() {
          file = GET_PATH_STR( FMDP/coffee/coffee.dat );
          run("Coffee");
        }
        
        void est_FactoryS() {
          file = GET_PATH_STR( FMDP/factory/tiny-factory.dat );
          run("TinyFactory");
        }
        
        void est_Factory() {
          file = GET_PATH_STR( FMDP/factory/factory.dat );
          run("Factory");
        }
        
        void est_Factory0() {
          file = GET_PATH_STR( FMDP/factory/factory0.dat );
          run("Factory0");
        }
        
        void est_Factory1() {
          file = GET_PATH_STR( FMDP/factory/factory1.dat );
          run("Factory1");
        }

    };
  }
}
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on;

