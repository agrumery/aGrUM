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
 * @brief The class for initializing DatabaseTables and RawDatabaseTables
 * from CSV files
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
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
        filename__(filename), delimiter__(delimiter),
        comment_marker__(commentmarker), quote_marker__(quoteMarker),
        first_row_has_names__(fileContainsNames),
        input_stream__(filename__, std::ifstream::in),
        parser__(input_stream__, delimiter, commentmarker, quoteMarker),
        var_names__(alloc) {
      // check that the input file was opened correctly
      if ((input_stream__.rdstate() & std::ifstream::failbit) != 0) {
        GUM_ERROR(IOError, "File " << filename << " not found");
      }

      // if the first line contains names, store them into the intializer
      if (fileContainsNames) {
        parser__.next();
        var_names__ = parser__.current();
      }

      GUM_CONSTRUCTOR(DBInitializerFromCSV);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(
       const DBInitializerFromCSV< ALLOC >&                          from,
       const typename DBInitializerFromCSV< ALLOC >::allocator_type& alloc) :
        DBInitializerFromCSV< ALLOC >(from.filename__,
                                      from.first_row_has_names__,
                                      from.delimiter__,
                                      from.quote_marker__,
                                      from.comment_marker__,
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
        DBInitializerFromCSV< ALLOC >(from.filename__,
                                      from.first_row_has_names__,
                                      from.delimiter__,
                                      from.quote_marker__,
                                      from.comment_marker__,
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
    DBInitializerFromCSV< ALLOC >& DBInitializerFromCSV< ALLOC >::operator=(
       const DBInitializerFromCSV< ALLOC >& from) {
      if (this != &from) {
        IDBInitializer< ALLOC >::operator=(from);
        filename__                       = from.filename__;
        delimiter__                      = from.delimiter__;
        comment_marker__                 = from.comment_marker__;
        quote_marker__                   = from.quote_marker__;
        first_row_has_names__            = from.first_row_has_names__;

        // open the CSV file
        input_stream__.close();
        input_stream__.open(filename__, std::ifstream::in);

        // check that the input file was opened correctly
        if ((input_stream__.rdstate() & std::ifstream::failbit) != 0) {
          GUM_ERROR(IOError, "File " << filename__ << " not found");
        }

        // make the parser use the new input stream
        parser__.useNewStream(input_stream__,
                              delimiter__,
                              comment_marker__,
                              quote_marker__);

        // if the first line contains names, store them into the intializer
        if (first_row_has_names__) {
          parser__.next();
          var_names__ = parser__.current();
        }
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >& DBInitializerFromCSV< ALLOC >::operator=(
       DBInitializerFromCSV< ALLOC >&& from) {
      return operator=(from);
    }


    // ask the child class for the names of the variables
    template < template < typename > class ALLOC >
    INLINE std::vector< std::string, ALLOC< std::string > >
           DBInitializerFromCSV< ALLOC >::variableNames_() {
      return var_names__;
    }


    // asks the child class for the content of the current row using strings
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::string, ALLOC< std::string > >&
                 DBInitializerFromCSV< ALLOC >::currentStringRow_() {
      return parser__.current();
    }


    // indicates whether there is a next row to read (and point on it)
    template < template < typename > class ALLOC >
    INLINE bool DBInitializerFromCSV< ALLOC >::nextRow_() {
      return parser__.next();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
