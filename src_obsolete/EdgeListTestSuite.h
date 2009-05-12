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
#include <cxxtest/TestSuite.h>
#include <agrum/gumGraphElements.h>
#include <agrum/gumEdgeList.h>

class EdgeListTestSuite: public CxxTest::TestSuite
{

    public:
        gumEdge *edge1, *edge2, *edge3, *edge4, *edge5, *edge6, *edge7, *edge8;

        void setUp()
        {
            edge1 = new gumEdge(1,2);
            edge2 = new gumEdge(2,3);
            edge3 = new gumEdge(3,4);
            edge4 = new gumEdge(4,5);
            edge5 = new gumEdge(5,6);
            edge6 = new gumEdge(6,7);
            edge7 = new gumEdge(7,8);
            edge8 = new gumEdge(9,9);
        }

        void tearDown()
        {
            delete edge1;
            delete edge2;
            delete edge3;
            delete edge4;
            delete edge5;
            delete edge6;
            delete edge7;
            delete edge8;
        }

        void testConstructor1()
        {
            TS_ASSERT_THROWS_NOTHING(gumEdgeList<gumEdge> edgeList);
        }

        void testConstructor2()
        {
            gumEdgeList<gumEdge> edgeList;
            TS_ASSERT_THROWS_NOTHING(gumEdgeList<gumEdge> copy(edgeList));
        }

        void testInsert()
        {
            gumEdgeList<gumEdge> edgeList;

            TS_ASSERT_THROWS_NOTHING(edgeList.insert(*edge1));
            TS_ASSERT_THROWS_NOTHING(edgeList.insert(*edge2));
            TS_ASSERT_THROWS_NOTHING(edgeList.insert(*edge3));
            TS_ASSERT_THROWS_NOTHING(edgeList.insert(*edge4));
            TS_ASSERT_THROWS_NOTHING(edgeList.insertEdge(*edge5));
            TS_ASSERT_THROWS_NOTHING(edgeList.insertEdge(*edge6));
            TS_ASSERT_THROWS_NOTHING(edgeList.insertEdge(*edge7));
            TS_ASSERT_THROWS_NOTHING(edgeList.insertEdge(*edge8));

            TS_ASSERT_EQUALS(edgeList.size(), 8);
        }

        void testGet()
        {
            gumEdgeList<gumEdge> edgeList;
            fill(edgeList);

            TS_ASSERT_EQUALS(edgeList.get(1, 2), *edge1);
            TS_ASSERT_EQUALS(edgeList.get(7, 6), *edge6);

            TS_ASSERT_EQUALS(edgeList.getEdge(4, 5), *edge4);
            TS_ASSERT_EQUALS(edgeList.getEdge(3, 2), *edge2);

            TS_ASSERT_THROWS_ANYTHING(edgeList.get(24,42));
        }

        void testErase1()
        {
            gumEdgeList<gumEdge> edgeList;
            fill(edgeList);

            TS_ASSERT_THROWS_NOTHING(edgeList.erase(1, 2));
            TS_ASSERT_THROWS_NOTHING(edgeList.erase(8, 7));
            TS_ASSERT_THROWS_NOTHING(edgeList.erase(1, 2));
            TS_ASSERT_THROWS_NOTHING(edgeList.erase(2, 1));

            TS_ASSERT_EQUALS(edgeList.size(), 6);

            TS_ASSERT_THROWS_NOTHING(edgeList.erase(*edge2));
            TS_ASSERT_THROWS_NOTHING(edgeList.erase(*edge3));
            TS_ASSERT_THROWS_NOTHING(edgeList.erase(*edge3));

            TS_ASSERT_EQUALS(edgeList.size(), 4);
        }

        void testExist()
        {
            gumEdgeList<gumEdge> edgeList;
            fill(edgeList);

            TS_ASSERT(edgeList.exists(1,2));
            TS_ASSERT(edgeList.exists(6,5));

            TS_ASSERT(! edgeList.exists(42,2));
            TS_ASSERT(! edgeList.exists(4,23424));
        }

        void testClear()
        {
            gumEdgeList<gumEdge> edgeList;
            TS_ASSERT(edgeList.empty());
            edgeList.insert(*edge1);
            edgeList.insert(*edge2);
            edgeList.insert(*edge3);
            edgeList.insert(*edge4);

            TS_ASSERT_EQUALS(edgeList.size(), 4);
            TS_ASSERT_THROWS_NOTHING(edgeList.clear());
            TS_ASSERT(edgeList.empty());
        }

        void testneighbours1()
        {
            gumEdgeList<gumEdge> edgeList;
            fill(edgeList);

            gumList< gumRefPtr<gumEdge> > neighbourList;
            TS_ASSERT_THROWS_NOTHING(neighbourList = edgeList.getneighbours(3));
            TS_ASSERT_EQUALS(neighbourList.size(), 2);
            gumEdge e1 = *(neighbourList.front());
            gumEdge e2 = *(neighbourList.back());

            TS_ASSERT((e1 == *edge2) || (e1 == *edge3));
            TS_ASSERT((e2 == *edge2) || (e2 == *edge3));
            TS_ASSERT(e1 != e2);

            TS_ASSERT_THROWS_NOTHING(neighbourList = edgeList.getneighbours(42));
            TS_ASSERT(neighbourList.empty());
        }

        void testneighbours2()
        {
            gumEdgeList<gumEdge> edgeList;
            fill(edgeList);

            TS_ASSERT_THROWS_NOTHING(edgeList.eraseneighbours(4));
            TS_ASSERT_EQUALS(edgeList.size(), 6);
            TS_ASSERT(! edgeList.exists(*edge3));
            TS_ASSERT(! edgeList.exists(*edge4));

            TS_ASSERT_THROWS_NOTHING(edgeList.eraseneighbours(42));
            TS_ASSERT_EQUALS(edgeList.size(), 6);
        }

