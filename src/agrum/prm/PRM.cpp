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
/**
 * @file
 * @brief Implementation of PRM.
 *
 * @author Lionel TORTI
 */

#include <agrum/prm/PRM.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/PRM.inl>
#endif // GUM_NO_INLINE

namespace gum {
  namespace prm {

// Default constructor.
    PRM::PRM() {
      GUM_CONSTRUCTOR( PRM );
      __addBuiltInTypes();
    }

// Destructor.
    PRM::~PRM() {
      GUM_DESTRUCTOR( PRM );
      __classMap.clear();
      __typeMap.clear();
      __systemMap.clear();

      for ( Set<System*>::iterator iter = __systems.begin();
            iter != __systems.end(); ++iter ) {
        delete *iter;
      }

      for ( Set<Class*>::iterator iter = __classes.begin();
            iter != __classes.end(); ++iter ) {
        delete *iter;
      }

      for ( Set<Interface*>::iterator iter = __interfaces.begin();
            iter != __interfaces.end(); ++iter ) {
        delete *iter;
      }

      for ( Set<Type*>::iterator iter = __types.begin();
            iter != __types.end(); ++iter ) {
        delete *iter;
      }
    }

// Add the built-in types in the PRM
    void
    PRM::__addBuiltInTypes() {
      LabelizedVariable var( "boolean", "built-in type", 0 );
      var.addLabel( "false" );
      var.addLabel( "true" );
      Type* boolean = new Type( var );
      __types.insert( boolean );
      __typeMap.insert( "boolean", boolean );
    }

  } /* namespace prm */
} /* namespace gum */

