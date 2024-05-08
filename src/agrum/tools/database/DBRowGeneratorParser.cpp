/**
 *
 *   Copyright (c) 2005-2024 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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

/**
 * @file
 * @brief Class to include at least once this header
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#include <agrum/tools/database/DBRowGeneratorParser.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/DBRowGeneratorParser_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DBRowGeneratorParser::DBRowGeneratorParser(const typename DatabaseTable::Handler& handler,
                                               const DBRowGeneratorSet& generator_set) :
        _handler_(handler),
        _generator_set_(generator_set), _generator_size_(generator_set.size()) {
      GUM_CONSTRUCTOR(DBRowGeneratorParser);
    }


    /// copy constructor
    DBRowGeneratorParser::DBRowGeneratorParser(const DBRowGeneratorParser& from) :
        _handler_(from._handler_), _generator_set_(from._generator_set_),
        _generator_size_(from._generator_size_) {
      GUM_CONS_CPY(DBRowGeneratorParser);
    }


    /// move constructor
    DBRowGeneratorParser::DBRowGeneratorParser(DBRowGeneratorParser&& from) :
        _handler_(std::move(from._handler_)), _generator_set_(std::move(from._generator_set_)),
        _generator_size_(from._generator_size_) {
      GUM_CONS_MOV(DBRowGeneratorParser);
    }


    /// virtual copy constructor
    DBRowGeneratorParser* DBRowGeneratorParser::clone() const {
      return new DBRowGeneratorParser(*this);
    }


    /// destructor
    DBRowGeneratorParser::~DBRowGeneratorParser() { GUM_DESTRUCTOR(DBRowGeneratorParser); }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
