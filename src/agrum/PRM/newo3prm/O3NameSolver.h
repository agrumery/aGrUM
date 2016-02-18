/**************************************************************************
*   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
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
 * @brief Headers for the O3NameSolver class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <string>
#include <algorithm>

#include <agrum/core/hashTable.h>
#include <agrum/PRM/PRM.h>
#include <agrum/PRM/newo3prm/o3prm.h>
#include <agrum/PRM/newo3prm/utils.h>
#include <agrum/PRM/newo3prm/errors.h>

#ifndef GUM_PRM_O3PRM_O3NAMESOLVER_H
#define GUM_PRM_O3PRM_O3NAMESOLVER_H

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      class O3NameSolver {

        public:
        using NameMap = HashTable<std::string, std::string>;

        O3NameSolver( PRM<GUM_SCALAR>& prm,
                      O3PRM& o3_prm,
                      ErrorsContainer& errors )
            : __prm( &prm )
            , __o3_prm( &o3_prm )
            , __errors( &errors ) {
          GUM_CONSTRUCTOR( O3NameSolver );
        }

        O3NameSolver( const O3NameSolver<GUM_SCALAR>& src )
            : __prm( src.__prm )
            , __o3_prm( src.__o3_prm )
            , __errors( src.__errors ) {
          GUM_CONS_CPY( O3NameSolver );
        }

        O3NameSolver( O3NameSolver<GUM_SCALAR>&& src )
            : __prm( std::move( src.__prm ) )
            , __o3_prm( std::move( src.__o3_prm ) )
            , __errors( std::move( src.__errors ) ) {
          GUM_CONS_MOV( O3NameSolver );
        }

        ~O3NameSolver() { GUM_DESTRUCTOR( O3NameSolver ); }

        O3NameSolver<GUM_SCALAR>&
        operator=( const O3NameSolver<GUM_SCALAR>& src ) {
          if ( this == &src ) {
            return *this;
          }
          __prm = src.__prm;
          __o3_prm = src.__o3_prm;
          __errors = src.__errors;
          return *this;
        }

        O3NameSolver<GUM_SCALAR>& operator=( O3NameSolver<GUM_SCALAR>&& src ) {
          if ( this == &src ) {
            return *this;
          }
          __prm = std::move( src.__prm );
          __o3_prm = std::move( src.__o3_prm );
          __errors = std::move( src.__errors );
          return *this;
        }

        bool resolveType( O3Label& name ) {
          // If empty string, we return an empty string
          if ( name.label() == "" ) {
            return true;
          }
          // If we've already found the interface real name
          if ( __typeName.exists( name.label() ) ) {
            name.label() = __typeName[name.label()];
            return true;
          }
          // If name exists as is
          if ( __prm->isType( name.label() ) ) {
            __typeName.insert( name.label(), name.label() );
            return true;
          }

          auto matches = std::vector<std::string>();

          // Trying with types
          for ( auto t : __prm->types() ) {
            if ( ends_with( t->name(), name.label() ) ) {
              matches.push_back( t->name() );
            }
          }
          // Trying with O3Types
          for ( auto& t : __o3_prm->types() ) {
            if ( ends_with( t->name().label(), name.label() ) ) {
              matches.push_back( t->name().label() );
            }
          }

          if ( matches.size() == 1 ) {  // One match is good
            __typeName.insert( name.label(), matches.back() );
            name.label() = matches.back();
            return true;

          } else if ( matches.size() == 0 ) {  // 0 match is not found

            // Unknown name type
            O3PRM_TYPE_NOT_FOUND( name, __errors );
            return false;

          } else {  // More than one match is ambiguous

            // Ambiguous name
            O3PRM_TYPE_AMBIGUOUS( name, matches, __errors );
            return false;
          }
        }

        bool resolveInterface( O3Label& name ) {
          // If empty string, we return an empty string
          if ( name.label() == "" ) {
            return true;
          }
          // If we've already found the interface real name
          if ( __interfaceName.exists( name.label() ) ) {
            name.label() = __interfaceName[name];
            return true;
          }
          // If name exists as is
          if ( __prm->isInterface( name.label() ) ) {
            __interfaceName.insert( name.label(), name.label() );
            return true;
          }
          auto matches = std::vector<std::string>();
          // Trying with interfaces
          for (auto i: __prm->interfaces()) {
            if (ends_with( i->name(), name.label()) ) {
              matches.push_back(i->name());
            }
          }
          // Trying with O3Interface
          for ( auto& i : __o3_prm->interfaces() ) {
            if (ends_with( i->name().label(), name.label()) ) {
              matches.push_back( i->name().label() );
            }
          }
          if ( matches.size() == 1 ) {  // One match is good
            __interfaceName.insert( name.label(), matches.back() );
            name.label() = matches.back();
            return true;
          } else if ( matches.size() == 0 ) {  // 0 match is not found
            // Unknown name type
            O3PRM_INTERFACE_NOT_FOUND( name, __errors );
            return false;
          } else {  // More than one match is ambiguous
            // Ambiguous name
            O3PRM_INTERFACE_AMBIGUOUS( name, matches, __errors );
            return false;
          }
        }

        bool resolveClass( O3Label& name ) {
          // If empty string, we return an empty string
          if ( name.label() == "" ) {
            return true;
          }
          // If we've already found super real name
          if ( __className.exists( name.label() ) ) {
            name.label() = __className[name.label()];
            return true;
          }
          // If super's name exists as is
          if ( __prm->isClass( name.label() ) ) {
            __className.insert( name.label(), name.label() );
            return true;
          }
          // Try to complete super type's module
          auto matches = std::vector<const Class<GUM_SCALAR>*>();
          std::copy_if( __prm->classes().begin(),
                        __prm->classes().end(),
                        std::back_inserter( matches ),
                        [&name]( const Class<GUM_SCALAR>* c ) {
                          return ends_with( c->name(), name.label() );
                        } );
          if ( matches.size() == 1 ) {  // One match is good
            __className.insert( name.label(), matches.back()->name() );
            name.label() = matches.back()->name();
            return true;
          } else if ( matches.size() == 0 ) {  // 0 match is not found
            // Unknown super interface
            O3PRM_CLASS_NOT_FOUND( name, __errors );
            return false;
          } else {  // More than one match is ambiguous
            // Ambiguous name
            O3PRM_CLASS_AMBIGUOUS( name, matches, __errors );
            return false;
          }
        }

        bool resolveReferenceName( O3Label& name ) {
          // If empty string, we return an empty string
          if ( name.label() == "" ) {
            return true;
          }
          // If we've already found the interface real name
          if ( __refName.exists( name.label() ) ) {
            name.label() = __refName[name.label()];
            return true;
          }
          // If name exists as is
          if ( __prm->isInterface( name.label() ) or
               __prm->isClass( name.label() ) ) {
            __refName.insert( name.label(), name.label() );
            return true;
          }
          auto matches = std::vector<const PRMObject*>();
          // Trying with classes
          std::copy_if( __prm->classes().begin(),
                        __prm->classes().end(),
                        std::back_inserter( matches ),
                        [&name]( const Class<GUM_SCALAR>* c ) {
                          return ends_with( c->name(), name.label() );
                        } );
          // Trying with interfaces
          std::copy_if( __prm->interfaces().begin(),
                        __prm->interfaces().end(),
                        std::back_inserter( matches ),
                        [&name]( const Interface<GUM_SCALAR>* i ) {
                          return ends_with( i->name(), name.label() );
                        } );
          if ( matches.size() == 1 ) {  // One match is good
            __refName.insert( name.label(), matches.back()->name() );
            name.label() = matches.back()->name();
            return true;
          } else if ( matches.size() == 0 ) {  // 0 match is not found
            // Unknown name type
            O3PRM_REFERENCE_NOT_FOUND( name, __errors );
            return false;
          } else {  // More than one match is ambiguous
            // Ambiguous name
            O3PRM_REFERENCE_AMBIGUOUS( name, matches, __errors );
            return false;
          }
        }

        private:
        PRM<GUM_SCALAR>* __prm;
        O3PRM* __o3_prm;
        ErrorsContainer* __errors;

        NameMap __typeName;
        NameMap __refName;
        NameMap __interfaceName;
        NameMap __className;
      };

    }  // o3prm
  }    // prm
}  // gum

#endif  // GUM_PRM_O3PRM_O3NAMESOLVER_H

