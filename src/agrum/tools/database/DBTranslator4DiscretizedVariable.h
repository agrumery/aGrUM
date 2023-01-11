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
 * @brief The databases' cell translators for discretized variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_TRANSLATOR_4_DISCRETIZED_VARIABLE_H
#define GUM_LEARNING_DB_TRANSLATOR_4_DISCRETIZED_VARIABLE_H

#include <agrum/agrum.h>
#include <agrum/tools/database/DBTranslator.h>
#include <agrum/tools/variables/discretizedVariable.h>


namespace gum {

  namespace learning {


    /** @class DBTranslator4DiscretizedVariable
     * @headerfile DBTranslator4DiscretizedVariable.h <agrum/tools/database/DBTranslator4DiscretizedVariable.h>
     * @brief The databases' cell translators for discretized variables
     *
     * Translators are used by DatabaseTable instances to transform datasets'
     * strings into DBTranslatedValue instances. The point is that strings are
     * not adequate for fast learning, they need to be preprocessed into a type
     * that can be analyzed quickly (the so-called DBTranslatedValue type).
     *
     * A DBTranslator4DiscretizedVariable is a translator that contains and
     * exploits a DiscretizedVariable for translations. Each time a string needs
     * be translated, we ask the DiscretizedVariable which discretization
     * interval contains the the number represented by the string. The
     * DBTranslatedValue corresponding to the translation of the string
     * contains in its discr_val field the index of this discretization interval.
     *
     * @warning Translators for discretized variables are not editable, that is,
     * you must provide the const variable that will be used for translations.
     * Enabling the editable mode would not make much sense.
     *
     * @par Here is an example of how to use this class:
     * @code
     * // create the translator, with possible missing symbols: "N/A" and "???"
     * // i.e., each time the translator reads a "N/A" or a "???" string, it
     * // won't translate it into a number but into a missing value.
     * std::vector<std::string> missing { "N/A", "???" };
     * gum::DiscretizedVariable<int> var ( "X1", "" );
     * var.addTick ( 1 );
     * var.addTick ( 3 );
     * var.addTick ( 10 );
     * gum::learning::DBTranslator4DiscretizedVariable translator( var,missing );
     *
     * // gets the DBTranslatedValue corresponding to some strings
     * auto val1 = translator.translate("5.2");
     * auto val2 = translator << "2";
     * // at this point, val1 and val2 are equal to
     * // gum::learning::DBTranslatedValue { std::size_t(1) } and
     * // gum::learning::DBTranslatedValue { std::size_t(0) } respectively
     * // because the first discretization interval corresponds to [1;3[ and
     * // the second one to [3;10[.
     *
     * // if the string contains a number outside the domain of the
     * // DiscretizedVariable, then a gum::NotFound exception is raised:
     * auto val3 = translator << "17"; // NotFound raised
     *
     * // add the numbers assigned to val1, val2
     * std::size_t sum = val1.discr_val + val2.discr_val;
     *
     * // translate missing values: val4 and val5 will be equal to:
     * // DBTranslatedValue { std::numeric_limits<std::size_t>::max () }
     * auto val4 = translator << "N/A";
     * auto val5 = translator.translate ( "???" );
     *
     * // the following instructions raise TypeError exceptions because the
     * // strings are not numbers
     * auto val6 = translator << "422x";
     * auto val7 = translator.translate ( "xxx" );
     *
     * // given a DBTranslatedValue that is supposed to contain the index of
     * // a discretization interval, get the string representing the interval.
     * std::string str;
     * str = translator.translateBack ( val1 );        // str = "[3,10["
     * str = translator >> val2;                       // str = "[1;3["
     * str = translator >> gum::learning::DBTranslatedValue {std::size_t(1)};
     *                                                 // str = "[3;10["
     *
     * // translate back missing values: the string will corresponds to one of
     * // the missing symbols known to the translator
     * str = translator >> val4; // str = "N/A" or "???"
     * str = translator >> val5; // str = "N/A" or "???"
     *
     * // get the variable stored within the translator
     * const gum::DiscretizedVariable<float>* var =
     *   dynamic_cast<const gum::DiscretizedVariable<float>*>
     *     ( translator.variable () );
     * @endcode
     *
     * @ingroup learning_database
     */
    class DBTranslator4DiscretizedVariable: public DBTranslator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor with a discretized variable as translator
      /** @param var a discretized variable which will be used for translations.
       * The translator keeps a copy of this variable
       * @param  missing_symbols the set of symbols in the dataset
       * representing missing values
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. During the construction, we check that the discretized
       * variable passed in argument has fewer discretization intervals than
       * the admissible dictionary size
       * @warning If the variable contained into the translator has a label
       * equal to a missing value symbol, the label will be taken into
       * account in the translation, not the missing value. */
      template < typename GUM_SCALAR >
      DBTranslator4DiscretizedVariable(const DiscretizedVariable< GUM_SCALAR >& var,
                                       const std::vector< std::string >&        missing_symbols,
                                       std::size_t                              max_dico_entries
                                       = std::numeric_limits< std::size_t >::max());

