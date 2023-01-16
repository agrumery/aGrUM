/***************************************************************************
 *   Copyright (c) 2005-2023 by Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)  *
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
 * @brief The class for parsing DatabaseTable rows and generating output rows
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_ROW_GENERATOR_PARSER_H
#define GUM_LEARNING_DB_ROW_GENERATOR_PARSER_H

#include <limits>

#include <agrum/agrum.h>
#include <agrum/tools/database/databaseTable.h>
#include <agrum/tools/database/DBRowGeneratorSet.h>

namespace gum {

  namespace learning {

    /** @class DBRowGeneratorParser
     * @headerfile DBRowGeneratorParser.h <agrum/tools/database/DBRowGeneratorParser.h>
     * @ingroup learning_database
     * @brief the class used to read a row in the database and to transform it
     * into a set of DBRow instances that can be used for learning.
     *
     * A DBRowGeneratorParser contains a handler on a DatabaseTable that enables
     * it to parse DBRows contained in the DatabaseTable. It also contains a
     * DBRowGeneratorSet that is used to create output rows for each parsed
     * DBRow. Note that if the DBRowGeneratorSet is empty, then
     * DBRowGeneratorParser simply outputs each parsed DBRow without additional
     * processing. To understand the difference between a DBRowGeneratorParser
     * and a DBRowGeneratorSet, the latter is designed to take as input only
     * one DBRow instance and to produce some output DBRow instances, whereas
     * the former is designed to parse the content of a DatabaseTable and to
     * produce from them some output DBRow instances.
     *
     * @par Usage example:
     * @code
     * // create and fill a database
     * gum::learning::DatabaseTable<> database ( ... );
     * .....
     *
     * // create a vector with the types of the columns of database
     * const std::vector<gum::learning::DBTranslatedValueType>
     *   col_types ( 10, gum::learning::DBTranslatedValueType::DISCRETE );
     *
     * // create a generator set
     * gum::learning::MyGenerator<>  generator1 ( col_types, 6 );
     * gum::learning::MyGenerator2<> generator2 ( col_types, 4 );
     * gum::learning::DBRowGeneratorSet<> genset;
     * genset.insertGenerator ( generator1 );
     * genset.insertGenerator ( generator2 );
     *
     * // create the DBRowGeneratorParser
     * gum::learning::DBRowGeneratorParser<>
     *   parser ( database.handler (), genset );
     *
     * // use the parser to parse all the database and to apply all the
     * // transformations induced by generator1 and generator2
     * while ( parser.hasRows () ) {
     *   const auto& dbrow = parser.row();
     *   // do something with dbrow
     * }
     * @endcode
     */
    class DBRowGeneratorParser {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowGeneratorParser(const typename DatabaseTable::Handler& handler,
                           const DBRowGeneratorSet&               generator_set);

      /// copy constructor
      DBRowGeneratorParser(const DBRowGeneratorParser& from);

      /// move constructor
      DBRowGeneratorParser(DBRowGeneratorParser&& filter);

      /// virtual copy constructor
      virtual DBRowGeneratorParser* clone() const;

      /// destructor
      virtual ~DBRowGeneratorParser();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowGeneratorParser& operator=(const DBRowGeneratorParser& from);

      /// move operator
      DBRowGeneratorParser& operator=(DBRowGeneratorParser&& from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /** @brief returns true if there are still rows that can be output by the
       * DBRowGeneratorParser
       *
       * The usual way of calling this method is to encapsulate it into a while
       * loop whose stopping condition is when the handler has no more rows.
       * This loop shall be inside a try-catch statement that enables to
       * stop properly the loop when the NotFound exception is raised. In most
       * practical cases, this exception will never be raised, but if you use
       * a row generator that enables to return 0 row (say, for instance an
       * intelligent EM that does not return any row when there are too many
       * missing data) and if the last rows of the database are such that this
       * generator will return no row, then the exception will be raised.
       * Actually, it is not efficient to parse all the database to detect such
       * a case before trying to return the rows, especially because this
       * situation is very unlikely to occur. So a correct code to use method
       * row () is like:
       * @code
       * try {
       *   while ( parser.hasRows () ) {
       *     const auto& row = parser.row ();
       *     do_whatever_you_want_with_the_row... ;
       *   }
       * }
       * catch ( NotFound& ) { // stop, there are no more rows to process }
       * @endcode
       */
      bool hasRows();

      /// returns a new output row with its corresponding weight
      /** The usual way of calling this method is to encapsulate it into a while
       * loop whose stopping condition is when the handler has no more rows.
       * This loop shall be inside a try-catch statement that enables to
       * stop properly the loop when the NotFound exception is raised. In most
       * practical cases, this exception will never be raised, but if you use
       * a row generator that enables to return 0 row (say, for instance an
       * intelligent EM that does not return any row when there are too many
       * missing data) and if the last rows of the database are such that this
       * generator will return no row, then the exception will be raised.
       * Actually, it is not efficient to parse all the database to detect such
       * a case before trying to return the rows, especially because this
       * situation is very unlikely to occur. So a correct code to use method
       * row () is like:
       * @code
       * try {
       *   while ( parser.hasRows () ) {
       *     const auto& row = parser.row ();
       *     do_whatever_you_want_with_the_row... ;
       *   }
       * }
       * catch ( NotFound& ) { // stop, there are no more rows to process }
       * @endcode
       */
      const DBRow< DBTranslatedValue >& row();

      /// resets the parser
      void reset();

      /// returns the handler used by the parser
      typename DatabaseTable::Handler& handler();

      /// returns the handler used by the parser
      const typename DatabaseTable::Handler& handler() const;

      /// returns a reference on the database
      const DatabaseTable& database() const;

      /// returns the generator set that is actually used
      DBRowGeneratorSet& generatorSet();

      /// returns the generator set that is actually used
      const DBRowGeneratorSet& generatorSet() const;

      /// sets the area in the database the handler will handle
      /** In addition to setting the area that will be parsed by the handler,
       * this method makes the handler point to the beginning of the area.
       * @param begin the first row to be handled
       * @param end the handler handles rows in interval [begin,end). Thus,
       * the endth row is not included in the set of rows handled.
       * @warning if begin is greater than end, these values are swapped.
       * @throw NullElement is raised if the handler does not point to
       * any database
       * @throw SizeError is raised if end is greater than the number of
       * rows of the database */
      void setRange(std::size_t begin, std::size_t end);

      /** @brief sets the columns of interest: the output DBRow needs only
       * contain values fot these columns
       *
       * This method is useful, e.g., for EM-like algorithms that need to know
       * which unobserved variables/values need be filled.
       *
       * @throw OperationNotAllowed is raised if the generator set has already
       * started generating output rows and is currently in a state where the
       * generation is not completed yet (i.e., we still need to call the
       * generate() method to complete it). */
      void setColumnsOfInterest(const std::vector< std::size_t >& cols_of_interest);

      /** @brief sets the columns of interest: the output DBRow needs only
       * contain values fot these columns
       *
       * This method is useful, e.g., for EM-like algorithms that need to know
       * which unobserved variables/values need be filled.
       *
       * @throw OperationNotAllowed is raised if the generator set has already
       * started generating output rows and is currently in a state where the
       * generation is not completed yet (i.e., we still need to call the
       * generate() method to complete it). */
      void setColumnsOfInterest(std::vector< std::size_t >&& cols_of_interest);

      /// assign a new Bayes net to all the generators that depend on a BN
      /** Typically, generators based on EM or K-means depend on a model to
       * compute correctly their outputs. Method setBayesNet enables to
       * update their BN model.
       * @warning if one generator that relies on Bayes nets cannot be assigned
       * new_bn, then no generator is updated and an exception is raised. */
      template < typename GUM_SCALAR >
      void setBayesNet(const BayesNet< GUM_SCALAR >& new_bn);

      /// @}


      private:
      /// the handler that is really used to parse the database
      typename DatabaseTable::Handler _handler_;

      /// the set of DBRow generators (might be empty)
      DBRowGeneratorSet _generator_set_;

      /// the size of the generator set
      std::size_t _generator_size_;
    };

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/tools/database/DBRowGeneratorParser_tpl.h>

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/tools/database/DBRowGeneratorParser_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_ROW_GENERATOR_PARSER_H */

