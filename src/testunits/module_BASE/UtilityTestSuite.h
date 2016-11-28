/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}@lip6.fr   *
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
#include <cxxtest/AgrumTestSuite.h>
#include <cxxtest/testsuite_utils.h>

#include <agrum/variables/discretizedVariable.h>
#include <agrum/variables/labelizedVariable.h>

#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/utilityTable.h>

namespace gum_tests {

  class UtilityTestSuite : public CxxTest::TestSuite {
    public:
    void testCreation() {
      gum::UtilityTable<double> p( new gum::MultiDimArray<double>() );
      TS_ASSERT( p.empty() );

      gum::LabelizedVariable a( "a", "first var", 2 ), b( "b", "second var", 4 ),
          c( "c", "third var", 5 );
      TS_GUM_ASSERT_THROWS_NOTHING( p << a << b << c );
    }

    void testFactory() {
      gum::UtilityTable<double> p1( new gum::MultiDimArray<double>() );
      TS_GUM_ASSERT_THROWS_NOTHING( delete p1.newFactory() );
    }
  };
}
