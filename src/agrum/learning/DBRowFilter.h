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
 * @brief The class representing rows in tabular database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_ROW_FILTER_H
#define GUM_LEARNING_DB_ROW_FILTER_H


#include <agrum/config.h>
#include <agrum/learning/DBHandler.h>
#include <agrum/learning/DBRowTranslatorSet.h>
#include <agrum/learning/filteredRowGeneratorSet.h>


namespace gum {

  
  namespace learning {
    

    /** @class DBRowFilter
     * @ingroup learning_group
     * @brief the class used to read a row in the database and to transform it
     * into a set of FilteredRows that can be used for learning. A DBRowFilter
     * contains a handler on the database that will actually interact with it
     * to read DBRows, a set of cell translators that will transform the DBRows
     * into FilteredRows, and a set of generators that will transform the
     * filtered rows into new sets of filtered rows (see FilteredRowGenerator
     * for more details about generators). An example of the construction of
     * a row filter can be as follows (using helper function make_row_filter that
     * produces a DBRowFilter):
     * @code
     * Database::Handler handler ( database );
     *
     * auto translators = make_translators
     *   ( Create<CellTranslator1, Col<0> > (),
     *     Create<CellTranslator1, Col<1>, 4 > () );
     *
     * auto generators =  make_generators
     *   ( EMGenerator (),
     *     TimeSlicerGenerator () );
     *
     *
     * auto filter = make_row_filter ( handler, translators, generators );
     * @endcode
     */
    template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    class DBRowFilter {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBRowFilter ( DBHandler& handler,
                    const TranslatorSet& translator_set,
                    const GeneratorSet& generator_set,
                    bool require_cell_init = true ) noexcept;

      /// copy constructor
      DBRowFilter
      ( const DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>& filter ) noexcept;

      /// destructor
      virtual ~DBRowFilter () noexcept;

      /// @}

  
      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// returns true if there are still rows that can be output by the RowFilter
      bool hasRows () noexcept;

      /// returns a new output row with its corresponding weight
      FilteredRow& row () noexcept;
      
      /// resets the filter
      virtual void reset ();

      // returns the handler used by the filter
      DBHandler& handler () noexcept;

      /// @}

    protected:
      /// the handler that is really used to parse the database
      DBHandler __handler;
      
      /// the DBRowTranslatorSet used to translate the database cells
      TranslatorSet __translator_set;

      /// the generator of FilteredRows
      GeneratorSet __generator_set;


      /// initialize the cell filters by parsing once the database
      void __initCellFilters ();
      
    };


    /// a helper used to easily create a DBRowFilter
    /** The following code shows how to use this function:
     * @code
     * Database::Handler handler ( database );
     *
     * auto translators = make_translators
     *   ( Create<CellTranslator1, Col<0> > (),
     *     Create<CellTranslator1, Col<1>, 4 > () );
     *
     * auto generators =  make_generators
     *   ( EMGenerator (),
     *     TimeSlicerGenerator () );
     *
     * // create a filter that will parse the DB to initialize the cell filters
     * auto filter = make_row_filter ( handler, translators, generators );
     *
     * // create a filter that will not initialize the cell filters
     * auto filter = make_row_filter ( handler, translators, generators, false );
     * @endcode
     */
    template<typename DBHandler, typename TranslatorSet, typename GeneratorSet>
    constexpr DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>
    make_row_filter ( DBHandler& handler,
                      const TranslatorSet& translator_set,
                      const GeneratorSet& generator_set,
                      bool require_cell_init = true ) {
      return DBRowFilter<DBHandler,TranslatorSet,GeneratorSet>
        ( handler, translator_set, generator_set );
    }

    
  } /* namespace learning */

  
} /* namespace gum */


// always include the template implementation
#include <agrum/learning/DBRowFilter.tcc>


#endif /* GUM_LEARNING_DB_ROW_FILTER_H */
