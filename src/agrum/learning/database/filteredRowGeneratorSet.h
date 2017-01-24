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
 * @brief class for packing sets of generators into a row filter
 *
 * When learning Bayesian networks, the records of the train database are
 * used to construct contingency tables that are either exploited in
 * statistical conditional independence tests or in scores. To achieve this,
 * the values observed in the database's records are first translated by
 * DBRowTranslators into indices in the "finite" domain of the corresponding
 * random variables. Unfortunately, not all the values can be translated
 * by DBRowTranslators because these translations are performed "statically"
 * once at the beginning of the learning. Actually, some values need be
 * translted dynamically during the learning: this is the case for instance
 * of unobserved values that need be filled by EM-algorithms (hence resulting
 * in different translations during the learning phase) or continuous values
 * that can be discretized by adaptive discretization algorithms. To perform
 * these dynamic translations, aGrUM provides the FilteredRowGenerator
 * classes. Whenever a record is processed during learning, this record is
 * passed to a set of FilteredRowGenerators which will be called iteratively
 * to produce one or several records (e.g., EM produces multiple records)
 * that will be used to compute the contingency tables used by scores and
 * independence tests. To make the use of these FilteredRowGenerators, they
 * are packed into a FilteredRowGeneratorSet which, when called, will call
 * iteratively all the FilteredRowGenerators.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_FILTERED_ROW_GENERATOR_SET_H
#define GUM_LEARNING_FILTERED_ROW_GENERATOR_SET_H

#include <agrum/config.h>
#include <agrum/learning/database/filteredRow.h>
#include <agrum/learning/database/filteredRowGenerator.h>

namespace gum {

  namespace learning {


    /** @class FilteredRowGeneratorSet
     * @ingroup learning_group
     * @brief The class used to pack generators into a row filter
     *
     * When learning Bayesian networks, the records of the train database are
     * used to construct contingency tables that are either exploited in
     * statistical conditional independence tests or in scores. To achieve this,
     * the values observed in the database's records are first translated by
     * DBRowTranslators into indices in the "finite" domain of the corresponding
     * random variables. Unfortunately, not all the values can be translated
     * by DBRowTranslators because these translations are performed "statically"
     * once at the beginning of the learning. Actually, some values need be
     * translted dynamically during the learning: this is the case for instance
     * of unobserved values that need be filled by EM-algorithms (hence resulting
     * in different translations during the learning phase) or continuous values
     * that can be discretized by adaptive discretization algorithms. To perform
     * these dynamic translations, aGrUM provides the FilteredRowGenerator
     * classes. Whenever a record is processed during learning, this record is
     * passed to a set of FilteredRowGenerators which will be called iteratively
     * to produce one or several records (e.g., EM produces multiple records)
     * that will be used to compute the contingency tables used by scores and
     * independence tests. To make the use of these FilteredRowGenerators, they
     * are packed into a FilteredRowGeneratorSet which, when called, will call
     * iteratively all the FilteredRowGenerators.
     */
    template <typename Generator>
    class FilteredRowGeneratorSet {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      FilteredRowGeneratorSet ();
 
      /// copy constructor
      FilteredRowGeneratorSet ( const FilteredRowGeneratorSet<Generator>& from );

      /// move constructor
      FilteredRowGeneratorSet( FilteredRowGeneratorSet<Generator>&& from );

      /// destructor
      ~FilteredRowGeneratorSet ();

      /// @}
      

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      FilteredRowGeneratorSet<Generator>&
      operator=( const FilteredRowGeneratorSet<Generator>& );

      /// move operator
      FilteredRowGeneratorSet<Generator>&
      operator=( FilteredRowGeneratorSet<Generator>&& );

      /// returns the ith generator
      /** @throws NotFound is raised if there are fewer than i generators in
       * the generator set */
      Generator& operator[] ( Idx i );

      /// returns the ith generator
      /** @throws NotFound is raised if there are fewer than i generators in
       * the generator set */
      const Generator& operator[]( Idx i ) const;

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// inserts a new generator of type Generator at the end of the set
      void insertGenerator ();

      /// inserts a new generator of type Generator at the end of the set
      template <typename... Args>
      void emplaceGenerator ( Args&&... args );

      /// inserts a new generator at the end of the set
      template <class NewGenerator>
      void insertGenerator( const NewGenerator& generator );

      /// returns the number of generators 
      Size nbGenerators() const noexcept;
      
      /** @brief returns true if there are still rows that can be output
       * by the set of generators */
      bool hasRows();

      /// sets the input row from which the generators will create new rows
      bool setInputRow( FilteredRow& row );

      /// generate new rows from the input row
      FilteredRow& generate ();

      /// resets the filter
      void reset();

      /// @}

      private:
      /// the vector of all the generators
      std::vector<Generator*> __generators;

      /// the number of generators
      Generator* __last_generator { nullptr };

      /// the final row outputed by the set of generators
      FilteredRow* __output_row { nullptr };



      /** @brief returns true if there are still rows that can be output
       * by the set of generators */
      bool __hasRows( std::size_t i );

      /// sets the input row from which the generators will create new rows
      bool __setInputRow( std::size_t i,
                          FilteredRow& row );

      /// generate new rows from the input row
      FilteredRow& __generate ( std::size_t i );
    };

  } /* namespace learning */

} /* namespace gum */

// always include the template implementation
#include <agrum/learning/database/filteredRowGeneratorSet_tpl.h>


#endif /* GUM_LEARNING_FILTERED_ROW_GENERATOR_SET_H */
