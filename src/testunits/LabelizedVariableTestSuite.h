/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
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

#include <iostream>
#include <string>

#include <cxxtest/AgrumTestSuite.h>
#include <agrum/multidim/labelizedVariable.h>

class LabelizedVariableTestSuite: public CxxTest::TestSuite {

  public:

    void testCopy() {
      gum::LabelizedVariable var1( "var1", "this is var1", 2 );
      gum::LabelizedVariable var2( "var2", "this is var2", 2 );

      gum::LabelizedVariable var3( var1 );
      gum::LabelizedVariable var4( "var4", "this is var4" );
      TS_GUM_ASSERT_THROWS_NOTHING( var4 = var2 );

      TS_ASSERT_EQUALS( var4, var2 );
      TS_ASSERT_EQUALS( var1, var3 );
      TS_ASSERT_DIFFERS( var4, var1 );
    }

    void testLabels() {
      gum::LabelizedVariable var1( "var1", "this is var1" , 0 );
      TS_ASSERT_EQUALS( var1.domainSize(), ( gum::Size )0 );
      TS_ASSERT( var1.empty() );
      var1.addLabel( "4" ).addLabel( "3" ).addLabel( "2" ).addLabel( "1" );

      TS_ASSERT_EQUALS( var1.domainSize(), ( gum::Size )4 );
      TS_ASSERT_EQUALS( var1.label( 1 ), "3" );
      TS_ASSERT_EQUALS( var1["3"], ( gum::Idx ) 1 );
    }
};
// kate: indent-mode cstyle; space-indent on; indent-width 2; replace-tabs on; 
