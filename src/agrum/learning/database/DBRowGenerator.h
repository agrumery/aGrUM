
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
 * @brief The base class for all DBRow generators
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DBROW_GENERATOR_H
#define GUM_LEARNING_DBROW_GENERATOR_H

#include <vector>

#include <agrum/agrum.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/DBTranslatedValue.h>

namespace gum {

  namespace learning {

    /** @enum DBRowGeneratorGoal
     * @headerfile DBRowGenerator.h <agrum/learning/database/DBRowGenerator.h>
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
     * @headerfile DBRowGenerator.h <agrum/learning/database/DBRowGenerator.h>
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
     * template <template<typename> class ALLOC = std::allocator>
     * class DuplicateGenerator : public DBRowGenerator<ALLOC> {
     * public:
     *
     *   /// type for the allocators passed in arguments of methods
     *   using allocator_type = ALLOC<DBTranslatedValue>;
     *
     *   // ######################################################################
     *   // Constructors / Destructors
     *   // ######################################################################
     *
     *   /// default constructor
     *   DuplicateGenerator( const std::vector<DBTranslatedValueType,
     *                                  ALLOC<DBTranslatedValueType>> column_types,
     *                       const std::size_t nb_duplicates,
     *                       const allocator_type& alloc  = allocator_type () )
     *     : DBRowGenerator<ALLOC> ( column_types, alloc )
     *     , __nb_duplicates ( nb_duplicates ) {}
     *
     *   /// copy constructor with a given allocator
     *   DuplicateGenerator( const DuplicateGenerator<ALLOC>& from,
     *                       const allocator_type& alloc )
     *     : DBRowGenerator<ALLOC>( from, alloc )
     *     , __input_row( from.__input_row )
     *     , __nb_duplicates ( from.__nb_duplicates ) {}
     *
     *   /// copy constructor
     *   DuplicateGenerator( const DuplicateGenerator<ALLOC>& from )
     *     : DuplicateGenerator<ALLOC> ( from, from.getAllocator () ) {}
     *
     *   /// move constructor with a given allocator
     *   DuplicateGenerator( DuplicateGenerator<ALLOC>&& from,
     *                       const allocator_type& alloc )
     *     : DBRowGenerator<ALLOC> ( std::move( from ), alloc )
     *     , __input_row( from.__input_row )
     *     , __nb_duplicates ( from.__nb_duplicates ) {}
     *
     *   /// move constructor
     *   DuplicateGenerator( DuplicateGenerator<ALLOC>&& from )
     *     : DuplicateGenerator<ALLOC> ( std::move(from), from.getAllocator() ) {}
     *
     *   /// virtual copy constructor with a given allocator
     *   virtual DuplicateGenerator<ALLOC>*
     *   clone ( const allocator_type& alloc ) const {
     *     ALLOC<DuplicateGenerator<ALLOC>> allocator ( alloc );
     *     DuplicateGenerator<ALLOC>* generator = allocator.allocate(1);
     *     try { allocator.construct ( generator, *this, alloc ); }
     *     catch ( ... ) {
     *       allocator.deallocate ( generator, 1 );
     *       throw;
     *     }
     *     return generator;
     *   }
     *
     *
     *   /// virtual copy constructor
     *   virtual DuplicateGenerator<ALLOC>* clone () const {
     *     return clone ( this->getAllocator () );
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
     *   DuplicateGenerator<ALLOC>&
     *   operator=( const DuplicateGenerator<ALLOC>& from ) {
     *     DBRowGenerator<ALLOC>::operator=( from );
     *     __input_row = from.__input_row;
     *     __nb_duplicates = from.__nb_duplicates;
     *     return *this;
     *   }
     *
     *   /// move operator
     *   DuplicateGenerator<ALLOC>& operator=( DuplicateGenerator<ALLOC>&& from ) {
     *     DBRowGenerator<ALLOC>::operator=( std::move( from ) );
     *     __input_row = from.__input_row;
     *     __nb_duplicates = from.__nb_duplicates;
     *     return *this;
     *   }
     *
     *
     *   // ######################################################################
     *   // Accessors / Modifiers
     *   // ######################################################################
     *
     *   /// generates new lines from those the generator gets in input
     *   virtual const DBRow<DBTranslatedValue,ALLOC>& generate() final {
     *     this->decreaseRemainingRows();
     *     return *__input_row;
     *   }
     *
     *
     * protected:
     *
     *   /// computes the rows it will provide in output
     *   virtual std::size_t
     *     _computeRows( const DBRow<DBTranslatedValue,ALLOC>& row ) final {
     *     __input_row = &row;
     *     return __nb_duplicates;
     *   }
     *
     *
     * private:
     *   /// the row used as input to generate the output DBRows
     *   const DBRow<DBTranslatedValue,ALLOC>* __input_row { nullptr };
     *
     *   /// the number of times we return each input row
     *   std::size_t __nb_duplicates { std::size_t(1) };
     * };
     * @endcode
     */
    template < template < typename > class ALLOC = std::allocator >
    class DBRowGenerator {
      public:
      /// type for the allocators passed in arguments of methods
      using allocator_type = ALLOC< DBTranslatedValue >;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      /** @param column_types indicates for each column whether this is a
       * continuous or a discrete one
       * @param alloc the allocator used by all the methods */
      DBRowGenerator(
         const std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > >
                                  column_types,
         const DBRowGeneratorGoal goal,
         const allocator_type&    alloc = allocator_type());

