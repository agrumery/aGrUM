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
 * @brief A cell translator translating any DBCell
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_CELL_TRANSLATOR_UNIVERSAL_H
#define GUM_LEARNING_CELL_TRANSLATOR_UNIVERSAL_H

#include <string>
#include <vector>

#include <agrum/config.h>
#include <agrum/core/sequence.h>
#include <agrum/core/bijection.h>
#include <agrum/learning/database/DBCellTranslator.h>

namespace gum {

  namespace learning {

    /** @class CellTranslatorUniversal
     * @ingroup learning_group
     * @brief A cell translator translating any DBCell
     */
    class CellTranslatorUniversal : public DBCellTranslator<1, 1> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      /** @param values The user can specify the values which the cell translator
       * should expect. If none is specified, the cell translator will determine
       * the values stored in the database by parsing it. If values are specified
       * and check_database is set to false, then the translator will assume that
       * the sequence of values specified is exactly what it shall translate and,
       * therefore, it will not parse the database to initialize itself. However,
       * if some value encountered during translations does not belong to sequence
       * values, an exception will be raised during those translations. So,
       * be sure to give exactly the values the translator should encounter. Also
       * beware that the domain size of the variable the translator will process
       * is usually determined by asking the translator how many values it
       * encountered, so avoid providing values that do not exist in the database
       * (else this will certainly make scorings/independence tests incorrect).
       * If sequence "values" is specified and check_database is set to true, then
       * the translator will initialize itself by parsing the database and will
       * handle only those values it encountered in the database; sequence values
       * will then only be used to specifiy the order in which the values
       * encountered in the database will appear in the set of values of the
       * variable at the end of the learning. This feature proves particularly
       * useful for learning non-stationary Bayesian networks.
       * @param check_database indicates whether we shall parse the database to
       * initialize the translator. */
      CellTranslatorUniversal(Sequence<float> values = Sequence<float>(),
                              bool check_database = true);

      /// default constructor
      /** @param values The user can specify the values which the cell translator
       * should expect. If none is specified, the cell translator will determine
       * the values stored in the database by parsing it. If values are specified
       * and check_database is set to false, then the translator will assume that
       * the sequence of values specified is exactly what it shall translate and,
       * therefore, it will not parse the database to initialize itself. However,
       * if some value encountered during translations does not belong to sequence
       * values, an exception will be raised during those translations. So,
       * be sure to give exactly the values the translator should encounter. Also
       * beware that the domain size of the variable the translator will process
       * is usually determined by asking the translator how many values it
       * encountered, so avoid providing values that do not exist in the database
       * (else this will certainly make scorings/independence tests incorrect).
       * If sequence "values" is specified and check_database is set to true, then
       * the translator will initialize itself by parsing the database and will
       * handle only those values it encountered in the database; sequence values
       * will then only be used to specifiy the order in which the values
       * encountered in the database will appear in the set of values of the
       * variable at the end of the learning. This feature proves particularly
       * useful for learning non-stationary Bayesian networks.
       * @param check_database indicates whether we shall parse the database to
       * initialize the translator. */
      CellTranslatorUniversal(Sequence<std::string> values,
                              bool check_database = true);

      /// copy constructor
      CellTranslatorUniversal(const CellTranslatorUniversal &from);

      /// move constructor
      CellTranslatorUniversal(CellTranslatorUniversal &&from);

      /// virtual copy constructor
      virtual CellTranslatorUniversal *copyFactory() final;

      /// destructor
      virtual ~CellTranslatorUniversal();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      CellTranslatorUniversal &operator=(const CellTranslatorUniversal &from);

      /// move operator
      CellTranslatorUniversal &operator=(CellTranslatorUniversal &&from);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// perform the translation
      void translate();

      /// initialize the cell translator by a first database parsing
      /** If initialization is required, this method is called for each row
       * of the database used for initialization. */
      void initialize();

      /// perform a post initialization after the database parsing
      void postInitialize();

      /// add the number of modalities discovered in the database into a vector
      void modalities(std::vector<unsigned int> &modal) const noexcept;

      /// returns whether the translator needs a DB parsing to initialize itself
      bool requiresInitialization() const noexcept;

