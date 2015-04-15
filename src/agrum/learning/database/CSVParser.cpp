/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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

#include <agrum/learning/database/CSVParser.h>

#ifdef GUM_NO_INLINE
#include <agrum/learning/database/CSVParser.inl>
#endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    CSVParser::CSVParser(std::istream &in, const std::string &delimiter,
                         const char commentmarker, const char quoteMarker)
        : __line(), __delimiter(delimiter), __spaces(" \t\r"),
          __delimiterPlusSpaces(__delimiter + __spaces), __noLine((Size)0),
          __commentMarker(commentmarker), __quoteMarker(quoteMarker), __in(in),
          __data(), __emptyData(true) {
      GUM_CONSTRUCTOR(CSVParser);
    }

    CSVParser::~CSVParser() { GUM_DESTRUCTOR(CSVParser); }

    // for debugginh purpose
    /*
      char getTheChar(const std::string& str,Size pos) {
      return (pos<std::string::npos)?str.at(pos):'$';
      }
    */

    void CSVParser::__getNextTriplet(const std::string &str,
                                     Size &first_letter_token, Size &next_token,
                                     Size &last_letter_token, Size from) const {
      first_letter_token = str.find_first_not_of(__spaces, from);

      if (first_letter_token == std::string::npos) {
        next_token = last_letter_token = first_letter_token;
        return;
      }

      if (str.at(first_letter_token) == __quoteMarker) {
        last_letter_token = __correspondingQuoteMarker(str, first_letter_token);

        if (last_letter_token == std::string::npos)
          GUM_SYNTAX_ERROR("String quote missing", noLine(), first_letter_token);

        next_token = str.find_first_of(__delimiter, last_letter_token + 1);
        Size next_char = str.find_first_not_of(__spaces, last_letter_token + 1);

        if (next_char < next_token) {
          GUM_SYNTAX_ERROR("Delimiter missing at line", noLine(), next_char);
        }
      } else {
        next_token = str.find_first_of(__delimiter, first_letter_token);

        if (next_token == std::string::npos) {
          last_letter_token = str.find_last_not_of(__spaces, next_token);
        } else if (next_token == first_letter_token) {
          last_letter_token = first_letter_token;
        } else {
          last_letter_token =
              str.find_last_not_of(__delimiterPlusSpaces, next_token - 1);
        }
      }
    }

    void CSVParser::__tokenize(const std::string &s) {
      // looking for first commentMarker not in a string
      Size commentMarker = s.find_first_of(__commentMarker, 0);
      Size quoteMarker = s.find_first_of(__quoteMarker, 0);
      Size quoteMarkerEnd;

      while (quoteMarker < commentMarker) {
        quoteMarkerEnd = __correspondingQuoteMarker(s, quoteMarker);

        if (quoteMarkerEnd == std::string::npos)
          GUM_SYNTAX_ERROR("String quote missing", noLine(), quoteMarker);

        while (commentMarker < quoteMarkerEnd) { // the comment was in the quote
          commentMarker = s.find_first_of(__commentMarker, commentMarker + 1);
        }

        quoteMarker = s.find_first_of(__quoteMarker, quoteMarkerEnd + 1);
      }

      std::string str = s.substr(0, commentMarker);

      Size counter = 0, first_letter_token, next_token, last_letter_token;

      __getNextTriplet(str, first_letter_token, next_token, last_letter_token, 0);

      while (std::string::npos != first_letter_token &&
             std::string::npos != last_letter_token) {
        if (__data.size() <= counter)
          __data.resize(counter + 1);

        if (first_letter_token == next_token) {
          __data[counter] = "";
        } else if (last_letter_token >= first_letter_token) {
          const Size fieldlength = last_letter_token + 1 - first_letter_token;
          __data[counter].resize(fieldlength);
          __data[counter].assign(str, first_letter_token, fieldlength);
        } else {
          __data[counter] = "";
        }

        counter++;

        if (next_token == std::string::npos)
          break;

        __getNextTriplet(str, first_letter_token, next_token, last_letter_token,
                         next_token + 1);
      }

      // case where we end up with an empty field ...
      if (first_letter_token == std::string::npos &&
          last_letter_token == first_letter_token &&
          next_token == first_letter_token) {
        counter++;
        __data.resize(counter);
        __data[counter - 1] = "";
      } else {
        __data.resize(counter);
      }

      __emptyData = false;
    }

  } // namespace learning

} // namespace gum
