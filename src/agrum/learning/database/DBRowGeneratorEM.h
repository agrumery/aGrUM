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
#include <agrum/learning/database/DBRowGenerator.h>

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
    class DBRowGeneratorEM : public DBRowGenerator< ALLOC > {
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
        const BayesNet<GUM_SCALAR>& bn,
        const Bijection< NodeId, std::size_t, ALLOC< std::size_t > >&
              nodeId2columns =
                Bijection< NodeId, std::size_t, ALLOC< std::size_t > >(),
        const allocator_type&       alloc = allocator_type());

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
      virtual DBRowGeneratorEM< GUM_SCALAR, ALLOC >* clone() const;

      /// virtual copy constructor with a given allocator
      virtual DBRowGeneratorEM< GUM_SCALAR, ALLOC >*
        clone(const allocator_type& alloc) const;

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

      /// @}


      protected:
      /// computes the rows it will provide as output
      virtual std::size_t
        _computeRows(const DBRow< DBTranslatedValue, ALLOC >& row) override final;


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the row used as input to generate the output DBRows
      const DBRow< DBTranslatedValue, ALLOC >* __input_row{nullptr};

      /// the Bayesian network used to fill the unobserved values
      const BayesNet<GUM_SCALAR>* __bn;

      /// the mapping betwen the BN's node ids and the database's columns
      Bijection< NodeId, std::size_t, ALLOC< std::size_t > >
      __nodeId2columns;

      /// the set of missing columns of the current row
      std::vector< std::size_t, ALLOC< std::size_t > > __missing_cols;

      /// the number of missing values in the current row
      std::size_t __nb_miss;

      /// the joint probability of the missing variables of the current row
      Potential< GUM_SCALAR > __joint_proba;

      /// an instantiation over the joint proba
      Instantiation* __joint_inst {nullptr};
      
      /// the row that we return if there are missing values
      DBRow< DBTranslatedValue, ALLOC > __filled_row;

      /// the weight of the original input row
      double __original_weight;

  
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/learning/database/DBRowGeneratorEM_tpl.h>

#endif /* GUM_LEARNING_DBROW_GENERATOR_EM_H */
