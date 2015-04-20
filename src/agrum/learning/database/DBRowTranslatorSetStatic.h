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
 * @brief The "meta-programming" set of translators stored into a row filter
 *
 * Basically, there are two ways to create the cell translators needed to parse
 * a database: either you know at compile time the columns of the database you
 * will wish to extract and you should definitely use the DBRowTranslatorSetStatic
 * class to store the cell translators; or you know the columns to extract at
 * run time and you should use the DBRowTranslatorSetDynamic class.
 * DBRowTranslatorSetStatic is a "meta-programming" based class that packs the
 * cell filters in a most efficient way (essentially, all methods can be inlined,
 * which makes this class the fastest one). DBRowTranslatorSetDynamics are less
 * efficient but are more general. If all their cell translators are identical,
 * translator's methods are also inlined, otherwise, methods' calls induce virtual
 * function overheads.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_ROW_TRANSLATOR_SET_STATIC_H
#define GUM_LEARNING_DB_ROW_TRANSLATOR_SET_STATIC_H

#include <string>

#include <agrum/config.h>
#include <agrum/learning/database/DBCellTranslator.h>
#include <agrum/learning/database/DBCellTranslatorCreate.h>

namespace gum {

  namespace learning {

#ifndef DOXYGEN_SHOULD_SKIP_THIS

    /** @brief the wrapper of Create<>s that will be used to translate
     * a set of database cells */
    template <int OUTPUT_SIZE, typename... Translators>
    class BasicDBRowTranslatorSetStatic;

    /** @brief the end of the recursive definition of
     * BasicDBRowTranslatorSetStatic: should do nothing */
    template <int OUTPUT_SIZE> class BasicDBRowTranslatorSetStatic<OUTPUT_SIZE> {
      public:
      static constexpr unsigned int output_size = OUTPUT_SIZE;

      BasicDBRowTranslatorSetStatic() noexcept {}
      BasicDBRowTranslatorSetStatic(
          const BasicDBRowTranslatorSetStatic<OUTPUT_SIZE> &) noexcept {}
      BasicDBRowTranslatorSetStatic(
          BasicDBRowTranslatorSetStatic<OUTPUT_SIZE> &&) noexcept {}
      ~BasicDBRowTranslatorSetStatic() noexcept {}
      BasicDBRowTranslatorSetStatic<OUTPUT_SIZE> &
      operator=(const BasicDBRowTranslatorSetStatic<OUTPUT_SIZE> &) noexcept {
        return *this;
      }
      BasicDBRowTranslatorSetStatic<OUTPUT_SIZE> &
      operator=(BasicDBRowTranslatorSetStatic<OUTPUT_SIZE> &&) noexcept {
        return *this;
      }

      void setInputRow(const DBRow &) noexcept {}
      void setOutputRow(FilteredRow &) noexcept {}
      void translate() noexcept {}
      void initialize() noexcept {}
      void postInitialize() noexcept {}
      bool requiresInitialization() const noexcept { return false; }
      void modalities(std::vector<unsigned int> &) const noexcept {}
      std::string translateBack(unsigned int, unsigned int) const {
        GUM_ERROR(NotFound, "the variable could not be translated back: it "
                            "was not found by the translator");
      }
    };

