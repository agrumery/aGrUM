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
 * @brief The class representing a tabular database stored in RAM
 *
 * Class DatabaseTable represents a tabular database that stores in the
 * computer's random access memory (RAM) its content as a vector of DBRows
 * of DBTranslatedValue instances.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_DATABASE_TABLE_H
#define GUM_DATABASE_TABLE_H

#include <cmath>

#include <agrum/agrum.h>
#include <agrum/core/set.h>
#include <agrum/learning/database/DBCell.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/IDatabaseTable.h>
#include <agrum/learning/database/DBTranslatorSet.h>
#include <agrum/learning/database/DBTranslator4ContinuousVariable.h>

namespace gum {

  namespace learning {

    /** @class DatabaseTable
     * @brief The class representing a tabular database as used by learning tasks.
     * @headerfile databaseTable.h <agrum/learning/database/databaseTable.h>
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
     * std::size_t nb_rows = database.content().size()
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
     * database.ignoreColumn ( 3 ); // remove the 4th column of the CSV file
     * // now, the database contains columns 0, 1, 2, 4, 5, 6, 7 of the
     * // CSV file. If we wish to remove Column 5 of the CSV file:
     * database.ignoreColumn ( 3 ); // remove the 4th column of the CSV file
     * // now, the database contains columns 0, 1, 2, 4, 6, 7 of the CSV file.
     * // if we wish to remove the 3rd column of the DatabaseTable, i.e.,
     * // column 2 of the CSV, either we determine that this actually correspond
     * // to column 2 of the CSV and we use database.ignoreColumn ( 2 ) or
     * // we call:
     * database.ignoreColumn ( 2, false ); // false => 2 = the 3rd column of
     * // the DatabaseTable, not the 3rd column of the CSV file
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
    template <template<typename> class ALLOC = std::allocator>
    class DatabaseTable : public IDatabaseTable<DBTranslatedValue,ALLOC> {
      public:

      /// the type for the vectors used in the DatabaseTable
      template <typename TX_DATA>
      using DBVector = std::vector<TX_DATA,ALLOC<TX_DATA>>;

      /// a row of the database
      template<typename TX_DATA>
      using Row = DBRow<TX_DATA,ALLOC>;

      /// the type for the matrices stored into the database
      template<typename TX_DATA>
      using Matrix = DBVector<Row<TX_DATA>>;

      template <template<typename> class XALLOC>
      using MissingValType = std::vector<std::string,XALLOC<std::string>>;

      /// the unsafe handler type
      using Handler =
        typename IDatabaseTable<DBTranslatedValue,ALLOC>::Handler;

      /// the safe handler type
      using HandlerSafe =
        typename IDatabaseTable<DBTranslatedValue,ALLOC>::HandlerSafe;
      
      /// Types for STL compliance.
      /// @{
      using value_type = Row<DBTranslatedValue>;
      using reference = value_type&;
      using const_reference = const value_type&;
      using pointer = value_type*;
      using const_pointer = const value_type*;
      using size_type = std::size_t;
      using difference_type = std::ptrdiff_t;
      using iterator = Handler;
      using iterator_safe = HandlerSafe;
      using allocator_type = ALLOC<DBTranslatedValue>;
      /// @}


      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################
      /// @{

      /// default constructor
      template <template<typename> class XALLOC = ALLOC>
      DatabaseTable( const DBTranslatorSet<ALLOC>& translators =
                     DBTranslatorSet<ALLOC> (),
                     const MissingValType<XALLOC>& missing_symbols =
                     MissingValType<XALLOC> (),
                     const allocator_type& alloc = allocator_type () );

      /// copy constructor
      DatabaseTable( const DatabaseTable<ALLOC>& from );

      /// copy constructor with a given allocator
      DatabaseTable( const DatabaseTable<ALLOC>& from,
                     const allocator_type& alloc );

      /// move constructor
      DatabaseTable( DatabaseTable<ALLOC>&& from );

      /// move constructor with a given allocator
      DatabaseTable( DatabaseTable<ALLOC>&& from,
                     const allocator_type& alloc );

      /// virtual copy constructor
      virtual DatabaseTable<ALLOC>* clone () const final;

      /// virtual copy constructor with a given allocator
      virtual DatabaseTable<ALLOC>*
      clone ( const allocator_type& alloc ) const final;

      /// destructor
      virtual ~DatabaseTable ();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################
      /// @{

      /// copy operator
      DatabaseTable<ALLOC>&
      operator=( const DatabaseTable<ALLOC>& from );

      /// move constructor
      DatabaseTable<ALLOC>& operator=( DatabaseTable<ALLOC>&& from );

      /// @}

      
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################
      /// @{

      /// insert a new translator into the database table
      /** @param translator This translator is copied into the DatabaseTable
       * @param input_column indicates which column in the original dataset
       * (usually a CSV file) the translator will read
       * @throws OperationNotAllowed if the databaseTable is not empty,
       * i.e., it already contains some rows, or if the column is marked
       * as being ignored (through method ignoreColumn).
       * @warning if there already exists a translator reading the input
       * column passed in argument, the method does nothing. In particular,
       * it raises no exception.
       * @throw DuplicateElement is raised if there already exists a translator
       * reading the column passed in argument. */
      void insertTranslator ( const DBTranslator<ALLOC>& translator,
                              const std::size_t input_column );

