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
 * @brief The databases' cell translators for labelized variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_TRANSLATOR_4_LABELIZED_VARIABLE_H
#define GUM_LEARNING_DB_TRANSLATOR_4_LABELIZED_VARIABLE_H


#include <agrum/agrum.h>
#include <agrum/tools/database/DBTranslator.h>
#include <agrum/tools/database/DBCell.h>
#include <agrum/tools/variables/labelizedVariable.h>


namespace gum {

  namespace learning {


    /** @class DBTranslator4LabelizedVariable
     * @headerfile DBTranslator4LabelizedVariable.h <agrum/tools/database/DBTranslator4LabelizedVariable.h>
     * @brief The databases' cell translators for labelized variables
     *
     * Translators are used by DatabaseTable instances to transform datasets'
     * strings into DBTranslatedValue instances. The point is that strings are
     * not adequate for fast learning, they need to be preprocessed into a type
     * that can be analyzed quickly (the so-called DBTranslatedValue type).
     *
     * A DBTranslator4LabelizedVariable is a translator that contains and
     * exploits a LabelizedVariable for translations. Each time a string needs
     * be translated, we ask the LabelizedVariable to provide the index of
     * the label corresponding to the string. This index, when encoded into a
     * DBTranslatedValue, is precisely the translation of the string.
     *
     * @par Here is an example of how to use this class:
     * @code
     * // create the translator, with possible missing symbols: "N/A" and "???"
     * // i.e., each time the translator reads a "N/A" or a "???" string, it
     * // won't translate it into a number but into a missing value.
     * std::vector<std::string> missing { "N/A", "???" };
     * gum::learning::DBTranslator4LabelizedVariable translator ( missing );
     *
     * // gets the DBTranslatedValue corresponding to some strings:
     * auto val1 = translator.translate("xxx");
     * auto val2 = translator.translate("zzz");
     * auto val3 = translator << "yyy";
     * auto val2bis = translator.translate( "zzz" );
     * // In the first assignment, the translator initially contains an empty
     * // domain LabelizedVariable and it is by default in editable mode. So
     * // we add a new label "xxx" to the LabelizedVariable contained in the
     * // translator, and the index of this label is 0. Therefore, we have that
     * // val1 = DBTranslatedValue {std::size_t(0)}. Similarly, the assignments of
     * // val2 and val3 induce the additions of labels "zzz" and "yyy" into the
     * // LabelizedVariable. As a result, val2 = DBTranslatedValue {std::size_t(1)}
     * // and val3 = DBTranslatedValue {std::size_t(2)}. In the assigment of
     * // val2bis, label "zzz" already exists and its index is equal to 1. So
     * // val2bis = DBTranslatedValue {std::size_t(1)}.
     *
     * // add the numbers assigned to val1, val2, val3
     * std::size_t sum = val1.discr_val + val2.discr_val + val3.discr_val;
     *
     * // translate missing values: val4 and val5 will be equal to:
     * // DBTranslatedValue { std::numeric_limits<std::size_t>::max () }
     * auto val4 = translator << "N/A";
     * auto val5 = translator.translate ( "???" );
     *
     * // given a DBTranslatedValue that is supposed to contain a label's index,
     * // get the corresponding label.
     * std::string str;
     * str = translator.translateBack ( val1 );               // str = "xxx"
     * str = translator >> val2;                              // str = "zzz"
     * str = translator >> gum::learning::DBTranslatedValue {std::size_t(1)};
     *                                                        // str = "zzz"
     * // if there is no such label, Exception NotFound is raised:
     * str = translator >> gum::learning::DBTranslatedValue {std::size_t(4)};
     *
     * // translate back missing values: the string will corresponds to one of
     * // the missing symbols known to the translator
     * str = translator >> val4; // str = "N/A" or "???"
     * str = translator >> val5; // str = "N/A" or "???"
     *
     * // get the domain size of the variable stored into the translator
     * std::size_t size = translator.domainSize (); // size = 3
     *
     * // get the variable stored within the translator
     * const gum::LabelizedVariable* var =
     *   dynamic_cast<const gum::LabelizedVariable*> ( translator.variable () );
     *
     * // it is possible to create a translator for an already known variable.
     * // In this case, by default, the translator is not in editable mode, but
     * // this behavior can be changed passing the right arguments to the
     * // constructor of the translator, or using the setEditableDictionaryMode
     * // method.
     * gum::LabelizedVariable var ( "X1", "", 0 );
     * var.addLabel ( "toto" );
     * var.addLabel ( "titi" );
     * var.addLabel ( "tutu" );
     * gum::learning::DBTranslator4LabelizedVariable translator2 (var,missing);
     *
     * std::size_t index1 = translator2.translate ( "toto" ).discr_val; // = 0
     * std::size_t index2 = translator2.translate ( "tutu" ).discr_val; // = 2
     * std::size_t index3 = translator2.translate ( "N/A" ).discr_val;
     * // here index3 corresponds to the index of a missing value, hence it is
     * // equal to std::numeric_limits<std::size_t>::max ()
     *
     * // trying to translate a string which is not a label of var will raise
     * // Exception NotFound
     * translator2.translate ( "xxx" ); // NotFound
     * @endcode
     *
     * @ingroup learning_database
     */
    class DBTranslator4LabelizedVariable: public DBTranslator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor without any initial variable
      /** When using this constructor, it is assumed implicitly that the
       * dictionary contained into the translator is editable. So, when reading
       * the database, if we observe a label that has not been encountered
       * before, we add it into the dictionary of the translator (hence into
       * the variable contained by the translator).
       * @param  missing_symbols the set of symbols in the database
       * representing missing values
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       */
      DBTranslator4LabelizedVariable(const std::vector< std::string >& missing_symbols,
                                     std::size_t                       max_dico_entries
                                     = std::numeric_limits< std::size_t >::max());

