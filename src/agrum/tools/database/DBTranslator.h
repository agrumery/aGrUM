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
 * @brief The base class for all the tabular databases' cell translators
 *
 * This file contains the basis for interacting with a tabular database as
 * a translator. Every translator should derive from this class.
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_TRANSLATOR_H
#define GUM_LEARNING_DB_TRANSLATOR_H

#include <string>
#include <limits>
#include <vector>
#include <utility>

#include <agrum/agrum.h>
#include <agrum/tools/core/bijection.h>
#include <agrum/tools/core/set.h>
#include <agrum/tools/variables/variable.h>
#include <agrum/tools/database/DBTranslatedValue.h>


namespace gum {

  namespace learning {


    /** @class DBTranslator
     * @headerfile DBTranslator.h <agrum/tools/database/DBTranslator.h>
     * @brief The base class for all the tabular database cell translators
     *
     * Translators are used by DatabaseTable instances to transform datasets'
     * strings into DBTranslatedValue instances. The point is that strings are
     * not adequate for fast learning, they need to be preprocessed into a type
     * that can be analyzed quickly (the so-called DBTranslatedValue type).
     * The DBTranslator class is the abstract base class for all the translators
     * used in aGrUM.
     *
     * Here is an example of how to use it, illustrated with the
     * DBTranslator4ContinuousVariable class:
     *
     * @code
     * // create the translator, with possible missing symbols: "N/A" and "???"
     * // i.e., each time the translator reads a "N/A" or a "???" string, it
     * // won't translate it into a number but into a missing value.
     * std::vector<std::string> missing { "N/A", "???" };
     * gum::learning::DBTranslator4ContinuousVariable<> translator ( missing );
     *
     * // gets the DBTranslatedValue corresponding to some strings
     * auto val1 = translator.translate("5");   // val1 = DBTranslatedValue {5.0f}
     * auto val2 = translator.translate("4.2"); // val2 = DBTRanslatedValue {4.2f}
     * auto val3 = translator << "3.4";         // val3 = DBTranslatedValue {3.4f}
     *
     * // add the numbers assigned to val1, val2, val3
     * float sum = val1.cont_val + val2.cont_val + val3.cont_val;
     *
     * // translate missing values: val4 and val5 will be equal to:
     * // DBTranslatedValue { std::numeric_limits<float>::max () }
     * auto val4 = translator << "N/A";
     * auto val5 = translator.translate ( "???" );
     *
     * // the following instructions raise TypeError exceptions because the
     * // strings cannot be translated into real numbers
     * auto val6 = translator << "4.22x";
     * auto val7 = translator.translate ( "xxx" );
     *
     * // given a DBTranslatedValue that is supposed to contain a float, get
     * // the corresponding string. The strings should be equivalent to those
     * // indicated below (maybe they could contain more zeroes after the dot).
     * std::string str;
     * str = translator.translateBack ( val1 );        // str ~ "5.0"
     * str = translator >> val2;                       // str ~ "4.2"
     * str = translator >> gum::learning::DBTranslatedValue {7.2e3f};
     *                                                 // str ~ "7.2 e3"
     *
     * // translate back missing values: the string will corresponds to one of
     * // the missing symbols known to the translator
     * str = translator >> val4; // str = "N/A" or "???"
     * str = translator >> val5; // str = "N/A" or "???"
     *
     * // get the domain size of the variable stored into the translatator
     * // This size is only useful for translators with discrete variables
     * std::size_t size = translator.domainSize ();
     *
     * // get the variable stored within the translator
     * const gum::ContinuousVariable<float>* var =
     *   dynamic_cast<const gum::ContinuousVariable<float>*>
     *     ( translator.variable () );
     *@endcode
     *
     * @ingroup learning_database
     */
    class DBTranslator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      /** @param val_type indicates whether the DBTranslator deals with discrete
       * or continuous variables
       * @param editable_dictionary indicates whether the dictionary used for
       * translations can be updated dynamically when observing new string or
       * whether it should remain constant. To see how this parameter is handled,
       * see the child classes inheriting from DBTranslator
       * @param  missing_symbols the set of symbols in the database
       * representing missing values
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       */
      DBTranslator(DBTranslatedValueType             val_type,
                   const bool                        is_lossless,
                   const std::vector< std::string >& missing_symbols,
                   const bool                        editable_dictionary = true,
                   std::size_t max_dico_entries = std::numeric_limits< std::size_t >::max());

