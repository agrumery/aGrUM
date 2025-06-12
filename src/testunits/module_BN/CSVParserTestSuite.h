/***************************************************************************
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)   *
 *   info_at_agrum_dot_org                                               *
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

#include <gumtest/AgrumTestSuite.h>
#include <gumtest/utils.h>

#include <agrum/base/database/CSVParser.h>

namespace gum_tests {

  class [[maybe_unused]] CSVParserTestSuite: public CxxTest::TestSuite {
    private:
    std::size_t _noParsedLine_;

    std::size_t testParseString(std::string csvstring, std::string& res) {
      std::istringstream       in(csvstring);
      gum::learning::CSVParser parser(in, "(stream)");

      // no data parser for now
      TS_ASSERT_THROWS(parser.current(), const gum::NullElement&)
      TS_ASSERT_THROWS(parser.nbLine(), const gum::NullElement&)

      std::size_t count = 0;
      res               = "";

      while (parser.next()) {
        count++;
        const std::vector< std::string >& v = parser.current();

        for (std::size_t i = std::size_t(0); i < v.size(); i++) {
          res += v[i];
          res += (i == v.size() - 1) ? '|' : ':';
        }
      }

      _noParsedLine_ = parser.nbLine();
      return count;
    }

    public:
    GUM_ACTIVE_TEST(SimpleCSV) {
      std::string res;
      std::size_t count;

      // simpleCSV

      count = testParseString("1,2,3,4 \n 5,6,7,8 \n 9,10,11,12", res);

      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(3))
      TS_ASSERT_EQUALS(res, std::string("1:2:3:4|5:6:7:8|9:10:11:12|"))

      count = testParseString("1,2,3,4 \r\n 5,6,7,8 \r\n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(3))
      TS_ASSERT_EQUALS(res, std::string("1:2:3:4|5:6:7:8|9:10:11:12|"))

      // simpleCSV

      count = testParseString("1,2\t,3,4 \n 5,\t6,,8  \n\n\t\n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(5))
      TS_ASSERT_EQUALS(res, std::string("1:2:3:4|5:6::8|9:10:11:12|"))

      // simpleCSV

      count = testParseString(",,,, \n ,,,, \n ,,,,", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(3))
      TS_ASSERT_EQUALS(res, std::string("::::|::::|::::|"))
    };


    GUM_ACTIVE_TEST(SimpleCSVwithComment) {
      std::string res;
      std::size_t count;

      // simpleCSV with comment line

      count = testParseString("1,2,3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1:2:3:4|5:6:7:8|9:10:11:12|"))

      // simpleCSV with comment line

      count = testParseString("1,2,3,4 \n\t  # this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1:2:3:4|5:6:7:8|9:10:11:12|"))

      // simpleCSV with commented pa

      count = testParseString("1#,2,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1|5:6:7:8|9:10:11:12|"))

      count = testParseString("1  #,2,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1|5:6:7:8|9:10:11:12|"))

      count = testParseString("1 , # 2,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1:|5:6:7:8|9:10:11:12|"))

      count = testParseString("1 ,2 # ,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1:2|5:6:7:8|9:10:11:12|"))
    };

    GUM_ACTIVE_TEST(SimpleCSVwithDoubleQuote) {
      std::string res;
      std::size_t count;

      // simpleCSV with double quoted token
      count
         = testParseString("1,\"fjkdls2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1:fjkdls2:3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with double quoted token with separator in the double-quoted
      // token

      count = testParseString("1,\"fjk,dls2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12",
                              res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1:fjk,dls2:3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with double quoted token with # in the double-quoted token
      count = testParseString("1,\"fjk,dl#s2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12",
                              res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1:fjk,dl#s2:3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with double quoted token with double quote in the
      // double-quoted
      // token
      count = testParseString("1,\"fjk,dl\\\"s2\",3,4 \n# this is a comment \n "
                              "5,6,7,8 \n 9,10,11,12",
                              res);
      TS_ASSERT_EQUALS(count, std::size_t(3))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(4))
      TS_ASSERT_EQUALS(res, std::string("1:fjk,dl\\\"s2:3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with not ending double quoted token
      TS_ASSERT_THROWS(
         count
         = testParseString("1,\"fjk,dls2,3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res),
         const gum::SyntaxError&);

      try {
        count = testParseString("1,\"fjk,dls2,3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12",
                                res);
        TS_FAIL("gum::SyntaxError should have been thrown");
      } catch (gum::SyntaxError& f) {
        TS_ASSERT_DIFFERS(f.errorContent().find("String quote missing", 0), std::string::npos);
        TS_ASSERT_EQUALS(f.col(), std::size_t(2))
        TS_ASSERT_EQUALS(f.line(), std::size_t(1))
      }

      // simpleCSV with double quoted following by something
      TS_ASSERT_THROWS(count = testParseString("1,\"fjkdls2\" nothing should be here,3,4 \n# "
                                               "this is a comment \n 5,6,7,8 \n 9,10,11,12",
                                               res),
                       const gum::SyntaxError&);

      try {
        count = testParseString("1,\"fjkdls2\" nothing should be here,3,4 \n# this "
                                "is a comment \n 5,6,7,8 \n 9,10,11,12",
                                res);
        TS_FAIL("gum::FatalError should have been thrown");
      } catch (gum::SyntaxError& f) {
        TS_ASSERT_DIFFERS(f.errorContent().find("Delimiter missing", 0), std::string::npos);
        TS_ASSERT_EQUALS(f.col(), std::size_t(12))
        TS_ASSERT_EQUALS(f.line(), std::size_t(1))
      }
    };

    GUM_ACTIVE_TEST(SimpleCSVwithLabelsBeginningWithNumbers) {
      std::string res;
      std::size_t count;

      // simpleCSV with double quoted token
      count = testParseString("1,1a,1b\n2a,2,3c", res);
      TS_ASSERT_EQUALS(count, std::size_t(2))
      TS_ASSERT_EQUALS(_noParsedLine_, std::size_t(2))
      TS_ASSERT_EQUALS(res, std::string("1:1a:1b|2a:2:3c|"));
    }


    GUM_ACTIVE_TEST(UseNewStream) {
      std::string              csvstring1 = "1,2,3,4 \n 5,6,7,8 \n 9,10,11,12";
      std::istringstream       in1(csvstring1);
      gum::learning::CSVParser parser(in1, "(stream)");

      TS_ASSERT_THROWS(parser.current(), const gum::NullElement&)
      TS_ASSERT_THROWS(parser.nbLine(), const gum::NullElement&)

      parser.next();
      const std::vector< std::string >& v1 = parser.current();
      TS_ASSERT_EQUALS(v1[0], "1")
      TS_ASSERT_EQUALS(v1[2], "3")

      std::string        csvstring2 = "a,b,c,d\ne,f,g,h\n";
      std::istringstream in2(csvstring2);
      parser.useNewStream(in2);

      TS_ASSERT_THROWS(parser.current(), const gum::NullElement&)
      TS_ASSERT_THROWS(parser.nbLine(), const gum::NullElement&)

      parser.next();
      const std::vector< std::string >& v2 = parser.current();
      TS_ASSERT_EQUALS(v2[0], "a")
      TS_ASSERT_EQUALS(v2[2], "c")
    }

    GUM_ACTIVE_TEST(Guillemet) {
      {
        std::string              csvstring1 = "\"a\",\"b\",\"c d,e\" \n 9,10,11";
        std::istringstream       in1(csvstring1);
        gum::learning::CSVParser parser(in1, "(stream)");

        TS_ASSERT_THROWS(parser.current(), const gum::NullElement&)
        TS_ASSERT_THROWS(parser.nbLine(), const gum::NullElement&)

        parser.next();
        const std::vector< std::string >& v1 = parser.current();
        TS_ASSERT_EQUALS(v1[0], "a")
        TS_ASSERT_EQUALS(v1[1], "b")
        TS_ASSERT_EQUALS(v1[2], "c d,e")
      }
      {
        std::string              csvstring1 = "\"\",\"\",\"c d,e\" \n 9,10,11";
        std::istringstream       in1(csvstring1);
        gum::learning::CSVParser parser(in1, "(stream)");

        TS_ASSERT_THROWS(parser.current(), const gum::NullElement&)
        TS_ASSERT_THROWS(parser.nbLine(), const gum::NullElement&)

        parser.next();
        const std::vector< std::string >& v1 = parser.current();
        TS_ASSERT_EQUALS(v1[0], "")
        TS_ASSERT_EQUALS(v1[1], "")
        TS_ASSERT_EQUALS(v1[2], "c d,e")
      }
    }
    GUM_ACTIVE_TEST(Apostrophe) {
      {
        std::string              csvstring1 = "'a','b','c d,e' \n 9,10,11";
        std::istringstream       in1(csvstring1);
        gum::learning::CSVParser parser(in1, "(stream)");

        TS_ASSERT_THROWS(parser.current(), const gum::NullElement&)
        TS_ASSERT_THROWS(parser.nbLine(), const gum::NullElement&)

        parser.next();
        const std::vector< std::string >& v1 = parser.current();
        TS_ASSERT_EQUALS(v1[0], "'a'")
        TS_ASSERT_EQUALS(v1[1], "'b'")
        TS_ASSERT_EQUALS(v1[2], "'c d")
        TS_ASSERT_EQUALS(v1[3], "e'")
      }
      {
        std::string              csvstring1 = "'a','b','c d,e' \n 9,10,11";
        std::istringstream       in1(csvstring1);
        gum::learning::CSVParser parser(in1, "(stream)", ",", '#', '\'');

        TS_ASSERT_THROWS(parser.current(), const gum::NullElement&)
        TS_ASSERT_THROWS(parser.nbLine(), const gum::NullElement&)

        parser.next();
        const std::vector< std::string >& v1 = parser.current();
        TS_ASSERT_EQUALS(v1[0], "a")
        TS_ASSERT_EQUALS(v1[1], "b")
        TS_ASSERT_EQUALS(v1[2], "c d,e")
      }
    }

  };

}   // namespace gum_tests