      /// default constructor without any initial variable nor missing symbols
      /** When using this constructor, it is assumed implicitly that the
       * dictionary contained into the translator is editable. So, when reading
       * the database, if we observe a label that has not been encountered
       * before, we add it into the dictionary of the translator (hence into
       * the variable contained by the translator).
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       */
      DBTranslator4LabelizedVariable(std::size_t max_dico_entries
                                     = std::numeric_limits< std::size_t >::max());

      /// default constructor with a labelized variable as translator
      /** @param var a labelized variable whose labels will be used for
       * translations. The translator keeps a copy of this variable
       * @param  missing_symbols the set of symbols in the database
       * representing missing values
       * @param editable_dictionary the mode in which the translator will perform
       * translations: when false (the default), the translation of a string
       * that does not correspond to a label of var will raise a NotFound
       * exception; when true, the translator will try to add the string as
       * a new label into var (and therefore into the dictionary)
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       * @warning If the variable contained into the translator has a label
       * equal to a missing value symbol, the label will be taken into
       * account in the translations, not the missing value.
       */
      DBTranslator4LabelizedVariable(const LabelizedVariable&          var,
                                     const std::vector< std::string >& missing_symbols,
                                     const bool                        editable_dictionary = false,
                                     std::size_t                       max_dico_entries
                                     = std::numeric_limits< std::size_t >::max());

      /** @brief default constructor with a labelized variable as translator
       * but without missing symbols
       *
       * @param var a labelized variable whose labels will be used for
       * translations. The translator keeps a copy of this variable
       * @param editable_dictionary the mode in which the translator will perform
       * translations: when false (the default), the translation of a string
       * that does not correspond to a label of var will raise a NotFound
       * exception; when true, the translator will try to add the string as
       * a new label into var (and therefore into the dictionary)
       * @param max_dico_entries the max number of entries that the dictionary
       * can contain. If we try to add new entries in the dictionary, this will
       * be considered as an error and a SizeError exception will be raised
       * @warning If the variable contained into the translator has a label
       * equal to a missing value symbol, the label will be taken into
       * account in the translations, not the missing value.
       */
      DBTranslator4LabelizedVariable(const LabelizedVariable& var,
                                     const bool               editable_dictionary = false,
                                     std::size_t              max_dico_entries
                                     = std::numeric_limits< std::size_t >::max());