      /// default constructor without missing symbols
      /** @param val_type indicates whether the DBTranslator deals with discrete
       * or continuous variables
       * @param editable_dictionary indicates whether the dictionary used for
       * translations can be updated dynamically when observing new string or
       * whether it should remain constant. To see how this parameter is handled,
       * see the child classes inheriting from DBTranslator
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       */
      DBTranslator(DBTranslatedValueType val_type,
                   const bool            is_lossless,
                   const bool            editable_dictionary = true,
                   std::size_t max_dico_entries = std::numeric_limits< std::size_t >::max());

      /// copy constructor
      DBTranslator(const DBTranslator& from);

      /// move constructor
      DBTranslator(DBTranslator&& from);

      /// virtual copy constructor
      virtual DBTranslator* clone() const = 0;

      /// destructor
      virtual ~DBTranslator();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// alias for method translate
      DBTranslatedValue operator<<(const std::string& str);

      /// alias for method translateBack
      std::string operator>>(const DBTranslatedValue translated_val);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the translation of a string
      /** This method tries to translate a given string into the
       * DBTranslatedValue that should be stored into a DatabaseTable. If the
       * translator cannot find the translation in its current dictionary, then
       * two situations can
       * obtain:
       * -# if the translator is not in an editable dictionary mode, then the
       *    translator raises a NotFound exception.
       * -# if the translator is in an editable dictionary mode, i.e., it is
       *    allowed to update its dictionary, then it tries to add the string
       *    as a new value in the dictionary. Upon success, it returns the
       *    translated value, otherwise, it raises either:
       *    - a SizeError exception if the number of entries in the dictionary
       *      has already reached its maximum,
       *    - a TypeError exception if the string cannot be converted into a
       *      value that can be inserted into the dictionary
       *    - an OperationNotAllowed exception if the translation would induce
       *      incoherent behavior (e.g., a DBTranslator4ContinuousVariable that
       *      contains a variable whose domain is [x,y] as well as a missing
       *      value symbol z \f$\in\f$ [x,y]).
       *
       * @warning Note that missing values (i.e., string encoded as missing
       * symbols) are translated as std::numeric_limits<>::max ().
       * @param str the string that the DBTranslator will try to translate
       * @return the translated value of the string to be stored into a
       * DatabaseTable
       * @throws UnknownLabelInDatabase is raised if the translation cannot
       * be found and the translator is not in an editable dictionary mode.
       * @throws SizeError is raised if the number of entries in the dictionary
       * has already reached its maximum.
       * @throws OperationNotAllowed exception is raised if the translation
       * cannot be found and the insertion of the string into the translator's
       * dictionary fails because it would induce incoherent behavior (e.g.,
       * a DBTranslator4ContinuousVariable that contains a variable whose domain
       * is [x,y] as well as a missing value symbol z \f$\in\f$ [x,y]).
       * @throws TypeError is raised if the translation cannot be found and
       * the insertion of the string into the translator's dictionary fails
       * due to str being impossible to be converted into an appropriate type. */
      virtual DBTranslatedValue translate(const std::string& str) = 0;

      /// returns the original value for a given translation
      /** @param translated_val a value that should result from a translation
       * and for which we are looking for the corresponding DBTranslator's
       * variable's label (a string)
       * @return the string that was translated into a given DBTranslatedValue.
       * @warning when the translator is not a proper bijection, like, e.g.,
       * DBTranslator4DiscretizedVariable, the method returns the value of
       * the random variable corresponding to translated_val (i.e., for a
       * discretized variable, it would return the interval corresponding to
       * translated_val).
       * @throws UnknownLabelInDatabase is raised if this original value
       * cannot be found */
      virtual std::string translateBack(const DBTranslatedValue translated_val) const = 0;

      /// returns the domain size of a variable corresponding to the translations
      /** Assume that the translator has been fed with the observed values of
       * a random variable. Then it has produced a set of translated values. The
       * latter define the domain of the variable. When the variable is discrete,
       * values are assumed to span from 0 to a number n-1. In this case, the
       * domain size of the variable is n. When the function is continuous,
       * the domain size should be infinite and we return a
       * std::numeric_limits<std::size_t>::max() to represent it. Note that
       * missing values are encoded as std::numeric_limits<>::max () and are
       * not taken into account in the domain sizes. */
      virtual std::size_t domainSize() const = 0;

