/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
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
 * @brief Base class for DBRowGenerator classes that use a BN for computing
 * their outputs
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLWithBNIN
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_WITH_BN_H
#define GUM_LEARNING_DBROW_GENERATOR_WITH_BN_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/BN/inference/variableElimination.h>
#include <agrum/tools/database/DBRowGenerator.h>

namespace gum {

  namespace learning {

    /** @class DBRowGeneratorWithBN
     * @headerfile DBRowGeneratorWithBN.h <agrum/tools/database/DBRowGeneratorWithBN.h>
     * @brief Base class for DBRowGenerator classes that use a BN for computing
     * their outputs
     *
     * @ingroup learning_database
     *
     * This class is a DBRowGenerator that fills the unobserved values of the
     * nodes of interest as the WithBN algorithm does, i.e., by returning all the
     * possible completed rows with a weight corresponding to the probability
     * of the completion.
     * The standard usage of a DBRowGenerator is the following:
     * @code
     * // create a DatabaseTable and fill it
     * gum::learning::DBTranslatorSet set;
     * for ( int i = 0; i < 10; ++i )
     *   set.insertTranslator(gum::learning::DBTranslator4LabelizedVariable(),i);
     * gum::learning::DatabaseTable database ( set );
     * // fill the database
     *
     * // keep in a vector the types of the columns in the database
     * const std::vector<gum::learning::DBTranslatedValueType>
     *   column_types ( 10, gum::learning::DBTranslatedValueType::DISCRETE );
     *
     * // create the generator
     * gum::learning::DBRowGeneratorWithBN generator ( col_types );
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
    template < typename GUM_SCALAR = double >
    class DBRowGeneratorWithBN: public DBRowGenerator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowGeneratorWithBN(const std::vector< DBTranslatedValueType >& column_types,
                           const BayesNet< GUM_SCALAR >&               bn,
                           const DBRowGeneratorGoal                    goal,
                           const Bijection< NodeId, std::size_t >&     nodeId2columns
                           = Bijection< NodeId, std::size_t >());

      /// copy constructor
      DBRowGeneratorWithBN(const DBRowGeneratorWithBN< GUM_SCALAR >& from);

      /// move constructor
      DBRowGeneratorWithBN(DBRowGeneratorWithBN< GUM_SCALAR >&& from);

      /// destructor
      ~DBRowGeneratorWithBN();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// assign a new Bayes net to the generator
      virtual void setBayesNet(const BayesNet< GUM_SCALAR >& new_bn);

      /// returns the Bayes net used by the generator
      const BayesNet< GUM_SCALAR >& getBayesNet() const;

      /// @}


      protected:
      /// the Bayesian network used to fill the unobserved values
      const BayesNet< GUM_SCALAR >* bn_;

      /// the mapping betwen the BN's node ids and the database's columns
      Bijection< NodeId, std::size_t > nodeId2columns_;


      /// copy operator
      DBRowGeneratorWithBN< GUM_SCALAR >& operator=(const DBRowGeneratorWithBN< GUM_SCALAR >& from);

      /// move operator
      DBRowGeneratorWithBN< GUM_SCALAR >& operator=(DBRowGeneratorWithBN< GUM_SCALAR >&& from);
    };

  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/tools/database/DBRowGeneratorWithBN_tpl.h>

#endif /* GUM_LEARNING_DBROW_GENERATOR_WITH_BN_H */
