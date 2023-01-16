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
 * @brief The class representing a tabular database stored in RAM
 *
 * Class DatabaseTable represents a tabular database that stores in the
 * computer's random access memory (RAM) its content as a vector of DBRows
 * of DBTranslatedValue instances.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_DATABASE_TABLE_H
#define GUM_DATABASE_TABLE_H

#include <numeric>
#include <algorithm>
#include <functional>
#include <exception>
#include <vector>
#include <memory>

#include <agrum/agrum.h>
#include <agrum/tools/core/math/math_utils.h>
#include <agrum/tools/database/IDatabaseTable.h>
#include <agrum/tools/database/DBTranslatorSet.h>

namespace gum {

  namespace learning {

    /** @class DatabaseTable
     * @brief The class representing a tabular database as used by learning tasks.
     * @headerfile databaseTable.h <agrum/tools/database/databaseTable.h>
     * @ingroup learning_database
     *
     * Class DatabaseTable represents a tabular database that stores in the
     * computer's random access memory (RAM) its content as a vector of DBRows
     * of DBTranslatedValue instances. This class is very well suited for fast
     * learning algorithms.
     *
     * @par Usage example:
     * @code
     * // create the database from a CSV. This is not compulsory for
     * // DatabaseTable instances, but this is how we usually create
     * // DatabaseTable instances
     * gum::learning::DBInitializerFromCSV<> initializer ( "asia.csv" );
     * const auto& var_names = initializer.variableNames ();
     * gum::learning::DBTranslatorSet<> translator_set;
     * gum::learning::DBTranslator4LabelizedVariable<> translator;
     * for ( std::size_t i = 0; i < var_names.size(); ++i )
     *   translator_set.insertTranslator ( translator, i );
     * gum::learning::DatabaseTable<> database ( translator_set );
     * database.setVariableNames( initializer.variableNames () );
     *
     * // here, database contains the content of the asia.csv file.
     * // determine how many columns and rows the database contains
     * std::size_t nb_rows = database.nbRows();
     * std::size_t nb_cols = database.nbVariables ();
     *
     * // manually add a new row into the database
     * std::vector<std::string> row( 8, "toto" ); // asia has 8 columns
     * database.insertRow ( row );
     * gum::learning::DBRow<gum::learning::DBTranslatedValue>
     *   dbrow ( 8, gum::learning::DBTranslatedValue { std::size_t(0) } );
     * database.insertRow ( dbrow );
     * // insert 4 rows in a single call:
     * database.insertRows(
     *   std::vector<gum::learning::DBRow<gum::learning::DBTranslatedValue>>
     *   ( 4, dbrow ) );
     *
     * // erase some rows
     * database.eraseRow ( 12 );      // erase the 13th row of the database
     * database.eraseFirstRow ();     // erase the first row of the database
     * database.eraseLastRow ();      // erase the last row of the database
     * database.eraseFirstRows ( 2 ); // erase the first two rows
     * database.eraseLastRows ( 3 );  // erase the last three rows
     * database.eraseRows ( 2,4 );    // erase rows indexed from 2 to 4 (excluded)
     *
     * // parse the content of the database, the usual way
     * for ( const auto& dbrow : database )
     *   std::cout << dbrow.row() << "  weight: " << dbrow.weight() << std::endl;
     *
     * // ignore some columns of the database, i.e., remove them
     * database.ignoreColumn ( 3 ); // remove the column X3 of the CSV file
     * // now, the database contains columns 0, 1, 2, 4, 5, 6, 7 of the
     * // CSV file. If we wish to remove Column X5 of the CSV file:
     * database.ignoreColumn ( 5 ); // remove the column X5 of the CSV file
     * // now, the database contains columns 0, 1, 2, 4, 6, 7 of the CSV file.
     * // if we wish to remove the 5th column of the DatabaseTable, i.e.,
     * // column #4 of the CSV, either we determine that this actually correspond
     * // to column X6 of the CSV and we use database.ignoreColumn ( 6 ) or
     * // we call:
     * database.ignoreColumn ( 4, false ); // false => 4 = the 5th column of
     * // the DatabaseTable, not the 5th column/variable of the CSV file
     * // (remember that all column numbers start from 0).
     *
     * // display the columns of the CSV that were ignored and those that
     * // were kept:
     * std::vector<std::size_t> ignored_cols = database.ignoredColumns ();
     * std::vector<std::size_t> kept_cols = database.inputColumns ();
     *
     * // parse the content of the database using handlers
     * typename gum::learning::DatabaseTable<>::HandlerSafe handler( database );
     * typename gum::learning::DatabaseTable<>::Handler uhandler( database );
     * // by default, the handlers range over the whole database
     *
     * // change the range of rows handled by the DBHandler
     * std::cout << handler.setRange ( 1, 40 ); // now parses rows [1,40)
     * std::cout << handler.size ();   // displays 39: rows 1,...,39
     * std::cout << handler.DBSize (); // shows the number of rows in the database
     * std::cout << handler.numRow (); // displays 0: the handler currently
     * // points on the first row of its managed area [1,40)
     *
     * // move the handler to the next row
     * handler.nextRow();
     * std::cout << handler.numRow (); // displays 1: the handler points now
     * // on the second row of its managed area. This corresponds to the third
     * // DBRow of the database since the range of handler is [1,40)
     * ++handler; // move again to the next row
     * std::cout << handler.numRow (); // displays 2
     * handler += 4; // advances the pointer by 4 rows
     * std::cout << handler.numRow (); // displays 6
     *
     * // get the DBRow pointed to by the handler: this is the 7th DBRow
     * // of the database
     * const auto& xrow7 = handler.row ();     // get the DBRow, unsafe version
     * const auto& yrow7 = handler.rowSafe (); // get the DBRow, safe version
     * const std::vector<gum::learning::DBCell>& xrow = xrow7.row ();
     * const double xweight = xrow27.weight ();
     *
     * // another way to access the row
     * const auto& zrow7 = *handler; // get the DBRow, unsafe version
     *
     * // check whether there exist other rows managed by the handler after
     * // the current row
     * bool has_rows = handler.hasRows (); // true: there remains 33 rows
     *
     * // makes the handler point again on the 2nd row of the database
     * handler.reset (); // the handler points to the beginning of its area
     * std::cout << handler.numRow (); // displays 0: the handler currently
     * // points on the first row of its managed area [1,40)
     *
     * // see the variables' names, i.e., the names of the database's columns
     * const auto& vars = handler.variableNames();
     *
     * // parse all the rows managed
     * handler.reset ();
     * for ( auto end = handler.end (); handler != end; ++handler )
     *   std::cout << handler.row ().weight () << std::endl;
     *
     * // another possibility:
     * for ( const auto& row : handler )
     *   std::cout << row.weight () << std::endl;
     *
     *
     * // clear the content of the database and update the database's
     * // handlers
     * database.clear ();
     * @endcode
     */
    class DatabaseTable: public IDatabaseTable< DBTranslatedValue > {
      public:
      /// the type for the vectors used in the DatabaseTable
      template < typename TX_DATA >
      using DBVector = std::vector< TX_DATA >;

