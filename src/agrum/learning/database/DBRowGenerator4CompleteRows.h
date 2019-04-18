
/**
 *
 *  Copyright 2005-2019 Pierre-Henri WUILLEMIN et Christophe GONZALES (LIP6)
 *   {prenom.nom}_at_lip6.fr
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/** @file
 * @brief A DBRowGenerator class that returns the rows that are complete
 * (fully observed) w.r.t. the nodes of interest
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_4_COMPLETE_ROWS_H
#define GUM_LEARNING_DBROW_GENERATOR_4_COMPLETE_ROWS_H

#include <agrum/agrum.h>
#include <agrum/learning/database/DBRowGenerator.h>

namespace gum {

  namespace learning {

    /** @class DBRowGenerator4CompleteRows
     * @headerfile DBRowGenerator4CompleteRows.h <agrum/learning/database/DBRowGenerator4CompleteRows.h>
     * @brief A DBRowGenerator class that returns the rows that are complete
     * (fully observed) w.r.t. the nodes of interest
     *
     * @ingroup learning_database
     *
     * This class is a DBRowGenerator that returns all the rows that are
     * complete (i.e., fully observed) w.r.t. the nodes of interest. In other
     * words, whenever the values of the nodes of interest are observed in a
     * row, this one is returned, but if at least one node is unobserved, the
     * generator does not return anything. This class is useful for bootstraping
     * EM algorithms.
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
     * gum::learning::DBRowGenerator4CompleteRows<> generator ( col_types );
     *
     * // parse the database and produce output rows
     * for ( auto dbrow : database ) {
     *   generator.setInputRow ( dbrow );
     *   if ( generator.hasRows() ) {
     *     const auto& output_dbrow = generator.generate ();
     *     // do something with the output dbrow
     *   }
     * }
     * @endcode
     */
    template < template < typename > class ALLOC = std::allocator >
    class DBRowGenerator4CompleteRows : public DBRowGenerator< ALLOC > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< DBTranslatedValue >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowGenerator4CompleteRows(
         const std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > >
                               column_types,
         const allocator_type& alloc = allocator_type());

      /// copy constructor
      DBRowGenerator4CompleteRows(
         const DBRowGenerator4CompleteRows< ALLOC >& from);

      /// copy constructor with a given allocator
      DBRowGenerator4CompleteRows(const DBRowGenerator4CompleteRows< ALLOC >& from,
                                  const allocator_type& alloc);

      /// move constructor
      DBRowGenerator4CompleteRows(DBRowGenerator4CompleteRows< ALLOC >&& from);

      /// move constructor with a given allocator
      DBRowGenerator4CompleteRows(DBRowGenerator4CompleteRows< ALLOC >&& from,
                                  const allocator_type&                  alloc);

      /// virtual copy constructor
      virtual DBRowGenerator4CompleteRows< ALLOC >* clone() const override final;

      /// virtual copy constructor with a given allocator
      virtual DBRowGenerator4CompleteRows< ALLOC >*
         clone(const allocator_type& alloc) const override final;

      /// destructor
      ~DBRowGenerator4CompleteRows();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowGenerator4CompleteRows< ALLOC >&
         operator=(const DBRowGenerator4CompleteRows< ALLOC >& from);

      /// move operator
      DBRowGenerator4CompleteRows< ALLOC >&
         operator=(DBRowGenerator4CompleteRows< ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// generates one ouput DBRow for each DBRow passed to method setInputRow
      /** @warning if this method is applied while the row it should return is
       * incomplete w.r.t. the nodes of interest, its behavior is uncertain
       * and will certainly result in a segmentation fault */
      virtual const DBRow< DBTranslatedValue, ALLOC >& generate() final;

      /// returns the allocator used
      allocator_type getAllocator() const;

      /// @}


      protected:
      /// computes the rows it will provide as output
      virtual std::size_t
         _computeRows(const DBRow< DBTranslatedValue, ALLOC >& row) final;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the row used as input to generate the output DBRows
      const DBRow< DBTranslatedValue, ALLOC >* __input_row{nullptr};

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/learning/database/DBRowGenerator4CompleteRows_tpl.h>

#endif /* GUM_LEARNING_DBROW_GENERATOR_4_COMPLETE_ROWS_H */