      /// copy constructor
      DBTranslator4LabelizedVariable(const DBTranslator4LabelizedVariable& from);

      /// move constructor
      DBTranslator4LabelizedVariable(DBTranslator4LabelizedVariable&& from);

      /// virtual copy constructor
      virtual DBTranslator4LabelizedVariable* clone() const;

      /// destructor
      virtual ~DBTranslator4LabelizedVariable();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTranslator4LabelizedVariable& operator=(const DBTranslator4LabelizedVariable& from);

      /// move operator
      DBTranslator4LabelizedVariable& operator=(DBTranslator4LabelizedVariable&& from);

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns the translation of a string
      /** This method tries to translate a given string into the
       * DBTranslatedValue that should be stored into a DatabaseTable. If the
       * translator cannot find the translation in its current dictionary, then
       * two situations can obtain:
       * -# if the translator is not in an editable dictionary mode, then the
       *    translator raises a NotFound exception.
       * -# if the translator is in an editable dictionary mode, i.e., it is
       *    allowed to update its dictionary, then it tries to add the string
       *    as a new value in the dictionary (or equivalently as a new label
       *    into its labelized variable). Upon success, it returns the
       *    translated value, otherwise, it raises a SizeError exception if the
       *    number of entries in the dictionary has already reached its maximum.
       *
       * @warning Note that missing values (i.e., string encoded as missing
       * symbols) are translated as std::numeric_limits<std::size_t>::max ().
       * @warning If the variable contained into the translator has a label
       * equal to a missing value symbol, the label will be taken into
       * account in the translation, not the missing value.
       * @param str the string that the translator will try to translate
       * @return the translated value of the string to be stored into a
       * DatabaseTable
       * @throws UnknownLabelInDatabase is raised if the translation cannot be
       * found and the translator is not in an editable dictionary mode.
       * @throws SizeError is raised if the number of entries in the dictionary
       * has already reached its maximum.
       */
      virtual DBTranslatedValue translate(const std::string& str) final;

      /// returns the original value for a given translation
      /** @return the string that was translated into a given DBTranslatedValue.
       * @param translated_val a DBTranslatedValue which is supposed to contain
       * the index of a label of the LabelizedVariable contained in the
       * translator. The method should return this label
       * @throws UnknownLabelInDatabase is raised if this original value
       * cannot be found */
      virtual std::string translateBack(const DBTranslatedValue translated_val) const final;

      /// returns the domain size of a variable corresponding to the translations
      /** Assume that the translator has been fed with the observed values of
       * a random variable. Then it has produced a set of translated values. The
       * latter define the domain of the variable. The domainSize is the size
       * of this domain. In other words, this corresponds to the number of
       * labels of the LabelizedVariable contained in the translator.
       * Note that missing values are not taken into account in the domain
       * sizes. */
      virtual std::size_t domainSize() const final;

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
      virtual bool needsReordering() const final;

      /** @brief performs a reordering of the dictionary and returns a mapping
       * from the old translated values to the new ones.
       *
       * When a reordering is needed, i.e., string values must be translated
       * differently. Method reorder() computes how the translations should be
       * changed. It updates accordingly the dictionary and returns the mapping
       * that enables changing the old dictionary values into the new ones.
       * @warning If there is no reordering to perform, the method returns
       * an empty hashtable. */
      virtual HashTable< std::size_t, std::size_t > reorder() final;

      /// returns the variable stored into the translator
      virtual const LabelizedVariable* variable() const final;

      /// returns the translation of a missing value
      virtual DBTranslatedValue missingValue() const final;

      /// @}

#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the LabelizedVariable assigned to the translator, if any
      LabelizedVariable _variable_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };


  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/DBTranslator4LabelizedVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_TRANSLATOR_4_LABELIZED_VARIABLE_H */
