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

#include <cxxtest/AgrumTestSuite.h>
#include <agrum/graphs/nodeGraphPart.h>


class NodeGraphPartTestSuite: public CxxTest::TestSuite {
  public:

    void testConstructor() {
      TS_GUM_ASSERT_THROWS_NOTHING( gum::NodeGraphPart ngp );
    }

    void testInsertion() {
			gum::NodeGraphPart ngp;
			TS_ASSERT(ngp.empty());

			ngp.insertNode();
			TS_ASSERT(! ngp.empty());
			TS_ASSERT_EQUALS(ngp.size(),(gum::Size)1);

			ngp.insertNode();
			TS_ASSERT_EQUALS(ngp.size(),(gum::Size)2);

			ngp.insertNode();
			TS_ASSERT_EQUALS(ngp.size(),(gum::Size)3);
			
			gum::NodeId next=ngp.nextNodeId();
			gum::NodeId next2=ngp.insertNode();
			TS_ASSERT_EQUALS(next,next2);

			TS_GUM_ASSERT_THROWS_NOTHING(ngp.insertNode(next2+1));
			TS_ASSERT_THROWS(ngp.insertNode(next2+1),gum::DuplicateElement);
 		}

		void testSuppression() {
			gum::NodeGraphPart ngp;
			ngp.insertNode();
			ngp.insertNode();
			gum::NodeId id3=ngp.insertNode();
			ngp.insertNode();

			ngp.eraseNode(id3);
			TS_GUM_ASSERT_THROWS_NOTHING(ngp.eraseNode(id3));
			TS_ASSERT_EQUALS(ngp.size(),(gum::Size)3);

			TS_GUM_ASSERT_THROWS_NOTHING(ngp.insertNode(id3));
			TS_ASSERT_THROWS(ngp.insertNode(id3),gum::DuplicateElement);
			TS_ASSERT_EQUALS(ngp.size(),(gum::Size)4);

			ngp.clear();
			TS_ASSERT_EQUALS(ngp.size(),(gum::Size)0);
		}

		void testCopy() {
			gum::NodeGraphPart ngp;
			ngp.insertNode();
			ngp.insertNode();
			gum::NodeId id3=ngp.insertNode();
			ngp.insertNode();
			ngp.eraseNode(id3);

			gum::NodeGraphPart ngp2(ngp);
			TS_ASSERT_EQUALS(ngp.toString(),ngp2.toString());

			gum::NodeGraphPart ngp3=ngp;
			TS_ASSERT_EQUALS(ngp.toString(),ngp3.toString());

			gum::NodeGraphPart ngp4;
			TS_ASSERT(ngp4.empty());
			ngp4=ngp;
			TS_ASSERT_EQUALS(ngp.toString(),ngp3.toString());
		}
};

