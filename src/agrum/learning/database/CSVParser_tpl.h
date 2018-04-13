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
 * @author Pierre-Henri WUILLEMIN and Christophe GONZALES
 *
 */
#include <agrum/learning/database/CSVParser.h>

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
      : __line()
      , __delimiter( delimiter )
      , __spaces( " \t\r" )
      , __delimiterPlusSpaces( __delimiter + __spaces )
      , __nbLine( std::size_t(0) )
      , __commentMarker( commentmarker )
      , __quoteMarker( quoteMarker )
      , __emptyData( true )
      , __instream( &instream )
      , __data( alloc ) {
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
    void CSVParser<ALLOC>::__getNextTriplet( const std::string& str,
                                             std::size_t& first_letter_token,
                                             std::size_t& next_token,
                                             std::size_t& last_letter_token,
                                             std::size_t  from ) const {
      first_letter_token = str.find_first_not_of( __spaces, from );

      if ( first_letter_token == std::string::npos ) {
        next_token = last_letter_token = first_letter_token;
        return;
      }

      if ( str.at( first_letter_token ) == __quoteMarker ) {
        last_letter_token = __correspondingQuoteMarker( str, first_letter_token );

        if ( last_letter_token == std::string::npos )
          GUM_SYNTAX_ERROR( "String quote missing", (Size) nbLine(), first_letter_token );

        next_token = str.find_first_of( __delimiter, last_letter_token + 1 );
        std::size_t next_char =
          str.find_first_not_of( __spaces, last_letter_token + 1 );

        if ( next_char < next_token ) {
          GUM_SYNTAX_ERROR( "Delimiter missing at line", (Size) nbLine(), next_char );
        }
      }
      else {
        next_token = str.find_first_of( __delimiter, first_letter_token );

        if ( next_token == std::string::npos ) {
          last_letter_token = str.find_last_not_of( __spaces, next_token );
        }
        else if ( next_token == first_letter_token ) {
          last_letter_token = first_letter_token;
        }
        else {
          last_letter_token = 
              str.find_last_not_of( __delimiterPlusSpaces, next_token - 1 );
        }
      }
    }

    
    template <template<typename> class ALLOC>
    void CSVParser<ALLOC>::__tokenize( const std::string& s ) {
      // looking for first commentMarker not in a string
      std::size_t commentMarker = s.find_first_of( __commentMarker, 0 );
      std::size_t quoteMarker = s.find_first_of( __quoteMarker, 0 );
      std::size_t quoteMarkerEnd;

      while ( quoteMarker < commentMarker ) {
        quoteMarkerEnd = __correspondingQuoteMarker( s, quoteMarker );

        if ( quoteMarkerEnd == std::string::npos )
          GUM_SYNTAX_ERROR( "String quote missing", (Size) nbLine(), quoteMarker );

        while ( commentMarker < quoteMarkerEnd ) {  // the comment was in the quote
          commentMarker = s.find_first_of( __commentMarker, commentMarker + 1 );
        }

        quoteMarker = s.find_first_of( __quoteMarker, quoteMarkerEnd + 1 );
      }

      std::string str = s.substr( 0, commentMarker );

      std::size_t counter = 0, first_letter_token, next_token, last_letter_token;

      __getNextTriplet(
          str, first_letter_token, next_token, last_letter_token, 0 );

      while ( ( std::string::npos != first_letter_token ) &&
              ( std::string::npos != last_letter_token ) ) {
        if ( __data.size() <= counter ) __data.resize( counter + 1 );

        if ( first_letter_token == next_token ) {
          __data[counter] = "";
        }
        else if ( last_letter_token >= first_letter_token ) {
          const std::size_t fieldlength =
            last_letter_token + 1 - first_letter_token;
          __data[counter].resize( fieldlength );
          __data[counter].assign( str, first_letter_token, fieldlength );
        }
        else {
          __data[counter] = "";
        }

        counter++;

        if ( next_token == std::string::npos ) break;

        __getNextTriplet( str,
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
        __data.resize( counter );
        __data[counter - 1] = "";
      }
      else {
        __data.resize( counter );
      }

      __emptyData = false;
    }


    /// reopens a new input stream to parse
    template <template<typename> class ALLOC>
    void CSVParser<ALLOC>::useNewStream (std::istream&      instream,
                                         const std::string& delimiter,
                                         const char         commentmarker,
                                         const char         quoteMarker ) {
      __line.clear ();
      __delimiter = delimiter;
      __spaces =  " \t\r";
      __delimiterPlusSpaces = __delimiter + __spaces;
      __nbLine = std::size_t(0);
      __commentMarker = commentmarker;
      __quoteMarker = quoteMarker;
      __emptyData = true;
      __instream = &instream;
      __data.clear ();
    }

    
    // gets the next line of the csv stream and parses it
    template <template<typename> class ALLOC>
    INLINE bool CSVParser<ALLOC>::next () {
      while ( getline( *__instream, __line ) ) {
        __nbLine++;

        if ( __line.size() == std::size_t (0) ) continue;

        // fast recognition of commented or empty lines lines
        std::size_t lastPos =
          __line.find_first_not_of( __spaces, std::size_t(0) );
        
        if ( lastPos == std::string::npos ) continue;

        if ( __line.at( lastPos ) == __commentMarker ) continue;

        __tokenize( __line );
        return true;
      }

      return false;
    }

    
    // search for quote taking into account the '\'...
    template <template<typename> class ALLOC>
    INLINE std::size_t
    CSVParser<ALLOC>::__correspondingQuoteMarker( const std::string& str,
                                                  std::size_t pos ) const {
      std::size_t res = pos, before;

      while ( true ) {
        res = str.find_first_of( __quoteMarker, res + 1 );

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
      if ( __emptyData )
        GUM_ERROR( NullElement, "No parsed data" );

      return __data;
    }
    

    // returns the current nbLine of parser line
    template <template<typename> class ALLOC>
    INLINE const std::size_t CSVParser<ALLOC>::nbLine() const {
      if ( __nbLine == 0 )
        GUM_ERROR( NullElement, "No parsed data" );

      return __nbLine;
    }

  }  // namespace learning

}  // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
