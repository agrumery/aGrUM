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
 * @brief The class for preprocessing a database without altering it (identity)
 *
 * The idea of DBTransform is the following: upon initialization, a tabular
 * database first creates its vector of DBRows (for instance by reading a CSV
 * file). The DBCells of the database are filled automatically from the read
 * strings but these fillings may not always be the most appropriate (for
 * instance, for ensuring that all the DBCells of a given column of the
 * database have the same type). Therefore, prior to learning from the
 * database, the latter should execute a preprocessing step which update as
 * needed its DBRows. This is precisely the goal of the DBTransform classes.
 * When no preprocessing is needed, i.e., we should keep the DBRow vector
 * unchanged, simply use DBTransformIdentity.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_TRANSFORM_IDENTITY_H
#define GUM_LEARNING_DB_TRANSFORM_IDENTITY_H

#include <vector>
#include <string>

#include <agrum/config.h>
#include <agrum/learning/database/DBTransform.h>

namespace gum {

  namespace learning {

    // ============================================================================
    // DB TRANSFORM IDENTITY
    // ============================================================================
    /** @class DBTransformIdentity
     * @ingroup learning_group
     * @brief The class for preprocessing a database without altering it (identity)
     *
     * The idea of DBTransform is the following: upon initialization, a tabular
     * database first creates its vector of DBRows (for instance by reading a CSV
     * file). The DBCells of the database are filled automatically from the read
     * strings but these fillings may not always be the most appropriate (for
     * instance, for ensuring that all the DBCells of a given column of the
     * database have the same type). Therefore, prior to learning from the
     * database, the latter should execute a preprocessing step which update as
     * needed its DBRows. This is precisely the goal of the DBTransform classes.
     * When no preprocessing is needed, i.e., we should keep the DBRow vector
     * unchanged, simply use DBTransformIdentity.
     */
    class DBTransformIdentity : public DBTransform {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBTransformIdentity();

      /// copy constructor
      DBTransformIdentity(const DBTransformIdentity &);

      /// move constructor
      DBTransformIdentity(DBTransformIdentity &&);

      /// destructor
      virtual ~DBTransformIdentity();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTransformIdentity &operator=(const DBTransformIdentity &);

      /// move operator
      DBTransformIdentity &operator=(DBTransformIdentity &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// transforms a vector of DBrows
      /** @return true if the transformation could be performed successfully. */
      virtual bool transform(std::vector<DBRow> &db,
                             std::vector<std::string> miss) const;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_DB_TRANSFORM_IDENTITY_H */
