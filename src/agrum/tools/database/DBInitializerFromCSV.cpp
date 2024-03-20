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
 * @brief The class for initializing DatabaseTables and RawDatabaseTables
 * from CSV files
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */

#include <agrum/tools/database/DBInitializerFromCSV.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/// include the inlined functions if necessary
#  ifdef GUM_NO_INLINE
#    include <agrum/tools/database/DBInitializerFromCSV_inl.h>
#  endif /* GUM_NO_INLINE */

namespace gum {

  namespace learning {

    /// default constructor
    DBInitializerFromCSV::DBInitializerFromCSV(const std::string filename,
                                               bool              fileContainsNames,
                                               const std::string delimiter,
                                               const char        commentmarker,
                                               const char        quoteMarker) :
        IDBInitializer(IDBInitializer::InputType::STRING), _filename_(filename),
        _delimiter_(delimiter), _comment_marker_(commentmarker), _quote_marker_(quoteMarker),
        _first_row_has_names_(fileContainsNames), _input_stream_(_filename_, std::ifstream::in),
        _parser_(_input_stream_, filename, delimiter, commentmarker, quoteMarker) {
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

    /// copy constructor
    DBInitializerFromCSV::DBInitializerFromCSV(const DBInitializerFromCSV& from) :
        DBInitializerFromCSV(from._filename_,
                             from._first_row_has_names_,
                             from._delimiter_,
                             from._quote_marker_,
                             from._comment_marker_) {}

    /// move constructor
    DBInitializerFromCSV::DBInitializerFromCSV(DBInitializerFromCSV&& from) :
        DBInitializerFromCSV(from._filename_,
                             from._first_row_has_names_,
                             from._delimiter_,
                             from._quote_marker_,
                             from._comment_marker_) {}

    /// virtual copy constructor
    DBInitializerFromCSV* DBInitializerFromCSV::clone() const {
      return new DBInitializerFromCSV(*this);
    }

    /// destructor
    DBInitializerFromCSV::~DBInitializerFromCSV() { GUM_DESTRUCTOR(DBInitializerFromCSV); }

    /// copy operator
    DBInitializerFromCSV& DBInitializerFromCSV::operator=(const DBInitializerFromCSV& from) {
      if (this != &from) {
        IDBInitializer::operator=(from);
        _filename_            = from._filename_;
        _delimiter_           = from._delimiter_;
        _comment_marker_      = from._comment_marker_;
        _quote_marker_        = from._quote_marker_;
        _first_row_has_names_ = from._first_row_has_names_;

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
    DBInitializerFromCSV& DBInitializerFromCSV::operator=(DBInitializerFromCSV&& from) {
      return operator=(from);
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
