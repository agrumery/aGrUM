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

class ArcListTestSuite: public CxxTest::TestSuite
{

    public:
        gumArc *arc1, *arc2, *arc3, *arc4, *arc5, *arc6, *arc7, *arc8;
        gumArc *revArc1, *revArc2, *revArc3, *revArc4;

        void setUp()
        {
            arc1 = new gumArc(1,2);
            arc2 = new gumArc(2,3);
            arc3 = new gumArc(3,4);
            arc4 = new gumArc(4,5);
            arc5 = new gumArc(5,6);
            arc6 = new gumArc(6,7);
            arc7 = new gumArc(7,8);
            arc8 = new gumArc(9,9);

            revArc1 = new gumArc(2,1);  // Reverse Arc
            revArc2 = new gumArc(3,2);
            revArc3 = new gumArc(4,3);
            revArc4 = new gumArc(5,4);
        }

        void tearDown()
        {
            delete arc1;
            delete arc2;
            delete arc3;
            delete arc4;
            delete arc5;
            delete arc6;
            delete arc7;
            delete arc8;

            delete revArc1;
            delete revArc2;
            delete revArc3;
            delete revArc4;
        }

        void testConstructor1()
        {
            TS_ASSERT_THROWS_NOTHING(gumArcList<gumArc> arcList);
        }

        void testConstructor2()
        {
            gumArcList<gumArc> arcList;
            TS_ASSERT_THROWS_NOTHING(gumArcList<gumArc> copy(arcList));
        }

        void testInsert()
        {
            gumArcList<gumArc> arcList;

            TS_ASSERT_THROWS_NOTHING(arcList.insert(*arc1));
            TS_ASSERT_THROWS_NOTHING(arcList.insert(*arc2));
            TS_ASSERT_THROWS_NOTHING(arcList.insert(*arc3));
            TS_ASSERT_THROWS_NOTHING(arcList.insert(*arc4));
            TS_ASSERT_THROWS_NOTHING(arcList.insertArc(*arc5));
            TS_ASSERT_THROWS_NOTHING(arcList.insertArc(*arc6));
            TS_ASSERT_THROWS_NOTHING(arcList.insertArc(*arc7));
            TS_ASSERT_THROWS_NOTHING(arcList.insertArc(*arc8));

            TS_ASSERT_THROWS_NOTHING(arcList.insertArc(*revArc1));
            TS_ASSERT_THROWS_NOTHING(arcList.insertArc(*revArc2));
            TS_ASSERT_THROWS_NOTHING(arcList.insertArc(*revArc3));
            TS_ASSERT_THROWS_NOTHING(arcList.insertArc(*revArc4));

            TS_ASSERT_EQUALS(arcList.size(), (gumSize)12);
        }

        void testGet()
        {
            gumArcList<gumArc> arcList;
            fill(arcList);

            TS_ASSERT_EQUALS(arcList.get(1, 2), *arc1);
            TS_ASSERT_DIFFERS(arcList.get(1,2), *revArc1);
            TS_ASSERT_EQUALS(arcList.get(2,1), *revArc1);
            TS_ASSERT_EQUALS(arcList.get(6, 7), *arc6);

            TS_ASSERT_EQUALS(arcList.getArc(4, 5), *arc4);
            TS_ASSERT_EQUALS(arcList.getArc(3, 2), *revArc2);

            TS_ASSERT_THROWS_ANYTHING(arcList.get(8,7));
            TS_ASSERT_THROWS_ANYTHING(arcList.get(24,42));
        }

        void testErase1()
        {
            gumArcList<gumArc> arcList;
            fill(arcList);

            TS_ASSERT_THROWS_NOTHING(arcList.erase(1, 2));  // arc1
            TS_ASSERT_THROWS_NOTHING(arcList.erase(8, 7));  // nothing
            TS_ASSERT_THROWS_NOTHING(arcList.erase(1, 2));  // nothing
            TS_ASSERT_THROWS_NOTHING(arcList.erase(2, 1));  // revArc1

            TS_ASSERT_EQUALS(arcList.size(), (gumSize)10);

            TS_ASSERT_THROWS_NOTHING(arcList.erase(*arc2));
            TS_ASSERT_THROWS_NOTHING(arcList.erase(*arc3));
            TS_ASSERT_THROWS_NOTHING(arcList.erase(*arc3));

            TS_ASSERT_EQUALS(arcList.size(), (gumSize)8);
        }

        void testExist()
        {
            gumArcList<gumArc> arcList;
            fill(arcList);

            TS_ASSERT(arcList.exists(1, 2));
            TS_ASSERT(arcList.exists(4, 3));
            TS_ASSERT(! (arcList.exists(6, 5)));

            TS_ASSERT(! arcList.exists(42, 2));
            TS_ASSERT(! arcList.exists(4, 23424));
        }

        void testClear()
        {
            gumArcList<gumArc> arcList;
            TS_ASSERT(arcList.empty());
            arcList.insert(*arc1);
            arcList.insert(*arc2);
            arcList.insert(*arc3);
            arcList.insert(*arc4);

            TS_ASSERT_EQUALS(arcList.size(), (gumSize)4);
            TS_ASSERT_THROWS_NOTHING(arcList.clear());
            TS_ASSERT(arcList.empty());
        }

