/***************************************************************************
 *   Copyright (c) 2005-2023 by Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)  *
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
/** @file
 * @brief Class for fast parsing of CSV file (never more than one line in
 * application memory)
 *
 * Typical use :
 * @code
 * // open the CSV file
 * std::string filename="foo.csv"
 * std::ifstream in(filename.c_str());
 * gum::learning::CSVParser csvp(in);
 *
 * // read each line in the CSV file
 * while (csvp.next()) {
 *   csvp.current ();
 * }
 *
 * in.close();
 * @endcode
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */

#ifndef GUM_CSV_PARSER_H
#define GUM_CSV_PARSER_H

#include <istream>
#include <string>
#include <vector>

#include <agrum/agrum.h>

namespace gum {

  namespace learning {

    /** @class CSVParser
     * @ingroup learning_database
     * @headerfile CSVParser.h <agrum/tools/database/CSVParser.h>
     * @brief  Class for fast parsing of CSV file (never more than one
     * line in application memory)
     *
     * Typical use:
     * @code
     * // open the CSV file
     * std::string filename="foo.csv"
     * std::ifstream in(filename.c_str());
     * gum::learning::CSVParser csvp(in);
     *
     * // read each line in the CSV file
     * while (csvp.next()) {
     *  csvp.current ();
     * }
     *
     * in.close();
     * @endcode
     */
    class CSVParser {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      /** @param in an input stream containing the CSV
       * @param delimiter the character that acts as the column separator in
       * the CSV
       * @param commentmarker the character that marks the beginning of a comment
       * @param quoteMarker the character that is used to quote the sentences
       * in the CSV
       */
      CSVParser(std::istream&      in,
                const std::string& filename,
                const std::string& delimiter     = ",",
                const char         commentmarker = '#',
                const char         quoteMarker   = '"');

      /// destructor
      virtual ~CSVParser();

      /// @}


      // ########################################################################
      /// @name Accessors / Modifiers
      // ########################################################################
      /// @{

      /// gets the next line of the csv stream and parses it
      /** @return false if there is no next line
       */
      bool next();

      /// returns the current parsed line
      /** @throw NullElement is raised if there is no data
       */
      const std::vector< std::string >& current() const;

      /// returns the current line number within the stream
      std::size_t nbLine() const;

      /// reopens a new input stream to parse
      void useNewStream(std::istream&      in,
                        const std::string& delimiter     = ",",
                        const char         commentmarker = '#',
                        const char         quoteMarker   = '"');

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      void _getNextTriplet_(const std::string& str,
                            std::size_t&       first_letter_token,
                            std::size_t&       next_token,
                            std::size_t&       last_letter_token,
                            std::size_t        from) const;

      void _tokenize_(const std::string& str);

      std::size_t _correspondingQuoteMarker_(const std::string& str, std::size_t pos) const;


      std::string _line_;
      std::string _delimiter_;
      std::string _spaces_;
      std::string _delimiterPlusSpaces_;
      std::size_t _nbLine_;
      char        _commentMarker_;
      char        _quoteMarker_;
      bool        _emptyData_;

      std::istream*              _instream_;
      std::vector< std::string > _data_;
      const std::string          _filename_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  }   // namespace learning

}   // namespace gum

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/tools/database/CSVParser_inl.h>
#endif /* GUM_NO_INLINE */

#endif   // GUM_CSV_PARSER_H
