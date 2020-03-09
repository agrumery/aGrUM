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
 * @brief A DBRowGenerator class that returns exactly k times the rows it gets
 * in input when there has been an even number of times method setInputDBRow
 * has been called, else it returns 0 output row
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_MY_GENERATOR2_H
#define GUM_LEARNING_MY_GENERATOR2_H

#include <agrum/agrum.h>
#include <agrum/tools/database/DBRowGenerator.h>

namespace gum {

  namespace learning {

    template <template<typename> class ALLOC = std::allocator>
    class MyGenerator2 : public DBRowGenerator<ALLOC> {
    public:

      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC<DBTranslatedValue>;
      
      // ##########################################################################
      // @name Constructors / Destructors
      // ##########################################################################

      /// default constructor
      MyGenerator2( const std::vector<DBTranslatedValueType,
                                     ALLOC<DBTranslatedValueType>> column_types,
                   const std::size_t nb_duplicates,
                   const allocator_type& alloc  = allocator_type () )
        : DBRowGenerator<ALLOC> ( column_types, DBRowGeneratorGoal::OTHER_THINGS_THAN_REMOVE_MISSING_VALUES, alloc )
        , __nb_duplicates ( nb_duplicates ) {
        GUM_CONSTRUCTOR( MyGenerator2 );
      }

      /// copy constructor with a given allocator
      MyGenerator2( const MyGenerator2<ALLOC>& from,
                   const allocator_type& alloc )
        : DBRowGenerator<ALLOC>( from, alloc )
        , __input_row( from.__input_row )
        , __nb_duplicates ( from.__nb_duplicates )
        , __even_setInputRow ( from.__even_setInputRow ) {
        GUM_CONS_CPY( MyGenerator2 );
      }

      /// copy constructor
      MyGenerator2( const MyGenerator2<ALLOC>& from )
         : MyGenerator2<ALLOC> ( from, from.getAllocator () ) {}
        

      /// move constructor with a given allocator
      MyGenerator2( MyGenerator2<ALLOC>&& from,
                   const allocator_type& alloc )
        : DBRowGenerator<ALLOC> ( std::move( from ), alloc )
        , __input_row( from.__input_row )
        , __nb_duplicates ( from.__nb_duplicates )
        , __even_setInputRow ( from.__even_setInputRow ) {
        GUM_CONS_MOV( MyGenerator2 );
      }


      /// move constructor
      MyGenerator2( MyGenerator2<ALLOC>&& from )
        : MyGenerator2<ALLOC> ( std::move( from ), from.getAllocator () ) {}

      
      /// virtual copy constructor with a given allocator
      virtual MyGenerator2<ALLOC>* clone ( const allocator_type& alloc ) const {
        ALLOC<MyGenerator2<ALLOC>> allocator ( alloc );
        MyGenerator2<ALLOC>* generator = allocator.allocate(1);
        try {
          allocator.construct ( generator, *this, alloc );
        }
        catch ( ... ) {
          allocator.deallocate ( generator, 1 );
          throw;
        }
        return generator;
      }

      
      /// virtual copy constructor
      virtual MyGenerator2<ALLOC>* clone () const {
        return clone ( this->getAllocator () );
      }


      /// destructor
      ~MyGenerator2() {
        GUM_DESTRUCTOR( MyGenerator2 );
      }
      

      // ##########################################################################
      // @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      MyGenerator2<ALLOC>& operator=( const MyGenerator2<ALLOC>& from ) {
        DBRowGenerator<ALLOC>::operator=( from );
        __input_row = from.__input_row;
        __nb_duplicates = from.__nb_duplicates;
        __even_setInputRow = from.__even_setInputRow;
        return *this;
      }
    

      /// move operator
      MyGenerator2<ALLOC>& operator=( MyGenerator2<ALLOC>&& from ) {
        DBRowGenerator<ALLOC>::operator=( std::move( from ) );
        __input_row = from.__input_row;
        __nb_duplicates = from.__nb_duplicates;
        __even_setInputRow = from.__even_setInputRow;
        return *this;
      }
    

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// generates new lines from those the generator gets in input
      virtual const DBRow<DBTranslatedValue,ALLOC>& generate() final {
        this->decreaseRemainingRows();
        return *__input_row;
      }
  
      
    protected:

      /// computes the rows it will provide in output
      virtual std::size_t
      _computeRows( const DBRow<DBTranslatedValue,ALLOC>& row ) final {
        __even_setInputRow = ! __even_setInputRow;
        if ( __even_setInputRow ) {
          __input_row = &row;
          return __nb_duplicates;
        }
        else {
          return std::size_t(0);
        }
      }


    private:
     
      /// the row used as input to generate the output DBRows
      const DBRow<DBTranslatedValue,ALLOC>* __input_row { nullptr };

      /// the number of times we return each input row
      std::size_t __nb_duplicates { std::size_t(1) };

      // indicates whether there has been an even number of times method
      // setInputDBRow has been called
      bool __even_setInputRow { false };
      
    };

  } /* namespace learning */

} /* namespace gum */


#endif /* GUM_LEARNING_MY_GENERATOR2_H */
