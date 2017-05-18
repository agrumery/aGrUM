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
 *   (gumSize) 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.   *
 ***************************************************************************/
#include <iostream>
#include <string>
#include <vector>

#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/BN/BayesNet.h>
#include <agrum/BN/algorithms/MarkovBlanket.h>

namespace gum_tests {
  class MarkovBlanketTestSuite : public CxxTest::TestSuite {
    public:
    void testChain() {
      auto bn = gum::BayesNet<int>::fastPrototype( "a->b->c" );
      {
        auto mb = gum::MarkovBlanket( bn, "a" );

        TS_ASSERT_EQUALS( mb.size(), 2u );
        TS_ASSERT_EQUALS( mb.sizeArcs(), 1u );
      }
      {
        auto mb = gum::MarkovBlanket( bn, 0 );

        TS_ASSERT_EQUALS( mb.size(), 2u );
        TS_ASSERT_EQUALS( mb.sizeArcs(), 1u );
      }

      {
        auto mb = gum::MarkovBlanket( bn, "b" );

        TS_ASSERT_EQUALS( mb.size(), 3u );
        TS_ASSERT_EQUALS( mb.sizeArcs(), 2u );
      }
      {
        auto mb = gum::MarkovBlanket( bn, 1 );

        TS_ASSERT_EQUALS( mb.size(), 3u );
        TS_ASSERT_EQUALS( mb.sizeArcs(), 2u );
      }

      {
        auto mb = gum::MarkovBlanket( bn, "c" );

        TS_ASSERT_EQUALS( mb.size(), 2u );
        TS_ASSERT_EQUALS( mb.sizeArcs(), 1u );
      }
      {
        auto mb = gum::MarkovBlanket( bn, 2 );

        TS_ASSERT_EQUALS( mb.size(), 2u );
        TS_ASSERT_EQUALS( mb.sizeArcs(), 1u );
      }
    }
  };

}  // gum_tests
