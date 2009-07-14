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

#include <agrum/core/list.h>

#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/discretizedVariable.h>

#include "classes/DV_DummyListener.h"

class DiscretizedVariableListenerTestSuite: public CxxTest::TestSuite {

  public:
    void testCreation() {
      TS_GUM_ASSERT_THROWS_NOTHING( DV_DummyListener listener );
    }

    void testCreateVarsAndInsertion() {
      DV_DummyListener *listener = new DV_DummyListener();
      gum::List<gum::Id> idList;

      fill( *listener,idList );

      for ( gum::List<gum::Id>::iterator iter = idList.begin();
            iter != idList.end();
            ++iter ) {
        TS_GUM_ASSERT_THROWS_NOTHING( listener->getDiscretizedVariable( *iter ) );
      }

      TS_ASSERT_EQUALS( listener->getDiscretizedVariable( idList[0] ).name(), "a" );

      TS_ASSERT_EQUALS( listener->getDiscretizedVariable( idList[1] ).name(), "b" );
      TS_ASSERT_EQUALS( listener->getDiscretizedVariable( idList[2] ).name(), "c" );
      TS_ASSERT_EQUALS( listener->getDiscretizedVariable( idList[3] ).name(), "d" );
      TS_ASSERT_EQUALS( listener->getDiscretizedVariable( idList[4] ).name(), "e" );

      TS_ASSERT_THROWS_ANYTHING( listener->getDiscretizedVariable( 42 ) );

      delete listener;
    }

    void testDomainChg() {
      DV_DummyListener *listener = new DV_DummyListener();
      gum::List<gum::Id> list;
      fill( *listener, list );
      //gum::DiscretizedVariable<int>& var = listener->getDiscretizedVariable( list[0] );
      TS_GUM_ASSERT_THROWS_NOTHING( listener->getDiscretizedVariable( list[0] ).addTick( 1 ) );
      TS_ASSERT_EQUALS( listener->domainChgCount, 1 );

      for ( int i = 1; i < 5; i++ ) {
        TS_GUM_ASSERT_THROWS_NOTHING( listener->getDiscretizedVariable( list[i] ).addTick( i ) );
      }

      TS_ASSERT_EQUALS( listener->domainChgCount, 5 );

      delete listener;
    }

    void testNameChg() {
      DV_DummyListener listener;
      gum::List<gum::Id> list;
      fill( listener, list );

      gum::DiscretizedVariable<int>& var = listener.getDiscretizedVariable( list[0] );
      TS_GUM_ASSERT_THROWS_NOTHING( var.setName( "foo-" + var.name() ) );
      TS_ASSERT_EQUALS( listener.nameChgCount, 1 );

      for ( int i = 1; i < 5; i++ ) {
        gum::DiscretizedVariable<int>& var = listener.getDiscretizedVariable( list[i] );
        TS_GUM_ASSERT_THROWS_NOTHING( var.setName( "foo-" + var.name() ) );
      }

      TS_ASSERT_EQUALS( listener.nameChgCount, 5 );
    }

    void testDescChg() {
      DV_DummyListener listener;
      gum::List<gum::Id> list;
      fill( listener, list );

      gum::DiscretizedVariable<int>& var = listener.getDiscretizedVariable( list[0] );
      TS_GUM_ASSERT_THROWS_NOTHING( var.setDescription( "bar! " + var.description() ) );
      TS_ASSERT_EQUALS( listener.descChgCount, 1 );

      for ( int i = 1; i < 5; i++ ) {
        gum::DiscretizedVariable<int>& var = listener.getDiscretizedVariable( list[i] );
        TS_GUM_ASSERT_THROWS_NOTHING( var.setDescription( "bar! " + var.description() ) );
      }

      TS_ASSERT_EQUALS( listener.descChgCount, 5 );
    }

    void testCantChgListener() {

      DV_DummyListener listener1;
      DV_DummyListener listener2;
      gum::List<gum::Id> list;

      fill( listener1, list );

      gum::DiscretizedVariable<int>& var = listener1.getDiscretizedVariable( list[0] );
      TS_ASSERT_THROWS_ANYTHING( var.setDiscreteVariableListener( &listener2 ) );
    }


  private:
    void fill( DV_DummyListener& listener, gum::List<gum::Id>& idList ) {
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( listener.newDiscretizedVariable( "a", "The varial a" ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( listener.newDiscretizedVariable( "b", "The varial b" ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( listener.newDiscretizedVariable( "c", "The varial c" ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( listener.newDiscretizedVariable( "d", "The varial d" ) ) );
      TS_GUM_ASSERT_THROWS_NOTHING( idList.insert( listener.newDiscretizedVariable( "e", "The varial e" ) ) );
    }

};