        void testGetParents()
        {
            gumArcList<gumArc> arcList;
            fill(arcList);

            gumList< gumRefPtr< gumArc > > parentList = arcList.getParents(2);
            // Node 2 have 1 and 3 has parents.
            TS_ASSERT_EQUALS(parentList.size(), (gumSize)2);
            gumArc parent1 = *(parentList.front());
            gumArc parent2 = *(parentList.back());

            TS_ASSERT((parent1.first() == 1) || (parent1.first() == 3));
            TS_ASSERT((parent2.first() == 1) || (parent2.first() == 3));
        }


        void testGetChildren()
        {
            gumArcList<gumArc> arcList;
            fill(arcList);

            gumList< gumRefPtr< gumArc > > childList = arcList.getChildren(5);
            // Node 5 have 6 and 4 has childs.
            TS_ASSERT_EQUALS(childList.size(), 2);
            gumArc child1 = *(childList.front());
            gumArc child2 = *(childList.back());

            TS_ASSERT((child1.second() == 4) || (child1.second() == 6));
            TS_ASSERT((child2.second() == 4) || (child2.second() == 6));
        }

        void testEquality()
        {
            gumArcList<gumArc> arcList1;
            fill(arcList1);
            gumArcList<gumArc> copy(arcList1);
            gumArcList<gumArc> arcList2;
            fill(arcList2);

            TS_ASSERT_EQUALS(arcList1, copy);
            TS_ASSERT_EQUALS(copy, arcList1);
            TS_ASSERT_EQUALS(arcList2, arcList1);

            gumArcList<gumArc> arcList3;
            arcList3.insert(*arc1);
            arcList3.insert(*arc3);
            arcList3.insert(*arc5);
            arcList3.insert(*arc7);

            TS_ASSERT(arcList1 != arcList3);
            TS_ASSERT(arcList3 != arcList1);
        }

        void testCopy()
        {
            gumArcList<gumArc> arcList;
            gumArcList<gumArc> copy1(arcList);
            fill(arcList);
            gumArcList<gumArc> copy2(arcList);

            TS_ASSERT_EQUALS(arcList, copy2);
            TS_ASSERT_DIFFERS(arcList, copy1);
            TS_ASSERT(copy1.empty());

            for (gumArcList<gumArc>::iterator iter = arcList.begin(); iter != arcList.end(); ++iter)
                TS_ASSERT(copy2.exists(*iter));
        }

        void testShallowCopy()
        {
            gumArcList<gumArc> arcList;
            gumArcList<gumArc> scopy1;
            scopy1.shallowCopy(arcList);
            fill(arcList);
            gumArcList<gumArc> scopy2;
            scopy2.shallowCopy(arcList);

            TS_ASSERT(! scopy1.shallowEqual(arcList));
            TS_ASSERT(scopy1.shallowDifferent(arcList));

            TS_ASSERT(scopy2.shallowEqual(arcList));
            TS_ASSERT(! scopy2.shallowDifferent(arcList));
        }

        void testShallowErase()
        {
            gumArcList<gumArc> source;
            gumArcList<gumArc> scopy1;
            gumArcList<gumArc> scopy2;

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
            for (gumArcList<gumArc>::iterator iter = source.begin();
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
            gumArcList<gumArc> source;
            gumArcList<gumArc> scopy1;
            gumArcList<gumArc> scopy2;
            gumArcList<gumArc> scopy3;

            TS_ASSERT_THROWS_NOTHING(scopy1.shallowCopy(source));
            TS_ASSERT_THROWS_NOTHING(scopy2.shallowCopy(source));

            TS_ASSERT_THROWS_NOTHING(scopy3.shallowCopy(source));
            fill(source);

            int sourceSize = source.size();

            int i = 0;
            for (gumArcList<gumArc>::iterator iter = source.begin();
                 iter != source.end();
                 ++iter)
            {
                if ((i % 2) == 0) {
                    TS_ASSERT_THROWS_NOTHING(scopy1.shallowInsert(source.shallowGet(*iter)));
                } else {
                    TS_ASSERT_THROWS_NOTHING(scopy2.shallowInsert(source.shallowGet(*iter)));
                }

                int node1 = (*iter).first();
                int node2 = (*iter).second();
                if (source.exists(node1, node2) && !(source.exists(node2, node1)))
                {
                    TS_ASSERT_THROWS_ANYTHING(scopy3.shallowInsert(source.shallowGet(node2, node1)));
                    TS_ASSERT_THROWS_NOTHING(scopy3.shallowInsert(source.shallowGet(node1, node2)));
                }
                else if (source.exists(node2, node1))
                {
                    TS_ASSERT_THROWS_NOTHING(scopy3.shallowInsert(source.shallowGet(node2, node1)));
                }
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
        void fill(gumArcList<gumArc> &arcList)
        {
            arcList.insert(*arc1);
            arcList.insert(*arc2);
            arcList.insert(*arc3);
            arcList.insert(*arc4);
            arcList.insert(*arc5);
            arcList.insert(*arc6);
            arcList.insert(*arc7);
            arcList.insert(*arc8);

            arcList.insert(*revArc1);
            arcList.insert(*revArc2);
            arcList.insert(*revArc3);
            arcList.insert(*revArc4);
        }
};

