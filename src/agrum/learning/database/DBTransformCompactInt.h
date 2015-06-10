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
 * @brief A class that transforms DBCells into compact ints (to speed-up learning)
 *
 * This class is intended to be used after the initialization of a filteredRow
 * on a database. By using this filteredRow, we can convert the database into a
 * table of "compact ints", i.e., of numbers from 0 to N-1, where N is the
 * number of different observations of the variable in the database. Compact ints
 * then allow a very fast structure learning.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_TRANSFORM_COMPACT_INT_H
#define GUM_LEARNING_DB_TRANSFORM_COMPACT_INT_H

#include <vector>
#include <string>

#include <agrum/config.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/DBRowFilter.h>
#include <agrum/learning/database/DBTransform.h>

namespace gum {

  namespace learning {

    // ============================================================================
    // DB TRANSFORM COMPACT INT
    // ============================================================================
    /** @class DBTransformCompactInt
     * @ingroup learning_group
     * @brief A class that transforms DBCells into compact ints
     * (to speed-up learning)
     *
     * This class is intended to be used after the initialization of a filteredRow
     * on a database. By using this filteredRow, we can convert the database into a
     * table of "compact ints", i.e., of numbers from 0 to N-1, where N is the
     * number of different observations of the variable in the database. Compact
     * ints then allow a very fast structure learning by allowing to use
     * DBCellTranslatorCompactIntId translators.
     */
    class DBTransformCompactInt {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBTransformCompactInt();

      /// copy constructor
      DBTransformCompactInt(const DBTransformCompactInt &);

      /// move constructor
      DBTransformCompactInt(DBTransformCompactInt &&);

      /// destructor
      virtual ~DBTransformCompactInt();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTransformCompactInt &operator=(const DBTransformCompactInt &);

      /// move operator
      DBTransformCompactInt &operator=(DBTransformCompactInt &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// transforms a vector of DBrows into a vector of compact int DBRows */
      template <typename DBHandler, typename TranslatorSet, typename GeneratorSet>
      bool
      transform(DBRowFilter<DBHandler, TranslatorSet, GeneratorSet> &filter) const;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

/// always include the template implementations
#include <agrum/learning/database/DBTransformCompactInt.tcc>

#endif /* GUM_LEARNING_DB_TRANSFORM_COMPACT_INT_H */
