/***************************************************************************
 *   Copyright (C) 2005-2020 by Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)  *
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
 * @author Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
 *
 */
#include <agrum/tools/database/CSVParser.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <template<typename> class ALLOC>
    CSVParser<ALLOC>::CSVParser(
      std::istream&      instream,
      const std::string& delimiter,
      const char         commentmarker,
      const char         quoteMarker,
      const typename CSVParser<ALLOC>::allocator_type& alloc )
      : line__()
      , delimiter__( delimiter )
      , spaces__( " \t\r" )
      , delimiterPlusSpaces__( delimiter__ + spaces__ )
      , nbLine__( std::size_t(0) )
      , commentMarker__( commentmarker )
      , quoteMarker__( quoteMarker )
      , emptyData__( true )
      , instream__( &instream )
      , data__( alloc ) {
      GUM_CONSTRUCTOR( CSVParser );
    }

    
    /// destructor
    template <template<typename> class ALLOC>
    CSVParser<ALLOC>::~CSVParser() {
      GUM_DESTRUCTOR( CSVParser );
    }


    // for debugging purpose
    /*
      char getTheChar(const std::string& str,Size pos) {
      return (pos<Size(std::string::npos))?str.at(pos):'$';
      }
    */

    
    template <template<typename> class ALLOC>
    void CSVParser<ALLOC>::getNextTriplet__( const std::string& str,
                                             std::size_t& first_letter_token,
                                             std::size_t& next_token,
                                             std::size_t& last_letter_token,
                                             std::size_t  from ) const {
      first_letter_token = str.find_first_not_of( spaces__, from );

      if ( first_letter_token == std::string::npos ) {
        next_token = last_letter_token = first_letter_token;
        return;
      }

      if ( str.at( first_letter_token ) == quoteMarker__ ) {
        last_letter_token = correspondingQuoteMarker__( str, first_letter_token );

        if ( last_letter_token == std::string::npos )
          GUM_SYNTAX_ERROR( "String quote missing", (Size) nbLine(), first_letter_token );

        next_token = str.find_first_of( delimiter__, last_letter_token + 1 );
        std::size_t next_char =
          str.find_first_not_of( spaces__, last_letter_token + 1 );

        if ( next_char < next_token ) {
          GUM_SYNTAX_ERROR( "Delimiter missing at line", (Size) nbLine(), next_char );
        }
      }
      else {
        next_token = str.find_first_of( delimiter__, first_letter_token );

        if ( next_token == std::string::npos ) {
          last_letter_token = str.find_last_not_of( spaces__, next_token );
        }
        else if ( next_token == first_letter_token ) {
          last_letter_token = first_letter_token;
        }
        else {
          last_letter_token = 
              str.find_last_not_of( delimiterPlusSpaces__, next_token - 1 );
        }
      }
    }

    
    template <template<typename> class ALLOC>
    void CSVParser<ALLOC>::tokenize__( const std::string& s ) {
      // looking for first commentMarker not in a string
      std::size_t commentMarker = s.find_first_of( commentMarker__, 0 );
      std::size_t quoteMarker = s.find_first_of( quoteMarker__, 0 );
      std::size_t quoteMarkerEnd;

      while ( quoteMarker < commentMarker ) {
        quoteMarkerEnd = correspondingQuoteMarker__( s, quoteMarker );

        if ( quoteMarkerEnd == std::string::npos )
          GUM_SYNTAX_ERROR( "String quote missing", (Size) nbLine(), quoteMarker );

        while ( commentMarker < quoteMarkerEnd ) {  // the comment was in the quote
          commentMarker = s.find_first_of( commentMarker__, commentMarker + 1 );
        }

        quoteMarker = s.find_first_of( quoteMarker__, quoteMarkerEnd + 1 );
      }

      std::string str = s.substr( 0, commentMarker );

      std::size_t counter = 0, first_letter_token, next_token, last_letter_token;

      getNextTriplet__(
          str, first_letter_token, next_token, last_letter_token, 0 );

      while ( ( std::string::npos != first_letter_token ) &&
              ( std::string::npos != last_letter_token ) ) {
        if ( data__.size() <= counter ) data__.resize( counter + 1 );

        if ( first_letter_token == next_token ) {
          data__[counter] = "";
        }
        else if ( last_letter_token >= first_letter_token ) {
          const std::size_t fieldlength =
            last_letter_token + 1 - first_letter_token;
          data__[counter].resize( fieldlength );
          data__[counter].assign( str, first_letter_token, fieldlength );
        }
        else {
          data__[counter] = "";
        }

        counter++;

        if ( next_token == std::string::npos ) break;

        getNextTriplet__( str,
                          first_letter_token,
                          next_token,
                          last_letter_token,
                          next_token + 1 );
      }

      // case where we end up with an empty field ...
      if ( ( first_letter_token == std::string::npos ) &&
           ( last_letter_token == first_letter_token ) &&
           ( next_token == first_letter_token ) ) {
        counter++;
        data__.resize( counter );
        data__[counter - 1] = "";
      }
      else {
        data__.resize( counter );
      }

      emptyData__ = false;
    }


    /// reopens a new input stream to parse
    template <template<typename> class ALLOC>
    void CSVParser<ALLOC>::useNewStream (std::istream&      instream,
                                         const std::string& delimiter,
                                         const char         commentmarker,
                                         const char         quoteMarker ) {
      line__.clear ();
      delimiter__ = delimiter;
      spaces__ =  " \t\r";
      delimiterPlusSpaces__ = delimiter__ + spaces__;
      nbLine__ = std::size_t(0);
      commentMarker__ = commentmarker;
      quoteMarker__ = quoteMarker;
      emptyData__ = true;
      instream__ = &instream;
      data__.clear ();
    }

    
    // gets the next line of the csv stream and parses it
    template <template<typename> class ALLOC>
    INLINE bool CSVParser<ALLOC>::next () {
      while ( getline( *instream__, line__ ) ) {
        nbLine__++;

        if ( line__.size() == std::size_t (0) ) continue;

        // fast recognition of commented or empty lines lines
        std::size_t lastPos =
          line__.find_first_not_of( spaces__, std::size_t(0) );
        
        if ( lastPos == std::string::npos ) continue;

        if ( line__.at( lastPos ) == commentMarker__ ) continue;

        tokenize__( line__ );
        return true;
      }

      return false;
    }

    
    // search for quote taking into account the '\'...
    template <template<typename> class ALLOC>
    INLINE std::size_t
    CSVParser<ALLOC>::correspondingQuoteMarker__( const std::string& str,
                                                  std::size_t pos ) const {
      std::size_t res = pos, before;

      while ( true ) {
        res = str.find_first_of( quoteMarker__, res + 1 );

        if ( res == std::string::npos ) return res;  // no quote found

        before = str.find_last_not_of( '\\', res - 1 );

        if ( before == std::string::npos )
          return res;  // quote found, it is the good one

        if ( ( res - before ) % 2 == 1 )
          return res;  // the quote is the good one, even if there are some '\'
                       // before
      }
    }


    // returns the current parsed line
    template <template<typename> class ALLOC>
    INLINE const std::vector<std::string,ALLOC<std::string>>&
    CSVParser<ALLOC>::current () const {
      if ( emptyData__ )
        GUM_ERROR( NullElement, "No parsed data" );

      return data__;
    }
    

    // returns the current nbLine of parser line
    template <template<typename> class ALLOC>
    INLINE const std::size_t CSVParser<ALLOC>::nbLine() const {
      if ( nbLine__ == 0 )
        GUM_ERROR( NullElement, "No parsed data" );

      return nbLine__;
    }

  }  // namespace learning

}  // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
