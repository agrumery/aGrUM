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
 * @brief The base class for all DBRow generators
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_H
#define GUM_LEARNING_DBROW_GENERATOR_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/tools/database/DBRow.h>
#include <agrum/tools/database/DBTranslatedValue.h>

namespace gum {

  namespace learning {

    /** @enum DBRowGeneratorGoal
     * @headerfile DBRowGenerator.h <agrum/tools/database/DBRowGenerator.h>
     * @brief the type of things that a DBRowGenerator is designed for
     *
     * @ingroup learning_database
     */
    enum class DBRowGeneratorGoal : char {
      // the generator's goal is only to remove all missing values
      ONLY_REMOVE_MISSING_VALUES,

      // the generator does something else than just missing values
      OTHER_THINGS_THAN_REMOVE_MISSING_VALUES
    };


    /** @class DBRowGenerator
     * @headerfile DBRowGenerator.h <agrum/tools/database/DBRowGenerator.h>
     * @ingroup learning_database
     * @brief The base class for all DBRow generators
     *
     * A DBRowGenerator instance takes as input a DBRow containing
     * DBTranslatedValue instances provided directly by a DatabaseTable or
     * resulting from a DBRow generation by another DBRowGenerator. Then,
     * it produces from 0 to several instances of DBRow of DBTranslatedValue.
     * This is essentially useful to deal with missing values: during learning,
     * when a DBRow contains some missing values, what should we do with it?
     * Should we discard it? Should we use an EM algorithm to produce several
     * DBRows weighted by their probability of occurrence? Should we use a
     * K-means algorithm to produce only one DBRow of highest probability of
     * occurrence? Using the appropriate DBRowGenerator, you can apply any
     * of these rules when your learning algorithm parses the DatabaseTable.
     * You just need to indicate which DBRowGenerator to use, no line of
     * code needs be changed in your high-level learning algorithm.
     *
     * As an example of how a DBRowGenerator works, an "Identity" DBRowGenerator
     * takes as input a DBRow and returns it without any further processing, so
     * it "produces" only one output DBRow. An EM DBRowGenerator takes in input
     * a DBRow in which some cells may be missing. In this case, it produces all
     * the possible combinations of values that these missing values may take and
     * it assigns to these combinations a weight proportional to their probability
     * of occurrence according to a given model. As such, it may most often produce
     * several output DBRows.
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
     *   while ( generator.hasRows () ) {
     *     const auto& output_dbrow = generator.generate ();
     *     // do something with the output dbrow
     *   }
     * }
     * @endcode
     *
     * All DBRowGenerator classes should derive from this class. It takes care
     * of the interaction with the RecordCounter / Score classes. The user
     * who wishes to create a new DBRowGenerator, say for instance, one that
     * outputs k times the input row, just has to define the following class
     * (not all the constructors/destructors are required, but we provide them
     * for self-consistency), the important part of which is located from the
     * "Accessors / Modifiers" section on:
     * @code
     * class DuplicateGenerator : public DBRowGenerator {
     * public:
     *
     *   // ######################################################################
     *   // Constructors / Destructors
     *   // ######################################################################
     *
     *   /// default constructor
     *   DuplicateGenerator(const std::vector< DBTranslatedValueType > column_types,
     *                      const std::size_t                          nb_duplicates)
     *     : DBRowGenerator ( column_types )
     *     ,  _nb_duplicates_ ( nb_duplicates ) {}
     *
     *   /// copy constructor
     *   DuplicateGenerator( const DuplicateGenerator& from)
     *     : DBRowGenerator( from)
     *     ,  _input_row_( from. _input_row_ )
     *     ,  _nb_duplicates_ ( from. _nb_duplicates_ ) {}
     *
     *   /// move constructor
     *   DuplicateGenerator(DuplicateGenerator&& from)
     *     : DBRowGenerator ( std::move( from ))
     *     ,  _input_row_( from. _input_row_ )
     *     ,  _nb_duplicates_ ( from. _nb_duplicates_ ) {}
     *
     *   /// virtual copy constructor
     *   virtual DuplicateGenerator* clone () const {
     *     return new DuplicateGenerator(*this);
     *   }
     *
     *   /// destructor
     *   ~DuplicateGenerator() {}
     *
     *
     *   // ######################################################################
     *   // Operators
     *   // ######################################################################
     *
     *   /// copy operator
     *   DuplicateGenerator&
     *   operator=( const DuplicateGenerator& from ) {
     *     DBRowGenerator::operator=( from );
     *      _input_row_ = from. _input_row_;
     *      _nb_duplicates_ = from. _nb_duplicates_;
     *     return *this;
     *   }
     *
     *   /// move operator
     *   DuplicateGenerator& operator=( DuplicateGenerator&& from ) {
     *     DBRowGenerator::operator=( std::move( from ) );
     *      _input_row_ = from. _input_row_;
     *      _nb_duplicates_ = from. _nb_duplicates_;
     *     return *this;
     *   }
     *
     *
     *   // ######################################################################
     *   // Accessors / Modifiers
     *   // ######################################################################
     *
     *   /// generates new lines from those the generator gets in input
     *   virtual const DBRow< DBTranslatedValue >& generate() final {
     *     this->decreaseRemainingRows();
     *     return * _input_row_;
     *   }
     *
     *
     * protected:
     *
     *   /// computes the rows it will provide in output
     *   virtual std::size_t
     *     computeRows_( const DBRow< DBTranslatedValue >& row ) final {
     *      _input_row_ = &row;
     *     return  _nb_duplicates_;
     *   }
     *
     *
     * private:
     *   /// the row used as input to generate the output DBRows
     *   const DBRow< DBTranslatedValue >*  _input_row_ { nullptr };
     *
     *   /// the number of times we return each input row
     *   std::size_t  _nb_duplicates_ { std::size_t(1) };
     * };
     * @endcode
     */
    class DBRowGenerator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      /** @param column_types indicates for each column whether this is a
       * continuous or a discrete one
       */
      DBRowGenerator(const std::vector< DBTranslatedValueType >& column_types,
                     const DBRowGeneratorGoal                    goal);

