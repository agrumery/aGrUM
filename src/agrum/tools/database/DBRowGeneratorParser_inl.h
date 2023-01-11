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
 * @brief Template implementation of the DBRowGeneratorParser class
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// copy operator
    INLINE DBRowGeneratorParser& DBRowGeneratorParser::operator=(const DBRowGeneratorParser& from) {
      if (this != &from) {
        _handler_        = from._handler_;
        _generator_set_  = from._generator_set_;
        _generator_size_ = from._generator_size_;
      }
      return *this;
    }


    /// move operator
    INLINE DBRowGeneratorParser& DBRowGeneratorParser::operator=(DBRowGeneratorParser&& from) {
      if (this != &from) {
        _handler_        = std::move(from._handler_);
        _generator_set_  = std::move(from._generator_set_);
        _generator_size_ = from._generator_size_;
      }
      return *this;
    }


    /// returns true if there are still rows that can be output by the RowFilter
    INLINE bool DBRowGeneratorParser::hasRows() {
      if (!_generator_size_) return _handler_.hasRows();

      while (!_generator_set_.hasRows()) {
        if (!_handler_.hasRows()) return false;
        _generator_set_.setInputRow(*_handler_);
        ++_handler_;
      }
      return true;
    }


    /// returns a new output row with its corresponding weight
    INLINE const DBRow< DBTranslatedValue >& DBRowGeneratorParser::row() {
      // if there exists no generator in the generator set, we just return
      // the row pointed to by the handler
      if (!_generator_size_) {
        const auto& pointed_row = *_handler_;
        ++_handler_;
        return pointed_row;
      } else {
        return _generator_set_.generate();
      }
    }


    /// resets the filter
    INLINE void DBRowGeneratorParser::reset() {
      _handler_.reset();
      _generator_set_.reset();
    }


    // returns the handler used by the filter
    INLINE typename DatabaseTable::Handler& DBRowGeneratorParser::handler() { return _handler_; }


    // returns the handler used by the filter
    INLINE const typename DatabaseTable::Handler& DBRowGeneratorParser::handler() const {
      return _handler_;
    }


    /// returns a reference on the database
    INLINE const DatabaseTable& DBRowGeneratorParser::database() const {
      return static_cast< const DatabaseTable& >(_handler_.database());
    }


    /// returns the generator set that is actually used
    INLINE DBRowGeneratorSet& DBRowGeneratorParser::generatorSet() { return _generator_set_; }


    /// sets the area in the database the handler will handle
    INLINE void DBRowGeneratorParser::setRange(std::size_t begin, std::size_t end) {
      _handler_.setRange(begin, end);
    }


    /// returns the generator set that is actually used
    INLINE const DBRowGeneratorSet& DBRowGeneratorParser::generatorSet() const {
      return _generator_set_;
    }


    /** @brief sets the columns of interest: the output DBRow needs only
     * contain values fot these columns */
    INLINE void DBRowGeneratorParser::setColumnsOfInterest(
       const std::vector< std::size_t >& cols_of_interest) {
      _generator_set_.setColumnsOfInterest(cols_of_interest);
    }


    /** @brief sets the columns of interest: the output DBRow needs only
     * contain values fot these columns */
    INLINE void
       DBRowGeneratorParser::setColumnsOfInterest(std::vector< std::size_t >&& cols_of_interest) {
      _generator_set_.setColumnsOfInterest(std::move(cols_of_interest));
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
