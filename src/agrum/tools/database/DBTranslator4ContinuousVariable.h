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
 * @brief The databases' cell translators for continuous variables
 *
 * @author Christophe GONZALES(_at_AMU) and Pierre-Henri WUILLEMIN(_at_LIP6)
 */
#ifndef GUM_LEARNING_DB_TRANSLATOR_4_CONTINUOUS_VARIABLE_H
#define GUM_LEARNING_DB_TRANSLATOR_4_CONTINUOUS_VARIABLE_H

#include <string>

#include <agrum/agrum.h>
#include <agrum/tools/database/DBTranslator.h>
#include <agrum/tools/variables/continuousVariable.h>


namespace gum {

  namespace learning {


    /** @class DBTranslator4ContinuousVariable
     * @headerfile DBTranslator4ContinuousVariable.h <agrum/tools/database/DBTranslator4ContinuousVariable.h>
     * @brief The databases' cell translators for continuous variables
     *
     * Translators are used by DatabaseTable instances to transform datasets'
     * strings into DBTranslatedValue instances. The point is that strings are
     * not adequate for fast learning, they need to be preprocessed into a type
     * that can be analyzed quickly (the so-called DBTranslatedValue type).
     *
     * A DBTranslator4ContinuousVariable is a translator that contains and
     * exploits a ContinuousVariable for translations. Each time a string needs
     * be translated, we ask the ContinuousVariable whether it belongs to its
     * domain (which is supposed to be of type [x_min,x_max]). If this is the
     * case, then the DBTranslatedValue corresponding to the translation of the
     * string contains the floating point number specified in the string.
     *
     * @par Here is an example of how to use this class:
     * @code
     * // create the translator, with possible missing symbols: "N/A" and "???"
     * // i.e., each time the translator reads a "N/A" or a "???" string, it
     * // won't translate it into a number but into a missing value.
     * std::vector<std::string> missing { "N/A", "???" };
     * gum::learning::DBTranslator4ContinuousVariable translator ( missing );
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
     * // get the variable stored within the translator
     * const gum::ContinuousVariable<float>* var =
     *   dynamic_cast<const gum::ContinuousVariable<float>*>
     *     ( translator.variable () );
     *
     * // it is possible to create a translator for an already known variable.
     * // In this case, by default, the translator is not in editable mode, but
     * // this behavior can be changed passing the right arguments to the
     * // constructor of the translator, or using the setEditableDictionaryMode
     * // method. Here, we create a continuous variable whose domain is [-2,10]
     * gum::ContinuousVariable<float> var ( "X", "", -2, 10 );
     * gum::learning::DBTranslator4ContinuousVariable translator2 (var,missing);
     *
     * float xval1 = translator2.translate ( "-1.4" ).cont_val; // xval1 = -1.4
     * float xval2 = translator2.translate ( "7" ).cont_val;    // xval2 = 7
     * float xval3 = translator2.translate ( "N/A" ).cont_val;
     * // here xval3 corresponds to a missing value, hence it is equal to
     * // std::numeric_limits<float>::max ()
     *
     * // trying to translate a string which is outside the domain of var will
     * // raise Exception NotFound
     * translator2.translate ( "20" ); // NotFound
     * @endcode
     *
     * @ingroup learning_database
     */
    class DBTranslator4ContinuousVariable: public DBTranslator {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor without any initial variable
      /** When using this constructor, it is assumed implicitly that the
       * continuous variable has a range from minus infinity to plus infinity.
       * If the fit_range parameter is on, the range of the variable is updated
       * so that it precisely fits the range of the observed values in the
       * database.
       * @param  missing_symbols the set of symbols in the database
       * representing missing values
       * @param fit_range if true, the range of the variable is updated
       * so that it precisely fits the range of the observed values in the
       * database, else the range is kept to (-inf,inf)
       */
      DBTranslator4ContinuousVariable(const std::vector< std::string >& missing_symbols,
                                      const bool                        fit_range = false);

