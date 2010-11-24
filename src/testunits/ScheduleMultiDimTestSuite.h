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
#include <agrum/multidim/labelizedVariable.h>
#include <agrum/multidim/scheduleMultiDim.h>
#include <agrum/multidim/potential.h>

namespace gum {

  namespace tests {

    class ScheduleMultiDimTestSuite: public CxxTest::TestSuite {
    public:
      void test_construct () {
        std::vector<LabelizedVariable*> vars ( 10 );
        for (unsigned int i = 0; i < 10; ++i) {
          std::stringstream str;
          str << "x" << i;
          std::string s = str.str();
          vars[i] = new LabelizedVariable (s, s, 4);
        }

        Sequence<const DiscreteVariable*> seq;
        seq << vars[0] << vars[2] << vars[4];
        
        gum::ScheduleMultiDim<float> f1 ( seq );
        TS_ASSERT ( f1.isAbstract () );
        TS_ASSERT_THROWS( f1.multiDim(), gum::NotFound );

        gum::ScheduleMultiDim<float> f2 ( f1 );
        TS_ASSERT ( f2.isAbstract () );
        TS_ASSERT_THROWS( f2.multiDim(), gum::NotFound );
        TS_ASSERT ( f1 == f2 );
        TS_ASSERT ( f1.id() == f2.id() );
        TS_ASSERT ( f1.variables () == f2.variables () );

        gum::ScheduleMultiDim<float> f3 ( seq );
        TS_ASSERT ( f1 != f3 );
        TS_ASSERT ( f1.id() != f3.id() );
        TS_ASSERT ( f1.variables () == f3.variables () );

        gum::Potential<float> pot;
        pot << *(vars[0]) << *(vars[2]) << *(vars[4]);
        gum::ScheduleMultiDim<float> f4 ( pot );
        TS_ASSERT ( ! f4.isAbstract () );
        TS_ASSERT ( f1 != f4 );

        gum::ScheduleMultiDim<float> f5 ( f4 );
        TS_ASSERT ( ! f5.isAbstract () );
        TS_ASSERT ( f5 == f4 );
        TS_ASSERT ( f4.id() == f5.id() );
        TS_ASSERT ( f4.multiDim() == pot );

        gum::Potential<float> pot2;
        pot2 << *(vars[1]) << *(vars[2]) << *(vars[4]);
        f4.setMultiDim ( pot2 );
        TS_ASSERT ( f5.multiDim() == pot2 );
        f3.setMultiDim ( pot2 );
        TS_ASSERT ( f3.multiDim() == pot2 );

        std::string str = f3.toString ();
        f3 = f5;
        TS_ASSERT ( f5 == f3 );
        
        for (unsigned int i = 0; i < vars.size(); ++i)
          delete vars[i];
        
       }

    };

    
  } /* namespace tests */

  
} /* namespace gum */