      /// insert a new translator into the database table
      /** The first template parameter (GUM_SCALAR) is necessary only for
       * inserting variables of true types DiscretizedVariable and
       * ContinuousVariable, which depend on the GUM_SCALAR parameter type.
       * However, usually, when you use this function, this is to add into
       * the TranslatorSet the variables of a BayesNet<GUM_SCALAR>. As such, you
       * can safely call insert all the variables of this Bayesian network
       * using inertTranslator<GUM_SCALAR> ( bn.variable() ... ) instructions.
       * @param var the variable that will be contained into the translator
       * @param input_column the index of the column that this new translator
       * should read in the database.
       * @param  missing_symbols the set of symbols in the database
       * representing missing values
       * @warning if there already exists a translator reading the input
       * column passed in argument, the method does nothing. In particular,
       * it raises no exception.
       * @throws OperationNotAllowed if the databaseTable is not empty,
       * i.e., it already contains some rows, or if the column is marked
       * as being ignored (through method ignoreColumn).
       * @throw DuplicateElement is raised if there already exists a translator
       * reading the column passed in argument.
       */
      template <typename GUM_SCALAR = float,
                template<typename> class XALLOC = ALLOC>
      void insertTranslator(
        const Variable& var,
        const std::size_t input_column,
        std::vector<std::string,XALLOC<std::string>> missing_symbols =
        std::vector<std::string,XALLOC<std::string>> () );

      /** @brief erases either the kth translator or that parsing the kth
       * column of the input dataset
       *
       * Translators read an input dataset that is not necessarily the same as
       * the content of the DatabaseTable. For instance, a CSV may contain 10
       * columns, but if a DatabaseTable only contains two translators reading
       * columns 3 and 5 respectively, then the DatabaseTable only contains 2
       * columns. When k_is_input_col is set to false, Parameter k passed in
       * argument corresponds to either 0 or 1, i.e., to the index of one of
       * these two output columns. When k_is_input_col is set to true, the
       * translator to be erased is the one that parses the kth column of the
       * input database.
       * @warning if the translator does not exists, nothing is done. In
       * particular, no exception is raised. */
      void eraseTranslator ( const std::size_t k,
                             const bool k_is_input_col = false );

      /// returns the set of translators
      const DBTranslatorSet<ALLOC>& translatorSet () const;

      /** @brief returns either the kth translator of the database table or that
       * reading the kth column of the input database
       *
       * Translators read an input dataset that is not necessarily the same as
       * the content of the DatabaseTable. For instance, a CSV may contain 10
       * columns, but if a DatabaseTable only contains two translators reading
       * columns 3 and 5 respectively, then the DatabaseTable only contains 2
       * columns. When k_is_input_col is set to false, Parameter k passed in
       * argument corresponds to either 0 or 1, i.e., the index of one of these
       * two columns. When k_is_input_col is set to true, the translator returned
       * is that which parses the kth column of the input database.
       * @throw UndefinedElement is raised if there is no translator
       * corresponding to k. */
      const DBTranslator<ALLOC>&
      translator ( const std::size_t k,
                   const bool k_is_input_col = false ) const;
      
      /** @brief returns either the kth variable of the database table or that
       * corresponding to the kth column of the input database
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
      const Variable& variable ( const std::size_t k,
                                 const bool k_is_input_col = false ) const;
      
      /// sets the names of the variables
      using IDatabaseTable<DBTranslatedValue,ALLOC>::setVariableNames;
      
      /// sets the names of the variables
      /** This method can be called in two different ways: either the names
       * correspond precisely to the columns stored into the database table
       * (in this case, parameter from_external_object is equal to false),
       * or they corresponds to the columns of an external database (e.g., a
       * CSV file) from which we potentially excluded some columns and,
       * consequently, these columns should not be taken into account (in this
       * case, parameter from_external_object is equal to true). As an
       * example, imagine that the database table is created from a CSV file
       * with 5 columns named X0, X1, X2, X3 and X4 respectivly. Suppose that
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
      virtual void
      setVariableNames( const std::vector<std::string,ALLOC<std::string>>& names,
                        const bool from_external_object = true ) final;

      /// makes the database table ignore from now on the kth column
      /** This method can be called in two different ways: either k refers to
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
       * The method also erases the translator assigned to column k, if any.
       * If the DatabaseTable contains some rows, then their column
       * corresponding to k is removed. If the resulting DatabaseTable
       * contains only empty rows, then those are removed.
       * 
       * @param k the column to remove. See Method setVariableNames for a
       * detailed description on how k is computed.
       * @param from_external_object indicates whether k refers to the kth
       * column of an original external database (true) or to the current kth
       * column of the DatabaseTable (false). */
      virtual void ignoreColumn ( const std::size_t k,
                                  const bool from_external_object = true ) final;

