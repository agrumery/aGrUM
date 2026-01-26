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

#undef GUM_CURRENT_SUITE
#undef GUM_CURRENT_MODULE
#define GUM_CURRENT_SUITE  CSVParser
#define GUM_CURRENT_MODULE BN

namespace gum_tests {

  struct CSVParserTestSuite {
    private:
    std::size_t _noParsedLine_ = 0u;

    std::size_t testParseString(std::string csvstring, std::string& res) {
      std::istringstream       in(csvstring);
      gum::learning::CSVParser parser(in, "(stream)");

      // no data parser for now
      CHECK_THROWS_AS(parser.current(), const gum::NullElement&);
      CHECK_THROWS_AS(parser.nbLine(), const gum::NullElement&);

      std::size_t count = 0;
      res               = "";

      while (parser.next()) {
        count++;
        const std::vector< std::string >& v = parser.current();

        for (auto i = static_cast< std::size_t >(0); i < v.size(); i++) {
          res += v[i];
          res += (i == v.size() - 1) ? '|' : ':';
        }
      }

      _noParsedLine_ = parser.nbLine();
      return count;
    }

    public:
    void testSimpleCSV() {
      std::string res;
      std::size_t count;

      // simpleCSV

      count = testParseString("1,2,3,4 \n 5,6,7,8 \n 9,10,11,12", res);

      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(3));
      CHECK((res) == (std::string("1:2:3:4|5:6:7:8|9:10:11:12|")));

      count = testParseString("1,2,3,4 \r\n 5,6,7,8 \r\n 9,10,11,12", res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(3));
      CHECK((res) == (std::string("1:2:3:4|5:6:7:8|9:10:11:12|")));

      // simpleCSV

      count = testParseString("1,2\t,3,4 \n 5,\t6,,8  \n\n\t\n 9,10,11,12", res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(5));
      CHECK((res) == (std::string("1:2:3:4|5:6::8|9:10:11:12|")));

      // simpleCSV

      count = testParseString(",,,, \n ,,,, \n ,,,,", res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(3));
      CHECK((res) == (std::string("::::|::::|::::|")));
    };