      /** @brief default constructor with a discretized variable as translator
       * but without missing symbols
       *
       * @param var a discretized variable which will be used for translations.
       * The translator keeps a copy of this variable
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. During the construction, we check that the discretized
       * variable passed in argument has fewer discretization intervals than
       * the admissible dictionary size
       * @warning If the variable contained into the translator has a label
       * equal to a missing value symbol, the label will be taken into
       * account in the translation, not the missing value. */
      template < typename GUM_SCALAR >
      DBTranslator4DiscretizedVariable(const DiscretizedVariable< GUM_SCALAR >& var,
                                       std::size_t                              max_dico_entries
                                       = std::numeric_limits< std::size_t >::max());

      /// default constructor with a IDiscretized variable as translator
      /** @param var a IDiscretized variable which will be used for translations.
       * The translator keeps a copy of this variable
       * @param  missing_symbols the set of symbols in the dataset
       * representing missing values
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. During the construction, we check that the discretized
       * variable passed in argument has fewer discretization intervals than
       * the admissible dictionary size
       * @warning If the variable contained into the translator has a label
       * equal to a missing value symbol, the label will be taken into
       * account in the translation, not the missing value. */
      DBTranslator4DiscretizedVariable(const IDiscretizedVariable&       var,
                                       const std::vector< std::string >& missing_symbols,
                                       std::size_t                       max_dico_entries
                                       = std::numeric_limits< std::size_t >::max());

      /** @brief default constructor with a IDiscretized variable as translator
       * but without missing symbols
       *
       * @param var a discretized variable which will be used for translations.
       * The translator keeps a copy of this variable
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. During the construction, we check that the discretized
       * variable passed in argument has fewer discretization intervals than
       * the admissible dictionary size
       * @warning If the variable contained into the translator has a label
       * equal to a missing value symbol, the label will be taken into
       * account in the translation, not the missing value. */
      DBTranslator4DiscretizedVariable(const IDiscretizedVariable& var,
                                       std::size_t                 max_dico_entries
                                       = std::numeric_limits< std::size_t >::max());

      /// copy constructor
      DBTranslator4DiscretizedVariable(const DBTranslator4DiscretizedVariable& from);

      /// move constructor
      DBTranslator4DiscretizedVariable(DBTranslator4DiscretizedVariable&& from);

      /// virtual copy constructor
      virtual DBTranslator4DiscretizedVariable* clone() const;

      /// destructor
      virtual ~DBTranslator4DiscretizedVariable();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTranslator4DiscretizedVariable& operator=(const DBTranslator4DiscretizedVariable& from);

      /// move operator
      DBTranslator4DiscretizedVariable& operator=(DBTranslator4DiscretizedVariable&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the translation of a string
      /** This method tries to translate a given string into the
       * DBTranslatedValue that should be stored into a databaseTable. If the
       * translator cannot find the translation in its current dictionary, then
       * the translator raises either a TypeError if the string is not a number
       * or a NotFound exception.
       *
       * @warning Note that missing values (i.e., string encoded as missing
       * symbols) are translated as std::numeric_limits<std::size_t>::max ().
       * @warning If the variable contained into the translator has a
       * discretization interval that contains a missing value symbol, the
       * interval will be taken into account in the translation, not the missing
       * value.
       * @return the translated value of the string to be stored into a
       * DatabaseTable
       * @throws UnknownLabelInDatabase is raised if the translation cannot be
       * found.
       * @throws TypeError is raised if the translation cannot be found and
       * the translator and the string does not correspond to a number. */
      virtual DBTranslatedValue translate(const std::string& str) final;

      /// returns the original value for a given translation
      /** @return the string that was translated into a given DBTranslatedValue.
       * @throws UnknownLabelInDatabase is raised if this original value
       * cannot be found */
      virtual std::string translateBack(const DBTranslatedValue translated_val) const final;

      /// returns the number of discretization intervals used for translations
      /** @warning Note that missing values are encoded as
       * std::numeric_limits::max () and are not taken into account in the
       * domain sizes. */
      virtual std::size_t domainSize() const final;

      /// indicates that the translator is never in editable dictionary mode
      virtual bool hasEditableDictionary() const final;

      /// sets/unset the editable dictionary mode
      virtual void setEditableDictionaryMode(bool new_mode) final;

      /// indicates that the translations should never be reordered
      virtual bool needsReordering() const final;

      /** @brief returns an empty HashTable to indicate that no reordering
       * is needed. */
      virtual HashTable< std::size_t, std::size_t > reorder() final;

      /// returns the variable stored into the translator
      virtual const IDiscretizedVariable* variable() const final;

      /// returns the translation of a missing value
      virtual DBTranslatedValue missingValue() const final;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the DiscretizedVariable used for translations
      DiscretizedVariable< float > _variable_;

      // the DiscretizedVariable returned by method variable ()
      // We must return a IDiscretizedVariable because the user may have
      // saved into the translator a DiscretizedVariable<X>, with X != float
      IDiscretizedVariable* _real_variable_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };


  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/tools/database/DBTranslator4DiscretizedVariable_tpl.h>

// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/DBTranslator4DiscretizedVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_TRANSLATOR_4_DISCRETIZED_VARIABLE_H */
