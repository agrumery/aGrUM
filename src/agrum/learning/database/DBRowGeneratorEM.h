
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
 * @brief A DBRowGenerator class that returns incomplete rows as EM would do
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_EM_H
#define GUM_LEARNING_DBROW_GENERATOR_EM_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/core/bijection.h>
#include <agrum/BN/BayesNet.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/learning/database/DBRowGeneratorWithBN.h>

namespace gum {

  namespace learning {

    /** @class DBRowGeneratorEM
     * @headerfile DBRowGeneratorEM.h <agrum/learning/database/DBRowGeneratorEM.h>
     * @brief A DBRowGenerator class that returns incomplete rows as EM would do
     *
     * @ingroup learning_database
     *
     * This class is a DBRowGenerator that fills the unobserved values of the
     * nodes of interest as the EM algorithm does, i.e., by returning all the
     * possible completed rows with a weight corresponding to the probability
     * of the completion.
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
     * gum::learning::DBRowGeneratorEM<> generator ( col_types );
     *
     * // parse the database and produce output rows
     * for ( auto dbrow : database ) {
     *   generator.setInputRow ( dbrow );
     *   while ( generator.hasRows() ) {
     *     const auto& output_dbrow = generator.generate ();
     *     // do something with the output dbrow
     *   }
     * }
     * @endcode
     */
    template < typename GUM_SCALAR = double,
               template < typename > class ALLOC = std::allocator >
    class DBRowGeneratorEM : public DBRowGeneratorWithBN< GUM_SCALAR, ALLOC > {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< DBTranslatedValue >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowGeneratorEM(
         const std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > >
                                       column_types,
         const BayesNet< GUM_SCALAR >& bn,
         const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
            nodeId2columns =
               Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
         const allocator_type& alloc = allocator_type());

      /// copy constructor
      DBRowGeneratorEM(const DBRowGeneratorEM< GUM_SCALAR, ALLOC >& from);

      /// copy constructor with a given allocator
      DBRowGeneratorEM(const DBRowGeneratorEM< GUM_SCALAR, ALLOC >& from,
                       const allocator_type&                        alloc);

      /// move constructor
      DBRowGeneratorEM(DBRowGeneratorEM< GUM_SCALAR, ALLOC >&& from);

      /// move constructor with a given allocator
      DBRowGeneratorEM(DBRowGeneratorEM< GUM_SCALAR, ALLOC >&& from,
                       const allocator_type&                   alloc);

      /// virtual copy constructor
      virtual DBRowGeneratorEM< GUM_SCALAR, ALLOC >* clone() const override final;

      /// virtual copy constructor with a given allocator
      virtual DBRowGeneratorEM< GUM_SCALAR, ALLOC >*
         clone(const allocator_type& alloc) const override final;

      /// destructor
      ~DBRowGeneratorEM();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowGeneratorEM< GUM_SCALAR, ALLOC >&
         operator=(const DBRowGeneratorEM< GUM_SCALAR, ALLOC >& from);

      /// move operator
      DBRowGeneratorEM< GUM_SCALAR, ALLOC >&
         operator=(DBRowGeneratorEM< GUM_SCALAR, ALLOC >&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// generates one ouput DBRow for each DBRow passed to method setInputRow
      virtual const DBRow< DBTranslatedValue, ALLOC >& generate() override final;

      /// assign a new Bayes net to the generator
      virtual void
         setBayesNet(const BayesNet< GUM_SCALAR >& new_bn) override final;

      /// returns the allocator used
      allocator_type getAllocator() const;

      /// @}


      protected:
      /// computes the rows it will provide as output
      virtual std::size_t
         _computeRows(const DBRow< DBTranslatedValue, ALLOC >& row) override final;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the row used as input to generate the output DBRows
      const DBRow< DBTranslatedValue, ALLOC >* __input_row{nullptr};

      /// the set of missing columns of the current row
      std::vector< std::size_t, ALLOC< std::size_t > > __missing_cols;

      /// the number of missing values in the current row
      std::size_t __nb_miss;

      /// the joint probability of the missing variables of the current row
      Potential< GUM_SCALAR > __joint_proba;

      /// an instantiation over the joint proba
      Instantiation* __joint_inst{nullptr};

      /// the row that we return if there are missing values
      /** DBRowGeneratorSets produce new rows in advance. So, when they
       * return a row, they have already computed the new row. To cope with
       * this, the DBRowGeneratorEM should have 2 filled_row: one that will
       * be used for the first row returned and one for the next one. Hence
       * __filled_row1 and __filled_row2, which are filled alternatively by
       * the content of the probabilities computed. */
      DBRow< DBTranslatedValue, ALLOC > __filled_row1;

      /// the row that we return if there are missing values
      DBRow< DBTranslatedValue, ALLOC > __filled_row2;

      /// indicates whether we should return filled_row1 or filled_row2
      bool __use_filled_row1{true};

      /// the weight of the original input row
      double __original_weight;


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/learning/database/DBRowGeneratorEM_tpl.h>

#endif /* GUM_LEARNING_DBROW_GENERATOR_EM_H */