    void testSimpleCSVwithComment() {
      std::string res;
      std::size_t count;

      // simpleCSV with comment line

      count = testParseString("1,2,3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1:2:3:4|5:6:7:8|9:10:11:12|")));

      // simpleCSV with comment line

      count = testParseString("1,2,3,4 \n\t  # this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1:2:3:4|5:6:7:8|9:10:11:12|")));

      // simpleCSV with commented pa

      count = testParseString("1#,2,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1|5:6:7:8|9:10:11:12|")));

      count = testParseString("1  #,2,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1|5:6:7:8|9:10:11:12|")));

      count = testParseString("1 , # 2,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1:|5:6:7:8|9:10:11:12|")));

      count = testParseString("1 ,2 # ,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1:2|5:6:7:8|9:10:11:12|")));
    };

    void testSimpleCSVwithDoubleQuote() {
      std::string res;
      std::size_t count;

      // simpleCSV with double quoted token
      count = testParseString("1,\"fjkdls2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12",
                              res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1:fjkdls2:3:4|5:6:7:8|9:10:11:12|")));

      // simpleCSV with double quoted token with separator in the double-quoted
      // token

      count = testParseString("1,\"fjk,dls2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12",
                              res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1:fjk,dls2:3:4|5:6:7:8|9:10:11:12|")));

      // simpleCSV with double quoted token with # in the double-quoted token
      count = testParseString("1,\"fjk,dl#s2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12",
                              res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1:fjk,dl#s2:3:4|5:6:7:8|9:10:11:12|")));

      // simpleCSV with double quoted token with double quote in the
      // double-quoted
      // token
      count = testParseString("1,\"fjk,dl\\\"s2\",3,4 \n# this is a comment \n "
                              "5,6,7,8 \n 9,10,11,12",
                              res);
      CHECK((count) == static_cast< std::size_t >(3));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(4));
      CHECK((res) == (std::string("1:fjk,dl\\\"s2:3:4|5:6:7:8|9:10:11:12|")));

      // simpleCSV with not ending double quoted token
      CHECK_THROWS_AS(
          count
          = testParseString("1,\"fjk,dls2,3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res),
          const gum::SyntaxError&);

      try {
        count = testParseString("1,\"fjk,dls2,3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12",
                                res);
        FAIL("gum::SyntaxError should have been thrown");
      } catch (gum::SyntaxError& f) {
        CHECK((f.errorContent().find("String quote missing", 0)) != (std::string::npos));
        CHECK((f.col()) == static_cast< std::size_t >(2));
        CHECK((f.line()) == static_cast< std::size_t >(1));
      }

      // simpleCSV with double quoted following by something
      CHECK_THROWS_AS(count = testParseString("1,\"fjkdls2\" nothing should be here,3,4 \n# "
                                              "this is a comment \n 5,6,7,8 \n 9,10,11,12",
                                              res),
                      const gum::SyntaxError&);

      try {
        count = testParseString("1,\"fjkdls2\" nothing should be here,3,4 \n# this "
                                "is a comment \n 5,6,7,8 \n 9,10,11,12",
                                res);
        FAIL("gum::FatalError should have been thrown");
      } catch (gum::SyntaxError& f) {
        CHECK((f.errorContent().find("Delimiter missing", 0)) != (std::string::npos));
        CHECK((f.col()) == static_cast< std::size_t >(12));
        CHECK((f.line()) == static_cast< std::size_t >(1));
      }
    };

    void testSimpleCSVwithLabelsBeginningWithNumbers() {
      std::string res;
      std::size_t count;

      // simpleCSV with double quoted token
      count = testParseString("1,1a,1b\n2a,2,3c", res);
      CHECK((count) == static_cast< std::size_t >(2));
      CHECK((_noParsedLine_) == static_cast< std::size_t >(2));
      CHECK((res) == (std::string("1:1a:1b|2a:2:3c|")));
    }

    static void testUseNewStream() {
      std::string              csvstring1 = "1,2,3,4 \n 5,6,7,8 \n 9,10,11,12";
      std::istringstream       in1(csvstring1);
      gum::learning::CSVParser parser(in1, "(stream)");

      CHECK_THROWS_AS(parser.current(), const gum::NullElement&);
      CHECK_THROWS_AS(parser.nbLine(), const gum::NullElement&);

      parser.next();
      const std::vector< std::string >& v1 = parser.current();
      CHECK((v1[0]) == ("1"));
      CHECK((v1[2]) == ("3"));

      std::string        csvstring2 = "a,b,c,d\ne,f,g,h\n";
      std::istringstream in2(csvstring2);
      parser.useNewStream(in2);

      CHECK_THROWS_AS(parser.current(), const gum::NullElement&);
      CHECK_THROWS_AS(parser.nbLine(), const gum::NullElement&);

      parser.next();
      const std::vector< std::string >& v2 = parser.current();
      CHECK((v2[0]) == ("a"));
      CHECK((v2[2]) == ("c"));
    }

    static void testGuillemet() {
      {
        std::string              csvstring1 = "\"a\",\"b\",\"c d,e\" \n 9,10,11";
        std::istringstream       in1(csvstring1);
        gum::learning::CSVParser parser(in1, "(stream)");

        CHECK_THROWS_AS(parser.current(), const gum::NullElement&);
        CHECK_THROWS_AS(parser.nbLine(), const gum::NullElement&);

        parser.next();
        const std::vector< std::string >& v1 = parser.current();
        CHECK((v1[0]) == ("a"));
        CHECK((v1[1]) == ("b"));
        CHECK((v1[2]) == ("c d,e"));
      }
      {
        std::string              csvstring1 = "\"\",\"\",\"c d,e\" \n 9,10,11";
        std::istringstream       in1(csvstring1);
        gum::learning::CSVParser parser(in1, "(stream)");

        CHECK_THROWS_AS(parser.current(), const gum::NullElement&);
        CHECK_THROWS_AS(parser.nbLine(), const gum::NullElement&);

        parser.next();
        const std::vector< std::string >& v1 = parser.current();
        CHECK((v1[0]) == (""));
        CHECK((v1[1]) == (""));
        CHECK((v1[2]) == ("c d,e"));
      }
    }

    static void testApostrophe() {
      {
        std::string              csvstring1 = "'a','b','c d,e' \n 9,10,11";
        std::istringstream       in1(csvstring1);
        gum::learning::CSVParser parser(in1, "(stream)");

        CHECK_THROWS_AS(parser.current(), const gum::NullElement&);
        CHECK_THROWS_AS(parser.nbLine(), const gum::NullElement&);

        parser.next();
        const std::vector< std::string >& v1 = parser.current();
        CHECK((v1[0]) == ("'a'"));
        CHECK((v1[1]) == ("'b'"));
        CHECK((v1[2]) == ("'c d"));
        CHECK((v1[3]) == ("e'"));
      }
      {
        std::string              csvstring1 = "'a','b','c d,e' \n 9,10,11";
        std::istringstream       in1(csvstring1);
        gum::learning::CSVParser parser(in1, "(stream)", ",", '#', '\'');

        CHECK_THROWS_AS(parser.current(), const gum::NullElement&);
        CHECK_THROWS_AS(parser.nbLine(), const gum::NullElement&);

        parser.next();
        const std::vector< std::string >& v1 = parser.current();
        CHECK((v1[0]) == ("a"));
        CHECK((v1[1]) == ("b"));
        CHECK((v1[2]) == ("c d,e"));
      }
    }
  };

  GUM_TEST_ACTIF(SimpleCSV)
  GUM_TEST_ACTIF(SimpleCSVwithComment)
  GUM_TEST_ACTIF(SimpleCSVwithDoubleQuote)
  GUM_TEST_ACTIF(SimpleCSVwithLabelsBeginningWithNumbers)
  GUM_TEST_ACTIF(UseNewStream)
  GUM_TEST_ACTIF(Guillemet)
  GUM_TEST_ACTIF(Apostrophe)

}   // namespace gum_tests