      /// default constructor without any initial variable nor missing symbol
      /** When using this constructor, it is assumed implicitly that the
       * continuous variable has a range from minus infinity to plus infinity.
       * If the fit_range parameter is on, the range of the variable is updated
       * so that it precisely fits the range of the observed values in the
       * database.
       * @param fit_range if true, the range of the variable is updated
       * so that it precisely fits the range of the observed values in the
       * database, else the range is kept to (-inf,inf)
       */
      DBTranslator4ContinuousVariable(const bool fit_range = false);

      /// default constructor with a continuous variable as translator
      /** @param var a continuous variable that will be used for
       * translations. The translator keeps a copy of this variable
       * @param  missing_symbols the set of symbols in the database
       * representing missing values
       * @param fit_range if true, the range of the variable is updated
       * so that it precisely fits the range of the observed values in the
       * database, else the range is kept to (-inf,inf)
       * @warning If a missing value symbol is a number included in the range
       * of the continuous variable, it will be discarded. If the fit_range
       * parameter is on, the range of the variable is updated so that it
       * can contain the range of the observed values in the database. */
      template < typename GUM_SCALAR >
      DBTranslator4ContinuousVariable(const ContinuousVariable< GUM_SCALAR >& var,
                                      const std::vector< std::string >&       missing_symbols,
                                      const bool                              fit_range = false);

      /** @brief default constructor with a continuous variable as translator
       * but without missing symbol
       *
       * @param var a continuous variable that will be used for
       * translations. The translator keeps a copy of this variable
       * @param fit_range if true, the range of the variable is updated
       * so that it precisely fits the range of the observed values in the
       * database, else the range is kept to (-inf,inf)
       * @warning If a missing value symbol is a number included in the range
       * of the continuous variable, it will be discarded. If the fit_range
       * parameter is on, the range of the variable is updated so that it
       * can contain the range of the observed values in the database. */
      template < typename GUM_SCALAR >
      DBTranslator4ContinuousVariable(const ContinuousVariable< GUM_SCALAR >& var,
                                      const bool                              fit_range = false);

      /// default constructor with a IContinuous variable as translator
      /** @param var a IContinuous variable that will be used for
       * translations. The translator keeps a copy of this variable
       * @param  missing_symbols the set of symbols in the database
       * representing missing values
       * @param fit_range if true, the range of the variable is updated
       * so that it precisely fits the range of the observed values in the
       * database, else the range is kept to (-inf,inf)
       * @warning If a missing value symbol is a number included in the range
       * of the continuous variable, it will be discarded. If the fit_range
       * parameter is on, the range of the variable is updated so that it
       * can contain the range of the observed values in the database. */
      DBTranslator4ContinuousVariable(const IContinuousVariable&        var,
                                      const std::vector< std::string >& missing_symbols,
                                      const bool                        fit_range = false);

      /** @brief default constructor with a IContinuous variable as translator
       * but without missing symbol
       *
       * @param var a IContinuous variable that will be used for
       * translations. The translator keeps a copy of this variable
       * @param fit_range if true, the range of the variable is updated
       * so that it precisely fits the range of the observed values in the
       * database, else the range is kept to (-inf,inf)
       * @warning If a missing value symbol is a number included in the range
       * of the continuous variable, it will be discarded. If the fit_range
       * parameter is on, the range of the variable is updated so that it
       * can contain the range of the observed values in the database. */
      DBTranslator4ContinuousVariable(const IContinuousVariable& var, const bool fit_range = false);

      /// copy constructor
      DBTranslator4ContinuousVariable(const DBTranslator4ContinuousVariable& from);

      /// move constructor
      DBTranslator4ContinuousVariable(DBTranslator4ContinuousVariable&& from);

      /// virtual copy constructor
      virtual DBTranslator4ContinuousVariable* clone() const;

      /// destructor
      virtual ~DBTranslator4ContinuousVariable();

