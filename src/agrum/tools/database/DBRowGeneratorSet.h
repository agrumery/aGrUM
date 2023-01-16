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
 * @brief class for packing sets of generators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_SET_H
#define GUM_LEARNING_DBROW_GENERATOR_SET_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/database/DBRowGeneratorWithBN.h>

namespace gum {

  namespace learning {


    /** @class DBRowGeneratorSet
     * @headerfile DBRowGeneratorSet.h <agrum/tools/database/DBRowGeneratorSet.h>
     * @ingroup learning_database
     * @brief The class used to pack sets of generators
     *
     * When learning Bayesian networks, the records of the train dataset are
     * used to construct contingency tables that are either exploited in
     * statistical conditional independence tests or in scores. To achieve this,
     * the values of the DatabaseTable's records need all be observed, i.e.,
     * there should be no missing value. When this is not the case, we need to
     * decide what to do with the records (actually the DBRows) that contain
     * missing values. Should we discard them? Should we use an EM algorithm to
     * substitute them by several fully-observed DBRows weighted by their
     * probability of occurrence? Should we use a K-means algorithm to substitute
     * them by only one DBRow of highest probability of occurrence? DBRowGenerator
     * classes are used to perform these substitutions. From one input DBRow,
     * they can produce from 0 to several output DBRows. DBRowGenerator instances
     * can be used in sequences, i.e., a first DBRowGenerator can, e.g., apply
     * an EM algorithm to produce many output DBRows, then these DBRows can
     * feed another DBRowGenerator that only keeps those whose weight is higher
     * than a given threshold. The purpose of Class DBRowGeneratorSet is to
     * contain this sequence of DBRowGenerator instances. The key idea is that it
     * makes the parsing of the output DBRow generated easier. For instance, if
     * we want to use a sequence of 2 generators, outputing 3 times and 4 times
     * the DBRows they get in input respectively, we could use the following
     * code:
     * @code
     * gum::learning::DatabaseTable database ( ... );
     * gum::learning::DBRowGeneratorDuplicate generator3 ( col_types, 3 );
     * gum::learning::DBRowGeneratorDuplicate generator4 ( col_types, 4 );
     *
     * for ( auto dbrow : database ) {
     *   generator3.setInputRow ( dbrow );
     *   while ( generator3.hasRows () ) {
     *     const auto& output3_dbrow = generator3.generate ();
     *     generator4.setInputRow ( output3_dbrow );
     *     while ( generator4.hasRows () ) {
     *       const auto& output4_dbrow = generator4.generate ();
     *       // do something with output4_dbrow
     *     }
     *   }
     * }
     * @endcode
     * For each input DBRow of the DatabaseTable, these while loops output
     * 3 x 4 = 12 identical DBRows.
     * As can be seen, when several DBRowGenerator instances are to be used
     * in sequence, the code is not very easy to write. The DBRowGeneratorSet
     * simplifies the coding as follows:
     * @code
     * gum::learning::DatabaseTable database ( ... );
     * gum::learning::DBRowGeneratorDuplicate generator3 ( col_types, 3 );
     * gum::learning::DBRowGeneratorDuplicate generator4 ( col_types, 4 );
     *
     * DBRowGeneratorSet genset;
     * genset.insertGenerator ( generator3 );
     * genset.insertGenerator ( generator4 );
     * for ( auto dbrow : database ) {
     *   genset.setInputRow ( dbrow );
     *   while ( genset.hasRows () ) {
     *     const auto& output_dbrow = genset.generate ();
     *     // do something with output_dbrow
     *   }
     * }
     * @endcode
     * As can be seen, whatever the number of DBRowGenerator instances packed
     * into the DBRowGeneratorSet, only one while loop is needed to
     * parse all the generated output DBRow instances.
     */
    class DBRowGeneratorSet {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowGeneratorSet();

      /// copy constructor
      DBRowGeneratorSet(const DBRowGeneratorSet& from);

      /// move constructor
      DBRowGeneratorSet(DBRowGeneratorSet&& from);

      /// virtual copy constructor
      virtual DBRowGeneratorSet* clone() const;

      /// destructor
      virtual ~DBRowGeneratorSet();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowGeneratorSet& operator=(const DBRowGeneratorSet& from);

      /// move operator
      DBRowGeneratorSet& operator=(DBRowGeneratorSet&& from);

      /// returns the ith generator
      /** @warning this operator assumes that there are at least i+1 generators.
       * So, it won't check that the ith generator actually exists. If unsure,
       * use method generatorSafe that performs this check. */
      DBRowGenerator& operator[](const std::size_t i);

      /// returns the ith generator
      /** @warning this operator assumes that there are at least i+1 generators.
       * So, it won't check that the ith generator actually exists. If unsure,
       * use method generatorSafe that performs this check. */
      const DBRowGenerator& operator[](const std::size_t i) const;

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// inserts a new generator at the end of the set
      /** @throw OperationNotAllowed is raised if the generator set has already
       * started generating output rows and is currently in a state where the
       * generation is not completed yet (i.e., we still need to call the
       * generate() method to complete it). */
      template < class Generator >
      void insertGenerator(const Generator& generator);

      /// inserts a new generator at the ith position of the set
      /** @throw OperationNotAllowed is raised if the generator set has already
       * started generating output rows and is currently in a state where the
       * generation is not completed yet (i.e., we still need to call the
       * generate() method to complete it). */
      template < class Generator >
      void insertGenerator(const Generator& generator, const std::size_t i);

