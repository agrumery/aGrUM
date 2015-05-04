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
 * @brief The class representing rows in tabular database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_ROW_FILTER_H
#define GUM_LEARNING_DB_ROW_FILTER_H

#include <limits>

#include <agrum/config.h>
#include <agrum/learning/database/DBHandler.h>
#include <agrum/learning/database/DBRowTranslatorSetStatic.h>
#include <agrum/learning/database/DBRowTranslatorSetDynamic.h>
#include <agrum/learning/database/filteredRowGeneratorSet.h>

namespace gum {

  namespace learning {

    /** @class DBRowFilter
     * @ingroup learning_group
     * @brief the class used to read a row in the database and to transform it
     * into a set of FilteredRows that can be used for learning.
     *
     * A DBRowFilter contains a handler on the database that will actually
     * interact with it to read DBRows, a set of cell translators that will
     * transform the DBRows into FilteredRows, and a set of generators that will
     * transform the filtered rows into new sets of filtered rows (see
     * FilteredRowGenerator for more details about generators). An example of the
     * construction of a row filter can be as follows (using helper function
     * make_DB_row_filter that produces a DBRowFilter):
     * @code
     * auto translators = make_translators
     *   ( Create<CellTranslator1, Col<0> > (),
     *     Create<CellTranslator1, Col<1>, 4 > () );
     *
     * auto generators =  make_generators
     *   ( EMGenerator (),
     *     TimeSlicerGenerator () );
     *
     * auto filter = make_DB_row_filter ( database, translators, generators );
     * @endcode
     */
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    class DBRowFilter {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowFilter(const DBHandler &handler, const TranslatorSet &translator_set,
                  const GeneratorSet &generator_set,
                  unsigned long initialization_range =
                      std::numeric_limits<unsigned long>::max()) noexcept;

      /// copy constructor
      DBRowFilter(const DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &
                      filter) noexcept;

      /// move constructor
      DBRowFilter(
          DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &&filter) noexcept;

      /// destructor
      virtual ~DBRowFilter() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &
      operator=(const DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &);

      /// move operator
      DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &
      operator=(DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns true if there are still rows that can be output by the RowFilter
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
       *   while ( handler.hasRows () ) {
       *     FilteredRow& row = row_filter.row ();
       *     do_whatever_you_want_with_the_row... ;
       *   }
       * }
       * catch ( NotFound& ) { // stop, there are no more rows to process }
       * @encode
       */
      bool hasRows() noexcept;

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
       *   while ( handler.hasRows () ) {
       *     FilteredRow& row = row_filter.row ();
       *     do_whatever_you_want_with_the_row... ;
       *   }
       * }
       * catch ( NotFound& ) { // stop, there are no more rows to process }
       * @encode
       */
      FilteredRow &row();

      /// resets the filter
      virtual void reset();

      /// returns the handler used by the filter
      DBHandler &handler() noexcept;

      /** @brief returns the number of modalities of the variables, as stored
       * into the cell filters */
      std::vector<unsigned int> modalities() const;

      /// returns the names of the variables
      const std::vector<std::string> &variableNames() const noexcept;

      /// returns the number of variables
      unsigned int nbVariables() const noexcept;

      /// returns the translator set that is actually used
      const TranslatorSet &translatorSet() const noexcept;

      /// returns the generator set that is actually used
      const GeneratorSet &generatorSet() const noexcept;

      /// @}

      protected:
      /// the handler that is really used to parse the database
      DBHandler __handler;

      /// the DBRowTranslatorSet used to translate the database cells
      TranslatorSet __translator_set;

      /// the generator of FilteredRows
      GeneratorSet __generator_set;

      /// initialize the cell filters by parsing once the database
      /** @param db_range the number of rows to parse in the database to
       * initialize the cell filters. If db_range is larger than the size of the
       * database, then the whole database is parsed. */
      void __initCellFilters(unsigned long db_range);
    };

    /// a helper used to easily create a DBRowFilter
    /** The following code shows how to use this function:
     * @code
     * auto translators = make_translators
     *   ( Create<CellTranslator1, Col<0> > (),
     *     Create<CellTranslator1, Col<1>, 4 > () );
     *
     * auto generators =  make_generators
     *   ( EMGenerator (),
     *     TimeSlicerGenerator () );
     *
     * // create a filter that will parse the DB to initialize the cell filters
     * auto filter = make_DB_row_filter( database, translators, generators );
     *
     * // create a filter that will not initialize the cell filters
     * auto filter = make_DB_row_filter( database, translators, generators, 0 );
     * @endcode
     */
    template <typename Database, typename TranslatorSet, typename GeneratorSet>
    constexpr DBRowFilter<typename Database::Handler, TranslatorSet, GeneratorSet>
    make_DB_row_filter(const Database &database, const TranslatorSet &translator_set,
                       const GeneratorSet &generator_set,
                       unsigned long initialization_range =
                           std::numeric_limits<unsigned long>::max()) {
      return DBRowFilter<typename Database::Handler, TranslatorSet, GeneratorSet>(
          database.handler(), translator_set, generator_set, initialization_range);
    }

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/learning/database/DBRowFilter.tcc>

#endif /* GUM_LEARNING_DB_ROW_FILTER_H */
