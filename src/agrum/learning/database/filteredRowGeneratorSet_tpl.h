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
 * @brief Template implementation of FilteredRowGeneratorSet
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {


    /// default constructor
    template <typename Generator>
    INLINE FilteredRowGeneratorSet<Generator>:: FilteredRowGeneratorSet () {
      GUM_CONSTRUCTOR( FilteredRowGeneratorSet );
    }

    
    /// copy constructor
    template <typename Generator>
    INLINE FilteredRowGeneratorSet<Generator>::FilteredRowGeneratorSet(
            const FilteredRowGeneratorSet<Generator>& from )
      : __generators ( from.__generators.size () )
      , __output_row ( from.__output_row ) {
      GUM_CONS_CPY( FilteredRowGeneratorSet );

      std::size_t size = from.__generators.size ();
      for ( std::size_t i = 0; i < size; ++i )
        __generators[i] = from.__generators[i]->copyFactory ();
      
      if ( size )
        __last_generator = *( __generators.rbegin () );
    }

    
    /// move constructor
    template <typename Generator>
    INLINE FilteredRowGeneratorSet<Generator>::FilteredRowGeneratorSet(
            FilteredRowGeneratorSet<Generator>&& from )
        : __generators ( std::move( from.__generators ) )
        , __output_row ( from.__output_row ) {
      GUM_CONS_MOV( FilteredRowGeneratorSet );
      __last_generator = from.__last_generator;
      from.__last_generator = nullptr;
      from.__output_row = nullptr;
    }

    
    /// destructor
    template <typename Generator>
    INLINE FilteredRowGeneratorSet<Generator>::~FilteredRowGeneratorSet() {
      GUM_DESTRUCTOR( FilteredRowGeneratorSet );

      for ( auto gen : __generators )
        delete gen;
    }
    

    /// copy operator
    template <typename Generator>
    INLINE FilteredRowGeneratorSet<Generator>&
    FilteredRowGeneratorSet<Generator>::operator=(
        const FilteredRowGeneratorSet<Generator>& from ) {
      if ( this != &from ) {
        // remove the old generators and produce new ones
        for ( auto gen : __generators )
          delete gen;
        std::size_t size = from.__generators.size ();
        for ( std::size_t i = 0; i < size; ++i )
            __generators[i] = from.__generators[i]->copyFactory ();
        if ( size )
          __last_generator = *( __generators.rbegin () );
        else
          __last_generator = nullptr;
        
        __output_row = from.__output_row;
      }
      return *this;
    }

    
    /// move operator
    template <typename Generator>
    INLINE FilteredRowGeneratorSet<Generator>&
    FilteredRowGeneratorSet<Generator>::operator=(
        FilteredRowGeneratorSet<Generator>&& from ) {
      if ( this != &from ) {
        __generators = std::move( from.__generators );
        __last_generator = from.__last_generator;
        from.__last_generator = nullptr;
        __output_row = from.__output_row;
      }
      return *this;
    }


    /// inserts a new generator of type Generator at the end of the set
    template <typename Generator>
    void FilteredRowGeneratorSet<Generator>::insertGenerator () {
      Generator* new_gen = new Generator;
      __generators.push_back ( new_gen );
      __last_generator = new_gen;
      __output_row = nullptr;
    }

    
    /// inserts a new generator of type Generator at the end of the set
    template <typename Generator>
    template <typename... Args>
    void FilteredRowGeneratorSet<Generator>::emplaceGenerator ( Args&&... args ) {
      Generator* new_gen = new Generator ( std::forward<Args>( args )... );
      __generators.push_back ( new_gen );
      __last_generator = new_gen;
      __output_row = nullptr;
    }

    
    /// inserts a new generator at the end of the set
    template <typename Generator>
    template <class NewGenerator>
    void FilteredRowGeneratorSet<Generator>::insertGenerator
    ( const NewGenerator& generator ) {
      NewGenerator* new_gen = new NewGenerator ( generator );
      __generators.push_back ( new_gen );
      __last_generator = new_gen;
      __output_row = nullptr;
    }


    /// returns the number of generators 
    template <typename Generator>
    INLINE Size
    FilteredRowGeneratorSet<Generator>::nbGenerators() const noexcept {
      return __generators.size ();
    }


    /// returns true if there are still rows that can be output by the RowFilter
    template <typename Generator>
    bool FilteredRowGeneratorSet<Generator>::__hasRows( std::size_t i ) {
      auto gen = __generators[i];

      if ( gen != __last_generator ) {
        if ( __hasRows( i+1 ) ) return true;
        while ( gen->hasRows() ) {
          if ( __setInputRow( i+1, gen->generate() ) ) {
            return true;
          }
        }
      }
      else {
        if ( __output_row != nullptr ) return true;
        if ( gen->hasRows() ) {
          __output_row = &( gen->generate () );
          return true;
        }
      }

      return false;
    }

    
    /// returns true if there are still rows that can be output by the RowFilter
    template <typename Generator>
    INLINE bool
    FilteredRowGeneratorSet<Generator>::hasRows() {
      return __hasRows ( 0 );
    }

        
    /// sets the input row from which the generator will create new rows
    template <typename Generator>
    bool FilteredRowGeneratorSet<Generator>::__setInputRow( std::size_t i,
                                                            FilteredRow& row ) {
      auto gen = __generators[i];
      gen->setInputRow( row );

      if ( gen != __last_generator ) {
        while ( gen->hasRows() ) {
          if ( __setInputRow( i+1, gen->generate () ) ) {
            return true;
          }
        }
      }
      else {
        if ( gen->hasRows() ) {
          __output_row = &( gen->generate () );
          return true;
        }
      }

      return false;
    }
      

    /// sets the input row from which the generator will create new rows
    template <typename Generator>
    INLINE bool
    FilteredRowGeneratorSet<Generator>::setInputRow( FilteredRow& row ) {
      return __setInputRow( 0, row );
    }


    /// generate new rows from the input row
    template <typename Generator>
    FilteredRow&
    FilteredRowGeneratorSet<Generator>::__generate( std::size_t i ) {
      auto gen = __generators[i];

      if ( gen != __last_generator ) {
        if ( ! __hasRows ( i+1 ) && gen->hasRows () )
          __setInputRow( i+1, gen->generate() );
        return __generate( i+1 );
      }
      else {
        if ( ( __output_row == nullptr ) && gen->hasRows () )
          __output_row = &( gen->generate () );
        FilteredRow& row = *( __output_row );
        __output_row = nullptr;
        return row;
      }

      // we should never arrive here
      GUM_ERROR ( UndefinedElement,
                  "the generator cannot generate rows anymore" );
    }
          
 
    /// generate new rows from the input row
    template <typename Generator>
    INLINE FilteredRow&
    FilteredRowGeneratorSet<Generator>::generate () {
      return __generate ( 0 );
    }
    

    /// resets the filter
    template <typename Generator>
    INLINE void FilteredRowGeneratorSet<Generator>::reset() {
      for ( auto gen : __generators )
        gen->reset ();
      __output_row = nullptr;
    }


  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
