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
 * @brief The helpers used to create sets of translators in row filters.
 *
 * The row filters used to transform database rows (DBRows) into FilteredRows
 * use translators to convert each database row into a FilteredRow and, then,
 * they use generators to convert the FilteredRow into one or several other
 * FilteredRows (for instance, EM generates several lines). To make it easy for
 * the user to pack CellTranslators into the row filter, this file contains a
 * helper called Create. Combined with helper "make_translators", the user can
 * easily specify which CellTranslators to use on which columns of the DBRow.
 * As an example, consider the following code:
 * @code
 * auto translators = make_translators
 *   ( Create<CellTranslator1, Col<0> > (),
 *     Create<CellTranslator1, Col<1>, 4 > (),
 *     Create<CellTranslator2, Col<0,5>, 4 > (),
 *     Create<CellTranslator2, Col<10,11>, 4, Incr<1,2> > ()
 *   );
 * @endcode
 * This code packs 13 translators (1 + 4 + 4 + 4). The first translator is of
 * type CellTranslator1 and will be applied on column 0 of the DBRow, the next 4
 * translators are also of type CellTranslator1 and will be applied on columns
 * 1, 2, 3, 4, respectively. The next 4 translators are of type CellTranslator2.
 * The first of them will be take columns 0 and 5 as input and will generate one
 * output column in the FilteredRow, the next one will take columns 1 and 6 as
 * input and will generate one output column, etc. Finally, the last 4 translators
 * are of type CellTranslator2 and will take as input pairs of columns (10,11),
 * (11,13), (12,15) and (13,17) respectively.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_CELL_TRANSLATOR_CREATE_H
#define GUM_LEARNING_DB_CELL_TRANSLATOR_CREATE_H

#include <vector>
#include <string>

#include <agrum/config.h>
#include <agrum/learning/database/column.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/filteredRow.h>
#include <agrum/learning/database/DBCellTranslator.h>

namespace gum {

