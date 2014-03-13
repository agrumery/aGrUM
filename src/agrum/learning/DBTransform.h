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
 * @brief The base class for preprocessing a database: this class performs an
 * identity
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_LEARNING_DB_TRANSFORM_H
#define GUM_LEARNING_DB_TRANSFORM_H

#include <vector>
#include <string>

#include <agrum/config.h>
#include <agrum/learning/DBRow.h>


namespace gum {

  
  namespace learning {


    // ============================================================================
    // DB TRANSFORM
    // ============================================================================
    /** @class DBTransform
     * @ingroup learning_group
     * @brief The base class for preprocessing a database. This class perform an
     * identity transformation
     */
    class DBTransform {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DBTransform ();

      /// copy constructor
      DBTransform ( const DBTransform& );

      /// destructor
      virtual ~DBTransform ();

      /// @}
      

      // ##########################################################################
      /// @name Operators
      // ##########################################################################

      /// @{

      /// copy operator
      DBTransform& operator= ( const DBTransform& );

      /// @}


      // ##########################################################################
      /// @name Accessors / Modifiers
      // ##########################################################################

      /// @{

      /// transforms a vector of DBrows
      /** @return true if the transformation could be performed successfully. */
      virtual bool transform ( std::vector<DBRow>& db,
                               std::vector<std::string> miss ) const;

      /// @}

    };
    
    
  } /* namespace learning */

  
} /* namespace gum */

    

#endif /* GUM_LEARNING_DB_TRANSFORM_H */