      /// copy constructor
      DBRowGenerator(const DBRowGenerator< ALLOC >& from);

      /// copy constructor with a given allocator
      DBRowGenerator(const DBRowGenerator< ALLOC >& from,
                     const allocator_type&          alloc);

      /// move constructor
      DBRowGenerator(DBRowGenerator< ALLOC >&& from);

      /// move constructor with a given allocator
      DBRowGenerator(DBRowGenerator< ALLOC >&& from, const allocator_type& alloc);

      /// virtual copy constructor
      virtual DBRowGenerator< ALLOC >* clone() const = 0;

      /// virtual copy constructor with a given allocator
      virtual DBRowGenerator< ALLOC >*
         clone(const allocator_type& alloc) const = 0;

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
      bool setInputRow(const DBRow< DBTranslatedValue, ALLOC >& row);

      /// generate new rows from the input row
      virtual const DBRow< DBTranslatedValue, ALLOC >& generate() = 0;

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
      virtual void setColumnsOfInterest(
         const std::vector< std::size_t, ALLOC< std::size_t > >& cols_of_interest);

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
      virtual void setColumnsOfInterest(
         std::vector< std::size_t, ALLOC< std::size_t > >&& cols_of_interest);

      /// returns the current set of columns of interest
      const std::vector< std::size_t, ALLOC< std::size_t > >&
         columnsOfInterest() const;

      /// returns the allocator used
      allocator_type getAllocator() const;

      /// returns the goal of the DBRowGenerator
      DBRowGeneratorGoal goal() const;

      /// @}


      protected:
      /// the number of output rows still to retrieve through the generate method
      std::size_t _nb_remaining_output_rows{std::size_t(0)};

      /// the types of the columns in the DatabaseTable
      /** This is useful to determine whether we need to use the .discr_val
       * field or the .cont_val field in DBTranslatedValue instances. */
      std::vector< DBTranslatedValueType, ALLOC< DBTranslatedValueType > >
         _column_types;

      /// the set of columns of interest
      std::vector< std::size_t, ALLOC< std::size_t > > _columns_of_interest;

      /// the goal of the DBRowGenerator (just remove missing values or not)
      DBRowGeneratorGoal _goal{
         DBRowGeneratorGoal::OTHER_THINGS_THAN_REMOVE_MISSING_VALUES};


      /// copy constructor
      DBRowGenerator< ALLOC >& operator=(const DBRowGenerator< ALLOC >&);

      /// move constructor
      DBRowGenerator< ALLOC >& operator=(DBRowGenerator< ALLOC >&&);

      /** @brief the method that computes the set of DBRow instances to output
       * after method setInputRow has been called */
      virtual std::size_t
         _computeRows(const DBRow< DBTranslatedValue, ALLOC >& row) = 0;
    };

  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/learning/database/DBRowGenerator_tpl.h>

#endif /* GUM_LEARNING_DBROW_GENERATOR_H */
