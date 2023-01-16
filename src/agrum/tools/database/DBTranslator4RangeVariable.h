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
 * @brief The databases' cell translators for range variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_TRANSLATOR_4_RANGE_VARIABLE_H
#define GUM_LEARNING_DB_TRANSLATOR_4_RANGE_VARIABLE_H

#include <agrum/agrum.h>
#include <agrum/tools/database/DBTranslator.h>
#include <agrum/tools/variables/rangeVariable.h>


namespace gum {

  namespace learning {


    /** @class DBTranslator4RangeVariable
     * @headerfile DBTranslator4RangeVariable.h <agrum/tools/database/DBTranslator4RangeVariable.h>
     * @brief The databases' cell translators for range variables
     *
     * Translators are used by DatabaseTable instances to transform datasets'
     * strings into DBTranslatedValue instances. The point is that strings are
     * not adequate for fast learning, they need to be preprocessed into a type
     * that can be analyzed quickly (the so-called DBTranslatedValue type).
     *
     * A DBTranslator4RangeVariable is a translator that contains and
     * exploits a RangeVariable for translations. Each time a string needs
     * be translated, we ask the RangeVariable whether its domain contains
     * the integer value represented in the string. If this is the case, then
     * the DBTranslatedValue corresponding to the translation of the string
     * contains in its discr_val field this integer value.
     *
     * @par Here is an example of how to use this class:
     * @code
     * // create the translator, with possible missing symbols: "N/A" and "???"
     * // i.e., each time the translator reads a "N/A" or a "???" string, it
     * // won't translate it into a number but into a missing value.
     * std::vector<std::string> missing { "N/A", "???" };
     * gum::learning::DBTranslator4RangeVariable translator ( missing );
     *
     * // gets the DBTranslatedValue corresponding to some strings
     * auto val1 = translator.translate("5");
     * auto val2 = translator.translate("4");
     * // at this point, val1 and val2 are equal to
     * // gum::learning::DBTranslatedValue { std::size_t(0) } and
     * // gum::learning::DBTranslatedValue { std::size_t(1) } respectively.
     * // In addition, the RangeVariable stored into the translator has
     * // a domain equal to {4,5}.
     * auto val3 = translator << "7";
     * // val3 is encoded as gum::learning::DBTranslatedValue { std::size_t(3) }
     * // because string "6" is implicitly encoded as
     * // gum::learning::DBTranslatedValue { std::size_t(3) }.
     * // In addition, the domain of the range variable is expanded to {4,5,6,7}.
     *
     * // add the numbers assigned to val1, val2, val3
     * std::size_t sum = val1.discr_val + val2.discr_val + val3.discr_val;
     *
     * // translate missing values: val4 and val5 will be equal to:
     * // DBTranslatedValue { std::numeric_limits<float>::max () }
     * auto val4 = translator << "N/A";
     * auto val5 = translator.translate ( "???" );
     *
     * // the following instructions raise TypeError exceptions because the
     * // strings cannot be translated into integers
     * auto val6 = translator << "422x";
     * auto val7 = translator.translate ( "xxx" );
     *
     * // given a DBTranslatedValue that is supposed to contain an integer in
     * // the range of the RangeVariable, get the corresponding string.
     * std::string str;
     * str = translator.translateBack ( val1 );        // str = "5"
     * str = translator >> val2;                       // str = "4"
     * str = translator >> gum::learning::DBTranslatedValue {std::size_t(2)};
     *                                                 // str = "6"
     *
     * // translate back missing values: the string will corresponds to one of
     * // the missing symbols known to the translator
     * str = translator >> val4; // str = "N/A" or "???"
     * str = translator >> val5; // str = "N/A" or "???"
     *
     * // get the variable stored within the translator
     * const gum::RangeVariable* var =
     *   dynamic_cast<const gum::RangeVariable*> ( translator.variable () );
     *
     * // it is possible to create a translator for an already known variable.
     * // In this case, by default, the translator is not in editable mode, but
     * // this behavior can be changed passing the right arguments to the
     * // constructor of the translator, or using the setEditableDictionaryMode
     * // method. Here, we create a range variable whose domain is {-2,...,10}
     * gum::RangeVariable var ( "X", "", -2, 10 );
     * gum::learning::DBTranslator4RangeVariable translator2 ( var, missing );
     *
     * auto xval1 = translator2.translate ( "-1" ).discr_val; // xval1 = 1
     * auto xval2 = translator2.translate ( "7" ).discr_val;  // xval2 = 9
     * auto xval3 = translator2.translate ( "N/A" ).discr_val;
     * // here xval3 corresponds to a missing value, hence it is equal to
     * // std::numeric_limits<size_t>::max ()
     *
     * // trying to translate a string which is outside the domain of var will
     * // raise Exception NotFound
     * translator2.translate ( "20" ); // NotFound
     * @endcode
     *
     * @ingroup learning_database
     */
    class DBTranslator4RangeVariable: public DBTranslator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor without any initial variable
      /** When using this constructor, it is assumed implicitly that the
       * dictionary contained into the translator is editable. So, when reading
       * the database, if we observe a value that has not been encountered
       * before, we update the range of the dictionary of the translator (hence
       * that of the variable contained by the translator).
       * @param missing_symbols the set of symbols in the dataset
       * representing missing values
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       */
      DBTranslator4RangeVariable(const std::vector< std::string >& missing_symbols,
                                 std::size_t                       max_dico_entries
                                 = std::numeric_limits< std::size_t >::max());

