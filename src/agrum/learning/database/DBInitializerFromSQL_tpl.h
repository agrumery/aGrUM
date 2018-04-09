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
 * @brief The class for initializing DatabaseTable and RawDatabaseTable
 * instances from SQL databases
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifdef _ODBC

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/learning/database/DBInitializerFromSQL.h>


namespace gum {

  namespace learning {

    /// default constructor
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>::DBInitializerFromSQL (
      const std::string& dataSource,
      const std::string& login,
      const std::string& password,
      const std::string& query,
      long               timeout,
      const typename DBInitializerFromSQL<ALLOC>::allocator_type& alloc )
      : IDBInitializer<ALLOC> ( IDBInitializer<ALLOC>::InputType::STRING, alloc )
      , __data_source ( dataSource )
      , __login ( login )
      , __password ( password )
      , __query ( query )
      , __timeout ( timeout )
      , __var_names ( alloc )
      , __parser ( alloc ) {
      // save the current locale because the connection to the database
      // will change it
      const std::string current_locale = std::setlocale(LC_NUMERIC, NULL );

      // perform the connection to the database
      __connection.connect ( dataSource, login, password, timeout );

      // restore the locale
      std::setlocale(LC_NUMERIC, current_locale.c_str () );

      // ask the parser to execute the query
      __parser.useNewQuery ( __connection, __query );
      
      // store the names of the columns into the intializer
      const std::size_t nb_cols = std::size_t ( __parser.nbColumns () );
      for ( std::size_t i = 0; i < nb_cols; ++i ) {
        __var_names.push_back ( __parser.columnName( i ) );
      }

      GUM_CONSTRUCTOR ( DBInitializerFromSQL );
    }


    /// copy constructor with a given allocator
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>::DBInitializerFromSQL (
      const DBInitializerFromSQL<ALLOC>& from,
      const allocator_type& alloc )
      : DBInitializerFromSQL<ALLOC> ( from.__data_source,
                                      from.__login,
                                      from.__password,
                                      from.__query,
                                      from.__timeout,
                                      alloc ) {}
   

    /// copy constructor
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>::DBInitializerFromSQL (
      const DBInitializerFromSQL<ALLOC>& from )
      : DBInitializerFromSQL<ALLOC> ( from, from.getAllocator () ) {}


    /// move constructor with a given allocator
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>::DBInitializerFromSQL (
      DBInitializerFromSQL<ALLOC>&& from,
      const allocator_type& alloc )
      : DBInitializerFromSQL<ALLOC> ( from.__data_source,
                                      from.__login,
                                      from.__password,
                                      from.__query,
                                      from.__timeout,
                                      alloc ) {}
    
    /// move constructor
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>::DBInitializerFromSQL (
      DBInitializerFromSQL<ALLOC>&& from )
      : DBInitializerFromSQL<ALLOC> ( std::move ( from ),
                                      from.getAllocator () ) {}

    
    /// virtual copy constructor with a given allocator
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>*
    DBInitializerFromSQL<ALLOC>::clone ( const allocator_type& alloc ) const {
      ALLOC<DBInitializerFromSQL<ALLOC>> allocator ( alloc );
      DBInitializerFromSQL<ALLOC>* new_initializer = allocator.allocate ( 1 );
      try {
        allocator.construct ( new_initializer, *this, alloc );
      }
      catch ( ... ) {
        allocator.deallocate ( new_initializer, 1 );
        throw;
      }

      return new_initializer;
    }

      
    /// virtual copy constructor
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>* DBInitializerFromSQL<ALLOC>::clone () const {
      return clone ( this->getAllocator () );
    }

 
    /// destructor
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>::~DBInitializerFromSQL () {
      GUM_DESTRUCTOR ( DBInitializerFromSQL );
    }

     
    /// copy operator
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>&
    DBInitializerFromSQL<ALLOC>::operator= (
      const DBInitializerFromSQL<ALLOC>& from ) {
      if ( this != &from ) {
        IDBInitializer<ALLOC>::operator= ( from );
        // check if the connection parameters have changed
        const bool connexion_changed =
          ( __data_source != from.__data_source ) ||
          ( __login       != from.__login ) ||
          ( __password    != from.__password ) ||
          ( __timeout     != from.__timeout );

        // save the new connection parameters
        __data_source = from.__data_source;
        __login       = from.__login;
        __password    = from.__password;
        __query       = from.__query;
        __timeout     = from.__timeout;

        // recreate the connection if needed
        if ( connexion_changed ) {
          if ( __connection.connected () )
            __connection.disconnect ();
          __connection.connect ( __data_source, __login, __password, __timeout );
        }
        
        // initiate the SQL parser
        __parser.useNewQuery ( __connection, __query );
        
        // store the names of the columns into the intializer
        __var_names.clear ();
        const std::size_t nb_cols = std::size_t ( __parser.nbColumns () );
        for ( std::size_t i = 0; i < nb_cols; ++i ) {
          __var_names.push_back ( __parser.columnName( i ) );
        }
      }

      return *this;
    }
    
        
    /// move operator
    template <template<typename> class ALLOC>
    DBInitializerFromSQL<ALLOC>&
    DBInitializerFromSQL<ALLOC>::operator= ( DBInitializerFromSQL<ALLOC>&& from ) {
      return operator= ( from );
    }

      
    /// returns the names of the variables
    template <template<typename> class ALLOC>
    INLINE std::vector<std::string,ALLOC<std::string>>
    DBInitializerFromSQL<ALLOC>::_variableNames () {
       return __var_names;
    }

    
    /// returns the content of the current row using strings
    template <template<typename> class ALLOC>
    INLINE const std::vector<std::string,ALLOC<std::string>>&
    DBInitializerFromSQL<ALLOC>::_currentStringRow () {
      return __parser.current();
    }
      

    /// indicates whether there is a next row to read (and point on it)
    template <template<typename> class ALLOC>
    INLINE bool
    DBInitializerFromSQL<ALLOC>::_nextRow () {
      return __parser.next ();
    }
    

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */

#endif  /* _ODBC */
