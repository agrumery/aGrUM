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
/** @file
 * @brief The inlined implementation of DBCells
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {

    
    // ===========================================================================
    // Handlers
    // ===========================================================================
    
    /// default constructor
    INLINE DatabaseVector::Handler::Handler ( const DatabaseVector& db ) :
    __row ( db.content () ),
    __end_index ( __row.size () ) {
      GUM_CONSTRUCTOR ( DatabaseVector::Handler );
    }
        

    /// copy constructor
    INLINE DatabaseVector::Handler::Handler ( const DatabaseVector::Handler& h ) :
      __row ( h.__row ),
      __index ( h.__index ),
      __begin_index ( h.__begin_index ),
      __end_index ( h.__end_index ) {
      GUM_CONS_CPY ( DatabaseVector::Handler );
    }

    
    /// destructor
    INLINE DatabaseVector::Handler::~Handler () {
      GUM_DESTRUCTOR ( DatabaseVector::Handler );
    }
      

    /// returns the number of rows managed by the handler
    INLINE unsigned long DatabaseVector::Handler::size () const noexcept {
      return __end_index - __begin_index;
    }


    /// return the number of rows of the whole database
    INLINE unsigned long DatabaseVector::Handler::DBSize () const noexcept {
      return __row.size ();
    }

    
    /// returns the current row pointed to by the handler
    INLINE const DBRow& DatabaseVector::Handler::row () const {
      if ( __index >= __end_index ) {
        GUM_ERROR ( OutOfBounds, "the handler has reached its end" );
      }
      return __row[ __index ];
    }


    /// makes the handler point to the next row
    INLINE void DatabaseVector::Handler::nextRow () noexcept {
      ++__index;
    }


    /// indicates whether the handler has reached its end or not
    INLINE bool DatabaseVector::Handler::hasRows () const noexcept {
      return ( __index < __end_index );
    }


    /// puts the handler to the beginning of the database area it handles
    INLINE void DatabaseVector::Handler::reset () noexcept {
      __index = __begin_index;
    }


    /// sets the area in the database the handler will handle
    INLINE void DatabaseVector::Handler::setRange ( unsigned long begin,
                                                    unsigned long end ) noexcept {
      if ( begin > end ) std::swap ( begin, end );
      __begin_index = begin;
      __end_index = end;
    }


    // ===========================================================================
    // Database Vectors
    // ===========================================================================

    /// default constructor
    INLINE DatabaseVector::DatabaseVector () {
      GUM_CONSTRUCTOR ( DatabaseVector );
    }


    /// copy constructor
    INLINE DatabaseVector::DatabaseVector ( const DatabaseVector& from ) :
      __data ( from.__data ) {
      GUM_CONS_CPY ( DatabaseVector );
    }


    /// move constructor
    INLINE DatabaseVector::DatabaseVector ( DatabaseVector&& from ) :
      __data ( std::move ( from.__data ) ) {
      GUM_CONS_MOV ( DatabaseVector );
    }


    /// destructor
    INLINE DatabaseVector::~DatabaseVector () {
      GUM_DESTRUCTOR ( DatabaseVector );
    }


    /// returns the content of the database
    INLINE const std::vector<DBRow>& DatabaseVector::content () const noexcept {
      return __data;
    }


    /// returns the content of the database
    INLINE std::vector<DBRow>& DatabaseVector::_content () noexcept {
      return __data;
    }

    
    /// returns a new handler on the database
    INLINE DatabaseVector::Handler DatabaseVector::handler () const {
      return Handler ( *this );
    }


  } /* namespace learning */

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