      /// a row of the database
      template < typename TX_DATA >
      using Row = DBRow< TX_DATA >;

      /// the type for the matrices stored into the database
      template < typename TX_DATA >
      using Matrix = std::vector< DBRow< TX_DATA > >;

      using MissingValType = std::vector< std::string >;

      /// the unsafe handler type
      using Handler = typename IDatabaseTable< DBTranslatedValue >::Handler;

      /// the safe handler type
      using HandlerSafe = typename IDatabaseTable< DBTranslatedValue >::HandlerSafe;

      using IsMissing = typename IDatabaseTable< DBTranslatedValue >::IsMissing;

      /// Types for STL compliance.
      /// @{
      using value_type      = Row< DBTranslatedValue >;
      using reference       = value_type&;
      using const_reference = const value_type&;
      using pointer         = value_type*;
      using const_pointer   = const value_type*;
      using size_type       = std::size_t;
      using difference_type = std::ptrdiff_t;
      using iterator        = Handler;
      using iterator_safe   = HandlerSafe;
      /// @}


      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      DatabaseTable(const MissingValType&  missing_symbols,
                    const DBTranslatorSet& translators = DBTranslatorSet());

      /// default constructor
      DatabaseTable(const DBTranslatorSet& translators = DBTranslatorSet());

      /// copy constructor
      DatabaseTable(const DatabaseTable& from);