      /// @}


      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTranslator4ContinuousVariable& operator=(const DBTranslator4ContinuousVariable& from);

      /// move operator
      DBTranslator4ContinuousVariable& operator=(DBTranslator4ContinuousVariable&& from);

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
       *    allowed to update its dictionary, then it tries to update the range
       *    of its dictionary to include the new value. Upon success, it returns
       *    the translated value, otherwise, it raises either:
       *    - a TypeError exception if the string cannot be converted into a
       *      value that can be inserted into the dictionary
       *    - an OperationNotAllowed exception if the translation would induce
       *      incoherent behavior (e.g., a DBTranslator4ContinuousVariable that
       *      contains a variable whose domain is [x,y] as well as a missing
       *      value symbol z \f$\in\f$ [x,y]).

       * @warning Note that missing values (i.e., string encoded as missing
       * symbols) are translated as std::numeric_limits<float>::max ().
       * @warning If the variable contained into the translator has a value in
       * its domain equal to a missing value symbol, this value will be taken
       * into account in the translations, not the missing value.
       * @return the translated value of the string to be stored into a
       * DatabaseTable
       * @throws UnknownLabelInDatabase is raised if the number represented by
       * the string is out of the range of the continuous variable and the
       * translator is not in an editable dictionary mode.
       * @throws OperationNotAllowed exception is raised if the translation
       * cannot be found and the insertion of the string into the translator's
       * dictionary fails because it would induce incoherent behavior (e.g.,
       * a DBTranslator4ContinuousVariable that contains a variable whose domain
       * is [x,y] as well as a missing value symbol z \f$\in\f$ [x,y]).
       * @throws TypeError is raised if the translation cannot be found and
       * the insertion of the string into the translator's dictionary fails
       * due to str being impossible to be converted into an appropriate type. */
      virtual DBTranslatedValue translate(const std::string& str) final;

      /// returns the original value for a given translation
      /** @return the string that was translated into a given DBTranslatedValue.
       * @throws UnknownLabelInDatabase is raised if this original value is
       * outside the domain of the continuous variable stored within the
       * translator */
      virtual std::string translateBack(const DBTranslatedValue translated_val) const final;

      /// returns std::numeric_limits<std::size_t>::max ()
      virtual std::size_t domainSize() const final;

      /// indicates that the translations should never be reordered
      virtual bool needsReordering() const final;

      /** @brief returns an empty mapping, indicating that old tanslations
       * are equal to the newly reordered ones. */
      virtual HashTable< std::size_t, std::size_t > reorder() final;

      /// returns the variable stored into the translator
      virtual const IContinuousVariable* variable() const final;

      /// returns the translation of a missing value
      virtual DBTranslatedValue missingValue() const final;

      /// @}


#ifndef DOXYGEN_SHOULD_SKIP_THIS

      private:
      // the ContinuousVariable really used by the translator. As its values
      // are floats, this speeds-up translations
      ContinuousVariable< float > _variable_;

      // the ContinuousVariablee returned by method variable ()
      // We must return a IContinuousVariable because the user may have
      // saved into the translator a ContinuousVariable<X>, with X != float
      IContinuousVariable* _real_variable_;

      // assign to each float missing symbol a Boolean indicating whether
      // we already translated it or not. If we translated it, then we cannot
      // change the range of the variable so that this range contains the symbol.
      HashTable< std::string, bool > _status_float_missing_symbols_;

      // a string containing a non real missing symbol
      // (useful for back translations)
      std::string _nonfloat_missing_symbol_;

      // indicates whether we should fit the range of the observed values
      bool _fit_range_;

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
    };

  } /* namespace learning */

} /* namespace gum */


// always include the template implementation
#include <agrum/tools/database/DBTranslator4ContinuousVariable_tpl.h>

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#  include <agrum/tools/database/DBTranslator4ContinuousVariable_inl.h>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_DB_TRANSLATOR_4_CONTINUOUS_VARIABLE_H */
