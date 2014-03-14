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
    INLINE DatabaseVectInRAM::Handler::Handler ( const DatabaseVectInRAM& db ) :
    __row ( db.content () ),
    __end_index ( __row.size () ) {
      GUM_CONSTRUCTOR ( DatabaseVectInRAM::Handler );
    }
        

    /// copy constructor
    INLINE
    DatabaseVectInRAM::Handler::Handler ( const DatabaseVectInRAM::Handler& h ) :
      __row ( h.__row ),
      __index ( h.__index ),
      __begin_index ( h.__begin_index ),
      __end_index ( h.__end_index ) {
      GUM_CONS_CPY ( DatabaseVectInRAM::Handler );
    }

    
    /// destructor
    INLINE DatabaseVectInRAM::Handler::~Handler () {
      GUM_DESTRUCTOR ( DatabaseVectInRAM::Handler );
    }
      

    /// returns the number of rows managed by the handler
    INLINE unsigned long DatabaseVectInRAM::Handler::size () const noexcept {
      return __end_index - __begin_index;
    }


    /// return the number of rows of the whole database
    INLINE unsigned long DatabaseVectInRAM::Handler::DBSize () const noexcept {
      return __row.size ();
    }

    
    /// returns the current row pointed to by the handler
    INLINE const DBRow& DatabaseVectInRAM::Handler::row () const {
      if ( __index >= __end_index ) {
        GUM_ERROR ( OutOfBounds, "the handler has reached its end" );
      }
      return __row[ __index ];
    }


    /// makes the handler point to the next row
    INLINE void DatabaseVectInRAM::Handler::nextRow () noexcept {
      ++__index;
    }


    /// indicates whether the handler has reached its end or not
    INLINE bool DatabaseVectInRAM::Handler::hasRows () const noexcept {
      return ( __index < __end_index );
    }


    /// puts the handler to the beginning of the database area it handles
    INLINE void DatabaseVectInRAM::Handler::reset () noexcept {
      __index = __begin_index;
    }


    /// sets the area in the database the handler will handle
    INLINE void DatabaseVectInRAM::Handler::setRange ( unsigned long begin,
                                                    unsigned long end ) noexcept {
      if ( begin > end ) std::swap ( begin, end );
      __begin_index = begin;
      __end_index = end;
    }


    // ===========================================================================
    // Database VectInRAMs
    // ===========================================================================

    /// default constructor
    INLINE DatabaseVectInRAM::DatabaseVectInRAM () {
      GUM_CONSTRUCTOR ( DatabaseVectInRAM );
    }


    /// copy constructor
    INLINE DatabaseVectInRAM::DatabaseVectInRAM ( const DatabaseVectInRAM& from ) :
      __data ( from.__data ) {
      GUM_CONS_CPY ( DatabaseVectInRAM );
    }


    /// move constructor
    INLINE DatabaseVectInRAM::DatabaseVectInRAM ( DatabaseVectInRAM&& from ) :
      __data ( std::move ( from.__data ) ) {
      GUM_CONS_MOV ( DatabaseVectInRAM );
    }


    /// destructor
    INLINE DatabaseVectInRAM::~DatabaseVectInRAM () {
      GUM_DESTRUCTOR ( DatabaseVectInRAM );
    }


    /// returns the content of the database
    INLINE const std::vector<DBRow>& DatabaseVectInRAM::content () const noexcept {
      return __data;
    }


    /// returns the content of the database
    INLINE std::vector<DBRow>& DatabaseVectInRAM::_content () noexcept {
      return __data;
    }

    
    /// returns a new handler on the database
    INLINE DatabaseVectInRAM::Handler DatabaseVectInRAM::handler () const {
      return Handler ( *this );
    }


  } /* namespace learning */

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