      /// move constructor
      DatabaseTable(DatabaseTable&& from);

      /// virtual copy constructor
      virtual DatabaseTable* clone() const final;

      /// destructor
      virtual ~DatabaseTable();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      DatabaseTable& operator=(const DatabaseTable& from);

      /// move constructor
      DatabaseTable& operator=(DatabaseTable&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// insert a new translator into the database table
      /** @param translator This translator is copied into the DatabaseTable
       * @param input_column indicates which column in the original dataset
       * (usually a CSV file) the translator will read
       * @param unique_column indicates whether the input column can be read by
       * several translators.
       * @return the index of the translator within the set of translators
       * @throws OperationNotAllowed if the input column is marked as ignored
       * @throws DuplicateElement if there already exists a translator
       * reading the input column passed in argument, and if the unique_column
       * is set to true
       * @warning if the database is not empty, i.e., it contains some records,
       * all the column of the database corresponding to the new translator is
       * filled with missing values.
       */
      std::size_t insertTranslator(const DBTranslator& translator,
                                   const std::size_t   input_column,
                                   const bool          unique_column = true);

      /// insert a new translator into the database table
      /** @param var the variable that will be contained into the translator
       * @param input_column indicates which column in the original dataset
       * (usually a CSV file) the translator will read
       * @param unique_column indicates whether the input column can be read by
       * several translators
       * @param missing_symbols the set of symbols in the database
       * representing missing values
       * @return the index of the translator within the set of translators
       * @throws OperationNotAllowed if the input column is marked as ignored
       * @throws DuplicateElement if there already exists a translator
       * reading the input column passed in argument, and if the unique_column
       * is set to true
       * @throws if the database is not empty, i.e., it contains some records,
       * all the columns of the database corresponding to the new translator
       * should be filled with missing values, which is impossible since we do
       * not know which symbols correspond to missing values. Therefore, we
       * raise a MissingValueInDatabase exception. If you do not want such a
       * behavior, use method insertTranslator in which you specify the set of
       * missing symbols.
       */
      std::size_t insertTranslator(const Variable&   var,
                                   const std::size_t input_column,
                                   const bool        unique_column = true);

      /// insert a new translator into the database table
      /** @param var the variable that will be contained into the translator
       * @param input_column indicates which column in the original dataset
       * (usually a CSV file) the translator will read
       * @param unique_column indicates whether the input column can be read by
       * several translators
       * @param missing_symbols the set of symbols in the database
       * representing missing values
       * @return the index of the translator within the set of translators
       * @throws OperationNotAllowed if the input column is marked as ignored
       * @throws DuplicateElement if there already exists a translator
       * reading the input column passed in argument, and if the unique_column
       * is set to true
       * @warning if the database is not empty, i.e., it contains some records,
       * all the column of the database corresponding to the new translator is
       * filled with missing values.
       */
      std::size_t insertTranslator(const Variable&            var,
                                   const std::size_t          input_column,
                                   std::vector< std::string > missing_symbols,
                                   const bool                 unique_column = true);

      /** @brief erases either the kth translator or all those parsing the kth
       * column of the input dataset
       *
       * Translators read an input dataset that is not necessarily the same as
       * the content of the DatabaseTable. For instance, a CSV may contain 10
       * columns, but if a DatabaseTable only contains two translators reading
       * columns 3 and 5 respectively, then the DatabaseTable only contains 2
       * columns. When k_is_input_col is set to false, Parameter k passed in
       * argument corresponds to either 0 or 1, i.e., to the index of one of
       * these two output columns. When k_is_input_col is set to true, the
       * translators to be erased are all those that parse the kth column of the
       * input database.
       * @warning if the translator does not exists, nothing is done. In
       * particular, no exception is raised. */
      void eraseTranslators(const std::size_t k, const bool k_is_input_col = false);

      /// change the translator of a database column
      /**
       * When changing the translator for a column, we update the content of the database.
       * @param new_translator the new translator to use
       * @param k either the column in the DatabaseTable (if k_is_input_col = false, the
       * default) or the first column in the DatabaseTable which corresponds to the kth
       * column of the input CSV (if k_is_input_col = true)
       * @param k_is_input_col see Parameter k
       * @warning if the translator does not exists, nothing is done. In
       * particular, no exception is raised.
       */
      void changeTranslator(DBTranslator&     new_translator,
                            const std::size_t k,
                            const bool        k_is_input_col = false);

      /// change the translator of a database column
      /**
       * When changing the translator for a column, we update the content of the database.
       * @param new_var The variable corresponding to the new translator
       * @param k k either the column in the DatabaseTable (if k_is_input_col = false, the
       * default) or the first column in the DatabaseTable which corresponds to the kth
       * column of the input CSV (if k_is_input_col = true)
       * @param k_is_input_col see Parameter k
       * @param missing_symbols if set, this corresponds to the set of missing symbols
       * used by the translator, otherwise (or if it is empty, this is the set of missing
       * symbols of the translator currently translating the kth column
       * @param editable_dictionary For those translators that can
       * enable/disable the update of their dictionary during the reading of
       * databases (e.g., DBTranslator4LabelizedVariable), this indicates whether
       * we allow or not such updates. For DBTranslator4ContinuousVariable, this
       * corresponds to the fit_range constructor's parameter.
       * @param max_dico_entries For translators that store explicitly their
       * dictionary in memory, this parameter specifies the max number of entries
       * in this dictionary
       * @warning if the translator does not exists, nothing is done. In
       * particular, no exception is raised.
       */
      void changeTranslator(const Variable&                   var,
                            const std::size_t                 k,
                            const bool                        k_is_input_col = false,
                            const std::vector< std::string >& missing_symbols
                            = std::vector< std::string >(),
                            const bool        editable_dictionary = false,
                            const std::size_t max_dico_entries
                            = std::numeric_limits< std::size_t >::max());

      /// returns the set of translators
      const DBTranslatorSet& translatorSet() const;

      /** @brief returns either the kth translator of the database table or the
       * first one reading the kth column of the input database
       *
       * Translators read an input dataset that is not necessarily the same as
       * the content of the DatabaseTable. For instance, a CSV may contain 10
       * columns, but if a DatabaseTable only contains two translators reading
       * columns 3 and 5 respectively, then the DatabaseTable only contains 2
       * columns. When k_is_input_col is set to false, Parameter k passed in
       * argument corresponds to either 0 or 1, i.e., the index of one of these
       * two columns. When k_is_input_col is set to true, the translator returned
       * is the first one that parses the kth column of the input database.
       * @throw UndefinedElement is raised if there is no translator
       * corresponding to k. */
      const DBTranslator& translator(const std::size_t k, const bool k_is_input_col = false) const;

      /// propose a set with translators better suited for the content of the database
      /**
       * @return A vector indicating for each column of the database in which a
       * better translator than the current one can be used, this better
       * translator.
       */
      std::vector< std::pair< Idx, std::shared_ptr< DBTranslator > > > betterTranslators() const;

      /** @brief returns either the kth variable of the database table or the
       * first one corresponding to the kth column of the input database
       *
       * Translators read an input dataset that is not necessarily the same as
       * the content of the DatabaseTable. For instance, a CSV may contain 10
       * columns, but if a DatabaseTable only contains two translators reading
       * columns 3 and 5 respectively, then the DatabaseTable only contains 2
       * columns. When k_is_input_col is set to false, Parameter k passed in
       * argument corresponds to either 0 or 1, i.e., the index of one of these
       * two columns. When k_is_input_col is set to true, the variable is that
       * of the translator that parses the kth column of the input database.
       * @throw UndefinedElement is raised if there is no translator
       * corresponding to k. */
      const Variable& variable(const std::size_t k, const bool k_is_input_col = false) const;

      /// sets the names of the variables
      using IDatabaseTable< DBTranslatedValue >::setVariableNames;

      /// sets the names of the variables
      /** This method can be called in two different ways: either the names
       * correspond precisely to the columns stored into the database table
       * (in this case, parameter from_external_object is equal to false),
       * or they correspond to the columns of an external database (e.g., a
       * CSV file) from which we potentially excluded some columns and,
       * consequently, the latter should not be taken into account (in this
       * case, parameter from_external_object is equal to true). As an
       * example, imagine that the database table is created from a CSV file
       * with 5 columns named X0, X1, X2, X3 and X4 respectively. Suppose that
       * we asked the database table to ignore columns X1 and X3. Then
       * setVariableNames( { "X0", "X1", "X2", "X3", "X4" }, true ) will
       * set the columns of the database table as { "X0", "X2", "X4" }. The
       * same result could be obtained by executing
       * setVariableNames( { "X0", "X2", "X4" }, false ), which specifies
       * directly the set of names to retain in the database table.
       * @param names the names of all the columns, including the ignored
       * columns if from_external_object is set to true, else excluding
       * them (i.e., this should precisely correspond to the columns stored
       * into the database table).
       * @param from_external_object a Boolean indicating whether parameter
       * names includes the columns ignored by the database table (true) or
       * not (false).
       * @throw SizeError is raised if the names passed in arguments cannot be
       * assigned to the columns of the DatabaseTable because the size of their
       * vector is inadequate. */
      virtual void setVariableNames(const std::vector< std::string >& names,
                                    const bool from_external_object = true) final;

      /** @brief makes the database table ignore from now on the kth column of
       * the input dataset or the column parsed by the kth translator
       *
       * This method can be called in two different ways: either k refers to
       * the current kth column of the database table (in this case parameter
       * from_external_object is set to false), or k corresponds to the kth
       * column of an original database used to fill the database table
       * (in this case from_external_object is set to true). Depending on
       * from_external_object's value, the ignored columns may differ. As an
       * example, imagine that the database table is created from a CSV file
       * with 5 columns named X0, X1, X2, X3 and X4 respectivly. Then a call to
       * ignoreColumn ( 1, true ) will exclude column X1 from the database table.
       * As a result, the database table columns are X0, X2, X3 and X4.
       * Therefore, subsequently calling ignoreColumn ( 1, false ) will result
       * in excluding X2 since X2 is the 2nd column (columns are indexed
       * starting from 0). So, now the database table's columns are
       * X0, X3 and X4. If, now, we call ignoreColumn ( 3, true ), this will
       * remove column X3 because, in the original database, X3 was the 4th
       * column.
       *
       * The method also erases all the translators corresponding to column k,
       * if any. If the DatabaseTable contains some rows, then their column
       * corresponding to k is removed. If the resulting DatabaseTable
       * contains only empty rows, then those are removed.
       *
       * @param k the column to remove. See Method setVariableNames for a
       * detailed description on how k is computed.
       * @param from_external_object indicates whether k refers to the kth
       * column of an original external database (true) or to the current kth
       * column of the DatabaseTable (false).
       * @throw UndefinedElement is raised if k refers to the position of a
       * translator that does not exist (k >= number of translators). */
      virtual void ignoreColumn(const std::size_t k, const bool from_external_object = true) final;

      /// returns  the set of columns of the original dataset that are ignored
      /** In this vector, all the column indices greater than or equal to its
       * last element are also ignored. */
      virtual const DBVector< std::size_t > ignoredColumns() const final;

      /** @brief returns the set of columns of the original dataset that are
       * present in the DatabaseTable */
      virtual const DBVector< std::size_t > inputColumns() const final;

      /** @brief returns the domain size of the kth variable of the database
       * table or of that of the first one corresponding to the kth column of
       * the input database
       *
       * Translators read an input dataset that is not necessarily the same as
       * the content of the DatabaseTable. For instance, a CSV may contain 10
       * columns, but if a DatabaseTable only contains two translators reading
       * columns 3 and 5 respectively, then the DatabaseTable only contains 2
       * columns. When k_is_input_col is set to false, Parameter k passed in
       * argument corresponds to either 0 or 1, i.e., the index of one of these
       * two columns. When k_is_input_col is set to true, the variable is that
       * of the translator that parses the kth column of the input database.
       * @throw UndefinedElement is raised if there is no translator
       * corresponding to k. */
      std::size_t domainSize(const std::size_t k, const bool k_is_input_col = false) const;

      /// returns the domain sizes of all the variables in the database table
      DBVector< std::size_t > domainSizes() const;

      /** @brief indicates whether a reordering is needed to sort the translations
       * of the kth translator or those of the first translator parsing the kth
       * column
       *
       * For a given translator, if the strings represented by the translations
       * are only numbers, the translations are considered to be sorted if and
       * only if they are sorted by increasing number. If the strings do not
       * only represent numbers, then translations are considered to be sorted
       * if and only if they are sorted lexicographically.
       *
       * When constructing dynamically its dictionary, the translator may
       * assign wrong DBTranslatedValue values to strings. For instance, a
       * translator reading sequentially integer strings 4, 1, 3, may map
       * 4 into DBTranslatedValue{std::size_t(0)},
       * 1 into DBTranslatedValue{std::size_t(1)} and
       * 3 into DBTranslatedValue{std::size_t(2)}, resulting in random variables
       * having domain {4,1,3}. The user may prefer having domain {1,3,4}, i.e.,
       * a domain specified with increasing values. This requires a
       * reordering. Method needsReodering() returns a Boolean indicating
       * whether such a reordering should be performed or whether the current
       * order is OK.
       *
       * Translators read an input dataset that is not necessarily the same as
       * the content of the DatabaseTable. For instance, a CSV may contain 10
       * columns, but if a DatabaseTable only contains two translators reading
       * columns 3 and 5 respectively, then the DatabaseTable only contains 2
       * columns. When k_is_input_col is set to false, Parameter k passed in
       * argument corresponds to either 0 or 1, i.e., the index of one of these
       * two columns. When k_is_input_col is set to true, the translator to be
       * reordered is that which parses the kth column of the input database.
       * @throw UndefinedElement is raised if there is no translator
       * corresponding to k. */
      bool needsReordering(const std::size_t k, const bool k_is_input_col = false) const;

      /** @brief performs a reordering of the kth translator or
       * of the first translator parsing the kth column of the input database
       *
       * For a given translator, if the strings represented by the translations
       * are only numbers, the translations are considered to be sorted if and
       * only if they are sorted by increasing number. If the strings do not
       * only represent numbers, then translations are considered to be sorted
       * if and only if they are sorted lexicographically.
       *
       * Translators read an input dataset that is not necessarily the same as
       * the content of the DatabaseTable. For instance, a CSV may contain 10
       * columns, but if a DatabaseTable only contains two translators reading
       * columns 3 and 5 respectively, then the DatabaseTable only contains 2
       * columns. When k_is_input_col is set to false, Parameter k passed in
       * argument corresponds to either 0 or 1, i.e., the index of one of these
       * two columns. When k_is_input_col is set to true, the translator to be
       * reordered is that which parses the kth column of the input database.
       * @throw UndefinedElement is raised if there is no translator
       * corresponding to k. */
      void reorder(const std::size_t k, const bool k_is_input_col = false);

      /// performs a reordering of all the columns
      /** For a given translator, if the strings represented by the translations
       * are only numbers, the translations are considered to be sorted if and
       * only if they are sorted by increasing number. If the strings do not
       * only represent numbers, then translations are considered to be sorted
       * if and only if they are sorted lexicographically. */
      void reorder();

      /// insert a new row at the end of the database
      using IDatabaseTable< DBTranslatedValue >::insertRow;

      /// insert a new row at the end of the database
      /** The new_row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns.
       * @throws SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns)
       * @throws UnknownLabelInDatabase is raised if the translation of an
       * element in the new row cannot be found and the corresponding translator
       * is not in an editable dictionary mode.
       * @throws SizeError is raised if the number of entries in the dictionary
       * of a translator has already reached its maximum.
       * @throws OperationNotAllowed exception is raised if the translation of
       * an element in new_row cannot be found and the insertion of the string
       * into the corresponding translator's dictionary fails because it would
       * induce incoherent behavior (e.g., a DBTranslator4ContinuousVariable
       * that contains a variable whose domain is [x,y] as well as a missing
       * value symbol z \f$\in\f$ [x,y]).
       * @throws TypeError is raised if the translation of an element in new_row
       * cannot be found and the insertion of the string into the translator's
       * dictionary fails due to str being impossible to be converted into an
       * appropriate type.
       */
      virtual void insertRow(const std::vector< std::string >& new_row) final;

      /// insert a new DBRow at the end of the database
      /** Unlike methods insertRow for data whose type is different from
       * DBTranslatedValue, this method assumes that the new row passed in
       * argument does not contain any data of the ignored columns. So,
       * basically, it could be copied as is into the database table.
       * @throw SizeError is raised if the size of the new_row is not equal to
       * the number of translators of the DatabaseTable
       * @InvalidArgument is raised if at least one element of new_row does
       * not belong to the domain of its corresponding translator.
       */
      virtual void insertRow(Row< DBTranslatedValue >&& new_row,
                             const IsMissing            contains_missing_data) final;

      /// insert a new row at the end of the database
      /** Unlike methods insertRow for data whose type is different from
       * DBTranslatedValue, this method assumes that the new row passed in
       * argument does not contain any data of the ignored columns. So,
       * basically, it could be copied as is into the database table.
       * @throw SizeError is raised if the size of the new_row is not equal to
       * the number of translators of the DatabaseTable
       * @InvalidArgument is raised if at least one element of new_row does
       * not belong to the domain of its corresponding translator.
       */
      virtual void insertRow(const Row< DBTranslatedValue >& new_row,
                             const IsMissing                 contains_missing_data) final;

      /// insert a new DBRow of DBCells at the end of the database
      /** The new_row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns) */
      virtual void insertRow(const Row< DBCell >& new_row) final;

      /// insert a new DBRow of DBCells at the end of the database
      /** The new_row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns) */
      virtual void insertRow(Row< DBCell >&& new_row) final;

      /// insert a set of new DBRows at the end of the database
      /** Unlike methods insertRows for data whose type is different from
       * DBTranslatedValue, this method assumes that the new rows passed in
       * argument do not contain any data of the ignored columns. So, basically,
       * these rows could be copied as is into the database table.
       * @throw SizeError is raised if the size of at least one row in new_rows
       * is not equal to the number of translators in the DatabaseTable
       * @InvalidArgument is raised if at least one element of new_row does
       * not belong to the domain of its corresponding translator.
       */
      virtual void insertRows(Matrix< DBTranslatedValue >&& new_rows,
                              const DBVector< IsMissing >&  rows_have_missing_vals) final;

      /// insert a set of new DBRows at the end of the database
      /** Unlike methods insertRows for data whose type is different from
       * DBTranslatedValue, this method assumes that the new rows passed in
       * argument do not contain any data of the ignored columns. So, basically,
       * these rows could be copied as is into the database table.
       * @throw SizeError is raised if the size of at least one row in new_rows
       * is not equal to the number of translators in the DatabaseTable
       * @InvalidArgument is raised if at least one element of new_row does
       * not belong to the domain of its corresponding translator.*/
      virtual void insertRows(const Matrix< DBTranslatedValue >& new_rows,
                              const DBVector< IsMissing >&       rows_have_missing_vals) final;

      /// insert a set of new DBRows at the end of the database
      /** The new rows passed in argument are supposed to come from an external
       * database. So they must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns) */
      virtual void insertRows(Matrix< DBCell >&& new_rows) final;

      /// insert a set of new DBRows at the end of the database
      /** The new rows passed in argument are supposed to come from an external
       * database. So they must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns) */
      virtual void insertRows(const Matrix< DBCell >& new_rows) final;

      /// erase the content of the database, including the names of the variables
      virtual void clear() final;

      // substitutes the kth translator by another one
      /* The method checks that:
       * 1/ it is possible to get back the original values of the database
       *    for the rows already translated.
       * 2/ that the new translator is capable of translating these values.
       *
       * If both checks passed, then it replaces the kth translator
       * by the one passed in arguments and retranslates with it the kth
       * cell of all the rows already contained in the database */

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      /// the set of translators used to convert the strings into floats
      DBTranslatorSet _translators_;

      /// the set of ignored columns asked by the user
      Set< std::size_t > _ignored_cols_;

      /** @brief check that a row's values are compatible with those of the
       * translators' variables */
      bool _isRowCompatible_(const Row< DBTranslatedValue >& row) const;

      /** @brief returns the index corresponding either to the kth translator or
       * to the first one that parses the kth column of the input dataset
       *
       * @warning if the translator does not exists, the function returns an
       * index which is greater than the number of translators */
      std::size_t _getKthIndex_(const std::size_t k, const bool k_is_input_col) const;

      /** @brief returns the indices corresponding either to the kth translator
       * or to all those that parse the kth column of the input dataset
       *
       * @warning the indices are sorted by deacreasing order */
      DBVector< std::size_t > _getKthIndices_(const std::size_t k, const bool k_is_input_col) const;

      /// a method to process the rows of the database in multithreading
      /** The function tries to execute function/functor exec_func using one
       * or several threads. If an exception is raised by at least one thread,
       * then function undo_func is executed to undo what exec_func
       * did, and the exception is rethrown.
       *
       * @param exec_func this should be a function/functor/lambda that
       * takes 3 arguments: the first one is an std::size_t containing the
       * index of the first row that it should process, the second argument is
       * an std::size_t equal to 1 + the index of the last row processed, so that
       * the processing is performed on [first,last). The last argument is an index of
       * identifying the thread in which exec_func is performed (the first one has index 0,
       * the second one Index 1, and so on. The return type of exec_func
       * is a void. If a thread executing exec_func raises an exception, then
       * before exiting, it should undo what it did.
       * @param undo_func a Function/functor/lambda with the same
       * prototype as exec_func. If a thread raises an exception, those that
       * did not raise exceptions should undo what they did in order to restore
       * the state that the database had before the execution of the thread. After
       * calling undo_func, they should have restored this state.
       */
      template < typename Functor1, typename Functor2 >
      void _threadProcessDatabase_(Functor1& exec_func, Functor2& undo_func);

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated implementations
#include <agrum/tools/database/databaseTable_tpl.h>

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/databaseTable_inl.h>
#endif /* GUM_NO_INLINE */


#endif /* GUM_DATABASE_TABLE_H */
