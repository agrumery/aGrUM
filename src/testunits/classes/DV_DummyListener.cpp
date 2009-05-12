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

#include "DV_DummyListener.h"

// Construtor
DV_DummyListener::DV_DummyListener():
    domainChgCount( 0 ), nameChgCount( 0 ), descChgCount( 0 ), __nextId( 0 ) {

}

// Destructor
DV_DummyListener::~DV_DummyListener() {
  for ( gum::HashTable<gum::Id, gum::LabelizedVariable*>::iterator iter = labelVarsTable.begin();
        iter != labelVarsTable.end();
        ++iter ) {
    delete *iter;
  }

  for ( gum::HashTable<gum::Id, gum::DiscretizedVariable<int>*>::iterator iter = discVarsTable.begin();
        iter != discVarsTable.end();
        ++iter ) {
    delete *iter;
  }
}

// newLabelizedVariable
gum::Id
DV_DummyListener::newLabelizedVariable( const std::string& aName,
                                        const std::string& aDesc,
                                        const int nbrLabel ) {
  gum::Id id = nextId();
  gum::DiscreteVariableListener* listener = ( gum::DiscreteVariableListener * ) this;
  gum::LabelizedVariable* ptr = new gum::LabelizedVariable( aName, aDesc, nbrLabel );
  ptr->setDiscreteVariableListener( listener );

  labelVarsTable.insert( id, ptr );

  return id;
}

// labelizedVariable
gum::LabelizedVariable&
DV_DummyListener::labelizedVariable( const gum::Id& id )
throw( gum::NotFound ) {
  gum::LabelizedVariable* ptr = labelVarsTable[id];

  return *ptr;
}

// newDiscretizedVariable
gum::Id
DV_DummyListener::newDiscretizedVariable( const std::string& aName,
    const std::string& aDesc ) {
  gum::Id id = nextId();
  gum::DiscreteVariableListener* listener = ( gum::DiscreteVariableListener * ) this;
  discretizedInt* ptr = new discretizedInt( aName, aDesc );
  ptr->setDiscreteVariableListener( listener );

  discVarsTable.insert( id, ptr );

  return id;
}

// labelizedVariable
discretizedInt&
DV_DummyListener::getDiscretizedVariable( const gum::Id& id )
throw( gum::NotFound ) {
  discretizedInt* ptr = discVarsTable[id];

  return *ptr;
}

// variableDomainChanged
void
DV_DummyListener::variableDomainChanged( gum::DiscreteVariableEvent& ) {
  domainChgCount++;
}

// variableNameChanged
void
DV_DummyListener::variableNameChanged( gum::DiscreteVariableEvent& ) {
  nameChgCount++;
}

// variableDescriptionChanged
void
DV_DummyListener::variableDescriptionChanged( gum::DiscreteVariableEvent& ) {
  descChgCount++;
}

// __nextId
gum::Id
DV_DummyListener::nextId() {
  return __nextId++;
}

