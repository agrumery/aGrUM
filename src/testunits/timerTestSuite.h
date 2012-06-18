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
#include <unistd.h>

#include <cxxtest/AgrumTestSuite.h>
#include <agrum/core/timer.h>

namespace gum_tests {

  class TimerTestSuite: public CxxTest::TestSuite {
    public:
      void testConstructors() {
        TS_GUM_ASSERT_THROWS_NOTHING ( gum::Timer t1 );
        TS_GUM_ASSERT_THROWS_NOTHING ( gum::Timer *t2 = new gum::Timer(); delete ( t2 ); );
        {
          gum::Timer t1; t1.reset();
          gum::Timer *t2 = new gum::Timer(), *t4;
          t2->reset();
          TS_GUM_ASSERT_THROWS_NOTHING ( gum::Timer t3 ( *t2 ) );
          TS_GUM_ASSERT_THROWS_NOTHING ( t4 = new gum::Timer ( t1 ) );
          t4->reset();

          delete t2;
          delete t4;
        }
      }

      void testTrivial() {
        gum::Timer t;
        gum::Timer tt;
        t.reset();
        double t1 = t.step();
        tt.reset();
        double t5 = t.step();
        sleep ( 1 );
        double t2 = t.pause();
        sleep ( 1 );
        double t3 = t.resume();
        sleep ( 1 );
        double t4 = t.step();
        double t6 = tt.step();

        TS_ASSERT ( ( t6 - t5 >= 3.0 ) && ( t6 - t5 < 3.002 ) );
        TS_ASSERT ( ( t4 - t1 >= 2.0 ) && ( t4 - t1 < 2.002 ) );
        TS_ASSERT ( ( t3 - t2 >= 0.0 ) && ( t3 - t2 < 0.002 ) );  

      }
  };
}
