/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr                                               *
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
#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <agrum/core/exceptions.h>
#include <agrum/core/utilsString.h>
#include <agrum/learning/database.h>

#ifdef GUM_NO_INLINE
#include <agrum/learning/database.inl>
#endif /* GUM_NO_INLINE */


// the function used to parse the mixed XML/CSV files
extern bool GUM_PARSE_XML_CSV_parser( gum::Database&, const std::string& );


namespace gum {


  // ==============================================================================
  /// basic constructor
  // ==============================================================================
  Database::Database() : __nb_nodes( 0 ), __nb_cases( 0 ), __cases( 0 ),__filename( "" ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( Database );

    // initialize the __iterators begin/rbegin/end/rend
    __iter_begin.initializeIterator
    ( *this, DatabaseIterator::GUM_DATABASE_ITER_BEGIN );
    __iter_rbegin.initializeIterator
    ( *this, DatabaseIterator::GUM_DATABASE_ITER_RBEGIN );
    __iter_end.initializeIterator
    ( *this, DatabaseIterator::GUM_DATABASE_ITER_END );
    __iter_rend.initializeIterator
    ( *this, DatabaseIterator::GUM_DATABASE_ITER_REND );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  Database::Database( const Database& from ) :
      __nb_nodes( from.__nb_nodes ), __nb_cases( from.__nb_cases ),
      __node_names( from.__node_names ), __node_name_per_id( from.__node_name_per_id ),
      __nb_modalities( from.__nb_modalities ), __modalities_names( from.__modalities_names ),
      __cases( new unsigned int [__nb_nodes * __nb_cases] ),__filename( from.__filename ) {
    // for debugging purposes
    GUM_CONS_CPY( Database );
    // copy the __cases
    memcpy( __cases, from.__cases, __nb_nodes * __nb_cases * sizeof( unsigned int ) );
    // initialize the __iterators begin/rbegin/end/rend
    __iter_begin.initializeIterator
    ( *this, DatabaseIterator::GUM_DATABASE_ITER_BEGIN );
    __iter_rbegin.initializeIterator
    ( *this, DatabaseIterator::GUM_DATABASE_ITER_RBEGIN );
    __iter_end.initializeIterator
    ( *this, DatabaseIterator::GUM_DATABASE_ITER_END );
    __iter_rend.initializeIterator
    ( *this, DatabaseIterator::GUM_DATABASE_ITER_REND );
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  Database& Database::operator= ( const Database& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY( Database );
      // remove the old content

      if ( __cases ) delete[] __cases;

      // detach the __iterators pointing to this
      for ( ListIterator<DatabaseIterator*> iter = __iterators.begin();
            iter != __iterators.end(); ++iter )
        ( *iter )->clear();

      // set the new content
      __nb_nodes = from.__nb_nodes;

      __nb_cases = from.__nb_cases;

      __node_names = from.__node_names;

      __node_name_per_id = from.__node_name_per_id;

      __nb_modalities = from.__nb_modalities;

      __filename= from.__filename;

      __cases = new unsigned int [__nb_nodes * __nb_cases];

      memcpy( __cases, from.__cases, __nb_nodes * __nb_cases * sizeof( unsigned int ) );

      // initialize the __iterators begin/rbegin/end/rend
      __iter_begin.initializeIterator
      ( *this, DatabaseIterator::GUM_DATABASE_ITER_BEGIN );

      __iter_end.initializeIterator
      ( *this, DatabaseIterator::GUM_DATABASE_ITER_END );

      __iter_rend.initializeIterator
      ( *this, DatabaseIterator::GUM_DATABASE_ITER_REND );

      __iter_rbegin.initializeIterator
      ( *this, DatabaseIterator::GUM_DATABASE_ITER_RBEGIN );
    }

    return *this;
  }

  // ==============================================================================
  /// destructor
  //== ==============================================================================
  Database::~Database() {
    // for debugging purposes
    GUM_DESTRUCTOR( Database );

    if ( __cases ) delete[] __cases;

    // detach the __iterators
    for ( ListIterator<DatabaseIterator*> iter = __iterators.begin();
          iter != __iterators.end(); ++iter )
      ( *iter )->clear();
  }

// local function which checks if a line is empty or is a comment line.
  bool isValidLine( std::string s ) {
    unsigned int l=s.length();

    if ( l==0 ) return false;

    for ( unsigned int i=0;i<l;i++ ) {
      const char& c=s.at( i );

      if ( c=='#' ) return false;

      if ( c!=' ' ) return true;
    }

    return false;
  }

  // ==============================================================================
  /// creates a new database from a pure CSV file
  // ==============================================================================
  Database Database::createFromCSV( const std::string& filename,
                                    char separator_separator,
                                    char field_delimiter,
                                    char escape_char ) {
    // create a new empty database, to be filled later
    Database database;
    // create, for each node, a hashtable associating to its modalities names
    // the index (ID) of the modality
    std::vector< HashTable<std::string, unsigned int> > modal_names;

    // get the number of nodes and the number of __cases n the database
    {
      std::ifstream inFile( filename.c_str(), std::ios::in );

      if ( ! inFile.is_open() )
        GUM_ERROR( IOError, "cannot open file" + filename );

      database.__filename=filename;

      // if the file is empty, do nothing
      if ( inFile.eof() )
        return database;

      // get the header line, its number of fields, and fill the nodes names
      std::string str;

      getline( inFile, str );

      if ( !inFile.good() )
        GUM_ERROR( IOError, "error parsing file" + filename );

      database.__node_names =
        SplitCSVLine( str, separator_separator, field_delimiter, escape_char );

      database.__nb_nodes = database.__node_names.size();

      for ( unsigned int i = 0; i < database.__nb_nodes; ++i )
        database.__node_name_per_id.insert( database.__node_names[i], i );

      // get the number of __cases
      for ( database.__nb_cases = 0; getline( inFile, str ); database.__nb_cases+=( isValidLine( str ) )?1:0 ); // we do not count empty lines
    }

    // open the file to fill the __cases and skip the header
    std::ifstream inFile( filename.c_str(), std::ios::in );

    if ( ! inFile.is_open() )
      GUM_ERROR( IOError, "cannot open file" + filename );

    std::string str;

    getline( inFile, str );

    // fill the __cases and get the modalities names
    modal_names.resize( database.__nb_nodes,HashTable<std::string, unsigned int>() );

    database.__nb_modalities.resize( database.__nb_nodes, 0 );

    database.__missing_value.resize( database.__nb_nodes, false );

    database.__cases = new unsigned int [database.__nb_cases * database.__nb_nodes];

    unsigned int *ptrcases = database.__cases;

    while ( ! inFile.eof() ) {
      // get the content of the new line
      getline( inFile, str );

      if ( ! isValidLine( str ) ) continue; // get rid of empty lines

      std::vector<std::string> line =
        SplitCSVLine( str, separator_separator, field_delimiter, escape_char );

      // check that it has exactly __nb_nodes fields
      if (( line.size() != database.__nb_nodes ) && ( line.size() != 0 ) ) {
        GUM_ERROR( IOError, "CSV file does not have a constant number of fields" );
      }

      // get the modalities
      for ( unsigned int i = 0; i < line.size(); ++i, ++ptrcases ) {
        if ( ! modal_names[i].exists( line[i] ) ) {
          modal_names[i].insert( line[i], database.__nb_modalities[i] );
          ++database.__nb_modalities[i];
        }

        *ptrcases = modal_names[i][line[i]];
      }
    }

    // store appropriately the names of the modalities
    database.__modalities_names.resize( database.__nb_nodes,
                                        std::vector<std::string>() );

    for ( unsigned int i = 0; i < database.__nb_nodes; ++i ) {
      database.__modalities_names[i].resize( database.__nb_modalities[i] );
      const HashTable<std::string, unsigned int>& modals = modal_names[i];

      for ( HashTableConstIterator<std::string, unsigned int>
            iter = modals.begin(); iter != modals.end(); ++iter ) {
        database.__modalities_names[i][*iter] = iter.key();
      }

      database.__missing_value[i]=modals.exists( "?" );
    }

    // create the __iterators begin/rbegin/end/rend
    database.__iter_begin.initializeIterator
    ( database, DatabaseIterator::GUM_DATABASE_ITER_BEGIN );

    database.__iter_rbegin.initializeIterator
    ( database, DatabaseIterator::GUM_DATABASE_ITER_RBEGIN );

    database.__iter_end.initializeIterator
    ( database, DatabaseIterator::GUM_DATABASE_ITER_END );

    database.__iter_rend.initializeIterator
    ( database, DatabaseIterator::GUM_DATABASE_ITER_REND );

    // return the newly constructed database
    return database;
  }

  // ==============================================================================
  /// creates a new database from a pure CSV file
  // ==============================================================================
  Database Database::createFromXmlCSV( const std::string& filename ) {
    // create a new empty database, to be filled later
    Database database;

    if ( ! GUM_PARSE_XML_CSV_parser( database, filename ) ) {
      GUM_ERROR( IOError, "error parsing file" + filename );
    }

    database.__filename=filename;

    // create the __iterators begin/rbegin/end/rend
    database.__iter_begin.initializeIterator
    ( database, DatabaseIterator::GUM_DATABASE_ITER_BEGIN );

    database.__iter_rbegin.initializeIterator
    ( database, DatabaseIterator::GUM_DATABASE_ITER_RBEGIN );

    database.__iter_end.initializeIterator
    ( database, DatabaseIterator::GUM_DATABASE_ITER_END );

    database.__iter_rend.initializeIterator
    ( database, DatabaseIterator::GUM_DATABASE_ITER_REND );

    // return the newly constructed database
    return database;
  }


} /* namespace gum */