      /// indicates whether the translator has an editable dictionary or not
      virtual bool hasEditableDictionary() const;

      /// sets/unset the editable dictionary mode
      virtual void setEditableDictionaryMode(bool new_mode);

      /// returns the translation from database indices to input strings
      virtual const Bijection< std::size_t, std::string >& getDictionary() const;

      /** @brief indicates whether a reordering is needed to make the
       * translations sorted
       *
       * If the strings represented by the translations are only numbers,
       * translations are considered to be sorted if and only if they are sorted
       * by increasing number. If the strings do not only represent numbers, then
       * translations are considered to be sorted if and only if they are sorted
       * lexicographically.
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
       * order is OK. */
      virtual bool needsReordering() const = 0;

      /** @brief performs a reordering of the dictionary and returns a mapping
       * from the old translated values to the new ones.
       *
       * When a reordering is needed, i.e., string values must be translated
       * differently, Method reorder() computes how the translations should be
       * changed. It updates accordingly the dictionary and returns the mapping
       * that enables changing the old dictionary values into the new ones.
       * Note that the hash table returned is expressed in terms of std::size_t
       * because only the translations for discrete random variables need be
       * reordered, those for continuous random variables are identity mappings.
       * @warning If there is no reordering to perform, the method returns
       * an empty hashtable. */
      virtual HashTable< std::size_t, std::size_t > reorder() = 0;

      /// returns the set of missing symbols taken into account by the translator
      const Set< std::string >& missingSymbols() const;

      /// indicates whether a string corresponds to a missing symbol
      bool isMissingSymbol(const std::string& str) const;

      /// returns the variable stored into the translator
      virtual const Variable* variable() const = 0;

      /// sets the name of the variable stored into the translator
      void setVariableName(const std::string& str) const;

      /// sets the name of the variable stored into the translator
      void setVariableDescription(const std::string& str) const;

      /// returns the type of values handled by the translator
      /** @returns either DBTranslatedValueType::DISCRETE if the translator
       * includes a discrete variable or DBTranslatedValueType::CONTINUOUS if
       * it contains a continuous variable. This is convenient to know how to
       * interpret the DBTranslatedValue instances produced by the DBTranslator:
       * either using their discr_val field or their cont_val field. */
      DBTranslatedValueType getValType() const;

      /// returns a Boolean indicating whether the translation is lossless or not
      /** Some translations can lose some information. For instance, a translator for a
       * discretized variable will translate all the values of a discretization interval as
       * the same value (the index of the interval). As such it looses some information
       * because, knowing this index, it is impossible to get back to the original value that
       * was translated. Method isLossless() indicates whether the translation never loses
       * any information or not. */
      bool isLossless() const;

      /// indicates whether a translated value corresponds to a missing value
      bool isMissingValue(const DBTranslatedValue& val) const;

      /// returns the translation of a missing value
      virtual DBTranslatedValue missingValue() const = 0;

      /// @}


      protected:
      // ##########################################################################
      /// @name Protected Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTranslator& operator=(const DBTranslator& from);

      /// move operator
      DBTranslator& operator=(DBTranslator&& from);

      /// @}

      /// indicates whether the translation is lossless (e.g., ranges) or not
      bool is_lossless_;

      /// indicates whether the dictionary can be updated or not
      bool is_dictionary_dynamic_;

      /// the maximum number of entries that the dictionary is allowed to contain
      std::size_t max_dico_entries_;

      /// the set of missing symbols
      Set< std::string > missing_symbols_;

      /// the bijection relating back translated values and their original strings.
      /** Note that the translated values considered here are of type std::size_t
       * because only the values for discrete variables need be stored, those
       * for continuous variables are actually identity mappings.
       * @warning only the values of the random variable are stored into this
       * bijection. Missing values are not considered here. */
      mutable Bijection< std::size_t, std::string > back_dico_;

      /// the type of the values translated by the translator
      DBTranslatedValueType val_type_;
    };


  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/DBTranslator_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_TRANSLATOR_H */
