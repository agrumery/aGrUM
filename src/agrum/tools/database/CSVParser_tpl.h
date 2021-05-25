/***************************************************************************
 *   Copyright (c) 2005-2020 by Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)  *
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
      :  _line_()
      ,  _delimiter_( delimiter )
      ,  _spaces_( " \t\r" )
      ,  _delimiterPlusSpaces_(  _delimiter_ +  _spaces_ )
      ,  _nbLine_( std::size_t(0) )
      ,  _commentMarker_( commentmarker )
      ,  _quoteMarker_( quoteMarker )
      ,  _emptyData_( true )
      ,  _instream_( &instream )
      ,  _data_( alloc ) {
      GUM_CONSTRUCTOR( CSVParser );
    }

    
    /// destructor
    template <template<typename> class ALLOC>
    CSVParser<ALLOC>::~CSVParser() {
      GUM_DESTRUCTOR( CSVParser );
    }
    
    template <template<typename> class ALLOC>
    void CSVParser<ALLOC>:: _getNextTriplet_( const std::string& str,
                                             std::size_t& first_letter_token,
                                             std::size_t& next_token,
                                             std::size_t& last_letter_token,
                                             std::size_t  from ) const {
      first_letter_token = str.find_first_not_of(  _spaces_, from );

      if ( first_letter_token == std::string::npos ) {
        next_token = last_letter_token = first_letter_token;
        return;
      }

      if ( str.at( first_letter_token ) ==  _quoteMarker_ ) {
        last_letter_token =  _correspondingQuoteMarker_( str, first_letter_token );

        if ( last_letter_token == std::string::npos )
          GUM_SYNTAX_ERROR( "String quote missing", (Size) nbLine(), first_letter_token );

        next_token = str.find_first_of(  _delimiter_, last_letter_token + 1 );
        std::size_t next_char =
          str.find_first_not_of(  _spaces_, last_letter_token + 1 );

        if ( next_char < next_token ) {
          GUM_SYNTAX_ERROR( "Delimiter missing at line", (Size) nbLine(), next_char );
        }
      }
      else {
        next_token = str.find_first_of(  _delimiter_, first_letter_token );

        if ( next_token == std::string::npos ) {
          last_letter_token = str.find_last_not_of(  _spaces_, next_token );
        }
        else if ( next_token == first_letter_token ) {
          last_letter_token = first_letter_token;
        }
        else {
          last_letter_token = 
              str.find_last_not_of(  _delimiterPlusSpaces_, next_token - 1 );
        }
      }
    }

    
    template <template<typename> class ALLOC>
    void CSVParser<ALLOC>:: _tokenize_( const std::string& s ) {
      // looking for first commentMarker not in a string
      std::size_t commentMarker = s.find_first_of(  _commentMarker_, 0 );
      std::size_t quoteMarker = s.find_first_of(  _quoteMarker_, 0 );
      std::size_t quoteMarkerEnd;

      while ( quoteMarker < commentMarker ) {
        quoteMarkerEnd =  _correspondingQuoteMarker_( s, quoteMarker );

        if ( quoteMarkerEnd == std::string::npos )
          GUM_SYNTAX_ERROR( "String quote missing", (Size) nbLine(), quoteMarker );

        while ( commentMarker < quoteMarkerEnd ) {  // the comment was in the quote
          commentMarker = s.find_first_of(  _commentMarker_, commentMarker + 1 );
        }

        quoteMarker = s.find_first_of(  _quoteMarker_, quoteMarkerEnd + 1 );
      }

      std::string str = s.substr( 0, commentMarker );

      std::size_t counter = 0, first_letter_token, next_token, last_letter_token;

       _getNextTriplet_(
          str, first_letter_token, next_token, last_letter_token, 0 );

      while ( ( std::string::npos != first_letter_token ) &&
              ( std::string::npos != last_letter_token ) ) {
        if (  _data_.size() <= counter )  _data_.resize( counter + 1 );

        if ( first_letter_token == next_token ) {
           _data_[counter] = "";
        }
        else if ( last_letter_token >= first_letter_token ) {
          const std::size_t fieldlength =
            last_letter_token + 1 - first_letter_token;
           _data_[counter].resize( fieldlength );
           _data_[counter].assign( str, first_letter_token, fieldlength );
        }
        else {
           _data_[counter] = "";
        }

        counter++;

        if ( next_token == std::string::npos ) break;

         _getNextTriplet_( str,
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
         _data_.resize( counter );
         _data_[counter - 1] = "";
      }
      else {
         _data_.resize( counter );
      }

       _emptyData_ = false;
    }


    /// reopens a new input stream to parse
    template <template<typename> class ALLOC>
    void CSVParser<ALLOC>::useNewStream (std::istream&      instream,
                                         const std::string& delimiter,
                                         const char         commentmarker,
                                         const char         quoteMarker ) {
       _line_.clear ();
       _delimiter_ = delimiter;
       _spaces_ =  " \t\r";
       _delimiterPlusSpaces_ =  _delimiter_ +  _spaces_;
       _nbLine_ = std::size_t(0);
       _commentMarker_ = commentmarker;
       _quoteMarker_ = quoteMarker;
       _emptyData_ = true;
       _instream_ = &instream;
       _data_.clear ();
    }

    
    // gets the next line of the csv stream and parses it
    template <template<typename> class ALLOC>
    INLINE bool CSVParser<ALLOC>::next () {
      while ( getline( * _instream_,  _line_ ) ) {
         _nbLine_++;

        if (  _line_.size() == std::size_t (0) ) continue;

        // fast recognition of commented or empty lines lines
        std::size_t lastPos =
           _line_.find_first_not_of(  _spaces_, std::size_t(0) );
        
        if ( lastPos == std::string::npos ) continue;

        if (  _line_.at( lastPos ) ==  _commentMarker_ ) continue;

         _tokenize_(  _line_ );
        return true;
      }

      return false;
    }

    
    // search for quote taking into account the '\'...
    template <template<typename> class ALLOC>
    INLINE std::size_t
    CSVParser<ALLOC>:: _correspondingQuoteMarker_( const std::string& str,
                                                  std::size_t pos ) const {
      std::size_t res = pos, before;

      while ( true ) {
        res = str.find_first_of(  _quoteMarker_, res + 1 );

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
      if (  _emptyData_ )
        GUM_ERROR( NullElement, "No parsed data" )

      return  _data_;
    }
    

    // returns the current nbLine of parser line
    template <template<typename> class ALLOC>
    INLINE const std::size_t CSVParser<ALLOC>::nbLine() const {
      if (  _nbLine_ == 0 )
        GUM_ERROR( NullElement, "No parsed data" )

      return  _nbLine_;
    }

  }  // namespace learning

}  // namespace gum

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
