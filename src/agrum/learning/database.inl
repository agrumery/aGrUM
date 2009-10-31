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

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  /// return a ref to filename
  INLINE const std::string& Database::filename() const {
    return __filename;
  }

  
  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                       GUM_DATABASE IMPLEMENTATION                   === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// get the ncaseth case in the database
  // ==============================================================================
  INLINE const unsigned int* Database::line( unsigned int ncase )
    const throw( NotFound ) {
    // check if the case exists
    if ( ncase >= __nb_cases )
      GUM_ERROR( NotFound, "ncase is too big" );

    return __cases + ncase * __nb_nodes;
  }

  // ==============================================================================
  /// get the number of modalities of all the nodes
  // ==============================================================================
  INLINE const std::vector<unsigned int>& Database::nbrModalities()
    const throw() {
    return __nb_modalities;
  }

  // ==============================================================================
  /// get the value of the ith node in the nth case of the database
  // ==============================================================================
  INLINE unsigned int Database::value( unsigned int n, unsigned int i )
    const throw( NotFound ) {
    // check if the value exists
    if (( n >= __nb_cases ) || ( i >= __nb_nodes ) )
      GUM_ERROR( NotFound, "ncase and/or nnode is too big" );

    return __cases[n * __nb_nodes + i];
  }

  // ==============================================================================
  /// returns the name of the kth node in the database
  // ==============================================================================
  INLINE const std::string& Database::nodeName( unsigned int k )
    const throw( NotFound ) {
    // check if the node exists
    if ( k >= __nb_nodes )
      GUM_ERROR( NotFound, "the kth node cannot be found" );

    return __node_names[k];
  }

  // ==============================================================================
  /// returns the names of all the nodes
  // ==============================================================================
  INLINE
  const std::vector<std::string>& Database::nodeNames() const {
    return __node_names;
  }


  /// is there any missing value in the database
  INLINE bool Database::hasMissingValue() const {
    for ( unsigned int k=0;k<__nb_nodes;++k ) {
      if ( hasMissingValue( k ) ) return true;
    }

    return false;
  }

  /// is there any missing value for this row
  INLINE bool Database::hasMissingValue( unsigned int k ) const {
    // check if the node exists
    if ( k >= __nb_nodes )
      GUM_ERROR( NotFound, "the kth node cannot be found" );

    return __missing_value[k];
  }


  // ==============================================================================
  /// returns the names of all the modalities of a given node
  // ==============================================================================
  INLINE const std::vector<std::string>&
  Database::modalitiesNames( unsigned int k ) const throw( NotFound ) {
    // check if the node exists
    if ( k >= __nb_nodes )
      GUM_ERROR( NotFound, "the kth node cannot be found" );

    return __modalities_names[k];
  }

  // ==============================================================================
  /// returns the kth modality name of the ith node
  // ==============================================================================
  INLINE const std::string&
  Database::modalityName( unsigned int k, unsigned int i )
    const throw( NotFound ) {
    // check if the node and modality do exist
    if (( i >= __nb_nodes ) || ( k>=__modalities_names[i].size() ) )
      GUM_ERROR( NotFound, "the node and/or modality cannot be found" );

    return __modalities_names[i][k];
  }

  // ==============================================================================
  /// get the number of modalities of the kth node
  // ==============================================================================
  INLINE unsigned int Database::nbrModalities( unsigned int k )
    const throw( NotFound ) {
    // check if the node exists
    if ( k >= __nb_nodes )
      GUM_ERROR( NotFound, "the kth node cannot be found" );

    return __nb_modalities[k];
  }

  // ==============================================================================
  /// get the number of __cases in the database
  // ==============================================================================
  INLINE unsigned int Database::nbrLines() const throw() {
    return __nb_cases;
  }

  // ==============================================================================
  /// get the number of nodes in the database
  // ==============================================================================
  INLINE unsigned int Database::nbrNodes() const throw() {
    return __nb_nodes;
  }

  // ==============================================================================
  /// returns an iterator pointing to the beginning of the database
  // ==============================================================================
  INLINE const DatabaseIterator& Database::begin() const throw() {
    return __iter_begin;
  }

  // ==============================================================================
  /// returns an iterator pointing just after the last element of the database
  // ==============================================================================
  INLINE const DatabaseIterator& Database::end() const throw() {
    return __iter_end;
  }

  // ==============================================================================
  /// returns an iterator pointing to the last element of the database
  // ==============================================================================
  INLINE const DatabaseIterator& Database::rbegin() const throw() {
    return __iter_rbegin;
  }

  // ==============================================================================
  /// returns an iterator pointing just before the first element of the database
  // ==============================================================================
  INLINE const DatabaseIterator& Database::rend() const throw() {
    return __iter_rend;
  }




  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                  GUM_DATABASE_ITERATOR IMPLEMENTATION               === */
  /* =========================================================================== */
  /* =========================================================================== */

  // ==============================================================================
  /// creates an iterator pointing toward nothing
  // ==============================================================================
  INLINE DatabaseIterator::DatabaseIterator() :
    __database( 0 ), __current_case( 0 ), __offset( 0 ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( DatabaseIterator );
  }

  // ==============================================================================
  /// basic constructor points toward the nb_caseth case of the database
  // ==============================================================================
  INLINE DatabaseIterator::DatabaseIterator
  ( const Database& data, unsigned int nb_case ) :
    __database( &data ), __offset( data.nbrNodes() ) {
    // for debugging purposes
    GUM_CONSTRUCTOR( DatabaseIterator );
    __current_case = data.line( nb_case );
    data.__iterators.pushFront( this );
  }

  // ==============================================================================
  /// copy constructor
  // ==============================================================================
  INLINE DatabaseIterator::DatabaseIterator( const DatabaseIterator& from ) :
    __database( from.__database ), __current_case( from.__current_case ),
    __offset( from.__offset ) {
    //for debugging purposes
    GUM_CONS_CPY( DatabaseIterator );
    if ( __database )
      __database->__iterators.pushFront( this );
  }

  // ==============================================================================
  /// destructor
  // ==============================================================================
  INLINE DatabaseIterator::~DatabaseIterator() {
    // for debugging purposes
    GUM_DESTRUCTOR( DatabaseIterator );

    // detach the iterator from its database __iterators list
    if ( __database )
      __database->__iterators.eraseByVal( this );
  }

  // ==============================================================================
  /// initializers used for begin/rbegin/end/rend
  // ==============================================================================
  INLINE void DatabaseIterator::__initializeIterator
  ( const Database& data, enum DatabaseIterator::IteratorLocation location ) {
    __database = &data;
    __offset = __database->nbrNodes();

    switch ( location ) {
    case GUM_DATABASE_ITER_BEGIN:
      __current_case = __database->__cases;
      break;
    case GUM_DATABASE_ITER_RBEGIN:
      __current_case =
        __database->__cases + __offset * ( __database->__nb_cases - 1 );
      break;
    case GUM_DATABASE_ITER_END:
      __current_case = __database->__cases + __offset * __database->__nb_cases;
      break;
    case GUM_DATABASE_ITER_REND:
      __current_case = __database->__cases - __offset;
      break;
    }
  }

  // ==============================================================================
  /// detach the iterator from the database it points to
  // ==============================================================================
  INLINE void DatabaseIterator::clear() {
    if ( __database )
      __database->__iterators.eraseByVal( this );

    __database = 0;
    __current_case = 0;
    __offset = 0;
  }

  // ==============================================================================
  /// copy operator
  // ==============================================================================
  INLINE DatabaseIterator& DatabaseIterator::operator=
  ( const DatabaseIterator& from ) {
    // avoid self assignment
    if ( this != &from ) {
      // for debugging purposes
      GUM_OP_CPY( DatabaseIterator );
      // detach the iterator from its current database

      if ( __database && ( from.__database != __database ) )
        __database->__iterators.eraseByVal( this );

      // if needed, attach the iterator to its new database
      if ( from.__database && ( from.__database != __database ) )
        from.__database->__iterators.pushFront( this );

      __database = from.__database;
      __current_case = from.__current_case;
      __offset = from.__offset;
    }

    return *this;
  }

  // ==============================================================================
  /// check whether two __iterators point toward the same case
  // ==============================================================================
  INLINE bool DatabaseIterator::operator== ( const DatabaseIterator& from )
    const throw() {
    return ( __current_case == from.__current_case );
  }

  // ==============================================================================
  /// check whether two __iterators point toward different __cases
  // ==============================================================================
  INLINE bool DatabaseIterator::operator!= ( const DatabaseIterator& from )
    const throw() {
    return ( __current_case != from.__current_case );
  }

  // ==============================================================================
  /// makes the iterator point toward the next case (safe, \sa operator++)
  // ==============================================================================
  INLINE DatabaseIterator& DatabaseIterator::nextCase() {
    if ( ! __database ||
         ( __current_case + __offset >= __database->end().__current_case ) )
      GUM_ERROR( OutOfBounds, "the next case does not exist" );

    __current_case += __offset;

    return *this;
  }

  // ==============================================================================
  /// makes the iterator point toward the next case (fast but unsafe)
  // ==============================================================================
  INLINE DatabaseIterator& DatabaseIterator::operator++ () {
    __current_case += __offset;
    return *this;
  }

  // ==============================================================================
  /// makes the iterator point toward the preceding case (safe, \sa operator--)
  // ==============================================================================
  INLINE DatabaseIterator& DatabaseIterator::prevCase() {
    if ( ! __database ||
         ( __current_case < __database->begin().__current_case + __offset ) )
      GUM_ERROR( OutOfBounds, "the previous case does not exist" );

    __current_case -= __offset;

    return *this;
  }

  // ==============================================================================
  /// makes the iterator point toward the previous case (fast but unsafe)
  // ==============================================================================
  INLINE DatabaseIterator& DatabaseIterator::operator-- () {
    __current_case -= __offset;
    return *this;
  }

  // ==============================================================================
  /// returns the value of the ith node in the current case (safe, \sa operator[])
  // ==============================================================================
  INLINE unsigned int DatabaseIterator::at( unsigned int i ) const {
    if ( ! __database || ( i >= __database->__nb_nodes ) )
      GUM_ERROR( NotFound, "there exists no ith node" );

    return __current_case[i];
  }

  // ==============================================================================
  /// returns the value of the ith node in the current case (fast but unsafe)
  // ==============================================================================
  INLINE unsigned int DatabaseIterator::operator[]( unsigned int i ) const {
    return __current_case[i];
  }

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
