/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
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
 * @brief The K2 algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#include <agrum/learning/K2.h>


namespace gum {

  
  namespace learning {


    /// default constructor
    K2::K2 () {
      GUM_CONSTRUCTOR ( learning::K2 );
    }

    
    /// copy constructor
    K2::K2 ( const K2& from ) {
      GUM_CONS_CPY ( learning::K2 );
    }


    /// move constructor
    K2::K2 ( K2&& from ) {
      GUM_CONS_MOV ( learning::K2 );
    }

    
    /// destructor
    K2::~K2 () {
      GUM_DESTRUCTOR ( learning::K2 );
    }


    /// copy operator
    K2& K2::operator= ( const K2& from ) {
      return *this;
    }
    

    /// move operator
    K2& K2::operator= ( K2&& from ) {
      return *this;
    }
    

  } /* namespace learning */
  
  
} /* namespace gum */
