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

#ifndef DV_DUMMY_LISTENERS_H
#define DV_DUMMY_LISTENERS_H

#include <string.h>

#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/discretizedVariable.h>
#include <agrum/core/hashTable.h>

#include <agrum/core/exceptions.h>

typedef gum::DiscretizedVariable<int> discretizedInt;

/**
 * A dummy gum::DiscreteVariableListener class.
 */
class DV_DummyListener: public gum::DiscreteVariableListener
{
  public:

    /*
     * For test purpose.
     */
    int domainChgCount;
    int nameChgCount;
    int descChgCount;

    DV_DummyListener( );

    virtual ~DV_DummyListener( );

    /*
     * Creates a gum::LabelizedVariable and stores it in labelVarsList.
     */
    gum::Id newLabelizedVariable(const std::string& aName,
                               const std::string& aDesc,
                               const int nbrLabel=0);

    gum::LabelizedVariable& labelizedVariable(const gum::Id& id)
      throw(gum::NotFound);

    /*
     * Creates a gum::DiscretizedVariable and stores it in discVarsList.
     */
    gum::Id newDiscretizedVariable(const std::string& aName,
                                 const std::string& aDesc);

    discretizedInt& getDiscretizedVariable(const gum::Id& id)
      throw(gum::NotFound);

    /*
     * gum::DiscreteVariableListener implementation
     */
    virtual void variableDomainChanged(gum::DiscreteVariableEvent& e);
    virtual void variableNameChanged(gum::DiscreteVariableEvent& e);
    virtual void variableDescriptionChanged(gum::DiscreteVariableEvent& e);

  private:
    gum::HashTable<gum::Id, gum::LabelizedVariable*> labelVarsTable;

    gum::HashTable<gum::Id, discretizedInt*> discVarsTable;

    gum::Id __nextId;

    gum::Id nextId();

};

#endif // DV_DUMMY_LISTENERS_H