    /** @brief the implementation of the wrapper of Create<>s (used to
     * translate the cells of a DBRow) */
    template <int OUTPUT_SIZE, typename Translator, typename... OtherTranslators>
    class BasicDBRowTranslatorSetStatic<OUTPUT_SIZE, Translator, OtherTranslators...>
        : public BasicDBRowTranslatorSetStatic<
              OUTPUT_SIZE + std::remove_reference<Translator>::type::output_size,
              OtherTranslators...> {
      public:
      /// the type of the subsequent Create<>s to apply
      using NextTranslators = BasicDBRowTranslatorSetStatic<
          OUTPUT_SIZE + std::remove_reference<Translator>::type::output_size,
          OtherTranslators...>;

      /// the number of columns written by all the BasicDBRowTranslatorSetStatics
      static constexpr unsigned int output_size = NextTranslators::output_size;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      protected:
      /// default constructor
      /** @param first_translator the Create<> for the first translator to apply
       * @param next_translators all the other Create<>s to apply */
      BasicDBRowTranslatorSetStatic(const Translator &first_translator,
                                    const OtherTranslators &... next_translators);

      /// copy constructor
      BasicDBRowTranslatorSetStatic(const BasicDBRowTranslatorSetStatic<
          OUTPUT_SIZE, Translator, OtherTranslators...> &from);

      /// move constructor
      BasicDBRowTranslatorSetStatic(BasicDBRowTranslatorSetStatic<
          OUTPUT_SIZE, Translator, OtherTranslators...> &&from);

      public:
      /// destructor
      ~BasicDBRowTranslatorSetStatic() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      protected:
      /// copy operator
      BasicDBRowTranslatorSetStatic<OUTPUT_SIZE, Translator, OtherTranslators...> &
      operator=(const BasicDBRowTranslatorSetStatic<OUTPUT_SIZE, Translator,
                                                    OtherTranslators...> &);

      /// move operator
      BasicDBRowTranslatorSetStatic<OUTPUT_SIZE, Translator, OtherTranslators...> &
      operator=(BasicDBRowTranslatorSetStatic<OUTPUT_SIZE, Translator,
                                              OtherTranslators...> &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// sets the input row to filter
      void setInputRow(const DBRow &row) noexcept;

      /// sets the output rows of the translators
      void setOutputRow(FilteredRow &output_row) noexcept;

      /// returns the current input DBRow
      const DBRow &inputRow() const;

      /// returns the current output row
      FilteredRow &outputRow();

      /// perform the translation of a database row
      void translate();

      /// returns the name of the jth value of the ith column
      std::string translateBack(unsigned int col, unsigned int translated_val) const;

      /// initialize the cell filters by parsing once the database
      /** If initialization is required, this method is called for each row
       * of the database used for initialization. */
      void initialize();

      /// perform a post initialization after the database parsing
      void postInitialize();

      /** @brief indicates whether one of the translators needs an initial parsing
       * of the database to initialize itself */
      bool requiresInitialization() const noexcept;

      /// push back the number of modalities of the variables of the output columns
      void modalities(std::vector<unsigned int> &modals) const;

      /// returns the size of the input as used by the cell translators
      unsigned int inputSize() const noexcept;

      /// returns the size of the output of the cell translators
      unsigned int outputSize() const noexcept;

      /// @}

      private:
      /// the Create<> translator that should be executed
      Translator __translator;

      /// only DBRowTranslatorSetStatics can create BasicDBRowTranslatorSetStatics
      template <typename... Translators> friend class DBRowTranslatorSetStatic;
    };

#endif // DOXYGEN_SHOULD_SKIP_THIS

    /** @class DBRowTranslatorSetStatic
     * @ingroup learning_group
     * @brief the "meta-programming" class that includes cell translator packs
     * into row filters
     *
     * Basically, there are two ways to create the cell translators needed to parse
     * a database: either you know at compile time the columns of the database you
     * will wish to extract and you should definitely use the
     * DBRowTranslatorSetStatic class to store the cell translators; or you know
     * the columns to extract at run time and you should use the
     * DBRowTranslatorSetDynamic class. DBRowTranslatorSetStatic is a
     * "meta-programming" based class that packs the cell filters in a most
     * efficient way (essentially, all methods can be inlined, which makes this
     * class the fastest one). DBRowTranslatorSetDynamics are less
     * efficient but are more general. If all their cell translators are
     * identical, translator's methods are also inlined, otherwise, methods' calls
     * induce virtual function overheads.
     *
     * This class enables efficient packing of cell translators into row filters.
     * To make it easy to use, you should prefer using its helper creation function
     * make_translators. The following code shows how to simply create a
     * DBRowTranslatorSetStatic:
     * @code
     * auto my_filter = make_translators
     * ( Create<Translate, Col<3> > (),   // use Translate to translate column 3
     *   Create<Translate, Col<5>, 4> (), // use Translate to translate cols 5 to 8
     *   Create<Translate2, Col<6,7> > (),// translate cols 6 and 7 and get the
     *                                    // result into 1 col of the output row
     *   Create<Translate2, Col<10,11>, 4, Incr<1,2> > ()
     * );
     * @endcode
     */
    template <typename... Translators>
    class DBRowTranslatorSetStatic
        : BasicDBRowTranslatorSetStatic<0, Translators...> {
      public:
      using TranslatorSetStatic = BasicDBRowTranslatorSetStatic<0, Translators...>;

      /// the size of the output filtered row
      static constexpr unsigned int output_size = TranslatorSetStatic::output_size;

      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      /** @param translators the set of DBCellTranslators to apply */
      DBRowTranslatorSetStatic(const Translators &... translators);

      /// copy constructor
      DBRowTranslatorSetStatic(const DBRowTranslatorSetStatic<Translators...> &from);

      /// move constructor
      DBRowTranslatorSetStatic(DBRowTranslatorSetStatic<Translators...> &&from);

      /// destructor
      ~DBRowTranslatorSetStatic() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowTranslatorSetStatic<Translators...> &
      operator=(const DBRowTranslatorSetStatic<Translators...> &);

      /// move operator
      DBRowTranslatorSetStatic<Translators...> &
      operator=(DBRowTranslatorSetStatic<Translators...> &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// execute all the translations on the current database row
      void translate();

      /// back-translate a given output (i.e., returns its input)
      /** @param col the column in _output_cols corresponding to the translated
       * value
       * @param translated_val the value in _output_cols of which we want to
       * know the original value (that which will be stored into the
       * Bayesian network) */
      std::string translateBack(unsigned int col, unsigned int translated_val) const;

      /// initialize the cell filters by parsing once the database
      void initialize();

      /// perform a post initialization after the database parsing
      void postInitialize();

      /** @brief indicates whether one of the translators needs an initial parsing
       * of the database to initialize itself */
      bool requiresInitialization() const noexcept;

      /// push back the number of modalities of the variables of the output columns
      void modalities(std::vector<unsigned int> &modals) const;

      /// sets the input row that shall be read by all the cell translators
      void setInputRow(const DBRow &row) noexcept;

      /// returns the current output row
      FilteredRow &outputRow() noexcept;

      /// returns the current input DBRow
      const DBRow &inputRow() const;

      /// returns the size of the input as used by the cell translators
      unsigned int inputSize() const noexcept;

      /// returns the size of the output of the cell translators
      unsigned int outputSize() const noexcept;

      /// @}

      private:
      /// the output row into which the translators write their output
      FilteredRow __output_row;
    };

    /// a function to create easily a DBRowTranslatorSetStatic
    /** Below is an example of the use of this function. Basically, all the
     * arguments are Create<> translators.
     * @code
     * auto my_filter = make_translators
     * ( Create<Translate, Col<3> > (), // use Translate to translate column 3
     *   Create<Translate, Col<5>, 4> (), // use Translate to translate cols 5 to 8
     *   Create<Translate2, Col<6,7> > (), // translate cols 6 and 7 and get the
     *                                     // result into 1 col of the output row
     *   Create<Translate2, Col<10,11>, 4, Incr<1,2> > ()
     * );
     * @endcode
     */
    template <typename... Args>
    constexpr DBRowTranslatorSetStatic<Args...>
    make_translators(const Args &... args) {
      return DBRowTranslatorSetStatic<Args...>(args...);
    }

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/learning/database/DBRowTranslatorSetStatic.tcc>

#endif /* GUM_LEARNING_DB_ROW_TRANSLATOR_SET_STATIC_H */
