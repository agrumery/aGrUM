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

/** @file
 * @brief Inlined implementation for initializing DatabaseTables and
 * RawDatabaseTables from files or sql databases
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#  include <agrum/tools/database/IDBInitializer.h>

namespace gum {

  namespace learning {

    /// default constructor
    INLINE IDBInitializer::IDBInitializer(typename IDBInitializer::InputType type) :
        _input_type_(type) {
      GUM_CONSTRUCTOR(IDBInitializer);
    }


    /// copy constructor
    INLINE IDBInitializer::IDBInitializer(const IDBInitializer& from) :
        _var_names_(from._var_names_), _input_type_(from._input_type_) {
      GUM_CONS_CPY(IDBInitializer);
    }


    /// move constructor
    INLINE IDBInitializer::IDBInitializer(IDBInitializer&& from) :
        _var_names_(std::move(from._var_names_)), _input_type_(from._input_type_) {
      GUM_CONS_MOV(IDBInitializer);
    }


    /// returns the names of the variables in the input database
    INLINE const std::vector< std::string >& IDBInitializer::variableNames() {
      if (_var_names_.empty()) _var_names_ = this->variableNames_();
      return _var_names_;
    }


    /// asks the child class for the content of the current row using strings
    INLINE const std::vector< std::string >& IDBInitializer::currentStringRow_() {
      GUM_ERROR(FatalError,
                "Method currentStringRow_ should not be used or it should be "
                "overloaded in children classes.");
    }


    /// asks the child class for the content of the current row using dbcells
    INLINE const DBRow< DBCell >& IDBInitializer::currentDBCellRow_() {
      GUM_ERROR(FatalError,
                "Method currentDBCellRow_ should not be used or it should be "
                "overloaded in children classes.");
    }

  } /* namespace learning */

} /* namespace gum */

#endif   // DOXYGEN_SHOULD_SKIP_THIS