      /// returns a given value as stored within the database
      std::string translateBack(unsigned int col, unsigned int translated_val) const;

      /// returns the name of the variable(s) the translator has processed
      void variableNames(const std::vector<std::string> &db_var,
                         std::vector<std::string> &output_vars) const;

      /// specify the set of possible values (to do before creating the row filter)
      /** @param values The user can specify the values which the cell translator
       * should expect. If none is specified, the cell translator will determine
       * the values stored in the database by parsing it. If values are specified
       * and check_database is set to false, then the translator will assume that
       * the sequence of values specified is exactly what it shall translate and,
       * therefore, it will not parse the database to initialize itself. However,
       * if some value encountered during translations does not belong to sequence
       * values, an exception will be raised during those translations. So,
       * be sure to give exactly the values the translator should encounter. Also
       * beware that the domain size of the variable the translator will process
       * is usually determined by asking the translator how many values it
       * encountered, so avoid providing values that do not exist in the database
       * (else this will certainly make scorings/independence tests incorrect).
       * If sequence "values" is specified and check_database is set to true, then
       * the translator will initialize itself by parsing the database and will
       * handle only those values it encountered in the database; sequence values
       * will then only be used to specifiy the order in which the values
       * encountered in the database will appear in the set of values of the
       * variable at the end of the learning. This feature proves particularly
       * useful for learning non-stationary Bayesian networks.
       * @param check_database indicates whether we shall parse the database to
       * initialize the translator.
       * @warning this method should be used before the translator is copied into
       * the DBRowFilter, i.e., before the latter is created (as the creation of
       * the row filter induces the parsing of the database). */
      void setUserValues(const Sequence<float> &values, bool check_database = true);

      /// specify the set of possible values (to do before creating the row filter)
      /** @param values The user can specify the values which the cell translator
       * should expect. If none is specified, the cell translator will determine
       * the values stored in the database by parsing it. If values are specified
       * and check_database is set to false, then the translator will assume that
       * the sequence of values specified is exactly what it shall translate and,
       * therefore, it will not parse the database to initialize itself. However,
       * if some value encountered during translations does not belong to sequence
       * values, an exception will be raised during those translations. So,
       * be sure to give exactly the values the translator should encounter. Also
       * beware that the domain size of the variable the translator will process
       * is usually determined by asking the translator how many values it
       * encountered, so avoid providing values that do not exist in the database
       * (else this will certainly make scorings/independence tests incorrect).
       * If sequence "values" is specified and check_database is set to true, then
       * the translator will initialize itself by parsing the database and will
       * handle only those values it encountered in the database; sequence values
       * will then only be used to specifiy the order in which the values
       * encountered in the database will appear in the set of values of the
       * variable at the end of the learning. This feature proves particularly
       * useful for learning non-stationary Bayesian networks.
       * @param check_database indicates whether we shall parse the database to
       * initialize the translator.
       * @warning this method should be used before the translator is copied into
       * the DBRowFilter, i.e., before the latter is created (as the creation of
       * the row filter induces the parsing of the database). */
      void setUserValues(const Sequence<std::string> &values,
                         bool check_database = true);

      /// returns the set of translations for string values in the database
      const Bijection<int, unsigned int> &stringTranslations() const noexcept;

      /// returns the set of translations for number values in the database
      const Bijection<float, unsigned int> &numberTranslations() const noexcept;

      virtual std::string toString() const noexcept;

      /// @}

      private:
      /// the next max translated value
      unsigned int __max_value{0};

      /// the set of numbers found so far in the database
      Bijection<float, unsigned int> __numbers;

      /// the set of strings (actually their indices) found so far in the database
      Bijection<int, unsigned int> __strings;

      /// the sequence of string values specified by the user
      Sequence<std::string> *__str_user_values{nullptr};

      /// the sequence of numeric values specified by the user
      Sequence<float> *__num_user_values{nullptr};

      /// indicates whether the translator shall be initialized by DB parsing
      bool __check_database{true};
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/DBCellTranslators/cellTranslatorUniversal.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_CELL_TRANSLATOR_UNIVERSAL_H */
