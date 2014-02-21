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
 

    /// destructor
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    BasicDBRowTranslatorSet<Idx,Translator,
                            OtherTranslators...>::~BasicDBRowTranslatorSet ()
      noexcept {
      GUM_DESTRUCTOR ( BasicDBRowTranslatorSet );
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
    template <int Idx, typename Translator, typename... OtherTranslators> INLINE
    void BasicDBRowTranslatorSet<Idx,Translator,
                                 OtherTranslators...>::translate () {
      __translator.translate ();
      NextTranslators::translate ();
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


    /// destructor
    template <typename... Translators> INLINE
    DBRowTranslatorSet<Translators...>::~DBRowTranslatorSet () noexcept {
      GUM_DESTRUCTOR ( DBRowTranslatorSet );
    }

    
    /// execute all the translations on the current database row
    template <typename... Translators> INLINE
    void DBRowTranslatorSet<Translators...>::translate () {
      TranslatorSet::translate ();
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


