/**
 *
 *   Copyright (c) 2005-2021 by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
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
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(
       const std::string                                             filename,
       bool                                                          fileContainsNames,
       const std::string                                             delimiter,
       const char                                                    commentmarker,
       const char                                                    quoteMarker,
       const typename DBInitializerFromCSV< ALLOC >::allocator_type& alloc) :
        IDBInitializer< ALLOC >(IDBInitializer< ALLOC >::InputType::STRING, alloc),
        _filename_(filename), _delimiter_(delimiter), _comment_marker_(commentmarker),
        _quote_marker_(quoteMarker), _first_row_has_names_(fileContainsNames),
        _input_stream_(_filename_, std::ifstream::in),
        _parser_(_input_stream_, filename, delimiter, commentmarker, quoteMarker),
        _var_names_(alloc) {
      // check that the input file was opened correctly
      if ((_input_stream_.rdstate() & std::ifstream::failbit) != 0) {
        GUM_ERROR(IOError, "File " << filename << " not found")
      }

      // if the first line contains names, store them into the intializer
      if (fileContainsNames) {
        _parser_.next();
        _var_names_ = _parser_.current();
      }

      GUM_CONSTRUCTOR(DBInitializerFromCSV);
    }


    /// copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(
       const DBInitializerFromCSV< ALLOC >&                          from,
       const typename DBInitializerFromCSV< ALLOC >::allocator_type& alloc) :
        DBInitializerFromCSV< ALLOC >(from._filename_,
                                      from._first_row_has_names_,
                                      from._delimiter_,
                                      from._quote_marker_,
                                      from._comment_marker_,
                                      alloc) {}

    /// copy constructor
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(const DBInitializerFromCSV< ALLOC >& from) :
        DBInitializerFromCSV< ALLOC >(from, from.getAllocator()) {}

    /// move constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(
       DBInitializerFromCSV< ALLOC >&&                               from,
       const typename DBInitializerFromCSV< ALLOC >::allocator_type& alloc) :
        DBInitializerFromCSV< ALLOC >(from._filename_,
                                      from._first_row_has_names_,
                                      from._delimiter_,
                                      from._quote_marker_,
                                      from._comment_marker_,
                                      alloc) {}


    /// move constructor
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >::DBInitializerFromCSV(DBInitializerFromCSV< ALLOC >&& from) :
        DBInitializerFromCSV< ALLOC >(std::move(from), from.getAllocator()) {}


    /// virtual copy constructor with a given allocator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >* DBInitializerFromCSV< ALLOC >::clone(
       const typename DBInitializerFromCSV< ALLOC >::allocator_type& alloc) const {
      ALLOC< DBInitializerFromCSV< ALLOC > > allocator(alloc);
      DBInitializerFromCSV< ALLOC >*         new_initializer = allocator.allocate(1);
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
    DBInitializerFromCSV< ALLOC >&
       DBInitializerFromCSV< ALLOC >::operator=(const DBInitializerFromCSV< ALLOC >& from) {
      if (this != &from) {
        IDBInitializer< ALLOC >::operator=(from);
        _filename_                       = from._filename_;
        _delimiter_                      = from._delimiter_;
        _comment_marker_                 = from._comment_marker_;
        _quote_marker_                   = from._quote_marker_;
        _first_row_has_names_            = from._first_row_has_names_;

        // open the CSV file
        _input_stream_.close();
        _input_stream_.open(_filename_, std::ifstream::in);

        // check that the input file was opened correctly
        if ((_input_stream_.rdstate() & std::ifstream::failbit) != 0) {
          GUM_ERROR(IOError, "File " << _filename_ << " not found")
        }

        // make the parser use the new input stream
        _parser_.useNewStream(_input_stream_, _delimiter_, _comment_marker_, _quote_marker_);

        // if the first line contains names, store them into the intializer
        if (_first_row_has_names_) {
          _parser_.next();
          _var_names_ = _parser_.current();
        }
      }

      return *this;
    }


    /// move operator
    template < template < typename > class ALLOC >
    DBInitializerFromCSV< ALLOC >&
       DBInitializerFromCSV< ALLOC >::operator=(DBInitializerFromCSV< ALLOC >&& from) {
      return operator=(from);
    }


    // ask the child class for the names of the variables
    template < template < typename > class ALLOC >
    INLINE std::vector< std::string, ALLOC< std::string > >
           DBInitializerFromCSV< ALLOC >::variableNames_() {
      return _var_names_;
    }


    // asks the child class for the content of the current row using strings
    template < template < typename > class ALLOC >
    INLINE const std::vector< std::string, ALLOC< std::string > >&
                 DBInitializerFromCSV< ALLOC >::currentStringRow_() {
      return _parser_.current();
    }


    // indicates whether there is a next row to read (and point on it)
    template < template < typename > class ALLOC >
    INLINE bool DBInitializerFromCSV< ALLOC >::nextRow_() {
      return _parser_.next();
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
