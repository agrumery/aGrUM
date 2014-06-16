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
 * @brief The class representing a tabular database created from a CSV
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_DATABASE_FROM_CSV_H
#define GUM_DATABASE_FROM_CSV_H


#include <string>
#include <vector>

#include <agrum/learning/database/databaseVectInRAM.h>
#include <agrum/learning/database/DBTransform.h>
#include <agrum/learning/database/DBTransformIdentity.h>


namespace gum {

  
  namespace learning {

    
    class DatabaseFromCSV : public DatabaseVectInRAM {
    public:
      // ##########################################################################
      /// @name Constructors / Destructors
      // ##########################################################################

      /// @{

      /// default constructor
      DatabaseFromCSV
      ( const std::string filename,
        bool fileContainsNames = true,
        const DBTransform& transform = DBTransformIdentity (),
        const std::string delimiter = ",",
        const char commentmarker = '#',
        const char quoteMarker = '"',
        const std::vector<std::string> missingVal = { "?", "N/A", "n/a" } );

      /// copy constructor
      DatabaseFromCSV ( const DatabaseFromCSV& from );

      /// move constructor
      DatabaseFromCSV ( DatabaseFromCSV&& from );

      /// destructor
      ~DatabaseFromCSV ();
      
      /// @}
      
    };
    

  } /* namespace learning */


} /* namespace gum */


#endif /* GUM_DATABASE_FROM_CSV_H */

