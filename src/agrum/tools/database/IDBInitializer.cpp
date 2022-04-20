/**
 *
 *   Copyright (c) 2005-2022 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @brief The base class for initializing DatabaseTables and RawDatabaseTables
 * from files or sql databases
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/database/IDBInitializer.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/IDBInitializer_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    // destructor
    IDBInitializer::~IDBInitializer() { GUM_DESTRUCTOR(IDBInitializer); }

    // copy operator
    IDBInitializer& IDBInitializer::operator=(const IDBInitializer& from) {
      if (this != &from) {
        _var_names_             = from._var_names_;
        _input_type_            = from._input_type_;
        _last_insertion_failed_ = false;
      }
      return *this;
    }

    // move constructor
    IDBInitializer& IDBInitializer::operator=(IDBInitializer&& from) {
      if (this != &from) {
        _var_names_             = std::move(from._var_names_);
        _input_type_            = from._input_type_;
        _last_insertion_failed_ = false;
      }
      return *this;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
