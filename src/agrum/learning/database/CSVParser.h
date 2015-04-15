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
/** @file
 * @brief Class for fast parsing of CSV file (never more than one line in application
 memory)
 *
 * Typical use :
 @code
 std::string filename="foo.csv"
 std::ifstream in(filename.c_str());
 gum::CSVParser csvp(in);

 while (csvp.next()) {
 csvp.currentData();
 }
 in.close();
 @endcode
 *
 * @author Pierre-Henri WUILLEMIN
 *
 */

#ifndef GUM_CSVPARSER_H
#define GUM_CSVPARSER_H

#include <istream>
#include <string>

#include <agrum/config.h>
#include <agrum/learning/database/DBRow.h>

namespace gum {

  namespace learning {

    class CSVParser {
      public:
      CSVParser(std::istream &in, const std::string &delimiter = ",",
                const char commentmarker = '#', const char quoteMarker = '"');
      virtual ~CSVParser();

      /**
       * gets the next line of the csv stream and parses it
       *
       * @return false if there is no next line
       */
      bool next();

      /**
       * @return the current parsed line
       *
       * @throw gum::NullElement if there is no data
       */
      const std::vector<std::string> &current() const;

      /**
       * return the current noLine of parser line
       */
      const Size noLine() const;

      private:
      void __getNextTriplet(const std::string &str, Size &first_letter_token,
                            Size &next_token, Size &last_letter_token,
                            Size from) const;
      void __tokenize(const std::string &str);
      Size __correspondingQuoteMarker(const std::string &str, Size pos) const;

      std::string __line;
      std::string __delimiter;
      std::string __spaces;
      std::string __delimiterPlusSpaces;
      Size __noLine;
      char __commentMarker;
      char __quoteMarker;
      std::istream &__in;
      std::vector<std::string> __data;
      bool __emptyData;
    };

  } // namespace learning

} // namespace gum

#ifndef GUM_NO_INLINE
#include <agrum/learning/database/CSVParser.inl>
#endif /* GUM_NO_INLINE */

#endif // GUM_CSVPARSER_H