      /// returns  the set of columns of the original dataset that are ignored
      /** In this vector, all the column indices higher than its last element
       * are also ignored. */
      virtual const DBVector<std::size_t> ignoredColumns () const final;

      /** @brief returns the set of columns of the original dataset that are
       * present in the DatabaseTable */
      virtual const DBVector<std::size_t> inputColumns () const final;

      /** @brief returns the domain size of the kth variable of the database
       * table or of that corresponding to the kth column of the input database
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
      std::size_t domainSize ( const std::size_t k,
                               const bool k_is_input_col = false ) const;

      /// returns the domain sizes of all the variables in the database table
      DBVector<std::size_t> domainSizes () const;
      
      /// insert a new row at the end of the database
      using IDatabaseTable<DBTranslatedValue,ALLOC>::insertRow;

      /// insert a new row at the end of the database
      /** The new_row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns) */
      virtual void insertRow (
           const std::vector<std::string,ALLOC<std::string>>& new_row ) final;

      /// insert a new DBRow at the end of the database
      /** Unlike methods insertRow for data whose type is different from
       * DBTranslatedValue, this method assumes that the new row passed in
       * argument does not contain any data of the ignored columns. So,
       * basically, it could be copied as is into the database table.
       * @throw SizeError is raised if the size of the new_row is not equal to
       * the number of translators of the DatabaseTable  */
      virtual void insertRow( Row<DBTranslatedValue>&& new_row ) final;

      /// insert a new row at the end of the database
      /** Unlike methods insertRow for data whose type is different from
       * DBTranslatedValue, this method assumes that the new row passed in
       * argument does not contain any data of the ignored columns. So,
       * basically, it could be copied as is into the database table.
       * @throw SizeError is raised if the size of the new_row is not equal to
       * the number of translators of the DatabaseTable  */
      virtual void insertRow( const Row<DBTranslatedValue>& new_row ) final;

      /// insert a new DBRow of DBCells at the end of the database
      /** The new_row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns) */
      virtual void insertRow ( const Row<DBCell>& new_row ) final;

      /// insert a new DBRow of DBCells at the end of the database
      /** The new_row passed in argument is supposed to come from an external
       * database. So it must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns) */
      virtual void insertRow ( Row<DBCell>&& new_row ) final;

      /// insert a set of new DBRows at the end of the database
      /** Unlike methods insertRows for data whose type is different from
       * DBTranslatedValue, this method assumes that the new rows passed in
       * argument do not contain any data of the ignored columns. So, basically,
       * these rows could be copied as is into the database table.
       * @throw SizeError is raised if the size of at least one row in new_rows
       * is not equal to the number of translators in the DatabaseTable  */
      virtual void insertRows( Matrix<DBTranslatedValue>&& new_rows ) final;

      /// insert a set of new DBRows at the end of the database
      /** Unlike methods insertRows for data whose type is different from
       * DBTranslatedValue, this method assumes that the new rows passed in
       * argument do not contain any data of the ignored columns. So, basically,
       * these rows could be copied as is into the database table.
       * @throw SizeError is raised if the size of at least one row in new_rows
       * is not equal to the number of translators in the DatabaseTable  */
      virtual void insertRows( const Matrix<DBTranslatedValue>& new_rows ) final;

      /// insert a set of new DBRows at the end of the database
      /** The new rows passed in argument are supposed to come from an external
       * database. So they must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns) */
      virtual void insertRows( Matrix<DBCell>&& new_rows ) final;

      /// insert a set of new DBRows at the end of the database
      /** The new rows passed in argument are supposed to come from an external
       * database. So they must contain data for the ignored columns.
       * @throw SizeError is raised if the vector of string cannot be inserted
       * in the DatabaseTable because its size does not allow a matching with
       * the columns of the DatabaseTable (taking into account the ignored
       * columns) */
      virtual void insertRows( const Matrix<DBCell>& new_rows ) final;

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
      DBTranslatorSet<ALLOC> __translators;

      /// the set of ignored columns asked by the user
      Set<std::size_t,ALLOC<std::size_t>> __ignored_cols;

      /** @brief check that a row's values are compatible with those of the
       * translators' variables */
      bool __isRowCompatible ( const Row<DBTranslatedValue>& row ) const;

      /** @brief returns the index corresponding either to the kth translator or
       * to that of the translator parsing the kth column of the input dataset
       *
       * @warning if the translator does not exists, the function returns an
       * index which is greater than the number of translators */
      std::size_t __getKthIndex ( const std::size_t k,
                                  const bool k_is_input_col ) const;
      
#endif /* DOXYGEN_SHOULD_SKIP_THIS */
      
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the templated implementations
#include <agrum/learning/database/databaseTable_tpl.h>

#endif /* GUM_DATABASE_TABLE_H */
