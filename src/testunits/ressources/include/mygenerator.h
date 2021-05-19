/***************************************************************************
 *   Copyright (c) 2005-2020 by Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)  *
 *   info_at_agrum_dot_org                                               *
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
 * in input
 *
 * @author Christophe GONZALES(@AMU) and Pierre-Henri WUILLEMIN(@LIP6)
 */
#ifndef GUM_LEARNING_MY_GENERATOR_H
#define GUM_LEARNING_MY_GENERATOR_H

#include <agrum/agrum.h>
#include <agrum/tools/database/DBRowGenerator.h>

namespace gum {

  namespace learning {

    template <template<typename> class ALLOC = std::allocator>
    class MyGenerator : public DBRowGenerator<ALLOC> {
    public:

      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC<DBTranslatedValue>;
      
      // ##########################################################################
      // @name Constructors / Destructors
      // ##########################################################################

      /// default constructor
      MyGenerator( const std::vector<DBTranslatedValueType,
                                     ALLOC<DBTranslatedValueType>> column_types,
                   const std::size_t nb_duplicates,
                   const allocator_type& alloc  = allocator_type () )
        : DBRowGenerator<ALLOC> ( column_types, DBRowGeneratorGoal::OTHER_THINGS_THAN_REMOVE_MISSING_VALUES, alloc )
        , nb_duplicates__ ( nb_duplicates ) {
        GUM_CONSTRUCTOR( MyGenerator );
      }

      /// copy constructor with a given allocator
      MyGenerator( const MyGenerator<ALLOC>& from,
                   const allocator_type& alloc )
        : DBRowGenerator<ALLOC>( from, alloc )
        , input_row__( from.input_row__ )
        , nb_duplicates__ ( from.nb_duplicates__ ) {
        GUM_CONS_CPY( MyGenerator );
      }

      /// copy constructor
      MyGenerator( const MyGenerator<ALLOC>& from )
         : MyGenerator<ALLOC> ( from, from.getAllocator () ) {}
        

      /// move constructor with a given allocator
      MyGenerator( MyGenerator<ALLOC>&& from,
                   const allocator_type& alloc )
        : DBRowGenerator<ALLOC> ( std::move( from ), alloc )
        , input_row__( from.input_row__ )
        , nb_duplicates__ ( from.nb_duplicates__ ) {
        GUM_CONS_MOV( MyGenerator );
      }


      /// move constructor
      MyGenerator( MyGenerator<ALLOC>&& from )
        : MyGenerator<ALLOC> ( std::move( from ), from.getAllocator () ) {}

      
      /// virtual copy constructor with a given allocator
      virtual MyGenerator<ALLOC>* clone ( const allocator_type& alloc ) const {
        ALLOC<MyGenerator<ALLOC>> allocator ( alloc );
        MyGenerator<ALLOC>* generator = allocator.allocate(1);
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
      virtual MyGenerator<ALLOC>* clone () const {
        return clone ( this->getAllocator () );
      }


      /// destructor
      ~MyGenerator() {
        GUM_DESTRUCTOR( MyGenerator );
      }
      

      // ##########################################################################
      // @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      MyGenerator<ALLOC>& operator=( const MyGenerator<ALLOC>& from ) {
        DBRowGenerator<ALLOC>::operator=( from );
        input_row__ = from.input_row__;
        nb_duplicates__ = from.nb_duplicates__;
        return *this;
      }
    

      /// move operator
      MyGenerator<ALLOC>& operator=( MyGenerator<ALLOC>&& from ) {
        DBRowGenerator<ALLOC>::operator=( std::move( from ) );
        input_row__ = from.input_row__;
        nb_duplicates__ = from.nb_duplicates__;
        return *this;
      }
    

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// generates new lines from those the generator gets in input
      virtual const DBRow<DBTranslatedValue,ALLOC>& generate() final {
        this->decreaseRemainingRows();
        return *input_row__;
      }
  
      
    protected:

      /// computes the rows it will provide in output
      virtual std::size_t
      computeRows_( const DBRow<DBTranslatedValue,ALLOC>& row ) final {
        input_row__ = &row;
        return nb_duplicates__;
      }


    private:
     
      /// the row used as input to generate the output DBRows
      const DBRow<DBTranslatedValue,ALLOC>* input_row__ { nullptr };

      /// the number of times we return each input row
      std::size_t nb_duplicates__ { std::size_t(1) };
      
    };

  } /* namespace learning */

} /* namespace gum */


#endif /* GUM_LEARNING_MY_GENERATOR_H */
