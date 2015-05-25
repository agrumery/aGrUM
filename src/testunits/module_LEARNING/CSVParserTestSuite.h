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

#include <cxxtest/AgrumTestSuite.h>
#include <testsuite_utils.h>

#include <agrum/learning/database/CSVParser.h>

namespace gum_tests {

  class CSVParserTestSuite : public CxxTest::TestSuite {
    private:
    gum::Size __noParsedLine;

    gum::Size testParseString(std::string csvstring, std::string &res) {
      std::istringstream in(csvstring);
      gum::learning::CSVParser parser(in);

      // no data parser for now
      TS_ASSERT_THROWS(parser.current(), gum::NullElement)
      TS_ASSERT_THROWS(parser.noLine(), gum::NullElement)

      gum::Size count = 0;
      res = "";

      while (parser.next()) {
        count++;
        const std::vector<std::string> &v = parser.current();

        for (gum::Idx i = 0; i < v.size(); i++) {
          res += v[i];
          res += (i == v.size() - 1) ? '|' : ':';
        }
      }

      __noParsedLine = parser.noLine();
      return count;
    };

    public:
    void testSimpleCSV() {
      std::string res;
      gum::Size count;

      // simpleCSV

      count = testParseString("1,2,3,4 \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)3);
      TS_ASSERT_EQUALS(res, std::string("1:2:3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV

      count = testParseString("1,2\t,3,4 \n 5,\t6,,8  \n\n\t\n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)5);
      TS_ASSERT_EQUALS(res, std::string("1:2:3:4|5:6::8|9:10:11:12|"));

      // simpleCSV

      count = testParseString(",,,, \n ,,,, \n ,,,,", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)3);
      TS_ASSERT_EQUALS(res, std::string("::::|::::|::::|"));
    };

    void testSimpleCSVwithComment() {
      std::string res;
      gum::Size count;

      // simpleCSV with comment line

      count = testParseString(
          "1,2,3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res, std::string("1:2:3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with comment line

      count = testParseString(
          "1,2,3,4 \n\t  # this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res, std::string("1:2:3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with commented pa

      count = testParseString(
          "1#,2,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res, std::string("1|5:6:7:8|9:10:11:12|"));

      count = testParseString(
          "1  #,2,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res, std::string("1|5:6:7:8|9:10:11:12|"));

      count = testParseString(
          "1 , # 2,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res, std::string("1:|5:6:7:8|9:10:11:12|"));

      count = testParseString(
          "1 ,2 # ,3,4 \n\t# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res, std::string("1:2|5:6:7:8|9:10:11:12|"));
    };

    void testSimpleCSVwithDoubleQuote() {
      std::string res;
      gum::Size count;

      // simpleCSV with double quoted token
      count = testParseString(
          "1,\"fjkdls2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res, std::string("1:\"fjkdls2\":3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with double quoted token with separator in the double-quoted token

      count = testParseString(
          "1,\"fjk,dls2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res, std::string("1:\"fjk,dls2\":3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with double quoted token with # in the double-quoted token
      count = testParseString(
          "1,\"fjk,dl#s2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res, std::string("1:\"fjk,dl#s2\":3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with double quoted token with double quote in the double-quoted
      // token
      count = testParseString(
          "1,\"fjk,dl\\\"s2\",3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12",
          res);
      TS_ASSERT_EQUALS(count, (gum::Size)3);
      TS_ASSERT_EQUALS(__noParsedLine, (gum::Size)4);
      TS_ASSERT_EQUALS(res,
                       std::string("1:\"fjk,dl\\\"s2\":3:4|5:6:7:8|9:10:11:12|"));

      // simpleCSV with not ending double quoted token
      TS_ASSERT_THROWS(
          count = testParseString(
              "1,\"fjk,dls2,3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12",
              res),
          gum::SyntaxError);

      try {
        count = testParseString(
            "1,\"fjk,dls2,3,4 \n# this is a comment \n 5,6,7,8 \n 9,10,11,12", res);
        TS_FAIL("gum::FatalError should have been thrown");
      } catch (gum::SyntaxError &f) {
        TS_ASSERT_DIFFERS(f.errorContent().find("String quote missing", 0),
                          std::string::npos);
        TS_ASSERT_EQUALS(f.col(), (gum::Size)2);
        TS_ASSERT_EQUALS(f.line(), (gum::Size)1);
      }

      // simpleCSV with double quoted following by something
      TS_ASSERT_THROWS(
          count = testParseString("1,\"fjkdls2\" nothing should be here,3,4 \n# "
                                  "this is a comment \n 5,6,7,8 \n 9,10,11,12",
                                  res),
          gum::SyntaxError);

      try {
        count = testParseString("1,\"fjkdls2\" nothing should be here,3,4 \n# this "
                                "is a comment \n 5,6,7,8 \n 9,10,11,12",
                                res);
        TS_FAIL("gum::FatalError should have been thrown");
      } catch (gum::SyntaxError &f) {
        TS_ASSERT_DIFFERS(f.errorContent().find("Delimiter missing", 0),
                          std::string::npos);
        TS_ASSERT_EQUALS(f.col(), (gum::Size)12);
        TS_ASSERT_EQUALS(f.line(), (gum::Size)1);
      }
    };
  };
}
