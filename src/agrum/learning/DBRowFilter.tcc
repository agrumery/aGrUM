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
 * @brief Template implementation of DBRowFilters
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  
  namespace learning {

    
    /// default constructor
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE
    DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>::DBRowFilter
    ( DBHandler& handler,
      const TranslatorSet& translator_set,
      const GeneratorSet& generator_set,
      bool require_cell_init ) noexcept :
      __handler ( handler ),
      __translator_set ( translator_set ),
      __generator_set ( generator_set ) {
      if ( require_cell_init )
        __initCellFilters ();
      GUM_CONSTRUCTOR ( DBRowFilter );
    }


    /// copy constructor
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE
    DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>::DBRowFilter
    ( const DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>& filter ) noexcept :
      __handler ( filter.__handler ),
      __translator_set ( filter.__translator_set ),
      __generator_set ( filter.__generator_set ) {
      GUM_CONS_CPY ( DBRowFilter );
    }
    

    /// destructor
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE
    DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>::~DBRowFilter () noexcept {
      GUM_DESTRUCTOR ( DBRowFilter );
    }

    
    /// returns true if there are still rows that can be output by the RowFilter
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE bool
    DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>::hasRows () noexcept {
      return __handler.hasRows () || __generator_set.hasRows ();
    }


    /// initialize the cell filters by parsing once the database
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    void
    DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>::__initCellFilters () {
      __handler.reset ();
      while ( __handler.hasRows () ) {
        // get the next row to parse from the database
        __translator_set.setInputRow ( __handler.row () );
        __handler.nextRow ();

        // initialize the cell filters with this row
        __translator_set.initialize ();
      }
      __handler.reset ();
    }


    /// returns a new output row with its corresponding weight
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE FilteredRow&
    DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>::row () noexcept {
      if ( ! __generator_set.hasRows () ) {
        // get the next row to parse from the database
        __translator_set.setInputRow ( __handler.row () );
        __handler.nextRow ();

        // translate the database cells
        __translator_set.translate ();

        // pass the translation to the filtered row generator
        __generator_set.setInputRow ( __translator_set.outputRow () );
      }
        
      return __generator_set.generate ();
    }
      

    /// resets the filter
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE void
    DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>::reset () {
      __handler.reset ();
      __generator_set.reset ();
    }

    
    // returns the handler used by the filter
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    INLINE DBHandler&
    DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>::handler () noexcept {
      return __handler;
    }

    
  } /* namespace learning */

  
} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
