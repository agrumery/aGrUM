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
 * @brief Template implementation of DBRowTranslatorSets
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {

    
    /// default constructor
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>::BasicDBRowTranslatorSet
    ( const Translator& first_translator,
      const OtherTranslators&... next_translators ) :
      NextTranslators ( next_translators ... ),
      __translator ( first_translator ) {
      GUM_CONSTRUCTOR ( BasicDBRowTranslatorSet );
      __translator.setOutputCols ( Idx );
    }
    

    /// copy constructor
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>::BasicDBRowTranslatorSet
    ( const BasicDBRowTranslatorSet<Idx,Translator,OtherTranslators...>& from ) :
      NextTranslators ( from ),
      __translator ( from.__translator ) {
      GUM_CONS_CPY ( BasicDBRowTranslatorSet );
      __translator.setOutputCols ( Idx );
    }
 

    /// move constructor
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>::BasicDBRowTranslatorSet
    ( BasicDBRowTranslatorSet<Idx,Translator,OtherTranslators...>&& from ) :
      NextTranslators ( std::move ( from ) ),
      __translator ( std::move ( from.__translator ) ) {
      GUM_CONS_MOV ( BasicDBRowTranslatorSet );
      __translator.setOutputCols ( Idx );
    }
 

    /// destructor
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>::~BasicDBRowTranslatorSet ()
      noexcept {
      GUM_DESTRUCTOR ( BasicDBRowTranslatorSet );
    }

    
    /// copy operator
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>&
    BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>::operator=
    ( const BasicDBRowTranslatorSet<Idx,Translator,OtherTranslators...>& from ) {
      if ( this != &from ) {
        NextTranslators::operator= ( from );
        __translator = from.__translator;
        __translator.setOutputCols ( Idx );
      }
      return *this;
    }

    
    /// move operator
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>&
    BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>::operator=
    ( BasicDBRowTranslatorSet<Idx,Translator,OtherTranslators...>&& from ) {
      if ( this != &from ) {
        NextTranslators::operator= ( std::move ( from ) );
        __translator = std::move ( from.__translator );
        __translator.setOutputCols ( Idx );
      }
      return *this;
    }
 

    /// sets the input row to filter
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    void BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>::setInputRow
    ( const DBRow& row ) noexcept {
      __translator.setInputRow ( row );
      NextTranslators::setInputRow ( row );
    }

    
    /// sets the output rows of the translators
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    void BasicDBRowTranslatorSet<Idx,Translator,
                                 OtherTranslators...>::setOutputRow
    ( FilteredRow& output_row ) noexcept {
      __translator.setOutputRow ( output_row );
      NextTranslators::setOutputRow ( output_row );
    }

    
    /// perform the translation of a database row
    template <int Idx, typename Translator, typename... OtherTranslators>
    ALWAYS_INLINE
    void BasicDBRowTranslatorSet<Idx,Translator,
                                 OtherTranslators...>::translate () {
      __translator.translate ();
      NextTranslators::translate ();
    }


    /// initialize the cell filters by parsing once the database
    template <int Idx, typename Translator, typename... OtherTranslators>
    ALWAYS_INLINE
    void BasicDBRowTranslatorSet<Idx,Translator,
                                 OtherTranslators...>::initialize () {
      __translator.initialize ();
      NextTranslators::initialize ();
    }


    /// perform a post initialization after the database parsing
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    void BasicDBRowTranslatorSet<Idx,Translator,
                                 OtherTranslators...>::postInitialize () {
      __translator.postInitialize ();
      NextTranslators::postInitialize ();
    }

    
    /** @brief indicates whether one of the translators needs an initial parsing
     * of the database to initialize itself */
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    bool BasicDBRowTranslatorSet<Idx,Translator,
                                 OtherTranslators...>::requiresInitialization ()
      const noexcept {
      return __translator.requiresInitialization () ||
        NextTranslators::requiresInitialization ();
    }

    
    /// push back the number of modalities of the variables of the output columns
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    void BasicDBRowTranslatorSet<Idx,Translator,
                                 OtherTranslators...>::modalities
    ( std::vector<unsigned int>& modals ) const {
      __translator.modalities ( modals );
      NextTranslators::modalities ( modals );
    }


    /// default constructor
    template <typename... Translators> INLINE
    DBRowTranslatorSet<Translators...>::DBRowTranslatorSet
    ( const Translators&...  translators ) :
      TranslatorSet ( translators... ) {
      GUM_CONSTRUCTOR ( DBRowTranslatorSet );

      // resize appropriately the output vector w.r.t. the applied translators
      __output_row.row().resize ( TranslatorSet::output_size );

      // assign the class' output row to all the CellTranslators
      TranslatorSet::setOutputRow ( __output_row );
    }


    /// copy constructor
    template <typename... Translators> INLINE
    DBRowTranslatorSet<Translators...>::DBRowTranslatorSet
    ( const DBRowTranslatorSet<Translators...>& from ) :
      TranslatorSet ( from ) {
      GUM_CONS_CPY ( DBRowTranslatorSet );
      
      __output_row.row().resize ( TranslatorSet::output_size );
      TranslatorSet::setOutputRow ( __output_row );
    }


    /// move constructor
    template <typename... Translators> INLINE
    DBRowTranslatorSet<Translators...>::DBRowTranslatorSet
    ( DBRowTranslatorSet<Translators...>&& from ) :
      TranslatorSet ( std::move ( from ) ),
      __output_row ( std::move ( from.__output_row ) ) {
      GUM_CONS_MOV ( DBRowTranslatorSet );
      TranslatorSet::setOutputRow ( __output_row );
    }


    /// destructor
    template <typename... Translators> INLINE
    DBRowTranslatorSet<Translators...>::~DBRowTranslatorSet () noexcept {
      GUM_DESTRUCTOR ( DBRowTranslatorSet );
    }

    
    /// copy operator
    template <typename... Translators> INLINE
    DBRowTranslatorSet<Translators...>&
    DBRowTranslatorSet<Translators...>::operator=
    ( const DBRowTranslatorSet<Translators...>& from )  {
      if ( this != &from ) {
        TranslatorSet::operator= ( from );      
        __output_row.row().resize ( TranslatorSet::output_size );
        TranslatorSet::setOutputRow ( __output_row );
      }
      return *this;
    }

    
    /// move operator
    template <typename... Translators> INLINE
    DBRowTranslatorSet<Translators...>&
    DBRowTranslatorSet<Translators...>::operator=
    ( DBRowTranslatorSet<Translators...>&& from )  {
      if ( this != &from ) {
        TranslatorSet::operator= ( std::move ( from ) );      
        __output_row = std::move ( from.__output_row );
        TranslatorSet::setOutputRow ( __output_row );
      }
      return *this;
    }

    
    /// execute all the translations on the current database row
    template <typename... Translators> ALWAYS_INLINE
    void DBRowTranslatorSet<Translators...>::translate () {
      TranslatorSet::translate ();
    }

    
    /// initialize the cell filters by parsing once the database
    template <typename... Translators> ALWAYS_INLINE
    void DBRowTranslatorSet<Translators...>::initialize () {
      TranslatorSet::initialize ();
    }


    /// initialize the cell filters by parsing once the database
    template <typename... Translators> INLINE
    void DBRowTranslatorSet<Translators...>::postInitialize () {
      TranslatorSet::postInitialize ();
    }


    /** @brief indicates whether one of the translators needs an initial parsing
     * of the database to initialize itself */
    template <typename... Translators> INLINE
    bool DBRowTranslatorSet<Translators...>::requiresInitialization ()
      const noexcept {
      return TranslatorSet::requiresInitialization ();
    }


    /// push back the number of modalities of the variables of the output columns
    template <typename... Translators> INLINE
    void DBRowTranslatorSet<Translators...>::modalities
    ( std::vector<unsigned int>& modals ) const {
      return TranslatorSet::modalities ( modals );
    }

    
    /// sets the input row that shall be read by all the cell translators
    template <typename... Translators> INLINE
    void DBRowTranslatorSet<Translators...>::setInputRow
    ( const DBRow& row ) noexcept {
      TranslatorSet::setInputRow ( row );
    }

    
    /// returns the current output row
    template <typename... Translators> INLINE
    FilteredRow& DBRowTranslatorSet<Translators...>::outputRow () noexcept {
      return __output_row;
    }
    

  } /* namespace learning */

  
} /* namespace gum */



#endif /* DOXYGEN_SHOULD_SKIP_THIS */


