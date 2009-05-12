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

#include <cxxtest/TestSuite.h>
#include <agrum/core/list.h>
#include <agrum/multidim/discreteVariable.h>
#include <agrum/multidim/labelizedVariable.h>

#include "classes/DV_DummyListener.h"

class LabelizedVariableListenerTestSuite: public CxxTest::TestSuite
{
  
  public:
    void testCreation()
    {
      TS_GUM_ASSERT_THROWS_NOTHING(DV_DummyListener listener); 
    }

    void testCreateVarsAndInsertion()
    {
      DV_DummyListener *listener = new DV_DummyListener();
      gum::List<gum::Id> idList;

      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(listener->newLabelizedVariable("a", "The variable a", 2)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(listener->newLabelizedVariable("b", "The variable b", 2)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(listener->newLabelizedVariable("c", "The variable c", 2)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(listener->newLabelizedVariable("d", "The variable d", 2)));
      TS_GUM_ASSERT_THROWS_NOTHING(idList.insert(listener->newLabelizedVariable("e", "The variable e", 2)));

      for (gum::List<gum::Id>::iterator iter = idList.begin();
           iter != idList.end();
           ++iter)
      {
        TS_GUM_ASSERT_THROWS_NOTHING(listener->labelizedVariable(*iter));
      }

      TS_ASSERT_EQUALS(listener->labelizedVariable(idList[0]).name(), "a");
      TS_ASSERT_EQUALS(listener->labelizedVariable(idList[1]).name(), "b");
      TS_ASSERT_EQUALS(listener->labelizedVariable(idList[2]).name(), "c");
      TS_ASSERT_EQUALS(listener->labelizedVariable(idList[3]).name(), "d");
      TS_ASSERT_EQUALS(listener->labelizedVariable(idList[4]).name(), "e");

      TS_ASSERT_THROWS_ANYTHING(listener->labelizedVariable(42));

      delete listener;
    }

    void testDomainChg()
    {
      DV_DummyListener listener;
      gum::List<gum::Id> list;
      fill(listener, list);

      gum::LabelizedVariable& var = listener.labelizedVariable(list[0]);
      TS_GUM_ASSERT_THROWS_NOTHING(var.addLabel("new label"));
      TS_ASSERT_EQUALS(listener.domainChgCount, 1);

      for (int i = 1; i < 5; i++)
      {
        TS_GUM_ASSERT_THROWS_NOTHING(listener.labelizedVariable(list[i]).addLabel("new Label"));
      }

      TS_ASSERT_EQUALS(listener.domainChgCount, 5);
    }

    void testNameChg()
    {
      DV_DummyListener listener;
      gum::List<gum::Id> list;
      fill(listener, list);

      gum::LabelizedVariable& var = listener.labelizedVariable(list[0]);
      TS_GUM_ASSERT_THROWS_NOTHING(var.setName("foo-" + var.name()));
      TS_ASSERT_EQUALS(listener.nameChgCount, 1);

      for (int i = 1; i < 5; i++)
      {
        gum::LabelizedVariable& var = listener.labelizedVariable(list[i]);
        TS_GUM_ASSERT_THROWS_NOTHING(var.setName("foo-" + var.name()));
      }

      TS_ASSERT_EQUALS(listener.nameChgCount, 5);
    }

    void testDescChg()
    {
      DV_DummyListener listener;
      gum::List<gum::Id> list;
      fill(listener, list);

      gum::LabelizedVariable& var = listener.labelizedVariable(list[0]);
      TS_GUM_ASSERT_THROWS_NOTHING(var.setDescription("bar! " + var.description()));
      TS_ASSERT_EQUALS(listener.descChgCount, 1);

      for (int i = 1; i < 5; i++)
      {
        gum::LabelizedVariable& var = listener.labelizedVariable(list[i]);
        TS_GUM_ASSERT_THROWS_NOTHING(var.setDescription("bar! " + var.description()));
      }

      TS_ASSERT_EQUALS(listener.descChgCount, 5);
    }

    void testCantChgListener()
    {
      DV_DummyListener listener1;
      DV_DummyListener listener2;
      gum::List<gum::Id> list;

      fill(listener1, list);

      gum::LabelizedVariable& var = listener1.labelizedVariable(list[0]);
      TS_ASSERT_THROWS_ANYTHING(var.setDiscreteVariableListener(&listener2));
    }


  private:
    void fill(DV_DummyListener& listener, gum::List<gum::Id>& idList)
    {
      idList.insert(listener.newLabelizedVariable("a", "The varial a", 2));
      idList.insert(listener.newLabelizedVariable("b", "The varial b", 2));
      idList.insert(listener.newLabelizedVariable("c", "The varial c", 2));
      idList.insert(listener.newLabelizedVariable("d", "The varial d", 2));
      idList.insert(listener.newLabelizedVariable("e", "The varial e", 2));
    }

};

