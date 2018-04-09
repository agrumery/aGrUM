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
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
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
     * @headerfile CSVParser.h <agrum/learning/database/CSVParser.h>
     * @brief  Class for fast parsing of CSV file (never more than one
     * line in application memory)
     *
     * Typical use:
     * @code
     * // open the CSV file
     * std::string filename="foo.csv"
     * std::ifstream in(filename.c_str());
     * gum::learning::CSVParser<> csvp(in);
     *
     * // read each line in the CSV file
     * while (csvp.next()) {
     *  csvp.current ();
     * }
     *
     * in.close();
     * @endcode
     */
    template <template<typename> class ALLOC = std::allocator>
    class CSVParser {
      public:
      
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC<std::string>;

        
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
       * @param alloc the allocator used by all the methods
       */
      CSVParser( std::istream&      in,
                 const std::string& delimiter = ",",
                 const char         commentmarker = '#',
                 const char         quoteMarker = '"',
                 const allocator_type& alloc = allocator_type () );

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
      const std::vector<std::string,ALLOC<std::string>>& current() const;

      /// returns the current line number within the stream
      const std::size_t nbLine() const;

      /// reopens a new input stream to parse
      void useNewStream ( std::istream&      in,
                          const std::string& delimiter = ",",
                          const char         commentmarker = '#',
                          const char         quoteMarker = '"' );

      /// @}
      
     
#ifndef DOXYGEN_SHOULD_SKIP_THIS
      
    private:
      void __getNextTriplet( const std::string& str,
                             std::size_t&       first_letter_token,
                             std::size_t&       next_token,
                             std::size_t&       last_letter_token,
                             std::size_t        from ) const;
      
      void __tokenize( const std::string& str );

      std::size_t __correspondingQuoteMarker( const std::string& str,
                                              std::size_t pos ) const;

      
      std::string              __line;
      std::string              __delimiter;
      std::string              __spaces;
      std::string              __delimiterPlusSpaces;
      std::size_t              __nbLine;
      char                     __commentMarker;
      char                     __quoteMarker;
      bool                     __emptyData;

      std::istream*            __instream;
      std::vector<std::string,ALLOC<std::string>> __data;
      
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    };

  }  // namespace learning

}  // namespace gum

#include <agrum/learning/database/CSVParser_tpl.h>

#endif  // GUM_CSV_PARSER_H
