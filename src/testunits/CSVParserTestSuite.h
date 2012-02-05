/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
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

#define GUM_TRACE_ON

#include <cxxtest/AgrumTestSuite.h>

#include <agrum/config.h>
#include <agrum/learning/CSVParser/CSVParser.h>

#include "testsuite_utils.h"

namespace gum {

  namespace tests {

    class CSVParserTestSuite: public CxxTest::TestSuite {
      private:
      public:
        void testSimpleCSV() {
          std::string csv("1,2,3,4 \n 5,6,7,8 \n 9,10,11,12");

          std::istringstream in(csv);
          gum::CSVParser parser(in);

          // no data parser for now
          TS_ASSERT_THROWS(parser.currentData(),gum::NullElement)

          GUM_TRACE("here");
          gum::Size count=0;
          std::string res="";

          while (parser.next()) {
            count++;
            const std::vector<std::string>& v=parser.currentData();

            for (gum::Idx i=0;i<v.size();i++) res+=v[i];
          }

          TS_ASSERT_EQUALS(count,(Size)3);

          TS_ASSERT_EQUALS(res,std::string("123456789101112"));
        };
	
        void testSimpleCSVwithComment() {
          std::string csv("1,2,3,4 \n # this is a comment \n 5,6,7,8 \n 9,10,11,12");

          std::istringstream in(csv);
          gum::CSVParser parser(in);

          // no data parser for now
          TS_ASSERT_THROWS(parser.currentData(),gum::NullElement)

          GUM_TRACE("here");
          gum::Size count=0;
          std::string res="";

          while (parser.next()) {
            count++;
            const std::vector<std::string>& v=parser.currentData();

            for (gum::Idx i=0;i<v.size();i++) res+=v[i];
          }

          TS_ASSERT_EQUALS(count,(Size)3);

          TS_ASSERT_EQUALS(res,std::string("123456789101112"));
        };
    };
  }
}