        void testEquality()
        {
            gumEdgeList<gumEdge> edgeList1;
            fill(edgeList1);
            gumEdgeList<gumEdge> copy(edgeList1);
            gumEdgeList<gumEdge> edgeList2;
            fill(edgeList2);

            TS_ASSERT_EQUALS(edgeList1, copy);
            TS_ASSERT_EQUALS(copy, edgeList1);
            TS_ASSERT_EQUALS(edgeList2, edgeList1);

            gumEdgeList<gumEdge> edgeList3;
            edgeList3.insert(*edge1);
            edgeList3.insert(*edge3);
            edgeList3.insert(*edge5);
            edgeList3.insert(*edge7);

            TS_ASSERT(edgeList1 != edgeList3);
            TS_ASSERT(edgeList3 != edgeList1);
        }

        void testCopy()
        {
            gumEdgeList<gumEdge> edgeList;
            gumEdgeList<gumEdge> copy1(edgeList);
            fill(edgeList);
            gumEdgeList<gumEdge> copy2(edgeList);

            TS_ASSERT_EQUALS(edgeList, copy2);
            TS_ASSERT_DIFFERS(edgeList, copy1);
            TS_ASSERT(copy1.empty());

            for (gumEdgeList<gumEdge>::iterator iter = edgeList.begin(); iter != edgeList.end(); ++iter)
                TS_ASSERT(copy2.exists(*iter));
        }

        void testShallowCopy()
        {
            gumEdgeList<gumEdge> edgeList;
            gumEdgeList<gumEdge> scopy1;
            scopy1.shallowCopy(edgeList);
            fill(edgeList);
            gumEdgeList<gumEdge> scopy2;
            scopy2.shallowCopy(edgeList);

            TS_ASSERT(! scopy1.shallowEqual(edgeList));
            TS_ASSERT(scopy1.shallowDifferent(edgeList));

            TS_ASSERT(scopy2.shallowEqual(edgeList));
            TS_ASSERT(! scopy2.shallowDifferent(edgeList));
        }

        void testShallowErase()
        {
            gumEdgeList<gumEdge> source;
            gumEdgeList<gumEdge> scopy1;
            gumEdgeList<gumEdge> scopy2;

            fill(source);
            TS_ASSERT_THROWS_NOTHING(scopy1.shallowCopy(source));
            TS_ASSERT_THROWS_NOTHING(scopy2.shallowCopy(source));

            gumSize sourceSize = source.size();
            gumSize scopy1Size = scopy1.size();
            gumSize scopy2Size = scopy2.size();

            TS_ASSERT(source.shallowEqual(scopy1));
            TS_ASSERT(source.shallowEqual(scopy2));
            TS_ASSERT(scopy2.shallowEqual(scopy1));

            int i = 0;
            for (gumEdgeList<gumEdge>::iterator iter = source.begin();
                 iter != source.end();
                 ++iter)
            {
                if ((i % 2) == 0) {
                    scopy1.erase(*iter);
                } else {
                    scopy2.erase(*iter);
                }
                i++;
            }

            TS_ASSERT(scopy1.shallowDifferent(source));
            TS_ASSERT(scopy2.shallowDifferent(source));
            TS_ASSERT(scopy1.shallowDifferent(scopy2));

            TS_ASSERT_EQUALS(sourceSize, source.size());
            TS_ASSERT_EQUALS(sourceSize, scopy1.size() + scopy2.size());
            TS_ASSERT_DIFFERS(scopy1Size, scopy1.size());
            TS_ASSERT_DIFFERS(scopy2Size, scopy2.size());

        }

        void testShallowInsert()
        {
            gumEdgeList<gumEdge> source;
            gumEdgeList<gumEdge> scopy1;
            gumEdgeList<gumEdge> scopy2;
            gumEdgeList<gumEdge> scopy3;

            TS_ASSERT_THROWS_NOTHING(scopy1.shallowCopy(source));
            TS_ASSERT_THROWS_NOTHING(scopy2.shallowCopy(source));

            TS_ASSERT_THROWS_NOTHING(scopy3.shallowCopy(source));
            fill(source);

            int sourceSize = source.size();

            int i = 0;
            for (gumEdgeList<gumEdge>::iterator iter = source.begin();
                 iter != source.end();
                 ++iter)
            {
                if ((i % 2) == 0) {
                    TS_ASSERT_THROWS_NOTHING(scopy1.shallowInsert(source.shallowGet(*iter)));
                } else {
                    TS_ASSERT_THROWS_NOTHING(scopy2.shallowInsert(source.shallowGet(*iter)));
                }

                int node1 = (*iter).second(); // Second on purpose.
                int node2 = (*iter).first();
                TS_ASSERT_THROWS_NOTHING(scopy3.shallowInsert(source.shallowGet(node1, node2)));
                i++;
            }

            TS_ASSERT(scopy3.shallowEqual(source));
            TS_ASSERT(scopy1.shallowDifferent(source));
            TS_ASSERT(scopy2.shallowDifferent(source));
            TS_ASSERT(scopy1.shallowDifferent(scopy2));

            TS_ASSERT_EQUALS(scopy1.size() + scopy2.size(), scopy3.size());
            TS_ASSERT_EQUALS(source.size(), sourceSize);
        }

    private:
        void fill(gumEdgeList<gumEdge> &edgeList)
        {
            edgeList.insert(*edge1);
            edgeList.insert(*edge2);
            edgeList.insert(*edge3);
            edgeList.insert(*edge4);
            edgeList.insert(*edge5);
            edgeList.insert(*edge6);
            edgeList.insert(*edge7);
            edgeList.insert(*edge8);
        }
};

