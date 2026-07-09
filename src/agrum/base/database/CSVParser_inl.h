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
#pragma once

#include <agrum/base/database/CSVParser.h>   // to ease IDE parser
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/base/database/CSVParser.h>

namespace gum {

  namespace learning {

    // gets the next line of the csv stream and parses it

    // search for quote taking into account the '\'...

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