      /// returns the number of generators
      std::size_t nbGenerators() const noexcept;

      /// returns the number of generators (alias for nbGenerators)
      std::size_t size() const noexcept;

      /** @brief returns true if there are still rows that can be output
       * by the set of generators */
      bool hasRows();

      /// sets the input row from which the generators will create new rows
      /** @return true if the set of generators is able to generate output
       * rows from the input row passed in argument */
      bool setInputRow(const DBRow< DBTranslatedValue >& input_row);

      /// generates a new output row from the input row
      const DBRow< DBTranslatedValue >& generate();

      /// assign a new Bayes net to all the generators that depend on a BN
      /** Typically, generators based on EM or K-means depend on a model to
       * compute correctly their outputs. Method setBayesNet enables to
       * update their BN model. */
      template < typename GUM_SCALAR >
      void setBayesNet(const BayesNet< GUM_SCALAR >& new_bn);

      /// resets all the generators
      void reset();

      /// removes all the generators
      void clear();

      /** @brief sets the columns of interest: the output DBRow needs only
       * contain correct values fot these columns
       *
       * This method is useful, e.g., for EM-like algorithms that need to know
       * which unobserved variables/values need be filled. In this case, the
       * DBRowGenerator instances contained in the DBRowGeneratorSet still
       * output DBRows with the same columns as the DatabaseTable, but only the
       * columns of these DBRows corresponding to those passed in argument to
       * Method setColumnsOfInterest are meaningful. For instance, if a
       * DatabaseTable contains 10 columns and Method setColumnsOfInterest() is
       * applied with vector { 0, 3, 4 }, then the DBRowGenerator instances
       * contained in the DBRowGeneratorSet will output DBRows with 10 columns,
       * in which only columns 0, 3 and 4 are guaranteed to have correct values
       * (columns are always indexed, starting from 0).
       *
       * @throw OperationNotAllowed is raised if the generator set has already
       * started generating output rows and is currently in a state where the
       * generation is not completed yet (i.e., we still need to call the
       * generate() method to complete it). */
      void setColumnsOfInterest(const std::vector< std::size_t >& cols_of_interest);

      /** @brief sets the columns of interest: the output DBRow needs only
       * contain correct values fot these columns
       *
       * This method is useful, e.g., for EM-like algorithms that need to know
       * which unobserved variables/values need be filled. In this case, the
       * DBRowGenerator instances contained in the DBRowGeneratorSet still
       * output DBRows with the same columns as the DatabaseTable, but only the
       * columns of these DBRows corresponding to those passed in argument to
       * Method setColumnsOfInterest are meaningful. For instance, if a
       * DatabaseTable contains 10 columns and Method setColumnsOfInterest() is
       * applied with vector { 0, 3, 4 }, then the DBRowGenerator instances
       * contained in the DBRowGeneratorSet will output DBRows with 10 columns,
       * in which only columns 0, 3 and 4 are guaranteed to have correct values
       * (columns are always indexed, starting from 0).
       *
       * @throw OperationNotAllowed is raised if the generator set has already
       * started generating output rows and is currently in a state where the
       * generation is not completed yet (i.e., we still need to call the
       * generate() method to complete it). */
      void setColumnsOfInterest(std::vector< std::size_t >&& cols_of_interest);

      /// returns the current set of columns of interest
      const std::vector< std::size_t >& columnsOfInterest() const;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the vector of all the generators
      std::vector< DBRowGenerator* > _generators_;

      // the number of generators
      std::size_t _nb_generators_{std::size_t(0)};

      // the next output row to return when method generate is called
      const DBRow< DBTranslatedValue >* _output_row_{nullptr};

      // the generation of output rows can be viewed as the traversal of a
      // tree: each node of the tree correspond to the input row received by
      // a generator. So the root node is the row passed in argument to
      // the setInputDBrow() Method. From these input rows, generators produce
      // through their generate() method new output rows, which correspond to
      // the input rows of the next level of the tree. If we traverse this tree
      // in terms of generators rather than in terms of input rows, which makes
      // sense knowing our data structures, we need to know whether we should
      // call only Method generate() to move down the tree or whether we should
      // call first Method setInputDBrow() and then generate(). Actually, when
      // a generator receives a new input row, it should call Method
      // setInputDBrow(), which updates its data structure in order to be able
      // to subsequently produce new output rows using Method generate().
      // So, we need to discriminate between the situation in which Method
      // setInputDBrow() has already been called from the situation in which
      // we first need to call setInputDBrow(). The following vector allows this
      // discrimination: when its cells equal 0, we need to call setInputDBrow()
      // first, else when they equal 1, we just need to call the generate()
      // method.
      std::vector< int > _setInputRow_performed_;


      /// parse the row generation tree to produce a new row
      /** * @param input_row this is used only when i = 0. In this case, if the
       * first generator has not performed yet its call to setInputDBRow, then
       * we call this method, passing in argument the input_row
       * @param i indicate the generator from which we start the traversal
       */
      bool _produceNextRow_(const DBRow< DBTranslatedValue >* input_row, std::size_t i);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/database/DBRowGeneratorSet_tpl.h>

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/DBRowGeneratorSet_inl.h>
#endif /* GUM_NO_INLINE */


#endif /* GUM_LEARNING_DBROW_GENERATOR_SET_H */
