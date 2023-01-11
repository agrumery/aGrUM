/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/database/CSVParser.h>

namespace gum {

  namespace learning {

    // gets the next line of the csv stream and parses it
    INLINE bool CSVParser::next() {
      while (getline(*_instream_, _line_)) {
        _nbLine_++;

        if (_line_.size() == std::size_t(0)) continue;

        // fast recognition of commented or empty lines lines
        std::size_t lastPos = _line_.find_first_not_of(_spaces_, std::size_t(0));

        if (lastPos == std::string::npos) continue;

        if (_line_.at(lastPos) == _commentMarker_) continue;

        _tokenize_(_line_);
        return true;
      }

      return false;
    }


    // search for quote taking into account the '\'...
    INLINE std::size_t CSVParser::_correspondingQuoteMarker_(const std::string& str,
                                                             std::size_t        pos) const {
      std::size_t res = pos, before;

      while (true) {
        res = str.find_first_of(_quoteMarker_, res + 1);

        if (res == std::string::npos) return res;   // no quote found

        before = str.find_last_not_of('\\', res - 1);

        if (before == std::string::npos) return res;   // quote found, it is the good one

        if ((res - before) % 2 == 1)
          return res;   // the quote is the good one, even if there are some '\'
                        // before
      }
    }


    // returns the current parsed line
    INLINE const std::vector< std::string >& CSVParser::current() const {
      if (_emptyData_) GUM_ERROR(NullElement, "No parsed data")

      return _data_;
    }


    // returns the current nbLine of parser line
    INLINE std::size_t CSVParser::nbLine() const {
      if (_nbLine_ == 0) GUM_ERROR(NullElement, "No parsed data")

      return _nbLine_;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
