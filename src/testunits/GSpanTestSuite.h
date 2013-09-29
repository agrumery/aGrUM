/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/AgrumTestSuite.h>
#include "testsuite_utils.h"

#include <agrum/prm/gspan.h>

#include <agrum/prm/o3prm/O3prmReader.h>


namespace gum_tests {

  class GSpanTestSuite: public CxxTest::TestSuite {
    private:
      gum::prm::o3prm::O3prmReader<double>* __driver;
      std::string dot_dir;

      gum::prm::gspan::InterfaceGraph<double>* ig;

    public:
      void setUp() {
        __driver  = new gum::prm::o3prm::O3prmReader<double>();
        __driver->readFile( GET_PATH_STR( o3prm/specialprinters.o3prm ) );
        ig = new gum::prm::gspan::InterfaceGraph<double>( __driver->prm()->system( "m" ) );
      }

      void tearDown() {
        delete ig;

        if ( __driver->prm() != 0 ) delete __driver->prm();

        delete __driver;
      }

      void testInterfaceGraph() {
        TS_ASSERT_EQUALS( ig->graph().size(), ( gum::Size ) 1 + 5*2 + 4*3 + 4*3 + 5 + 3 + 4 );
        TS_ASSERT_EQUALS( ig->graph().sizeEdges(), ( gum::Size )( 5*2 + 3*4 + 4*3 ) + 5 + 3*3 + 4*2 );
      }

      void testTree() {
        gum::prm::GSpan<double>* gspan =nullptr;
        TS_GUM_ASSERT_THROWS_NOTHING( gspan = new gum::prm::GSpan<double>( * ( __driver->prm() ), __driver->prm()->system( "m" ) ) );

        if ( gspan != 0 ) {
          TS_GUM_ASSERT_THROWS_NOTHING( gspan->discoverPatterns() );
          TS_GUM_ASSERT_THROWS_NOTHING( delete gspan );
        }
      }

  };


} // tests

