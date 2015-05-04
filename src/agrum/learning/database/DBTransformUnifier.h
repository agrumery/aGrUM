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
 * @brief A class unifying the type of the DBCells for each column of the database
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_TRANSFORM_UNIFIER_H
#define GUM_LEARNING_DB_TRANSFORM_UNIFIER_H

#include <vector>
#include <string>

#include <agrum/config.h>
#include <agrum/learning/database/DBRow.h>
#include <agrum/learning/database/DBTransform.h>

namespace gum {

  namespace learning {

    // ============================================================================
    // DB TRANSFORM UNIFIER
    // ============================================================================
    /** @class DBTransformUnifier
     * @ingroup learning_group
     * @brief This class unifies the columns of the DB so that DBCells have the
     * same type (except for missing values) throughout the database
     */
    class DBTransformUnifier : public DBTransform {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBTransformUnifier();

      /// copy constructor
      DBTransformUnifier(const DBTransformUnifier &);

      /// move constructor
      DBTransformUnifier(DBTransformUnifier &&);

      /// destructor
      virtual ~DBTransformUnifier();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTransformUnifier &operator=(const DBTransformUnifier &);

      /// move operator
      DBTransformUnifier &operator=(DBTransformUnifier &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// indicates whether a BDCell contains a missing value
      bool isMissing(const DBCell &cell, const std::vector<std::string> &miss) const
          noexcept;

      /// transforms a vector of DBrows to unify the types of its columns
      /** @return true if the unification could be performed successfully. */
      bool transform(std::vector<DBRow> &db,
                     std::vector<std::string> miss = {"?", "N/A", "n/a"}) const;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_DB_TRANSFORM_UNIFIER_H */
