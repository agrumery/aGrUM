/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// returns the allocator used
    template < template < typename > class ALLOC >
    INLINE typename IDBInitializer< ALLOC >::allocator_type
       IDBInitializer< ALLOC >::getAllocator() const {
      return _var_names_.get_allocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(
       typename IDBInitializer< ALLOC >::InputType             type,
       const typename IDBInitializer< ALLOC >::allocator_type& alloc) :
        _var_names_(alloc),
        _input_type_(type) {
      GUM_CONSTRUCTOR(IDBInitializer);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(
       const IDBInitializer< ALLOC >&                          from,
       const typename IDBInitializer< ALLOC >::allocator_type& alloc) :
        _var_names_(from._var_names_, alloc),
        _input_type_(from._input_type_) {
      GUM_CONS_CPY(IDBInitializer);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(const IDBInitializer< ALLOC >& from) :
        IDBInitializer(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(
       IDBInitializer< ALLOC >&&                               from,
       const typename IDBInitializer< ALLOC >::allocator_type& alloc) :
        _var_names_(std::move(from._var_names_), alloc),
        _input_type_(from._input_type_) {
      GUM_CONS_MOV(IDBInitializer);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(IDBInitializer< ALLOC >&& from) :
        IDBInitializer(std::move(from), from.getAllocator()) {}


    /// destructor
    template < template < typename > class ALLOC >
    IDBInitializer< ALLOC >::~IDBInitializer() {
      GUM_DESTRUCTOR(IDBInitializer);
    }


    /// returns the names of the variables in the input database
    template < template < typename > class ALLOC >
    const std::vector< std::string, ALLOC< std::string > >&
       IDBInitializer< ALLOC >::variableNames() {
      if (_var_names_.empty()) _var_names_ = this->variableNames_();
      return _var_names_;
    }


    // copy operator
    template < template < typename > class ALLOC >
    IDBInitializer< ALLOC >&
       IDBInitializer< ALLOC >::operator=(const IDBInitializer< ALLOC >& from) {
      if (this != &from) {
        _var_names_             = from._var_names_;
        _input_type_            = from._input_type_;
        _last_insertion_failed_ = false;
      }
      return *this;
    }


    // move constructor
    template < template < typename > class ALLOC >
    IDBInitializer< ALLOC >& IDBInitializer< ALLOC >::operator=(IDBInitializer< ALLOC >&& from) {
      if (this != &from) {
        _var_names_             = std::move(from._var_names_);
        _input_type_            = from._input_type_;
        _last_insertion_failed_ = false;
      }
      return *this;
    }


    /// fills the rows of the database
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class DATABASE >
    INLINE void IDBInitializer< ALLOC >::fillDatabase(DATABASE< ALLOC >& database,
                                                      const bool         retry_insertion) {
      switch (_input_type_) {
        case InputType::STRING:
          _fillDatabaseFromStrings_(database, retry_insertion);
          return;

        case InputType::DBCELL:
          _fillDatabaseFromDBCells_(database, retry_insertion);
          return;

        default:
          GUM_ERROR(NotImplementedYet,
                    "fillDatabase has not been implemented yet for this "
                    "type of IDBInitializerInputType");
      }
    }


    /// fills the rows of the database using string inputs
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class DATABASE >
    void IDBInitializer< ALLOC >::_fillDatabaseFromStrings_(DATABASE< ALLOC >& database,
                                                            const bool         retry_insertion) {
      // if need be, try to reinsert the row that could not be inserted
      if (retry_insertion && _last_insertion_failed_) {
        database.insertRow(currentStringRow_());
        _last_insertion_failed_ = false;
      }

      // try to insert the next rows
      while (this->nextRow_()) {
        try {
          // read a new line in the input file and insert it into the database
          database.insertRow(currentStringRow_());
        } catch (...) {
          _last_insertion_failed_ = true;
          throw;
        }
      }
    }


    /// fills the rows of the database using DBCell inputs
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class DATABASE >
    void IDBInitializer< ALLOC >::_fillDatabaseFromDBCells_(DATABASE< ALLOC >& database,
                                                            const bool         retry_insertion) {
      // if need be, try to reinsert the row that could not be inserted
      if (retry_insertion && _last_insertion_failed_) {
        database.insertRow(currentDBCellRow_());
        _last_insertion_failed_ = false;
      }

      // try to insert the next rows
      while (this->nextRow_()) {
        try {
          // read a new line in the input file and insert it into the database
          database.insertRow(currentDBCellRow_());
        } catch (...) {
          _last_insertion_failed_ = true;
          throw;
        }
      }
    }


    /// asks the child class for the content of the current row using strings
    template < template < typename > class ALLOC >
    const std::vector< std::string, ALLOC< std::string > >&
       IDBInitializer< ALLOC >::currentStringRow_() {
      GUM_ERROR(FatalError,
                "Method currentStringRow_ should not be used or it should be "
                "overloaded in children classes.");
    }


    /// asks the child class for the content of the current row using dbcells
    template < template < typename > class ALLOC >
    const DBRow< DBCell, ALLOC >& IDBInitializer< ALLOC >::currentDBCellRow_() {
      GUM_ERROR(FatalError,
                "Method currentDBCellRow_ should not be used or it should be "
                "overloaded in children classes.");
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