      /// copy constructor
      DBRowGenerator(const DBRowGenerator& from);

      /// move constructor
      DBRowGenerator(DBRowGenerator&& from);

      /// virtual copy constructor
      virtual DBRowGenerator* clone() const = 0;

      /// destructor
      virtual ~DBRowGenerator();

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /** @brief returns true if there are still rows that can be output by
       * the DBRowGenerator */
      bool hasRows();

      /// sets the input row from which the generator will create its output rows
      /** @return a Boolean indicating whether, from this input DBRow, the
       * DBRowGenerator is capable of outputing at least one row or not */
      bool setInputRow(const DBRow< DBTranslatedValue >& row);

      /// generate new rows from the input row
      virtual const DBRow< DBTranslatedValue >& generate() = 0;

      /// decrease the number of remaining output rows
      /** When method setInputRow is performed, the DBRowGenerator knows how
       * many output rows it will be able to generate. Each time method
       * decreaseRemainingRows is called, we decrement this number. When the
       * number becomes equal to 0, then there remains no new output row to
       * generate. */
      void decreaseRemainingRows();

      /// resets the generator. There are therefore no more ouput row to generate
      virtual void reset();

      /** @brief sets the columns of interest: the output DBRow needs only
       * contain correct values fot these columns
       *
       * This method is useful, e.g., for EM-like algorithms that need to know
       * which unobserved variables/values need be filled. In this case, the
       * DBRowGenerator still outputs DBRows with the same columns as the
       * DatabaseTable, but only the columns of these DBRows corresponding to
       * those passed in argument to Method setColumnsOfInterest are meaningful.
       * For instance, if a DatabaseTable contains 10 columns and Method
       * setColumnsOfInterest() is applied with vector<> { 0, 3, 4 }, then the
       * DBRowGenerator will output DBRows with 10 columns, in which only
       * columns 0, 3 and 4 are guaranteed to have correct values (columns are
       * always indexed, starting from 0).
       */
      virtual void setColumnsOfInterest(const std::vector< std::size_t >& cols_of_interest);

      /** @brief sets the columns of interest: the output DBRow needs only
       * contain correct values fot these columns
       *
       * This method is useful, e.g., for EM-like algorithms that need to know
       * which unobserved variables/values need be filled. In this case, the
       * DBRowGenerator still outputs DBRows with the same columns as the
       * DatabaseTable, but only the columns of these DBRows corresponding to
       * those passed in argument to Method setColumnsOfInterest are meaningful.
       * For instance, if a DatabaseTable contains 10 columns and Method
       * setColumnsOfInterest() is applied with vector<> { 0, 3, 4 }, then the
       * DBRowGenerator will output DBRows with 10 columns, in which only
       * columns 0, 3 and 4 are guaranteed to have correct values (columns are
       * always indexed, starting from 0).
       */
      virtual void setColumnsOfInterest(std::vector< std::size_t >&& cols_of_interest);

      /// returns the current set of columns of interest
      const std::vector< std::size_t >& columnsOfInterest() const;

      /// returns the goal of the DBRowGenerator
      DBRowGeneratorGoal goal() const;

      /// @}


      protected:
      /// the number of output rows still to retrieve through the generate method
      std::size_t nb_remaining_output_rows_{std::size_t(0)};

      /// the types of the columns in the DatabaseTable
      /** This is useful to determine whether we need to use the .discr_val
       * field or the .cont_val field in DBTranslatedValue instances. */
      std::vector< DBTranslatedValueType > column_types_;

      /// the set of columns of interest
      std::vector< std::size_t > columns_of_interest_;

      /// the goal of the DBRowGenerator (just remove missing values or not)
      DBRowGeneratorGoal goal_{DBRowGeneratorGoal::OTHER_THINGS_THAN_REMOVE_MISSING_VALUES};


      /// copy constructor
      DBRowGenerator& operator=(const DBRowGenerator&);

      /// move constructor
      DBRowGenerator& operator=(DBRowGenerator&&);

      /** @brief the method that computes the set of DBRow instances to output
       * after method setInputRow has been called */
      virtual std::size_t computeRows_(const DBRow< DBTranslatedValue >& row) = 0;
    };

  } /* namespace learning */

} /* namespace gum */


// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/DBRowGenerator_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DBROW_GENERATOR_H */
