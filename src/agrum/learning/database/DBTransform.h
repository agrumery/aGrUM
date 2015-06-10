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
 * @brief The base class for preprocessing a database: convert a vector of DBRows
 * into another vector of DBRows
 *
 * The idea of DBTransform is the following: upon initialization, a tabular
 * database first creates its vector of DBRows (for instance by reading a CSV
 * file). The DBCells of the database are filled automatically from the read
 * strings but these fillings may not always be the most appropriate (for instance,
 * for ensuring that all the DBCells of a given column of the database have the
 * same type). Therefore, prior to learning from the database, the latter should
 * execute a preprocessing step which update as needed its DBRows. This is
 * precisely the goal of the DBTransform classes.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_TRANSFORM_H
#define GUM_LEARNING_DB_TRANSFORM_H

#include <vector>
#include <string>

#include <agrum/config.h>
#include <agrum/learning/database/DBRow.h>

namespace gum {

  namespace learning {

    // ============================================================================
    // DB TRANSFORM
    // ============================================================================
    /** @class DBTransform
     * @ingroup learning_group
     * @brief The base class for preprocessing a database: convert a vector of
     * DBRows into another vector of DBRows
     *
     * The idea of DBTransform is the following: upon initialization, a tabular
     * database first creates its vector of DBRows (for instance by reading a CSV
     * file). The DBCells of the database are filled automatically from the read
     * strings but these fillings may not always be the most appropriate (for
     * instance, for ensuring that all the DBCells of a given column of the
     * database have the same type). Therefore, prior to learning from the
     * database, the latter should execute a preprocessing step which update as
     * needed its DBRows. This is precisely the goal of the DBTransform classes.
     */
    class DBTransform {
      public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBTransform();

      /// copy constructor
      DBTransform(const DBTransform &);

      /// move constructor
      DBTransform(DBTransform &&);

      /// destructor
      virtual ~DBTransform();

      /// @}

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTransform &operator=(const DBTransform &);

      /// move operator
      DBTransform &operator=(DBTransform &&);

      /// @}

      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// transforms a vector of DBrows
      /** @return true if the transformation could be performed successfully. */
      virtual bool transform(std::vector<DBRow> &db,
                             std::vector<std::string> miss) const = 0;

      /// @}
    };

  } /* namespace learning */

} /* namespace gum */

#endif /* GUM_LEARNING_DB_TRANSFORM_H */
