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
 * @brief The implementation of tabular databases stored in memory (RAM)
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <agrum/learning/database/databaseTable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    // default constructor
    template <template<typename> class ALLOC>
    template <template<typename> class XALLOC>
    DatabaseTable<ALLOC>::DatabaseTable(
      const typename DatabaseTable<ALLOC>::template MissingValType<XALLOC>&
      missing_symbols,
      const DBTranslatorSet<ALLOC>& translators,
      const typename DatabaseTable<ALLOC>::allocator_type& alloc )
      : IDatabaseTable<DBTranslatedValue,ALLOC> (
          missing_symbols, std::vector<std::string,ALLOC<std::string>> (), alloc )
      , __translators ( translators, alloc ) {
      if ( translators.size () ) {
        // set the variables names according to those of the translators
        std::vector<std::string,ALLOC<std::string>>
          var_names ( translators.size () );
        for ( std::size_t i = std::size_t ( 0 ), size = translators.size ();
              i < size; ++i ) {
          var_names[i] = __translators.translator(i).variable()->name ();
        }
        setVariableNames ( var_names, false );
      }
      
      GUM_CONSTRUCTOR( DatabaseTable );
    }


    // default constructor
    template <template<typename> class ALLOC>
    DatabaseTable<ALLOC>::DatabaseTable(
      const DBTranslatorSet<ALLOC>& translators,
      const typename DatabaseTable<ALLOC>::allocator_type& alloc )
      : IDatabaseTable<DBTranslatedValue,ALLOC> (
          std::vector<std::string,ALLOC<std::string>> (), 
          std::vector<std::string,ALLOC<std::string>> (), alloc )
      , __translators ( translators, alloc ) {
      if ( translators.size () ) {
        // set the variables names according to those of the translators
        std::vector<std::string,ALLOC<std::string>>
          var_names ( translators.size () );
        for ( std::size_t i = std::size_t ( 0 ), size = translators.size ();
              i < size; ++i ) {
          var_names[i] = __translators.translator(i).variable()->name ();
        }
        setVariableNames ( var_names, false );
      }
      
      GUM_CONSTRUCTOR( DatabaseTable );
    }


    // copy constructor with a given allocator
    template <template<typename> class ALLOC>
    INLINE DatabaseTable<ALLOC>::DatabaseTable
    ( const DatabaseTable<ALLOC>& from,
      const typename DatabaseTable<ALLOC>::allocator_type& alloc )
      : IDatabaseTable<DBTranslatedValue,ALLOC> ( from, alloc )
      , __translators ( from.__translators, alloc )
      , __ignored_cols ( from.__ignored_cols ) {
      GUM_CONS_CPY( DatabaseTable );
    }


    // copy constructor
    template <template<typename> class ALLOC>
    INLINE DatabaseTable<ALLOC>::DatabaseTable
    ( const DatabaseTable<ALLOC>& from )
      : DatabaseTable<ALLOC> ( from, from.getAllocator () ) {}


    // move constructor with a given allocator
    template <template<typename> class ALLOC>
    INLINE
    DatabaseTable<ALLOC>::DatabaseTable(
      DatabaseTable<ALLOC>&& from,
      const typename DatabaseTable<ALLOC>::allocator_type& alloc )
      : IDatabaseTable<DBTranslatedValue,ALLOC> ( std::move( from ), alloc )
      , __translators ( std::move ( from.__translators ), alloc )
      , __ignored_cols ( std::move ( from.__ignored_cols ) ) {
      GUM_CONS_MOV( DatabaseTable );
    }


    // move constructor
    template <template<typename> class ALLOC>
    INLINE
    DatabaseTable<ALLOC>::DatabaseTable( DatabaseTable<ALLOC>&& from )
      : DatabaseTable<ALLOC> ( std::move ( from ), from.getAllocator () ) {}
      
    
    // virtual copy constructor with a given allocator
    template <template<typename> class ALLOC>
    DatabaseTable<ALLOC>* DatabaseTable<ALLOC>::clone (
      const typename DatabaseTable<ALLOC>::allocator_type& alloc ) const {
      ALLOC<DatabaseTable<ALLOC>> allocator ( alloc );
      DatabaseTable<ALLOC>* new_db = allocator.allocate ( 1 );
      try {
        allocator.construct ( new_db, *this, alloc );
      }
      catch ( ... ) {
        allocator.deallocate ( new_db, 1 );
        throw;
      }

      return new_db;
    }
    

    // virtual copy constructor
    template <template<typename> class ALLOC>
    DatabaseTable<ALLOC>* DatabaseTable<ALLOC>::clone () const {
      return clone ( this->getAllocator () );
    }
    

    // destructor
    template <template<typename> class ALLOC>
    INLINE DatabaseTable<ALLOC>::~DatabaseTable() {
      GUM_DESTRUCTOR( DatabaseTable );
    }
    

    // copy operator
    template <template<typename> class ALLOC>
    DatabaseTable<ALLOC>&
    DatabaseTable<ALLOC>::operator=
    ( const DatabaseTable<ALLOC>& from ) {
      if ( this != &from ) {
        IDatabaseTable<DBTranslatedValue,ALLOC>::operator= ( from );
        __translators = from.__translators;
        __ignored_cols = from.__ignored_cols;
      }

      return *this;
    }

    
    // move constructor
    template <template<typename> class ALLOC>
    DatabaseTable<ALLOC>&
    DatabaseTable<ALLOC>::operator=( DatabaseTable<ALLOC>&& from ) {
      if ( this != &from ) {
        IDatabaseTable<DBTranslatedValue,ALLOC>::operator= ( std::move ( from ) );
        __translators = std::move ( from.__translators );
        __ignored_cols = std::move ( from.__ignored_cols );
      }

      return *this;
    }

    
    /// insert a new translator into the database
    template <template<typename> class ALLOC>
    void
    DatabaseTable<ALLOC>::insertTranslator (
      const DBTranslator<ALLOC>& translator,
      const std::size_t input_column ) {
      // check that there is no ignored_column corresponding to column 
      if ( __ignored_cols.exists ( input_column ) )
        GUM_ERROR ( OperationNotAllowed,
                    "the column is marked as being ignored. So it is impossible "
                    "to create a translator for that column" );
      
      // check that there does not already exists a translator for this column
      const std::size_t size = __translators.size ();
      for ( std::size_t i = std::size_t(0); i < size; ++i ) {
        if ( __translators.inputColumn(i) == input_column ) return;
      }

      // if the databaseTable is not empty, raise an exception
      if ( IDatabaseTable<DBTranslatedValue,ALLOC>::_content().size () )
        GUM_ERROR ( OperationNotAllowed,
                    "it is impossible to add a new column in the databaseTable "
                    "because this one is not empty" );

      // insert the translator into the translator set
      const std::size_t pos =
        __translators.insertTranslator ( translator, input_column );

      // insert the name of the translator's variable to the set of variable names
      try {
        this->_variable_names.insert ( this->_variable_names.begin() + pos,
                                       translator.variable ()->name () );
      }
      catch ( ... ) {
        __translators.eraseTranslator ( pos );
        throw;
      }
    }


    /// insert a new translator into the database
    template <template<typename> class ALLOC>
    template <typename GUM_SCALAR, template<typename> class XALLOC>
    void DatabaseTable<ALLOC>::insertTranslator (
      const Variable& var,
      const std::size_t input_column,
      std::vector<std::string,XALLOC<std::string>> missing_symbols ) {
      // check that there is no ignored_column corresponding to column 
      if ( __ignored_cols.exists ( input_column ) )
        GUM_ERROR ( OperationNotAllowed,
                    "the column is marked as being ignored. So it is impossible "
                    "to create a translator for that column" );
      
      // check that there does not already exists a translator for this column
      const std::size_t size = __translators.size ();
      for ( std::size_t i = std::size_t(0); i < size; ++i ) {
        if ( __translators.inputColumn(i) == input_column ) return;
      }

      // if the databaseTable is not empty, raise an exception
      if ( IDatabaseTable<DBTranslatedValue,ALLOC>::_content().size () )
        GUM_ERROR ( OperationNotAllowed,
                    "it is impossible to add a new column in the databaseTable "
                    "because this one is not empty" );

      // insert the translator into the translator set
      const std::size_t pos =
        __translators.insertTranslator ( var, input_column, missing_symbols );

      // insert the name of the translator's variable to the set of variable names
      try {
        this->_variable_names.insert ( this->_variable_names.begin() + pos,
                                       var.name () );
      }
      catch ( ... ) {
        __translators.eraseTranslator ( pos );
        throw;
      }
    }
    

    /** @brief returns the index corresponding either to the kth translator or
     * to that of the translator parsing the kth column of the input dataset */
    template <template<typename> class ALLOC>
    INLINE std::size_t DatabaseTable<ALLOC>::__getKthIndex (
      const std::size_t k,
      const bool k_is_input_col ) const {
      if ( k_is_input_col ) {
        const std::size_t nb_trans = __translators.size ();
        for ( std::size_t i = std::size_t(0); i < nb_trans; ++i ) {
          if ( __translators.inputColumn(i) == k ) {
            return i;
          }
        }
        return nb_trans+1;
      }
      else {
        return k;
      }
    }

    
    // erases the kth translator, that of the kth column of the output database
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::eraseTranslator ( const std::size_t k,
                                                 const bool k_is_input_col ) {
      // find the position of the translator to be erased within the
      // translator set: this is variable kk below
      const std::size_t nb_trans = __translators.size ();
      const std::size_t kk = __getKthIndex ( k, k_is_input_col );

      // check if the translator exists
      if ( nb_trans <= kk ) return;

      // erase the translator and the corresponding variable name. If
      // there remains no more translator in the translator set,
      // __data should become empty
      __translators.eraseTranslator ( kk );
      this->_variable_names.erase ( this->_variable_names.begin() + kk );
      if ( this->_variable_names.empty () ) {
        IDatabaseTable<DBTranslatedValue,ALLOC>::eraseAllRows();
      }
      else {
        for ( auto& xrow : IDatabaseTable<DBTranslatedValue,ALLOC>::_content() ) {
          auto& row = xrow.row ();
          row.erase ( row.begin() + kk );
        }
      }
    }

    
    /// returns the set of translators
    template <template<typename> class ALLOC>
    INLINE const DBTranslatorSet<ALLOC>&
    DatabaseTable<ALLOC>::translatorSet () const {
      return __translators;
    }

    
    /// returns the kth translator of the database
    template <template<typename> class ALLOC>
    const DBTranslator<ALLOC>&
    DatabaseTable<ALLOC>::translator ( const std::size_t k,
                                       const bool k_is_input_col ) const {
      // find the position of the translator that we look for. This
      // is variable kk below
      const std::size_t nb_trans = __translators.size ();
      const std::size_t kk = __getKthIndex ( k, k_is_input_col );

      // check if the translator exists
      if ( nb_trans <= kk )
        GUM_ERROR ( UndefinedElement,
                    "the translator could not be found in the database table" );

      return __translators.translator ( kk );
    }

    
    /// returns the kth variable of the database
    template <template<typename> class ALLOC>
    const Variable&
    DatabaseTable<ALLOC>::variable ( const std::size_t k,
                                     const bool k_is_input_col ) const {
      // find the position of the translator that contains the variable.
      // This is variable kk below
      const std::size_t nb_trans = __translators.size ();
      const std::size_t kk = __getKthIndex ( k, k_is_input_col );

      // check if the translator exists
      if ( nb_trans <= kk )
        GUM_ERROR ( UndefinedElement,
                    "the variable could not be found in the database table" );

      return __translators.variable ( kk );
    }
    
    
    /// sets the names of the variables
    template <template<typename> class ALLOC>
    void
    DatabaseTable<ALLOC>::setVariableNames(
      const std::vector<std::string,ALLOC<std::string>>& names,
      const bool from_external_object ) {
      const std::size_t size = names.size ();
      const std::size_t nb_trans = __translators.size ();
      if ( ! from_external_object ) {
        if ( nb_trans != size ) {
          GUM_ERROR ( SizeError,
                      "the number of variable's names does not correspond "
                      "to the number of columns of the database table" );
        }

        // update the translator names
        for ( std::size_t i = std::size_t(0); i < size; ++i ) {
          __translators.translator(i).setVariableName ( names[i] );
        }
      }
      else {
        if ( nb_trans && ( __translators.highestInputColumn () >= size ) ) {
          GUM_ERROR ( SizeError,
                      "the number of variable's names does not correspond "
                      "to the number of columns of the database table" );
        }
 
        // update the translator names
        for ( std::size_t i = std::size_t(0); i < nb_trans; ++i ) {
          __translators.translator(i).setVariableName (
             names[__translators.inputColumn(i)] );
        }
      }

      // update _variable_names using the newly assigned translators names
      this->_variable_names.resize ( nb_trans );
      for ( std::size_t i = std::size_t(0); i < nb_trans; ++i ) {
        this->_variable_names[i] = __translators.variable(i).name ();
      }
    }


    /** @brief indicates that we should ignore the kth column of the original
     * database when inserting new rows */
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::ignoreColumn ( const std::size_t k,
                                              const bool from_external_object ) {
      // first, compute the value that k would have in an external database
      // and the position kk of the translator that refers to it
      const std::size_t nb_trans = __translators.size ();
      std::size_t pos = nb_trans + 1; // assign an erroneous value to pos
      std::size_t kk = k; // kk = k value for an external database
      if ( ! from_external_object ) {
        if ( k < nb_trans ) {
          pos = k;
          kk = __translators.inputColumn ( k );
        }
      }
      else {
        for ( std::size_t i = std::size_t(0); i < nb_trans; ++i ) {
          if ( __translators.inputColumn ( i ) == k ) {
            pos = i;
            break;
          }
        }
      }
      
      // if the column is already ignored, do nothing, else add it to the
      // set of user-defined ignored columns
      if ( __ignored_cols.exists ( kk ) ) return;
      __ignored_cols.insert ( kk );

      
      if ( pos < nb_trans ) {
        // here we found a translator parsing column kk, so we should erase it,
        // as well as the resulting column in __data and the _variable_names.
        // Note that if there remains no more variable left, __data should
        // become empty
        __translators.eraseTranslator ( pos, false );
        this->_variable_names.erase ( this->_variable_names.begin() + pos );
        if ( this->_variable_names.empty () ) {
          IDatabaseTable<DBTranslatedValue,ALLOC>::eraseAllRows();
        }
        else {
          for ( auto& xrow :
                  IDatabaseTable<DBTranslatedValue,ALLOC>::_content() ) {
            auto& row = xrow.row ();
            row.erase ( row.begin() + pos );
          }
        }
      }
    }


    /// returns  the set of ignored columns
    template <template<typename> class ALLOC>
    const typename DatabaseTable<ALLOC>::template DBVector<std::size_t>
    DatabaseTable<ALLOC>::ignoredColumns () const {
      const std::size_t nb_trans = __translators.size ();
 
      if ( nb_trans == std::size_t(0) ) {
        return DBVector<std::size_t> { std::size_t(0) };
      }
      
     const std::size_t highest = __translators.highestInputColumn ();
     DBVector<std::size_t> ignored_cols;
     ignored_cols.reserve ( highest - nb_trans + 2 );

      // add all the columns not handled by the translators, up to the highest
      // column handled by the translators
      for ( std::size_t i = std::size_t(0), j = std::size_t(0);
            i < highest; ++i ) {
        if ( i != __translators.inputColumn(j) )
          ignored_cols.push_back ( i );
        else
          ++j;
      }

      // add the column past the last translator
      ignored_cols.push_back ( highest + 1 );

      return ignored_cols;
    }

    
    /// returns the set of columns parsed
    template <template<typename> class ALLOC>
    const typename DatabaseTable<ALLOC>::template DBVector<std::size_t>
    DatabaseTable<ALLOC>::inputColumns () const {
      const std::size_t nb_trans = __translators.size ();
      if ( nb_trans == std::size_t(0) ) {
        return DBVector<std::size_t> ();
      }
      
      DBVector<std::size_t> input_cols ( nb_trans );
      for ( std::size_t i = std::size_t(0); i < nb_trans; ++i ) {
        input_cols[i] = __translators.inputColumn(i);
      }
      return input_cols;
    }


    /// returns the domain size of the kth variable
    template <template<typename> class ALLOC>
    std::size_t 
    DatabaseTable<ALLOC>::domainSize ( const std::size_t k,
                                       const bool k_is_input_col ) const {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = __translators.size ();
      const std::size_t kk = __getKthIndex ( k, k_is_input_col );

      // check if the translator exists
      if ( nb_trans <= kk )
        GUM_ERROR ( UndefinedElement,
                    "the variable could not be found in the database table" );

      return __translators.domainSize ( kk );
    }
    

    /// returns the domain sizes of all the variables in the database table
    template <template<typename> class ALLOC>
    INLINE std::vector<std::size_t,ALLOC<std::size_t>>
    DatabaseTable<ALLOC>::domainSizes () const {
      const std::size_t nb_trans = __translators.size ();
      std::vector<std::size_t> dom ( nb_trans );
      for ( std::size_t i = std::size_t(0); i < nb_trans; ++i ){
        dom[i] = __translators.domainSize ( i );
      }
      return dom;
    }


    // indicates whether a reordering is needed to make the kth
    // translator sorted by lexicographical order
    template <template<typename> class ALLOC>
    INLINE bool
    DatabaseTable<ALLOC>::needsReordering ( const std::size_t k,
                                            const bool k_is_input_col ) const {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = __translators.size ();
      const std::size_t kk = __getKthIndex ( k, k_is_input_col );

      // check if the translator exists
      if ( nb_trans <= kk )
        GUM_ERROR ( UndefinedElement,
                    "the translator could not be found in the database table" );

      return __translators.needsReordering ( kk );
    }

    
    // performs a a lexicographic reordering of the kth translator or
    // of the translator corresponding to the kth column of the input database
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::reorder ( const std::size_t k,
                                         const bool k_is_input_col ) {
      // find the position kk of the translator that contains the variable
      const std::size_t nb_trans = __translators.size ();
      const std::size_t kk = __getKthIndex ( k, k_is_input_col );

      // check if the translator exists
      if ( nb_trans <= kk )
        GUM_ERROR ( UndefinedElement,
                    "the translator could not be found in the database table" );

      // get the translation to perform
      auto updates = __translators.reorder ( kk );
      if ( updates.empty () ) return;
      
      const std::size_t size = updates.size ();
      std::vector<std::size_t,ALLOC<std::size_t>> new_values ( size );
      for ( const auto& update : updates ) {
        if ( update.first >= size )
          new_values.resize ( update.first + 1 );
        new_values[update.first] = update.second;
      }

      // apply the translations
      //auto nb_threads = thread::getMaxNumberOfThreads();
      for ( auto& xrow : this->_content () ) {
        auto& elt = xrow.row()[kk].discr_val;
        if ( elt != std::numeric_limits<std::size_t>::max () )
          elt = new_values[elt];
      }
    }


    /// performs a reordering of all the columns
    template <template<typename> class ALLOC>
    INLINE void DatabaseTable<ALLOC>::reorder () {
      const std::size_t nb_trans = __translators.size ();
      for ( std::size_t i = std::size_t(0); i < nb_trans; ++i )
        reorder ( i, false );
    }

    
    /// insert a new row at the end of the database
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::insertRow(
      const std::vector<std::string,ALLOC<std::string>>& new_row ) {
      // check that the row can be fully translated, i.e., it contains enough
      // columns to be translated
      const std::size_t row_size = new_row.size ();
      if ( row_size == std::size_t(0) ) return;
      
      if ( __translators.highestInputColumn () >= row_size ) {
        GUM_ERROR( SizeError,
                   "the new row is smaller than required to be inserted "
                   "into the database" );
      }

      // convert the new_row into a row of DBTranslatedValue
      const std::size_t nb_trans = __translators.size ();
      Row<DBTranslatedValue> dbrow;
      dbrow.reserve ( nb_trans );
      for ( std::size_t i = std::size_t (0); i < nb_trans; ++i ) {
        dbrow.pushBack ( __translators.translate ( new_row, i ) );
      }

      this->insertRow ( std::move ( dbrow ) );
    }

    
    /** @brief check that a row's values are compatible with those of the
     * translators' variables */
    template <template<typename> class ALLOC>
    bool DatabaseTable<ALLOC>::__isRowCompatible (
      const typename DatabaseTable<ALLOC>::template Row<DBTranslatedValue>& row )
      const {
      // check that the size of the row corresponds to that of the translators
      const std::size_t row_size = row.size ();
      if ( row_size != __translators.size () ) return false;

      const auto& translators = __translators.translators ();
      for ( std::size_t i = std::size_t(0); i < row_size; ++i ) {
        switch ( translators[i]->getValType () ) {
        case DBTranslatedValueType::DISCRETE:
          if ( row[i].discr_val >= translators[i]->domainSize () ) return false;
          break;

        case DBTranslatedValueType::CONTINUOUS:
          {
            const ContinuousVariable<float>* var =
              static_cast<const ContinuousVariable<float>*>
              ( translators[i]->variable () );
            if ( ( var->lowerBound () > row[i].cont_val ) ||
                 ( var->upperBound () < row[i].cont_val ) ) return false;
            break;
          }

        default:
          GUM_ERROR ( NotImplementedYet,
                      "Translated value type not supported yet" );
        }
      }

      return true;
    }

    
    /// insert a new DBRow at the end of the database
    template <template<typename> class ALLOC>
    INLINE void DatabaseTable<ALLOC>::insertRow(
      typename DatabaseTable<ALLOC>::template Row<DBTranslatedValue>&& new_row ) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      if ( ! __isRowCompatible ( new_row ) ) {
        GUM_ERROR( InvalidArgument,
                   "the new row is not compatible with the current translators" );
      }
      
      IDatabaseTable<DBTranslatedValue,ALLOC>::insertRow ( std::move ( new_row ) );
    }


    /// insert a new row at the end of the database
    template <template<typename> class ALLOC>
    INLINE void DatabaseTable<ALLOC>::insertRow(
      const typename DatabaseTable<ALLOC>::template
      Row<DBTranslatedValue>& new_row ) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      if ( ! __isRowCompatible ( new_row ) ) {
        GUM_ERROR( InvalidArgument,
                   "the new row is not compatible with the current translators" );
      }
      
      IDatabaseTable<DBTranslatedValue,ALLOC>::insertRow ( new_row );
    }















    

    // insert a new DBRow of DBCells at the end of the database
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::insertRow (
      const typename DatabaseTable<ALLOC>::template Row<DBCell>& new_row ) {
      GUM_ERROR ( FatalError, "not implemented yet" );
    }

    // insert a new DBRow of DBCells at the end of the database
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::insertRow (
      typename DatabaseTable<ALLOC>::template Row<DBCell>&& new_row ) {
      GUM_ERROR ( FatalError, "not implemented yet" );
    }






    
      
    /// insert a set of new DBRows at the end of the database
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::insertRows(
      typename DatabaseTable<ALLOC>::template Matrix<DBTranslatedValue>&& rows ) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      for ( const auto& new_row : rows ) {
        if ( ! __isRowCompatible ( new_row ) ) {
        GUM_ERROR( InvalidArgument,
                   "the new row is not compatible with the current translators" );
        }
      }
        
      IDatabaseTable<DBTranslatedValue,ALLOC>::insertRows ( std::move ( rows ) );
    }
      

    /// insert a set of new DBRows at the end of the database
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::insertRows(
      const typename DatabaseTable<ALLOC>::template
      Matrix<DBTranslatedValue>& new_rows ) {
      // check that the new rows values are compatible with the values of
      // the variables stored within the translators
      for ( const auto& new_row : new_rows ) {
        if ( ! __isRowCompatible ( new_row ) ) {
        GUM_ERROR( InvalidArgument,
                   "the new row is not compatible with the current translators" );
        }
      }
        
      IDatabaseTable<DBTranslatedValue,ALLOC>::insertRows ( new_rows );
    }





    
    /// insert a set of new DBRows at the end of the database
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::insertRows(
      typename DatabaseTable<ALLOC>::template Matrix<DBCell>&& new_rows ) {
      GUM_ERROR ( FatalError, "not implemented yet" );
    }

    
    /// insert a set of new DBRows at the end of the database
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::insertRows(
      const typename DatabaseTable<ALLOC>::template Matrix<DBCell>& new_rows ) {
      GUM_ERROR ( FatalError, "not implemented yet" );
    }





    
    

    
    /// erase the content of the database, including the names of the variables
    template <template<typename> class ALLOC>
    void DatabaseTable<ALLOC>::clear() {
      __translators.clear ();
      __ignored_cols.clear ();
      IDatabaseTable<DBTranslatedValue,ALLOC>::clear ();
    }
    
    

    
  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