      /// default constructor without any initial variable nor missing symbols
      /** When using this constructor, it is assumed implicitly that the
       * dictionary contained into the translator is editable. So, when reading
       * the database, if we observe a value that has not been encountered
       * before, we update the range of the dictionary of the translator (hence
       * that of the variable contained by the translator).
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       */
      DBTranslator4RangeVariable(std::size_t max_dico_entries
                                 = std::numeric_limits< std::size_t >::max());

      /// default constructor with a range variable as translator
      /** @param var a range variable which will be used for translations.
       * The translator keeps a copy of this variable
       * @param missing_symbols the set of symbols in the dataset
       * representing missing values
       * @param editable_dictionary the mode in which the translator will perform
       * translations: when false (the default), the translation of a string
       * that does not correspond to an integer within the range of var will
       * raise a NotFound exception; when true, the translator will try to
       * expand the domain of the RangeVariable so that the number represented in
       * the string belongs to this domain ((and therefore to the dictionary)
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       * @warning If the variable contained into the translator has a value in
       * the range that is equal to a missing value symbol, the range value will
       * be taken into account in the translations, not the missing value.
       */
      DBTranslator4RangeVariable(const RangeVariable&              var,
                                 const std::vector< std::string >& missing_symbols,
                                 const bool                        editable_dictionary = false,
                                 std::size_t                       max_dico_entries
                                 = std::numeric_limits< std::size_t >::max());

      /** @brief default constructor with a range variable as translator
       * but without missing symbols
       *
       * @param var a range variable which will be used for translations.
       * The translator keeps a copy of this variable
       * @param editable_dictionary the mode in which the translator will perform
       * translations: when false (the default), the translation of a string
       * that does not correspond to an integer within the range of var will
       * raise a NotFound exception; when true, the translator will try to
       * expand the domain of the RangeVariable so that the number represented in
       * the string belongs to this domain ((and therefore to the dictionary)
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       * @warning If the variable contained into the translator has a value in
       * the range that is equal to a missing value symbol, the range value will
       * be taken into account in the translations, not the missing value.
       */
      DBTranslator4RangeVariable(const RangeVariable& var,
                                 const bool           editable_dictionary = false,
                                 std::size_t          max_dico_entries
                                 = std::numeric_limits< std::size_t >::max());

      /// copy constructor
      DBTranslator4RangeVariable(const DBTranslator4RangeVariable& from);

      /// move constructor
      DBTranslator4RangeVariable(DBTranslator4RangeVariable&& from);

      /// virtual copy constructor
      virtual DBTranslator4RangeVariable* clone() const;

      /// destructor
      virtual ~DBTranslator4RangeVariable();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTranslator4RangeVariable& operator=(const DBTranslator4RangeVariable& from);

