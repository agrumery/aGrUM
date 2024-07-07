/**
 *
 *   Copyright (c) 2005-2024  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/** @file
 * @brief Class for fast parsing of CSV file (never more than one line in
 * application memory)
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *
 */
#include <agrum/base/database/CSVParser.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/base/database/CSVParser_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    CSVParser::CSVParser(std::istream&      instream,
                         const std::string& filename,
                         const std::string& delimiter,
                         const char         commentmarker,
                         const char         quoteMarker) :
        _line_(),
        _delimiter_(delimiter), _spaces_(" \t\r"), _delimiterPlusSpaces_(_delimiter_ + _spaces_),
        _nbLine_(std::size_t(0)), _commentMarker_(commentmarker), _quoteMarker_(quoteMarker),
        _emptyData_(true), _instream_(&instream), _filename_(filename) {
      GUM_CONSTRUCTOR(CSVParser);
    }


    /// destructor
    CSVParser::~CSVParser() { GUM_DESTRUCTOR(CSVParser); }


    void CSVParser::_getNextTriplet_(const std::string& str,
                                     std::size_t&       first_letter_token,
                                     std::size_t&       next_token,
                                     std::size_t&       last_letter_token,
                                     std::size_t        from) const {
      first_letter_token = str.find_first_not_of(_spaces_, from);

      if (first_letter_token == std::string::npos) {
        next_token = last_letter_token = first_letter_token;
        return;
      }

      if (str.at(first_letter_token) == _quoteMarker_) {
        last_letter_token = _correspondingQuoteMarker_(str, first_letter_token);

        if (last_letter_token == std::string::npos)
          GUM_SYNTAX_ERROR("String quote missing", _filename_, (Size)nbLine(), first_letter_token);

        next_token            = str.find_first_of(_delimiter_, last_letter_token + 1);
        std::size_t next_char = str.find_first_not_of(_spaces_, last_letter_token + 1);

        if (next_char < next_token) {
          GUM_SYNTAX_ERROR("Delimiter missing", _filename_, (Size)nbLine(), next_char);
        }

        // remove quote chars from the token
        first_letter_token += 1;
        last_letter_token -= 1;
      } else {
        next_token = str.find_first_of(_delimiter_, first_letter_token);

        if (next_token == std::string::npos) {
          last_letter_token = str.find_last_not_of(_spaces_, next_token);
        } else if (next_token == first_letter_token) {
          last_letter_token = first_letter_token;
        } else {
          last_letter_token = str.find_last_not_of(_delimiterPlusSpaces_, next_token - 1);
        }
      }
    }


    void CSVParser::_tokenize_(const std::string& s) {
      // looking for first commentMarker not in a string
      std::size_t commentMarker = s.find_first_of(_commentMarker_, 0);
      std::size_t quoteMarker   = s.find_first_of(_quoteMarker_, 0);
      std::size_t quoteMarkerEnd;

      while (quoteMarker < commentMarker) {
        quoteMarkerEnd = _correspondingQuoteMarker_(s, quoteMarker);

        if (quoteMarkerEnd == std::string::npos)
          GUM_SYNTAX_ERROR("String quote missing", _filename_, (Size)nbLine(), quoteMarker);

        while (commentMarker < quoteMarkerEnd) {   // the comment was in the quote
          commentMarker = s.find_first_of(_commentMarker_, commentMarker + 1);
        }

        quoteMarker = s.find_first_of(_quoteMarker_, quoteMarkerEnd + 1);
      }

      std::string str = s.substr(0, commentMarker);

      std::size_t counter = 0, first_letter_token, next_token, last_letter_token;

      _getNextTriplet_(str, first_letter_token, next_token, last_letter_token, 0);

      while ((std::string::npos != first_letter_token)
             && (std::string::npos != last_letter_token)) {
        if (_data_.size() <= counter) _data_.resize(counter + 1);

        if (first_letter_token == next_token) {
          _data_[counter] = "";
        } else if (last_letter_token >= first_letter_token) {
          const std::size_t fieldlength = last_letter_token + 1 - first_letter_token;
          _data_[counter].resize(fieldlength);
          _data_[counter].assign(str, first_letter_token, fieldlength);
        } else {
          _data_[counter] = "";
        }

        counter++;

        if (next_token == std::string::npos) break;

        _getNextTriplet_(str, first_letter_token, next_token, last_letter_token, next_token + 1);
      }

      // case where we end up with an empty field ...
      if ((first_letter_token == std::string::npos) && (last_letter_token == first_letter_token)
          && (next_token == first_letter_token)) {
        counter++;
        _data_.resize(counter);
        _data_[counter - 1] = "";
      } else {
        _data_.resize(counter);
      }

      _emptyData_ = false;
    }


    /// reopens a new input stream to parse
    void CSVParser::useNewStream(std::istream&      instream,
                                 const std::string& delimiter,
                                 const char         commentmarker,
                                 const char         quoteMarker) {
      _line_.clear();
      _delimiter_           = delimiter;
      _spaces_              = " \t\r";
      _delimiterPlusSpaces_ = _delimiter_ + _spaces_;
      _nbLine_              = std::size_t(0);
      _commentMarker_       = commentmarker;
      _quoteMarker_         = quoteMarker;
      _emptyData_           = true;
      _instream_            = &instream;
      _data_.clear();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
