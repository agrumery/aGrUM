/**
 *
 *   Copyright 2005-2020 Pierre-Henri WUILLEMIN(@LIP6) & Christophe GONZALES(@AMU)
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
      return var_names__.get_allocator();
    }


    /// default constructor
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(
       typename IDBInitializer< ALLOC >::InputType             type,
       const typename IDBInitializer< ALLOC >::allocator_type& alloc) :
        var_names__(alloc),
        input_type__(type) {
      GUM_CONSTRUCTOR(IDBInitializer);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    INLINE IDBInitializer< ALLOC >::IDBInitializer(
       const IDBInitializer< ALLOC >&                          from,
       const typename IDBInitializer< ALLOC >::allocator_type& alloc) :
        var_names__(from.var_names__, alloc),
        input_type__(from.input_type__) {
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
        var_names__(std::move(from.var_names__), alloc),
        input_type__(from.input_type__) {
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
      if (var_names__.empty()) var_names__ = this->variableNames_();
      return var_names__;
    }


    // copy operator
    template < template < typename > class ALLOC >
    IDBInitializer< ALLOC >&
       IDBInitializer< ALLOC >::operator=(const IDBInitializer< ALLOC >& from) {
      if (this != &from) {
        var_names__             = from.var_names__;
        input_type__            = from.input_type__;
        last_insertion_failed__ = false;
      }
      return *this;
    }


    // move constructor
    template < template < typename > class ALLOC >
    IDBInitializer< ALLOC >&
       IDBInitializer< ALLOC >::operator=(IDBInitializer< ALLOC >&& from) {
      if (this != &from) {
        var_names__             = std::move(from.var_names__);
        input_type__            = from.input_type__;
        last_insertion_failed__ = false;
      }
      return *this;
    }


    /// fills the rows of the database
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class DATABASE >
    INLINE void IDBInitializer< ALLOC >::fillDatabase(DATABASE< ALLOC >& database,
                                                      const bool retry_insertion) {
      switch (input_type__) {
        case InputType::STRING:
          fillDatabaseFromStrings__(database, retry_insertion);
          return;

        case InputType::DBCELL:
          fillDatabaseFromDBCells__(database, retry_insertion);
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
    void IDBInitializer< ALLOC >::fillDatabaseFromStrings__(
       DATABASE< ALLOC >& database,
       const bool         retry_insertion) {
      // if need be, try to reinsert the row that could not be inserted
      if (retry_insertion && last_insertion_failed__) {
        database.insertRow(currentStringRow_());
        last_insertion_failed__ = false;
      }

      // try to insert the next rows
      while (this->nextRow_()) {
        try {
          // read a new line in the input file and insert it into the database
          database.insertRow(currentStringRow_());
        } catch (...) {
          last_insertion_failed__ = true;
          throw;
        }
      }
    }


    /// fills the rows of the database using DBCell inputs
    template < template < typename > class ALLOC >
    template < template < template < typename > class > class DATABASE >
    void IDBInitializer< ALLOC >::fillDatabaseFromDBCells__(
       DATABASE< ALLOC >& database,
       const bool         retry_insertion) {
      // if need be, try to reinsert the row that could not be inserted
      if (retry_insertion && last_insertion_failed__) {
        database.insertRow(currentDBCellRow_());
        last_insertion_failed__ = false;
      }

      // try to insert the next rows
      while (this->nextRow_()) {
        try {
          // read a new line in the input file and insert it into the database
          database.insertRow(currentDBCellRow_());
        } catch (...) {
          last_insertion_failed__ = true;
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
