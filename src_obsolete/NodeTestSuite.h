/***************************************************************************
 *   Copyright (C) 2007 by Lionel Torti                                    *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cxxtest/TestSuite.h>

class NodeTestSuite: public CxxTest::TestSuite
{
    public:

        void testCreation1()
        {
            TS_ASSERT_THROWS_NOTHING(gumNode node1(1, "node 1"));
        }

        void testCreation2()
        {
            gumNode node1(1, "node 1");
            TS_ASSERT_THROWS_NOTHING(gumNode copy(node1));
        }

        void testEquality()
        {
            gumNode node1(1, "node 1");
            gumNode copy(node1);
            gumNode node2(2, "node 2");

            TS_ASSERT_EQUALS(node1, node1);
            TS_ASSERT_EQUALS(node1, copy);
            TS_ASSERT(node1 != node2);
            TS_ASSERT(copy != node2);
        }

        void testId()
        {
            gumNode node1(1, "node 1");

            TS_ASSERT_EQUALS(node1.getID(), 1);
        }


        void testLabel()
        {
            gumNode node1(1, "node 1");
            gumNode copy(node1);

            TS_ASSERT_EQUALS(node1.label(), "node 1");
            TS_ASSERT_THROWS_NOTHING(node1.setLabel("new node 1"));
            TS_ASSERT_EQUALS(node1.label(), "new node 1");

            TS_ASSERT_EQUALS(node1, copy);
            TS_ASSERT_EQUALS(copy.label(), "node 1");
        }

};

