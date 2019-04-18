
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
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
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// returns the allocator used
    template < template < typename > class ALLOC >
    INLINE typename IDBInitializer< ALLOC >::allocator_type
       IDBInitializer< ALLOC >::getAllocator() const {
      return __var_names.get_allocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(
       typename IDBInitializer< ALLOC >::InputType             type,
       const typename IDBInitializer< ALLOC >::allocator_type& alloc) :
        __var_names(alloc),
        __input_type(type) {
      GUM_CONSTRUCTOR(IDBInitializer);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(
       const IDBInitializer< ALLOC >&                          from,
       const typename IDBInitializer< ALLOC >::allocator_type& alloc) :
        __var_names(from.__var_names, alloc),
        __input_type(from.__input_type) {
      GUM_CONS_CPY(IDBInitializer);
    }


    /// copy constructor
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(
       const IDBInitializer< ALLOC >& from) :
        IDBInitializer(from, from.getAllocator()) {}


    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(
       IDBInitializer< ALLOC >&&                               from,
       const typename IDBInitializer< ALLOC >::allocator_type& alloc) :
        __var_names(std::move(from.__var_names), alloc),
        __input_type(from.__input_type) {
      GUM_CONS_MOV(IDBInitializer);
    }


    /// move constructor
    template < template < typename > class ALLOC >
    INLINE
       IDBInitializer< ALLOC >::IDBInitializer(IDBInitializer< ALLOC >&& from) :
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
      if (__var_names.empty()) __var_names = this->_variableNames();
      return __var_names;
    }


    // copy operator
    template < template < typename > class ALLOC >
    IDBInitializer< ALLOC >& IDBInitializer< ALLOC >::
                             operator=(const IDBInitializer< ALLOC >& from) {
      if (this != &from) {
        __var_names = from.__var_names;
        __input_type = from.__input_type;
        __last_insertion_failed = false;
      }
      return *this;
    }


    // move constructor
    template < template < typename > class ALLOC >
    IDBInitializer< ALLOC >& IDBInitializer< ALLOC >::
                             operator=(IDBInitializer< ALLOC >&& from) {
      if (this != &from) {
        __var_names = std::move(from.__var_names);
        __input_type = from.__input_type;
        __last_insertion_failed = false;
      }
      return *this;
    }


    /// fills the rows of the database
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class DATABASE >
    INLINE void IDBInitializer< ALLOC >::fillDatabase(DATABASE< ALLOC >& database,
                                                      const bool retry_insertion) {
      switch (__input_type) {
        case InputType::STRING:
          __fillDatabaseFromStrings(database, retry_insertion);
          return;

        case InputType::DBCELL:
          __fillDatabaseFromDBCells(database, retry_insertion);
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
    void IDBInitializer< ALLOC >::__fillDatabaseFromStrings(
       DATABASE< ALLOC >& database, const bool retry_insertion) {
      // if need be, try to reinsert the row that could not be inserted
      if (retry_insertion && __last_insertion_failed) {
        database.insertRow(_currentStringRow());
        __last_insertion_failed = false;
      }

      // try to insert the next rows
      while (this->_nextRow()) {
        try {
          // read a new line in the input file and insert it into the database
          database.insertRow(_currentStringRow());
        } catch (...) {
          __last_insertion_failed = true;
          throw;
        }
      }
    }


    /// fills the rows of the database using DBCell inputs
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class DATABASE >
    void IDBInitializer< ALLOC >::__fillDatabaseFromDBCells(
       DATABASE< ALLOC >& database, const bool retry_insertion) {
      // if need be, try to reinsert the row that could not be inserted
      if (retry_insertion && __last_insertion_failed) {
        database.insertRow(_currentDBCellRow());
        __last_insertion_failed = false;
      }

      // try to insert the next rows
      while (this->_nextRow()) {
        try {
          // read a new line in the input file and insert it into the database
          database.insertRow(_currentDBCellRow());
        } catch (...) {
          __last_insertion_failed = true;
          throw;
        }
      }
    }


    /// asks the child class for the content of the current row using strings
    template < template < typename > class ALLOC >
    const std::vector< std::string, ALLOC< std::string > >&
       IDBInitializer< ALLOC >::_currentStringRow() {
      GUM_ERROR(FatalError,
                "Method _currentStringRow should not be used or it should be "
                "overloaded in children classes.");
    }


    /// asks the child class for the content of the current row using dbcells
    template < template < typename > class ALLOC >
    const DBRow< DBCell, ALLOC >& IDBInitializer< ALLOC >::_currentDBCellRow() {
      GUM_ERROR(FatalError,
                "Method _currentDBCellRow should not be used or it should be "
                "overloaded in children classes.");
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
