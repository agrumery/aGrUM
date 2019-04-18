
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
 * @brief The class for initializing DatabaseTables and RawDatabaseTables
 * from CSV files
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(
       const std::string filename,
       bool              fileContainsNames,
       const std::string delimiter,
       const char        commentmarker,
       const char        quoteMarker,
       const typename DBInitializerFromCSV< ALLOC >::allocator_type& alloc) :
        IDBInitializer< ALLOC >(IDBInitializer< ALLOC >::InputType::STRING, alloc),
        __filename(filename), __delimiter(delimiter),
        __comment_marker(commentmarker), __quote_marker(quoteMarker),
        __first_row_has_names(fileContainsNames),
        __input_stream(__filename, std::ifstream::in),
        __parser(__input_stream, delimiter, commentmarker, quoteMarker),
        __var_names(alloc) {
      // check that the input file was opened correctly
      if ((__input_stream.rdstate() & std::ifstream::failbit) != 0) {
        GUM_ERROR(IOError, "File " << filename << " not found");
      }

      // if the first line contains names, store them into the intializer
      if (fileContainsNames) {
        __parser.next();
        __var_names = __parser.current();
      }

      GUM_CONSTRUCTOR(DBInitializerFromCSV);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(
       const DBInitializerFromCSV< ALLOC >&                          from,
       const typename DBInitializerFromCSV< ALLOC >::allocator_type& alloc) :
        DBInitializerFromCSV< ALLOC >(from.__filename,
                                      from.__first_row_has_names,
                                      from.__delimiter,
                                      from.__quote_marker,
                                      from.__comment_marker,
                                      alloc) {}

    /// copy constructor
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(
       const DBInitializerFromCSV< ALLOC >& from) :
        DBInitializerFromCSV< ALLOC >(from, from.getAllocator()) {}

    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(
       DBInitializerFromCSV< ALLOC >&&                               from,
       const typename DBInitializerFromCSV< ALLOC >::allocator_type& alloc) :
        DBInitializerFromCSV< ALLOC >(from.__filename,
                                      from.__first_row_has_names,
                                      from.__delimiter,
                                      from.__quote_marker,
                                      from.__comment_marker,
                                      alloc) {}


    /// move constructor
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(
       DBInitializerFromCSV< ALLOC >&& from) :
        DBInitializerFromCSV< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >* DBInitializerFromCSV< ALLOC >::clone(
       const typename DBInitializerFromCSV< ALLOC >::allocator_type& alloc) const {
      ALLOC< DBInitializerFromCSV< ALLOC > > allocator(alloc);
      DBInitializerFromCSV< ALLOC >* new_initializer = allocator.allocate(1);
      try {
        allocator.construct(new_initializer, *this, alloc);
      } catch (...) {
        allocator.deallocate(new_initializer, 1);
        throw;
      }

      return new_initializer;
    }


    /// virtual copy constructor
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >* DBInitializerFromCSV< ALLOC >::clone() const {
      return clone(this->getAllocator());
    }


    /// destructor
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::~DBInitializerFromCSV() {
      GUM_DESTRUCTOR(DBInitializerFromCSV);
    }


    /// copy operator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >& DBInitializerFromCSV< ALLOC >::
                                   operator=(const DBInitializerFromCSV< ALLOC >& from) {
      if (this != &from) {
        IDBInitializer< ALLOC >::operator=(from);
        __filename = from.__filename;
        __delimiter = from.__delimiter;
        __comment_marker = from.__comment_marker;
        __quote_marker = from.__quote_marker;
        __first_row_has_names = from.__first_row_has_names;

        // open the CSV file
        __input_stream.close();
        __input_stream.open(__filename, std::ifstream::in);

        // check that the input file was opened correctly
        if ((__input_stream.rdstate() & std::ifstream::failbit) != 0) {
          GUM_ERROR(IOError, "File " << __filename << " not found");
        }

        // make the parser use the new input stream
        __parser.useNewStream(
           __input_stream, __delimiter, __comment_marker, __quote_marker);

        // if the first line contains names, store them into the intializer
        if (__first_row_has_names) {
          __parser.next();
          __var_names = __parser.current();
        }
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >& DBInitializerFromCSV< ALLOC >::
                                   operator=(DBInitializerFromCSV< ALLOC >&& from) {
      return operator=(from);
    }


    // ask the child class for the names of the variables
    template < template < typename > class ALLOC >
    INLINE std::vector< std::string, ALLOC< std::string > >
           DBInitializerFromCSV< ALLOC >::_variableNames() {
      return __var_names;
    }


    // asks the child class for the content of the current row using strings
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::string, ALLOC< std::string > >&
                 DBInitializerFromCSV< ALLOC >::_currentStringRow() {
      return __parser.current();
    }


    // indicates whether there is a next row to read (and point on it)
    template < template < typename > class ALLOC >
    INLINE bool DBInitializerFromCSV< ALLOC >::_nextRow() {
      return __parser.next();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
