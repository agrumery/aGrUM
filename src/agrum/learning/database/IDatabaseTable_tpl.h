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
 * @brief The implementation of the common class for tabular databases
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/database/IDatabaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    // ===========================================================================
    // Unsafe handlers
    // ===========================================================================

    // default constructor
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE IDatabaseTable<T_DATA,ALLOC>::Handler::Handler
    ( const IDatabaseTable<T_DATA,ALLOC>& db )
      : DBHandler<T_DATA,ALLOC> ()
      , __db( &db )
      , __row( &( db.content() ) )
      , __end_index( std::size_t ( __row->size() ) ) {
      GUM_CONSTRUCTOR( IDatabaseTable::Handler );
    }

    
    // copy constructor
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    IDatabaseTable<T_DATA,ALLOC>::Handler::Handler
    ( const typename IDatabaseTable<T_DATA,ALLOC>::Handler& h )
      : DBHandler<T_DATA,ALLOC> ()
      , __db( h.__db )
      , __row( h.__row )
      , __index( h.__index )
      , __begin_index( h.__begin_index )
      , __end_index( h.__end_index ) {
      GUM_CONS_CPY( IDatabaseTable::Handler );
    }
    

    // move constructor
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    IDatabaseTable<T_DATA,ALLOC>::Handler::Handler
    ( typename IDatabaseTable<T_DATA,ALLOC>::Handler&& h )
        : DBHandler<T_DATA,ALLOC> ()
      , __db( h.__db )
      , __row( h.__row )
      , __index( h.__index )
      , __begin_index( h.__begin_index )
      , __end_index( h.__end_index ) {
      GUM_CONS_MOV( IDatabaseTable::Handler );
    }


    // destructor
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE IDatabaseTable<T_DATA,ALLOC>::Handler::~Handler() {
      GUM_DESTRUCTOR( IDatabaseTable::Handler );
    }

    
    // copy operator
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    typename IDatabaseTable<T_DATA,ALLOC>::Handler&
    IDatabaseTable<T_DATA,ALLOC>::Handler::operator=
    ( const typename IDatabaseTable<T_DATA,ALLOC>::Handler& h ) {
      __db = h.__db;
      __row = h.__row;
      __index = h.__index;
      __begin_index = h.__begin_index;
      __end_index = h.__end_index;
      return *this;
    }

    
    // move operator
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    typename IDatabaseTable<T_DATA,ALLOC>::Handler&
    IDatabaseTable<T_DATA,ALLOC>::Handler::operator=
    ( typename IDatabaseTable<T_DATA,ALLOC>::Handler&& h ) {
      __db = h.__db;
      __row = h.__row;
      __index = h.__index;
      __begin_index = h.__begin_index;
      __end_index = h.__end_index;
      return *this;
    }
    
    
    // returns the current row pointed to by the handler
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler::const_reference
    IDatabaseTable<T_DATA,ALLOC>::Handler::operator* () const {
      return __row->operator[]( __index );
    }

    
    // Dereferences the value pointed to by the handler (unsafe version)
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler::const_pointer
    IDatabaseTable<T_DATA,ALLOC>::Handler::operator-> () const {
      return &( __row->operator[]( __index ) );
    }

    
    // makes the handler point to the next row
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler& 
    IDatabaseTable<T_DATA,ALLOC>::Handler::operator++() {
      ++__index;
      return *this;
    }
    

    // makes the handler point to the previous row
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler& 
    IDatabaseTable<T_DATA,ALLOC>::Handler::operator--() {
      if ( __index > __begin_index ) --__index;
      return *this;
    }


    //  moves the handler by i rows
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler& 
    IDatabaseTable<T_DATA,ALLOC>::Handler::operator+= ( unsigned int i ) {
      __index += i;
      return *this;
    }

    
    //  moves back the handler by i rows
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler& 
    IDatabaseTable<T_DATA,ALLOC>::Handler::operator-= ( unsigned int i ) {
      if ( __index >= __begin_index + i ) __index -= i;
      else __index = __begin_index;
      return *this;
    }
    

    // checks whether two handlers point on the same row
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE bool IDatabaseTable<T_DATA,ALLOC>::Handler::operator==
    ( const Handler& handler ) const {
      return __index == handler.__index;
    }
    

    // checks whether two handlers point to different rows
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE bool IDatabaseTable<T_DATA,ALLOC>::Handler::operator!=
    ( const Handler& handler ) const {
      return __index != handler.__index;
    }
    
    
    // returns the number of rows managed by the handler
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE std::size_t
    IDatabaseTable<T_DATA,ALLOC>::Handler::size() const {
      return __end_index - __begin_index;
    }

    
    // return the number of rows of the whole database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE std::size_t
    IDatabaseTable<T_DATA,ALLOC>::Handler::DBSize() const {
      if ( __row != nullptr ) return  __row->size();
      else return std::size_t ( 0 );
    }
    

    // returns the current row pointed to by the handler
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler::const_reference
    IDatabaseTable<T_DATA,ALLOC>::Handler::rowSafe() const {
      if ( __index >= __end_index ) {
        GUM_ERROR( OutOfBounds, "the handler has reached its end" );
      }

      return __row->operator[]( __index );
    }


    // returns the current row pointed to by the handler
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler::reference
    IDatabaseTable<T_DATA,ALLOC>::Handler::rowSafe() {
      if ( __index >= __end_index ) {
        GUM_ERROR( OutOfBounds, "the handler has reached its end" );
      }

      return const_cast<Matrix<T_DATA>*> ( __row )->operator[]( __index );
    }
    

    // returns the current row pointed to by the handler (unsafe version)
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler::const_reference
    IDatabaseTable<T_DATA,ALLOC>::Handler::row() const {
      return __row->operator[]( __index );
    }

    
    // returns the current row pointed to by the handler (unsafe version)
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler::reference
    IDatabaseTable<T_DATA,ALLOC>::Handler::row() {
      return const_cast<Matrix<T_DATA>*> ( __row )->operator[]( __index );
    }
    

    // makes the handler point to the next row
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void
    IDatabaseTable<T_DATA,ALLOC>::Handler::nextRow() { ++__index; }


    // returns the number of the current row
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE std::size_t
    IDatabaseTable<T_DATA,ALLOC>::Handler::numRow() const {
      return ( __index >= __begin_index ) ? __index - __begin_index : 0;
    }

    // indicates whether the handler has reached its end or not
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE bool IDatabaseTable<T_DATA,ALLOC>::Handler::hasRows() const {
      return ( __index < __end_index );
    }

    // puts the handler to the beginning of the database area it handles
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::Handler::reset() {
      __index = __begin_index;
    }

    
    // returns a new handler that points to the beginning of the 
    // database area of the current handler */
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler
    IDatabaseTable<T_DATA,ALLOC>::Handler::begin () const {
      Handler handler (*this);
      handler.reset ();
      return handler;
    }


    // returns a new handler that points to the end of the 
    // database area of the current handler */
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler
    IDatabaseTable<T_DATA,ALLOC>::Handler::end () const {
     Handler handler (*this);
     handler.__index = __end_index;
      return handler;
    }


    // sets the area in the database the handler will handle
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void
    IDatabaseTable<T_DATA,ALLOC>::Handler::setRange( std::size_t begin,
                                                     std::size_t end ) {
      if ( begin > end ) std::swap( begin, end );

      // check that the end belongs to the database, else raise an exception
      if ( __row == nullptr ) {
        GUM_ERROR( NullElement,
                   "the handler does not point to any database" );
      }
      if ( end > __row->size() ) {
        GUM_ERROR( SizeError,
                   "the database has fewer rows than the upper range "
                   "specified to the handler" );
      }

      __begin_index = begin;
      __end_index = end;
      __index = begin;
    }
    

    // returns the current range of the handler
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE std::pair<std::size_t,std::size_t>
    IDatabaseTable<T_DATA,ALLOC>::Handler::range() const {
      return std::pair<std::size_t,std::size_t>( __begin_index, __end_index );
    }

    
    // returns the names of the variables
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    const typename
    IDatabaseTable<T_DATA,ALLOC>::Handler::template DBVector<std::string>&
    IDatabaseTable<T_DATA,ALLOC>::Handler::variableNames() const {
      return __db->variableNames();
    }

    // returns the number of variables (columns) of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE std::size_t
    IDatabaseTable<T_DATA,ALLOC>::Handler::nbVariables() const {
      if ( __db != nullptr ) return __db->variableNames().size();
      else return 0;
    }

    


    // ===========================================================================
    // Safe handlers
    // ===========================================================================

    // attach a new handler to the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::__attachHandler() {
      if ( this->__db != nullptr ) {
        this->__db->__attachHandler ( this );
      }
    }


    // detach a handler
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::__detachHandler() {
      if ( this->__db != nullptr ) {
        this->__db->__detachHandler ( this );
      }
    }
    

    // default constructor
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::HandlerSafe
    ( const IDatabaseTable<T_DATA,ALLOC>& db )
      : IDatabaseTable<T_DATA,ALLOC>::Handler ( db ) {
      __attachHandler();
      GUM_CONSTRUCTOR( IDatabaseTable::HandlerSafe );
    }

    
    // copy constructor
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::HandlerSafe
    ( const typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe& h )
      : IDatabaseTable<T_DATA,ALLOC>::Handler ( h ) {
      __attachHandler();
      GUM_CONS_CPY( IDatabaseTable::HandlerSafe );
    }
    

    // move constructor
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::HandlerSafe
    ( typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe&& h )
      : IDatabaseTable<T_DATA,ALLOC>::Handler ( std::move ( h ) ) {
      __attachHandler();
      GUM_CONS_MOV( IDatabaseTable::HandlerSafe );
    }


    // destructor
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::~HandlerSafe() {
      __detachHandler();
      GUM_DESTRUCTOR( IDatabaseTable::HandlerSafe );
    }

    
    // copy operator
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe&
    IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::operator=
    ( const typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe& h ) {
      if ( this->__db != h.__db ) {
        __detachHandler();
        this->__db = h.__db;
        __attachHandler();
      }

      IDatabaseTable<T_DATA,ALLOC>::Handler::operator= ( h );
      return *this;
    }


    // copy operator
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe&
    IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::operator=
    ( const typename IDatabaseTable<T_DATA,ALLOC>::Handler& h ) {
      return this->operator=
        ( dynamic_cast<const IDatabaseTable<T_DATA,ALLOC>::HandlerSafe&> ( h ) );
    }

    
    // move operator
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe&
    IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::operator=
    ( typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe&& h ) {
      if ( this->__db != h.__db ) {
        __detachHandler();
        this->__db = h.__db;
        __attachHandler();
      }

      IDatabaseTable<T_DATA,ALLOC>::Handler::operator= ( std::move ( h ) );
      return *this;
    }
    
    
    // move operator
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE
    typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe&
    IDatabaseTable<T_DATA,ALLOC>::HandlerSafe::operator=
    ( typename IDatabaseTable<T_DATA,ALLOC>::Handler&& h ) {
      return this->operator=
        ( std::move(dynamic_cast<IDatabaseTable<T_DATA,ALLOC>::HandlerSafe&>(h)));
    }
    
    

    // ===========================================================================
    // Database Tables
    // ===========================================================================

    // returns the allocator of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE ALLOC<T_DATA>
    IDatabaseTable<T_DATA,ALLOC>::getAllocator () const {
      return ALLOC<T_DATA> ( *this );
    }


    // create the end iterators
    template <typename T_DATA, template<typename> class ALLOC>
    void IDatabaseTable<T_DATA,ALLOC>::__createEndIterators () {
      const IDatabaseTable<T_DATA,ALLOC>& db = *this;
      ALLOC<iterator> allocator1 ( *this );
      __end = allocator1.allocate ( 1 );
      try {
        allocator1.construct ( __end, db );
      }
      catch ( ... ) {
        allocator1.deallocate ( __end, 1 );
        throw;
      }
      
      ALLOC<iterator_safe> allocator2 ( *this );
      try {
        __end_safe = allocator2.allocate ( 1 );
        try {
           allocator2.construct ( __end_safe, *this );
        }
        catch ( ... ) {
          allocator1.destroy ( __end );
          allocator1.deallocate ( __end, 1 );
          throw;
        }
      }
      catch ( ... ) {
        allocator2.deallocate ( __end_safe, 1 );
        throw;
      }
    }
    
      
    // default constructor
    template <typename T_DATA, template<typename> class ALLOC>
    template <template<typename> class VARALLOC,
              template<typename> class MISSALLOC>
    IDatabaseTable<T_DATA,ALLOC>::IDatabaseTable(
      const typename IDatabaseTable<T_DATA,ALLOC>::template
      MissingValType<MISSALLOC>& missing_symbols,
      const std::vector<std::string,VARALLOC<std::string>>& var_names,
      const ALLOC<T_DATA>& alloc )
      : ALLOC<T_DATA> ( alloc )
      , _variable_names ( alloc )
      , __data ( alloc )
      , __missing_symbols ( alloc )
      , __list_of_safe_handlers ( alloc ) {
      // copy the names
      _variable_names.reserve ( var_names.size () );
      for ( const auto& name : var_names )
        _variable_names.push_back ( name );
      
      // copy the missing symbols
      __missing_symbols.reserve ( missing_symbols.size () );
      for ( const auto& missing_symbol : missing_symbols )
        __missing_symbols.push_back ( missing_symbol );
      
      // create the end iterators
      __createEndIterators ();

      GUM_CONSTRUCTOR( IDatabaseTable );
    }


    // copy constructor with a given allocator
    template <typename T_DATA, template<typename> class ALLOC>
    IDatabaseTable<T_DATA,ALLOC>::IDatabaseTable
    ( const IDatabaseTable<T_DATA,ALLOC>& from,
      const typename IDatabaseTable<T_DATA,ALLOC>::allocator_type& alloc )
      : ALLOC<T_DATA> ( alloc )
      , _variable_names( from._variable_names, alloc )
      , __data( from.__data, alloc )
      , __missing_symbols ( from.__missing_symbols, alloc )
      , __list_of_safe_handlers ( alloc ) {
      // create the end iterators
      __createEndIterators ();
      
      GUM_CONS_CPY( IDatabaseTable );
    }


    // copy constructor
    template <typename T_DATA, template<typename> class ALLOC>
    IDatabaseTable<T_DATA,ALLOC>::IDatabaseTable
    ( const IDatabaseTable<T_DATA,ALLOC>& from )
      : IDatabaseTable<T_DATA,ALLOC> ( from, from.getAllocator () ) {}


    // move constructor with a given allocator
    template <typename T_DATA, template<typename> class ALLOC>
    IDatabaseTable<T_DATA,ALLOC>::IDatabaseTable(
      IDatabaseTable<T_DATA,ALLOC>&& from,
      const typename IDatabaseTable<T_DATA,ALLOC>::allocator_type& alloc )
      : ALLOC<T_DATA> ( alloc )
      , _variable_names( std::move( from._variable_names ), alloc )
      , __data( std::move( from.__data ), alloc )
      , __missing_symbols ( std::move ( from.__missing_symbols ), alloc )
      , __list_of_safe_handlers ( alloc ) {
      // create the end iterators
      __createEndIterators ();

      GUM_CONS_MOV( IDatabaseTable );
    }


    // move constructor
    template <typename T_DATA, template<typename> class ALLOC>
    IDatabaseTable<T_DATA,ALLOC>::IDatabaseTable(
      IDatabaseTable<T_DATA,ALLOC>&& from )
      : IDatabaseTable<T_DATA,ALLOC> ( std::move (from), from.getAllocator () ) {}

    
    /// virtual copy constructor with a given allocator
    template <typename T_DATA, template<typename> class ALLOC>
    IDatabaseTable<T_DATA,ALLOC>*
    IDatabaseTable<T_DATA,ALLOC>::clone ( const allocator_type& alloc ) const {
      ALLOC<IDatabaseTable<T_DATA,ALLOC>> allocator ( alloc );
      IDatabaseTable<T_DATA,ALLOC>* set = allocator.allocate(1);
      try {
        allocator.construct ( set, *this, alloc );
      }
      catch ( ... ) {
        allocator.deallocate ( set, 1 );
        throw;
      }
      return set;
    }

    
    /// virtual copy constructor
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE IDatabaseTable<T_DATA,ALLOC>*
    IDatabaseTable<T_DATA,ALLOC>::clone () const {
      return clone ( this->getAllocator () );
    }

    
    // destructor
    template <typename T_DATA, template<typename> class ALLOC>
    IDatabaseTable<T_DATA,ALLOC>::~IDatabaseTable() {
      // indicate to all the handlers that we are destructing the database
      __safe_handlers_mutex.lock ();
      for ( auto handler : __list_of_safe_handlers ) {
        handler->__db = nullptr;
        handler->__row = nullptr;
        handler->__end_index = 0;
        handler->__index = 0;
      }
      __safe_handlers_mutex.unlock ();

      ALLOC<iterator> allocator1 ( this->getAllocator () );
      allocator1.destroy ( __end );
      allocator1.deallocate ( __end, 1 );

      ALLOC<iterator_safe> allocator2 ( this->getAllocator () );
      allocator2.destroy ( __end_safe );
      allocator2.deallocate ( __end_safe, 1 );

      GUM_DESTRUCTOR( IDatabaseTable );
    }
    

    // copy operator
    template <typename T_DATA, template<typename> class ALLOC>
    IDatabaseTable<T_DATA,ALLOC>&
    IDatabaseTable<T_DATA,ALLOC>::operator=
    ( const IDatabaseTable<T_DATA,ALLOC>& from ) {
      if ( this != &from ) {
        // invalidate the current handlers
        __safe_handlers_mutex.lock ();
        for ( auto handler : __list_of_safe_handlers ) {
          handler->__db = nullptr;
          handler->__row = nullptr;
          handler->__end_index = 0;
          handler->__index = 0;
        }
        __list_of_safe_handlers.clear ();
        __safe_handlers_mutex.unlock ();

        __data = from.__data;
        _variable_names = from._variable_names;
        __missing_symbols = from.__missing_symbols;
          
        // update the end iterators
        const std::size_t db_size = __data.size();
        __end->__index = db_size;
        __end->__end_index = db_size;
        __end_safe->__index = db_size;
        __end_safe->__end_index = db_size;
        

      }

      return *this;
    }

    
    // move constructor
    template <typename T_DATA, template<typename> class ALLOC>
    IDatabaseTable<T_DATA,ALLOC>&
    IDatabaseTable<T_DATA,ALLOC>::operator=(
      IDatabaseTable<T_DATA,ALLOC>&& from ) {
      if ( this != &from ) {
        // invalidate the current handlers
        __safe_handlers_mutex.lock ();
        for ( auto handler : __list_of_safe_handlers ) {
          handler->__db = nullptr;
          handler->__row = nullptr;
          handler->__end_index = 0;
          handler->__index = 0;
        }
        __safe_handlers_mutex.unlock ();

        __data = std::move( from.__data );
        _variable_names = std::move( from._variable_names );
        __missing_symbols = std::move ( from.__missing_symbols );
        
        // update the end iterators
        const std::size_t db_size = __data.size();
        __end->__index = db_size;
        __end->__end_index = db_size;
        __end_safe->__index = db_size;
        __end_safe->__end_index = db_size;
      }

      return *this;
    }

    
    // returns a new unsafe handler pointing to the beginning of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler
    IDatabaseTable<T_DATA,ALLOC>::begin () const {
      return Handler ( *this );
    }


    // returns a new safe handler pointing to the beginning of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe
    IDatabaseTable<T_DATA,ALLOC>::beginSafe () const {
      return HandlerSafe ( *this );
    }

    
    // returns a new unsafe handler pointing to the end of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE const typename IDatabaseTable<T_DATA,ALLOC>::Handler&
    IDatabaseTable<T_DATA,ALLOC>::end () const noexcept {
      return *__end;
    }


    /// returns a new safe handler pointing to the beginning of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE const typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe&
    IDatabaseTable<T_DATA,ALLOC>::endSafe () const noexcept {
      return *__end_safe;
    }
    
    
    // returns a new unsafe handler on the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::Handler
    IDatabaseTable<T_DATA,ALLOC>::handler() const {
      return Handler( *this );
    }

    
    // returns a new safe handler on the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::HandlerSafe
    IDatabaseTable<T_DATA,ALLOC>::handlerSafe() const {
      return HandlerSafe( *this );
    }
    

    // returns the content of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE const typename IDatabaseTable<T_DATA,ALLOC>::template Matrix<T_DATA>&
    IDatabaseTable<T_DATA,ALLOC>::content() const noexcept {
      return __data;
    }

    
    // returns the content of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE typename IDatabaseTable<T_DATA,ALLOC>::template Matrix<T_DATA>&
    IDatabaseTable<T_DATA,ALLOC>::_content() noexcept {
      return __data;
    }


    // returns the variable names for all the columns
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE const std::vector<std::string,ALLOC<std::string>>&
    IDatabaseTable<T_DATA,ALLOC>::variableNames() const noexcept {
      return _variable_names;
    }

    
    // sets the names of the variables
    template <typename T_DATA, template<typename> class ALLOC>
    template <template<typename> class OTHER_ALLOC>
    void
    IDatabaseTable<T_DATA,ALLOC>::setVariableNames
    ( const std::vector<std::string,OTHER_ALLOC<std::string>>& names,
      const bool from_external_object ) {
      // copy the variable names into a vector allocated with the allocator
      // used by the database
      const std::size_t size = names.size ();
      DBVector<std::string> variable_names ( size );
      for ( std::size_t i = 0; i < size; ++i )
        variable_names[i] = names[i];

      this->setVariableNames ( variable_names, from_external_object );
    }


    /// returns the name of the kth column of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE const std::string&
    IDatabaseTable<T_DATA,ALLOC>::variableName ( const std::size_t k ) const {
      if ( _variable_names.size () <= k ) 
        GUM_ERROR ( OutOfBounds, "the database does not contain this column" );
      return _variable_names[k];
    }
    
      
    /// returns the index of the column whose name is passed in argument
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE std::size_t
    IDatabaseTable<T_DATA,ALLOC>::columnFromVariableName (
      const std::string& name ) const {
      const std::size_t size = _variable_names.size ();
      for ( std::size_t i = 0; i < size; ++i )
        if ( _variable_names[i] == name ) return i;

      GUM_ERROR ( UndefinedElement,
                  "the database contains no column with this name" );
    }
    

    // returns the number of variables (columns) of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE std::size_t IDatabaseTable<T_DATA,ALLOC>::nbVariables() const noexcept {
      return _variable_names.size();
    }

    
    // returns the number of records in the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE std::size_t IDatabaseTable<T_DATA,ALLOC>::size() const noexcept {
      return __data.size ();
    }

      
    // update the handlers when the size of the database changes
    template <typename T_DATA, template<typename> class ALLOC>
    void
    IDatabaseTable<T_DATA,ALLOC>::__updateHandlers( std::size_t new_size ) const {
      const std::size_t db_size = __data.size();

      __safe_handlers_mutex.lock ();
      for ( auto handler : __list_of_safe_handlers ) {
        if ( ( handler->__end_index == db_size ) ||
             ( handler->__end_index > new_size ) ) {
          handler->__end_index = new_size;
          // there is no need to update the index because, in safe handlers,
          // we always check that the index is less than end_index when trying
          // to access the rows
        }
      }
      __safe_handlers_mutex.unlock ();

      // update the end iterators
      __end->__index = new_size;
      __end->__end_index = new_size;
      __end_safe->__index = new_size;
      __end_safe->__end_index = new_size;
    }

    
    // attach a new handler to the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void
    IDatabaseTable<T_DATA,ALLOC>::__attachHandler( HandlerSafe* handler ) const {
      __safe_handlers_mutex.lock ();
      __list_of_safe_handlers.push_back( handler );
      __safe_handlers_mutex.unlock ();
    }


    // detach a handler
    template <typename T_DATA, template<typename> class ALLOC>
    void
    IDatabaseTable<T_DATA,ALLOC>::__detachHandler ( HandlerSafe* handler ) const {
      __safe_handlers_mutex.lock ();

      for ( auto iter = __list_of_safe_handlers.rbegin();
            iter != __list_of_safe_handlers.rend(); ++iter ) {
        if ( *iter == handler ) {
          *iter = __list_of_safe_handlers.back();
          __list_of_safe_handlers.pop_back();
          break;
        }
      }

      __safe_handlers_mutex.unlock ();
    }

    
    // checks whether a new row has the same size as the rest of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE bool
    IDatabaseTable<T_DATA,ALLOC>::_isRowSizeOK ( const std::size_t size ) const {
      return ( size == _variable_names.size() );
    }
    
    
    // insert a new row at the end of the database
    template <typename T_DATA, template<typename> class ALLOC>
    template <template<typename> class OTHER_ALLOC>
    void IDatabaseTable<T_DATA,ALLOC>::insertRow (
          const std::vector<std::string,OTHER_ALLOC<std::string>>& new_row ) {
      const std::size_t size = new_row.size ();
      std::vector<std::string,ALLOC<std::string>> good_typed_row ( size );
      for ( std::size_t i = 0; i < size; ++i )
        good_typed_row[i] = new_row[i];
      this->insertRow ( good_typed_row );
    }
    
      
    // insert a new DBRow at the end of the database
    template <typename T_DATA, template<typename> class ALLOC>
    void IDatabaseTable<T_DATA,ALLOC>::insertRow (
          typename IDatabaseTable<T_DATA,ALLOC>::template Row<T_DATA>&& new_row ) {
      // check that the size of the row is the same as the rest of the database
      if ( ! _isRowSizeOK ( new_row.size () ) )
        GUM_ERROR( SizeError,
                   "the new row has not the same size as the "
                   "rest of the database" );

      __updateHandlers( __data.size() + 1 );
      __data.push_back( std::move( new_row ) );
    }


    // insert a new DBRow at the end of the database
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::insertRow
    ( const typename IDatabaseTable<T_DATA,ALLOC>::template Row<T_DATA>& row ) {
      this->insertRow ( typename IDatabaseTable<T_DATA,ALLOC>::template
                        Row<T_DATA> ( row ) );
    }


    // insert a set of new DBRow at the end of the database
    template <typename T_DATA, template<typename> class ALLOC>
    void IDatabaseTable<T_DATA,ALLOC>::insertRows
    ( typename IDatabaseTable<T_DATA,ALLOC>::template Matrix<T_DATA>&& new_rows ) {
      if ( new_rows.empty() ) return;

      // check that all the rows have the same size
      const std::size_t new_size = new_rows[0].size();

      for ( const auto& row : new_rows ) {
        if ( row.size() != new_size ) {
          GUM_ERROR( SizeError,
                     "all the new rows do not have the same "
                     "nunber of columns" );
        }
      }

      // check that the sizes of the new rows are the same as the rest of
      // the database
      std::size_t db_size = __data.size();
      if ( ! _isRowSizeOK ( new_size ) ) {
        GUM_ERROR( SizeError,
                   "the new rows have not the same size as the "
                   "number of columns in the database" );
      }

      try {
        for ( auto row : new_rows ) {
          __data.push_back( std::move( row ) );
          ++db_size;
        }
      }
      catch ( ... ) {
        __updateHandlers( db_size );
        throw;
      }

      __updateHandlers( db_size );
    }


    // insert a set of new DBRow at the end of the database
    template <typename T_DATA, template<typename> class ALLOC>
    void IDatabaseTable<T_DATA,ALLOC>::insertRows
    ( const typename IDatabaseTable<T_DATA,ALLOC>::template
      Matrix<T_DATA>& new_rows ) {
      if ( new_rows.empty() ) return;

      // check that all the rows have the same size
      const std::size_t new_size = new_rows[0].size();

      for ( const auto& row : new_rows ) {
        if ( row.size() != new_size ) {
          GUM_ERROR( SizeError,
                     "all the new rows do not have the same "
                     "nunber of columns" );
        }
      }

      // check that the sizes of the new rows are the same as the rest of
      // the database
      std::size_t db_size = __data.size();

      if ( ! _isRowSizeOK ( new_size ) ) {
        GUM_ERROR( SizeError,
                   "the new rows have not the same size as the "
                   "number of columns in the database" );
      }

      try {
        for ( const auto& row : new_rows ) {
          __data.push_back( row );
          ++db_size;
        }
      }
      catch ( ... ) {
        __updateHandlers( db_size );
        throw;
      }

      __updateHandlers( db_size );
    }
    

    // erase a given row
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::eraseRow( std::size_t index ) {
      const std::size_t db_size = __data.size();

      if ( index < db_size ) {
        __updateHandlers( db_size - 1 );
        __data.erase( __data.begin() + index );
      }
    }
    

    // erase the last row
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::eraseLastRow() {
      const std::size_t db_size = __data.size();

      if ( db_size ) {
        __updateHandlers( db_size - 1 );
        __data.pop_back();
      }
    }
  

    // erase the first row
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::eraseFirstRow() {
      const std::size_t db_size = __data.size();

      if ( db_size ) {
        __updateHandlers( db_size - 1 );
        __data.erase( __data.begin() );
      }
    }

    
    // erase all the rows
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::eraseAllRows() {
      __updateHandlers( 0 );
      __data.clear();
    }

  
    // erase the k first rows
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void
    IDatabaseTable<T_DATA,ALLOC>::eraseFirstRows( const std::size_t nb_rows ) {
      const std::size_t db_size = __data.size();

      if ( nb_rows >= db_size ) {
        eraseAllRows();
      } else {
        __updateHandlers( db_size - nb_rows );
        __data.erase( __data.begin(), __data.begin() + nb_rows );
      }
    }
    

    // erase the k last rows
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void
    IDatabaseTable<T_DATA,ALLOC>::eraseLastRows( const std::size_t nb_rows ) {
      const std::size_t db_size = __data.size();

      if ( nb_rows >= db_size ) {
        eraseAllRows();
      } else {
        __updateHandlers( db_size - nb_rows );
        __data.erase( __data.begin() + ( db_size - nb_rows ),
                      __data.begin() + db_size );
      }
    }

    
    // erase the rows from the debth to the endth (not included)
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::eraseRows( std::size_t deb,
                                                         std::size_t end ) {
      if ( deb > end ) std::swap( deb, end );

      const std::size_t db_size = __data.size();

      if ( end >= db_size ) {
        if ( deb >= db_size ) {
          return;
        } else {
          eraseLastRows( db_size - deb );
        }
      } else {
        __updateHandlers( db_size - ( end - deb ) );
        __data.erase( __data.begin() + deb, __data.begin() + end );
      }
    }

    
    // erase the content of the database, including the names of the variables
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE void IDatabaseTable<T_DATA,ALLOC>::clear() {
      __updateHandlers( 0 );
      __data.clear();
      _variable_names.clear();
    }

  
    // returns the set of symbols for the missing values
    template <typename T_DATA, template<typename> class ALLOC>
    INLINE const std::vector<std::string,ALLOC<std::string>>&
    IDatabaseTable<T_DATA,ALLOC>::_missingSymbols () const {
      return __missing_symbols;
    }


    /// insert new rows at the end of the database
    template <template<typename> class ALLOC>
    void IDatabaseTableInsert4DBCell<ALLOC,true>::insertRows (
        const typename IDatabaseTableInsert4DBCell<ALLOC,true>::template
        DBVector<DBVector<std::string>>& new_rows ) {
      for ( const auto& new_row : new_rows )
        this->insertRow ( new_row );
    }

    
    /// insert new rows at the end of the database
    template <template<typename> class ALLOC>
    void IDatabaseTableInsert4DBCell<ALLOC,false>::insertRows (
        const typename IDatabaseTableInsert4DBCell<ALLOC,false>::template
        DBVector<DBVector<std::string>>& new_rows ) {
      for ( const auto& new_row : new_rows )
        this->insertRow ( new_row );
    }
  
  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
