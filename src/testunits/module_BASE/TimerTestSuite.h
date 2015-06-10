/***************************************************************************
 *   (C) 2007-2013 by Christophe GONZALES and Pierre-Henri WUILLEMIN       *
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
#include <testsuite_utils.h>

#include <agrum/core/timer.h>

namespace gum_tests {

  class TimerTestSuite : public CxxTest::TestSuite {
    public:
    void testConstructors() {
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Timer t1);
      TS_GUM_ASSERT_THROWS_NOTHING(gum::Timer *t2 = new gum::Timer(); delete (t2););
      {
        gum::Timer t1;
        t1.reset();

        gum::Timer *t2 = new gum::Timer();
        t2->reset();

        TS_GUM_ASSERT_THROWS_NOTHING(gum::Timer t3(*t2));

        gum::Timer *t4 = 0;
        TS_GUM_ASSERT_THROWS_NOTHING(t4 = new gum::Timer(t1));
        TS_ASSERT_DELTA(t4->step(), t1.step(), 1e-3);

        gum::Timer t3(*t2);
        TS_ASSERT_DELTA(t2->step(), t3.step(), 1e-3);

        gum::Timer t5 = t3;
        TS_ASSERT_DELTA(t5.step(), t3.step(), 1e-3);

        delete t2;
        delete t4;
      }
    }

    void testTrivial() {
      gum::Timer t;
      gum::Timer tt;
      int w = 0;

      t.reset();
      double t1 = t.step();
      test_waiting(++w);
      tt.reset();
      double t5 = t.step();
      simpleSleep(1);
      test_waiting(++w);
      double t2 = t.pause();
      simpleSleep(1);
      test_waiting(++w);
      double t3 = t.resume();
      simpleSleep(1);
      test_waiting(++w);
      double t4 = t.step();
      double t6 = tt.step();
      end_test_waiting();
      TS_ASSERT_DELTA(t6 - t5, 3.0, 1e-3);
      TS_ASSERT_DELTA(t4 - t1, 2.0, 1e-3);
      TS_ASSERT_DELTA(t3 - t2, 0.0, 1e-3);
    }

    inline void simpleSleep(double second) {
      gum::Timer t;
      t.reset();

      while (t.step() <= second)
        ;
    }
  };
}