  namespace learning {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /// the helper for Create that packs only 1 translator
    template <class Translator, typename Cols> class CreateOnce {
      public:
      /// the number of columns written by the execution of the translator
      static constexpr int output_size =
          std::remove_reference<Translator>::type::output_size;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// creates a wrapper to execute a translator on a given set of columns
      /** The set of columns given in argument corresponds to the columns
       * required by the translator to execute @b once. If you wish to apply the
       * translator several times, use class Create instead. */
      CreateOnce(const Translator &translator = Translator(),
                 const Cols &s = Cols());

      /// copy constructor
      CreateOnce(const CreateOnce<Translator, Cols> &call);

      /// move constructor
      CreateOnce(CreateOnce<Translator, Cols> &&call);

      /// destructor
      ~CreateOnce() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      CreateOnce<Translator, Cols> &
      operator=(const CreateOnce<Translator, Cols> &from);

      /// move operator
      CreateOnce<Translator, Cols> &operator=(CreateOnce<Translator, Cols> &&from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// sets the row that will be read by the translator
      void setInputRow(const DBRow &row) noexcept;

      /// sets the row to which the output will be written by the translator
      void setOutputRow(FilteredRow &row) noexcept;

      /** @brief sets the columns of the output vector which will be written
       * by the translator */
      void setOutputCols(unsigned int first_col) noexcept;

      /// returns the current input DBRow
      const DBRow &inputRow() const noexcept;

      /// returns the current output FilteredRow
      FilteredRow &outputFilteredRow() noexcept;

      /// returns the row of unsigned int of the current output FilteredRow
      std::vector<unsigned int> &outputRow() noexcept;

      /// returns the translator that does all the job
      Translator &getTranslator() noexcept;

      /// returns the translator that does all the job
      const Translator &getTranslator() const noexcept;

      /// execute a translation of the input row
      void translate();

      /// back-translate a given output (i.e., returns its input)
      /** @param col the column in _output_cols corresponding to the translated
       * value
       * @param translated_val the value in _output_cols of which we want to
       * know the original value (that which was actually read from the
       * database) */
      std::string translateBack(unsigned int col, unsigned int translated_val) const;

      /// initialize the cell filters by parsing once the database
      /** If initialization is required, this method is called for each row
       * of the database used for initialization. */
      void initialize();

      /// perform a post initialization after the database parsing
      void postInitialize();

      /** @brief indicates whether the translator needs an initial parsing of the
       * database to initialize itself */
      bool requiresInitialization() const noexcept;

      /// push back the number of modalities of the variables of the output columns
      void modalities(std::vector<unsigned int> &modals) const;

      /// returns the size of the input for this cell translator
      unsigned int inputSize() const noexcept;

      /// returns the size of the output for this cell translator
      unsigned int outputSize() const noexcept;

      /// @}

      private:
      /** @brief the cell translator that will be executed by each call to
       * CreateOnce */
      Translator __translator;
    };

#endif // DOXYGEN_SHOULD_SKIP_THIS

    /** @class Create
     * @ingroup learning_group
     * @brief The helper used to create packs of translators within row filters.
     *
     * The row filters used to transform database rows (DBRows) into FilteredRows
     * use translators to convert each database row into a FilteredRow and, then,
     * they use generators to convert the FilteredRow into one or several other
     * FilteredRows (for instance, EM generates several lines). To make it easy for
     * the user to pack CellTranslators into the row filter, helper Create shall be
     * used. Combined with helper "make_translators", the user can
     * easily specify which CellTranslators to use on which columns of the DBRow.
     * As an example, consider the following code:
     * @code
     * auto translators = make_translators
     *   ( Create<CellTranslator1, Col<0> > (),
     *     Create<CellTranslator1, Col<1>, 4 > (),
     *     Create<CellTranslator2, Col<0,5>, 4 > (),
     *     Create<CellTranslator2, Col<10,11>, 4, Incr<1,2> > ()
     *   );
     * @endcode
     * This code packs 13 translators (1 + 4 + 4 + 4). The first translator is of
     * type CellTranslator1 and will be applied on column 0 of the DBRow, the next
     * 4 translators are also of type CellTranslator1 and will be applied on
     * columns 1, 2, 3, 4, respectively. The next 4 translators are of type
     * CellTranslator2. The first of them will be take columns 0 and 5 as input
     * and will generate one output column in the FilteredRow, the next one will
     * take columns 1 and 6 as input and will generate one output column, etc.
     * Finally, the last 4 translators are of type CellTranslator2 and will take
     * as input pairs of columns (10,11),(11,13), (12,15) and (13,17) respectively.
     */
    template <typename Translator, typename Cols, int nb_times = 1,
              typename ColsIncr = typename Make_Default_Incr<Cols>::type>
    class Create : CreateOnce<Translator, Cols>,
                   Create<Translator, typename AddCols<Cols, ColsIncr>::type,
                          nb_times - 1, ColsIncr> {
      public:
      /// the number of columns written by the execution of the translator
      static constexpr int output_size =
          nb_times * std::remove_reference<Translator>::type::output_size;

      /// the type of the wrapper for the first translator applied
      using CurrentTranslator = CreateOnce<Translator, Cols>;

      /// the type of the wrapper for the next applications of the translator
      using NextTranslators =
          Create<Translator, typename AddCols<Cols, ColsIncr>::type, nb_times - 1,
                 ColsIncr>;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      Create();

      /// copy constructor
      Create(const Create<Translator, Cols, nb_times, ColsIncr> &call);

      /// move constructor
      Create(Create<Translator, Cols, nb_times, ColsIncr> &&call);

      /// destructor
      ~Create() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      Create<Translator, Cols, nb_times, ColsIncr> &
      operator=(const Create<Translator, Cols, nb_times, ColsIncr> &from);

      /// move operator
      Create<Translator, Cols, nb_times, ColsIncr> &
      operator=(Create<Translator, Cols, nb_times, ColsIncr> &&from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// sets the output columns written by all the applications of the translator
      void setOutputCols(unsigned int first_col) noexcept;

      /// sets the input row which will be read by the translator
      void setInputRow(const DBRow &row) noexcept;

      /// sets the row to which the translator will write its output
      void setOutputRow(FilteredRow &row) noexcept;

      /// returns the current input DBRow
      const DBRow &inputRow() const noexcept;

      /// returns the current output FilteredRow
      FilteredRow &outputFilteredRow() noexcept;

      /// returns the row of unsigned int of the current output FilteredRow
      std::vector<unsigned int> &outputRow() noexcept;

      /// apply the translator
      void translate();

      /// back-translate a given output (i.e., returns its input)
      /** @param col the column in _output_cols corresponding to the translated
       * value
       * @param translated_val the value in _output_cols of which we want to
       * know the original value (that which was actually read from the
       * database) */
      std::string translateBack(unsigned int col, unsigned int translated_val) const;

      /// initialize the cell filters by parsing once the database
      void initialize();

      /// perform a post initialization after the database parsing
      void postInitialize();

      /** @brief indicates whether the translator needs an initial parsing of the
       * database to initialize itself */
      bool requiresInitialization() const noexcept;

      /// push back the number of modalities of the variables of the output columns
      void modalities(std::vector<unsigned int> &modals) const;

      /// returns the size of the input for the cell translators
      unsigned int inputSize() const noexcept;

      /// returns the size of the output for the cell translators
      unsigned int outputSize() const noexcept;

      /// @}
    };

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /// end of the recursive definition of class Create
    template <typename Translator, typename Cols, typename ColsIncr>
    class Create<Translator, Cols, 0, ColsIncr> {
      public:
      void setOutputCols(unsigned int) noexcept {}
      void setInputRow(const DBRow &) noexcept {}
      void setOutputRow(FilteredRow &) noexcept {}
      void translate() noexcept {}
      void initialize() noexcept {}
      void postInitialize() noexcept {}
      bool requiresInitialization() const noexcept { return false; }
      void modalities(std::vector<unsigned int> &) const {}
      std::string translateBack(unsigned int, unsigned int) const {
        GUM_ERROR(UndefinedElement, "the set of translators does not contain "
                                    "the column to be translated back");
      }
    };

#endif // DOXYGEN_SHOULD_SKIP_THIS

  } /* namespace learning */

} /* namespace gum */

// always include template implementations
#include <agrum/learning/database/DBCellTranslatorCreate.tcc>

#endif /* GUM_LEARNING_DB_CELL_TRANSLATOR_CREATE_H */