      /// move operator
      DBTranslator4RangeVariable& operator=(DBTranslator4RangeVariable&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the translation of a string
      /** This method tries to translate a given string into the
       * DBTranslatedValue  that should be stored into a databaseTable. If the
       * translator cannot find the translation in its current dictionary, then
       * two situations can obtain:
       * -# if the translator is not in an editable dictionary mode, then the
       *    translator raises a NotFound exception.
       * -# if the translator is in an editable dictionary mode, i.e., it is
       *    allowed to update its dictionary, then it tries to update the range
       *    of its dictionary to include the new value. Upon success, it returns
       *    the translated value, otherwise, it raises either:
       *    - a TypeError exception if the string cannot be converted into a
       *      value that can be inserted into the dictionary
       *    - an OperationNotAllowed exception if the translation would induce
       *      incoherent behavior (e.g., a translator that
       *      contains a variable whose domain is [x,y] as well as a missing
       *      value symbol z \f$\in\f$ [x,y]).
       *    - a SizeError exception if the number of entries in the dictionary,
       *      i.e., the domain size of the RangeVariable, has already reached
       *      its maximum.
       *
       * @warning Note that missing values (i.e., string encoded as missing
       * symbols) are translated as std::numeric_limits<std::size_t>::max ().
       * @warning If the variable contained into the translator has a value in
       * its range equal to a missing value symbol, then this value will be
       * taken into account in the translation, not the missing value.
       * @return the translated value of the string to be stored into a
       * DatabaseTable
       * @throws UnknownLabelInDatabase is raised if the translation cannot
       * be found and the translator is not in an editable dictionary mode.
       * @throws SizeError is raised if the number of entries (the range) in
       * the dictionary has already reached its maximum.
       * @throws TypeError is raised if the translation cannot be found and
       * the translator is in an editable dictionary mode and the string does
       * not correspond to an integer.
       * @throws OperationNotAllowed exception is raised if the translation
       * cannot be found and the insertion of the string into the translator's
       * dictionary fails because it would induce incoherent behavior (e.g.,
       * a translator that contains a variable whose domain is {x,y,z,t} as
       * well as a missing value symbol z).
       */
      virtual DBTranslatedValue translate(const std::string& str) final;

      /// returns the original value for a given translation
      /** @return the string that was translated into a given DBTranslatedValue.
       * @throws UnknownLabelInDatabase is raised if this original value cannot
       * be found */
      virtual std::string translateBack(const DBTranslatedValue translated_val) const final;

      /// returns the domain size of a variable corresponding to the translations
      /** Returns the size of the range of the variable. */
      virtual std::size_t domainSize() const final;

      /** @brief indicates whether a reordering is needed to make the
       * translations sorted by increasing numbers
       *
       * When constructing dynamically its dictionary, the translator may
       * assign wrong DBTranslatedValue values to strings. For instance, a
       * translator reading sequentially integer strings 2, 1, 3, may map
       * 2 into DBTranslatedValue{std::size_t(0)},
       * 1 into DBTranslatedValue{std::size_t(1)} and
       * 3 into DBTranslatedValue{std::size_t(2)}, resulting in random variables
       * having domain {2,1,3}. The user may prefer having domain {1,2,3}, i.e.,
       * a domain specified with increasing values. This requires a
       * reordering. Method needsReodering() returns a Boolean indicating
       * whether such a reordering should be performed or whether the current
       * order is OK.
       */
      virtual bool needsReordering() const final;

      /** @brief performs a reordering of the dictionary and returns a mapping
       * from the old translated values to the new ones.
       *
       * When a reordering is needed, i.e., string values must be translated
       * differently, Method reorder() computes how the translations should be
       * changed. It updates accordingly the dictionary and returns the mapping
       * that enables changing the old dictionary values into the new ones.
       */
      virtual HashTable< std::size_t, std::size_t > reorder() final;

      /// returns the variable stored into the translator
      virtual const RangeVariable* variable() const final;

      /// returns the translation of a missing value
      virtual DBTranslatedValue missingValue() const final;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the RangeVariable assigned to the translator, if any
      RangeVariable _variable_;

      // assign to each integer missing symbol a Boolean indicating whether
      // we already translated it or not. If we translated it, then we cannot
      // change the range of the variable so that this range contains the symbol.
      HashTable< std::string, bool > _status_int_missing_symbols_;

      // the set of translations of the integer missing symbols found so far
      Set< long > _translated_int_missing_symbols_;

      // a string containing a non int missing symbol
      // (useful for back translations)
      std::string _nonint_missing_symbol_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };


  } /* namespace learning */

} /* namespace gum */


/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/DBTranslator4RangeVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_TRANSLATOR_4_RANGE_VARIABLE_H */
