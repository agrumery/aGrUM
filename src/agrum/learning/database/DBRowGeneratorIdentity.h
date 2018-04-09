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
 * @brief A DBRowGenerator class that returns exactly the rows it gets in input
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_IDENTITY_H
#define GUM_LEARNING_DBROW_GENERATOR_IDENTITY_H

#include <agrum/agrum.h>
#include <agrum/learning/database/DBRowGenerator.h>

namespace gum {

  namespace learning {

    /** @class DBRowGeneratorIdentity
     * @headerfile DBRowGeneratorIdentity.h <agrum/learning/database/DBRowGeneratorIdentity.h>
     * @brief A DBRowGenerator class that returns exactly the rows it gets
     * in input
     *
     * @ingroup learning_database
     *
     * This class is a dummy DBRowGenerator that can be essentially used for
     * debugging purposes. The DBRowGeneratorIdentity class is designed to
     * take as input DBRow instances via its method setInputRow and to ouput
     * them without any additional processing via its method generate. See
     * class DBRowGenerator for further details on how DBRowGenerator works.
     *
     * The standard usage of a DBRowGenerator is the following:
     * @code
     * // create a DatabaseTable and fill it
     * gum::learning::DBTranslatorSet<> set;
     * for ( int i = 0; i < 10; ++i )
     *   set.insertTranslator(gum::learning::DBTranslator4LabelizedVariable<>(),i);
     * gum::learning::DatabaseTable<> database ( set );
     * // fill the database
     *
     * // keep in a vector the types of the columns in the database
     * const std::vector<gum::learning::DBTranslatedValueType>
     *   column_types ( 10, gum::learning::DBTranslatedValueType::DISCRETE );
     *
     * // create the generator
     * gum::learning::DBRowGeneratorIdentity<> generator ( col_types );
     *
     * // parse the database and produce output rows
     * for ( auto dbrow : database ) {
     *   generator.setInputRow ( dbrow );
     *   const auto& output_dbrow = generator.generate ();
     *   // do something with the output dbrow
     * }
     * @endcode
     */
    template <template<typename> class ALLOC = std::allocator>
    class DBRowGeneratorIdentity : public DBRowGenerator<ALLOC> {
    public:

      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC<DBTranslatedValue>;
      
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowGeneratorIdentity(
        const std::vector<DBTranslatedValueType,
                          ALLOC<DBTranslatedValueType>> column_types,
        const allocator_type& alloc  = allocator_type () );

      /// copy constructor
      DBRowGeneratorIdentity( const DBRowGeneratorIdentity<ALLOC>& from );

      /// copy constructor with a given allocator
      DBRowGeneratorIdentity( const DBRowGeneratorIdentity<ALLOC>& from,
                              const allocator_type& alloc );

      /// move constructor
      DBRowGeneratorIdentity( DBRowGeneratorIdentity<ALLOC>&& from );

      /// move constructor with a given allocator
      DBRowGeneratorIdentity( DBRowGeneratorIdentity<ALLOC>&& from,
                              const allocator_type& alloc );

      /// virtual copy constructor
      virtual DBRowGeneratorIdentity<ALLOC>* clone () const;

      /// virtual copy constructor with a given allocator
      virtual DBRowGeneratorIdentity<ALLOC>*
      clone ( const allocator_type& alloc ) const; 

      /// destructor
      ~DBRowGeneratorIdentity();

      /// @}
      

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowGeneratorIdentity<ALLOC>&
      operator=( const DBRowGeneratorIdentity<ALLOC>& from );

      /// move operator
      DBRowGeneratorIdentity<ALLOC>&
      operator=( DBRowGeneratorIdentity<ALLOC>&& from );

      /// @}
      

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// generates one ouput DBRow for each DBRow passed to method setInputRow
      virtual const DBRow<DBTranslatedValue,ALLOC>& generate() final;

      /// @}

      
    protected:

      /// computes the rows it will provide as output
      virtual std::size_t
      _computeRows( const DBRow<DBTranslatedValue,ALLOC>& row ) final;

  
#ifndef DOXYGEN_SHOULD_SKIP_THIS
    
    private:
      
      /// the row used as input to generate the output DBRows
      const DBRow<DBTranslatedValue,ALLOC>* __input_row { nullptr };
     
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

    };

  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/learning/database/DBRowGeneratorIdentity_tpl.h>

#endif /* GUM_LEARNING_DBROW_GENERATOR_IDENTITY_H */
