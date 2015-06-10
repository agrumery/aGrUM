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
 * @brief An identity function cell translator on integers that assumes the
 * integers it reads forms a compact (interval { 0 ,..., n } )
 *
 * @warning If you are unsure that the integers that the cell translator will
 * parse form an interval { 0 ,..., n }, then, prefer using class
 * CellTranslatorNumber. The latter class is slower but safer.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_CELL_TRANSLATOR_COMPACT_INT_ID_H
#define GUM_LEARNING_CELL_TRANSLATOR_COMPACT_INT_ID_H

#include <string>
#include <vector>

#include <agrum/config.h>
#include <agrum/core/set.h>
#include <agrum/learning/database/DBCellTranslator.h>

namespace gum {

  namespace learning {

    /** @class CellTranslatorCompactIntId
     * @ingroup learning_group
     * @brief An identity function cell translator on integers that assumes the
     * integers it reads forms a compact (interval { 0 ,..., n } )
     *
     * @warning If you are unsure that the integers that the cell translator will
     * parse form an interval { 0 ,..., n }, then, prefer using class
     * CellTranslatorNumber. The latter class is slower but safer.
     */
    class CellTranslatorCompactIntId : public DBCellTranslator<1, 1> {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      /** @param By default, CellTranslatorCompactIntId parses the database at
       * initialization to check that its values actually form an interval
       * { 0 ,..., n }. If this is not the case, it raises an exception. However,
       * there exist cases where this check is unncessary (for instance in the
       * BNLearner class) and, by setting check_database to false, we avoid
       * this unnecessary step.
       * @warning Note that if you set check_database to false, method
       * modalities () will raise an exception because the modalities are
       * computed at initialization by parsing the database */
      CellTranslatorCompactIntId(bool check_database = true);

      /// copy constructor
      CellTranslatorCompactIntId(const CellTranslatorCompactIntId &from);

      /// move constructor
      CellTranslatorCompactIntId(CellTranslatorCompactIntId &&from);

      /// virtual copy constructor
      virtual CellTranslatorCompactIntId *copyFactory() final;

      /// destructor
      virtual ~CellTranslatorCompactIntId();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      CellTranslatorCompactIntId &operator=(const CellTranslatorCompactIntId &from);

      /// move operator
      CellTranslatorCompactIntId &operator=(CellTranslatorCompactIntId &&from);

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

      /// @}

      private:
      /// the set of values found so far
      Set<unsigned int> __values;

      /// do we need to parse the database at initialization?
      bool __check_database{true};
    };

  } /* namespace learning */

} /* namespace gum */

/// include the inlined functions if necessary
#ifndef GUM_NO_INLINE
#include <agrum/learning/database/DBCellTranslators/cellTranslatorCompactIntId.inl>
#endif /* GUM_NO_INLINE */

#endif /* GUM_LEARNING_CELL_TRANSLATOR_COMPACT_INT_ID_H */
