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
 * @brief The set of translators stored into a row filter
 *
 * When learning Bayesian networks, the records of the train database are
 * used to construct contingency tables that are either exploited in
 * statistical conditional independence tests or in scores. In both cases,
 * the values observed in the records must be translated into indices in
 * the finite domain of the corresponding random variables. DBCellTranslators
 * are used for this purpose. To make the parsing of all the columns of the
 * database easier, all the DBCellTranslators used are gathered into a single
 * DBRowTranslatorSet.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_ROW_TRANSLATOR_SET_H
#define GUM_LEARNING_DB_ROW_TRANSLATOR_SET_H

#include <vector>
#include <limits>

#include <agrum/config.h>
#include <agrum/learning/database/DBCellTranslator.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/filteredRow.h>

namespace gum {

  namespace learning {

    /** @class DBRowTranslatorSet
     * @ingroup learning_group
     * @brief the class for packing the translators used to preprocess
     * the database
     *
     * When learning Bayesian networks, the records of the train database are
     * used to construct contingency tables that are either exploited in
     * statistical conditional independence tests or in scores. In both cases,
     * the values observed in the records must be translated into indices in
     * the finite domain of the corresponding random variables.
     * DBCellTranslators are used for this purpose. To make the parsing of all
     * the columns of the database easier, all the DBCellTranslators used are
     * gathered into a DBRowTranslatorSet.
     */
    template <typename Translator>
    class DBRowTranslatorSet {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowTranslatorSet();

      /// copy constructor
      DBRowTranslatorSet( const DBRowTranslatorSet<Translator>& from );

      /// move constructor
      DBRowTranslatorSet( DBRowTranslatorSet<Translator>&& from );

      /// destructor
      ~DBRowTranslatorSet() noexcept;

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBRowTranslatorSet<Translator>&
      operator=( const DBRowTranslatorSet<Translator>& );

      /// move operator
      DBRowTranslatorSet<Translator>&
      operator=( DBRowTranslatorSet<Translator>&& );

      /// returns the ith translator
      /** @throws NotFound is raised if there are fewer than i translators in
       * the translator set */
      Translator& operator[]( Idx i );

      /// returns the ith translator
      /** @throws NotFound is raised if there are fewer than i translators in
       * the translator set */
      const Translator& operator[]( Idx i ) const;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// inserts new translators at the end of the set
      /** insert a new translator that will read consecutive columns starting
       * from deb_cols. If we wish to insert several translators, use an
       * nb_times different from 1. In this case, the other translators will
       * read columns of the database deduced from deb_cols by applying
       * increment incr. */
      void insertTranslator( Idx  deb_cols,
                             Size nb_times = 1,
                             Idx  incr = std::numeric_limits<Idx>::max () );

      /// inserts new translators at the end of the set
      /** insert a new translator that will read consecutive columns starting
       * from deb_cols. If we wish to insert several translators, use an
       * nb_times different from 1. In this case, the other translators will
       * read columns of the database deduced from deb_cols by applying
       * increment incr. */
      template <class NewTranslator>
#ifndef DOXYGEN_SHOULD_SKIP_THIS
      typename std::enable_if<isDBCellTranslator<NewTranslator>::value, void>::type
#else // for the doxygen documentation:
      void 
#endif // DOXYGEN_SHOULD_SKIP_THIS
      insertTranslator( const NewTranslator& translator,
          Idx  deb_cols,
          Size nb_times = 1,
          Idx  incr = std::numeric_limits<Idx>::max () );
      
      /// execute all the translations on the current database row
      void translate();

      /// initialize the cell translators by parsing once the database
      /** If initialization is required, this method is called for each row
       * of the database used for initialization. */
      void initialize();

      /// performs a post initialization after the database parsing
      void postInitialize();

      /** @brief indicates whether some of the translators need an initial
       * parsing
       * of the database to initialize itself */
      bool requiresInitialization() const noexcept;

      /// push back the number of modalities of the variables of the output
      /// columns
      void modalities( std::vector<Size>& modals ) const;

      /// sets the input row that shall be read by all the cell translators
      void setInputRow( const DBRow& row ) noexcept;

      /// returns the current input DBRow
      const DBRow& inputRow() const;

      /// returns the current output row
      FilteredRow& outputRow() noexcept;

      /// remove all the translators from the vector
      void clear() noexcept;

      /// returns the name of the jth value of the ith column
      std::string translateBack( Idx col, Idx translated_val ) const;

      /// returns the size of the input as used by the cell translators
      Size inputSize() const noexcept;

      /// returns the size of the output of the cell translators
      Size outputSize() const noexcept;

      /// returns the number of translators stored into the set
      Size nbTranslators() const noexcept;

      /// @}

      private:
      /// the vector of the cell filters
      std::vector<Translator*> __translators;

      /// the size of the output
      Size __output_size{0};

      /// the output row into which the translators write their output
      FilteredRow __output_row;
    };

  } /* namespace learning */

} /* namespace gum */

// always include templated implementation
#include <agrum/learning/database/DBRowTranslatorSet_tpl.h>

#endif /* GUM_LEARNING_DB_ROW_TRANSLATOR_SET_H */
