/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr                                                  *
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
// ============================================================================
#include <cxxtest/AgrumTestSuite.h>

#include <agrum/prm/skool/SkoolReader.h>

#include "testsuite_utils.h"


namespace gum_tests {

  class PRMReaderTestSuite: public CxxTest::TestSuite {
    public:
      void /*test*/BasicRead() {
        gum::prm::skool::SkoolReader reader;
        int res;
        TS_GUM_ASSERT_THROWS_NOTHING ( res=reader.readFile ( GET_PATH_STR ( skool/inference.skool ) ) );

        if ( res!=0 ) reader.showElegantErrors();

        TS_ASSERT_EQUALS ( res,0 );

        gum::prm::PRM* prm;
        TS_GUM_ASSERT_THROWS_NOTHING ( prm = reader.prm() );
        TS_ASSERT ( prm!= ( gum::prm::PRM* ) 0 );

        delete ( prm );
      }

      void testRegression1() {
        gum::prm::skool::SkoolReader reader;
        int res;
        TS_GUM_ASSERT_THROWS_NOTHING ( res=reader.readFile ( GET_PATH_STR ( skool/withinterfaces.skool ) ) );

        if ( res!=0 ) reader.showElegantErrors();

        TS_ASSERT_EQUALS ( res,0 );

        gum::prm::PRM* prm;
        TS_GUM_ASSERT_THROWS_NOTHING ( prm = reader.prm() );
        TS_ASSERT ( prm!= ( gum::prm::PRM* ) 0 );
        delete ( prm );
      }

      void /*test*/Regression2() {
        gum::prm::skool::SkoolReader reader;
        int res;
        TS_GUM_ASSERT_THROWS_NOTHING ( res=reader.readFile ( GET_PATH_STR ( skool/foo2.skool ) ) );

        //if ( res!=0 ) reader.showElegantErrors();

        TS_ASSERT_EQUALS ( res,0 );

        gum::prm::PRM* prm;
        TS_GUM_ASSERT_THROWS_NOTHING ( prm = reader.prm() );
        TS_ASSERT ( prm!= ( gum::prm::PRM* ) 0 );

        delete ( prm );
      }
  };

} // namespace gum_tests
