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
    __db ( &db ),
    __row ( &( db.content () ) ),
    __end_index ( __row->size () ) {
      GUM_CONSTRUCTOR ( DatabaseVectInRAM::Handler );
    }
        

    /// copy constructor
    INLINE
    DatabaseVectInRAM::Handler::Handler ( const DatabaseVectInRAM::Handler& h ) :
      __db ( h.__db ),
      __row ( h.__row ),
      __index ( h.__index ),
      __begin_index ( h.__begin_index ),
      __end_index ( h.__end_index ) {
      GUM_CONS_CPY ( DatabaseVectInRAM::Handler );
    }

    
    /// move constructor
    INLINE
    DatabaseVectInRAM::Handler::Handler ( DatabaseVectInRAM::Handler&& h ) :
      __db ( h.__db ),
      __row ( h.__row ),
      __index ( h.__index ),
      __begin_index ( h.__begin_index ),
      __end_index ( h.__end_index ) {
      GUM_CONS_MOV ( DatabaseVectInRAM::Handler );
    }

    
    /// destructor
    INLINE DatabaseVectInRAM::Handler::~Handler () {
      GUM_DESTRUCTOR ( DatabaseVectInRAM::Handler );
    }


    /// copy operator
    INLINE
    DatabaseVectInRAM::Handler&
    DatabaseVectInRAM::Handler::operator= ( const DatabaseVectInRAM::Handler& h ) {
      __db = h.__db;
      __row = h.__row;
      __index = h.__index;
      __begin_index = h.__begin_index;
      __end_index = h.__end_index;
      return *this;
    }


    /// move operator
    INLINE
    DatabaseVectInRAM::Handler&
    DatabaseVectInRAM::Handler::operator= ( DatabaseVectInRAM::Handler&& h ) {
      __db = h.__db;
      __row = h.__row;
      __index = h.__index;
      __begin_index = h.__begin_index;
      __end_index = h.__end_index;
      return *this;
    }


    /// returns the number of rows managed by the handler
    INLINE unsigned long DatabaseVectInRAM::Handler::size () const noexcept {
      return __end_index - __begin_index;
    }


    /// return the number of rows of the whole database
    INLINE unsigned long DatabaseVectInRAM::Handler::DBSize () const noexcept {
      return __row->size ();
    }

    
    /// returns the current row pointed to by the handler
    INLINE const DBRow& DatabaseVectInRAM::Handler::row () const {
      if ( __index >= __end_index ) {
        GUM_ERROR ( OutOfBounds, "the handler has reached its end" );
      }
      return __row->operator[] ( __index );
    }


    /// returns the current row pointed to by the handler
    INLINE DBRow& DatabaseVectInRAM::Handler::row () {
      if ( __index >= __end_index ) {
        GUM_ERROR ( OutOfBounds, "the handler has reached its end" );
      }
      return const_cast<std::vector<DBRow>*> ( __row )->operator[]  ( __index );
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
      __index = begin;
    }

    
    /// returns the current range of the handler
    INLINE std::pair<unsigned long, unsigned long>
    DatabaseVectInRAM::Handler::range () const noexcept {
      return std::pair<unsigned long,unsigned long> ( __begin_index, __end_index );
    }

    
    /// returns the names of the variables
    INLINE const std::vector<std::string>&
    DatabaseVectInRAM::Handler::variableNames () const noexcept {
      return __db->variableNames ();
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
      __data ( from.__data ),
      __variable_names ( from.__variable_names ) {
      GUM_CONS_CPY ( DatabaseVectInRAM );
    }


    /// move constructor
    INLINE DatabaseVectInRAM::DatabaseVectInRAM ( DatabaseVectInRAM&& from ) :
      __data ( std::move ( from.__data ) ),
      __variable_names ( std::move ( from.__variable_names ) ) {
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


    /// returns the variable names for all the columns
    INLINE const std::vector<std::string>&
    DatabaseVectInRAM::variableNames () const noexcept {
      return __variable_names;
    }
    

    /// returns the variable names for all the columns
    INLINE std::vector<std::string>&
    DatabaseVectInRAM::_variableNames () noexcept {
      return __variable_names;
    }

    
    /// returns the number of variables (columns) of the database
    INLINE unsigned int DatabaseVectInRAM::nbVariables () const noexcept {
      if ( __data.empty () ) return 0;
      return __data[0].size ();
    }


    /// returns a new handler on the database
    INLINE DatabaseVectInRAM::Handler DatabaseVectInRAM::handler () const {
      return Handler ( *this );
    }


  } /* namespace learning */

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
