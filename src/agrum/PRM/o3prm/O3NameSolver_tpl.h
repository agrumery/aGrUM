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
 * @brief Implentation for the O3NameSolver class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/o3prm/O3NameSolver.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      INLINE O3NameSolver<GUM_SCALAR>::O3NameSolver( PRM<GUM_SCALAR>& prm,
                                                     O3PRM&           o3_prm,
                                                     ErrorsContainer& errors )
          : __prm( &prm )
          , __o3_prm( &o3_prm )
          , __errors( &errors ) {
        GUM_CONSTRUCTOR( O3NameSolver );
      }

      template <typename GUM_SCALAR>
      INLINE
      O3NameSolver<GUM_SCALAR>::O3NameSolver( const O3NameSolver<GUM_SCALAR>& src )
          : __prm( src.__prm )
          , __o3_prm( src.__o3_prm )
          , __errors( src.__errors )
          , __typeName( src.__typeName )
          , __eltName( src.__eltName )
          , __refName( src.__refName )
          , __interfaceName( src.__interfaceName )
          , __className( src.__className ) {
        GUM_CONS_CPY( O3NameSolver );
      }

      template <typename GUM_SCALAR>
      INLINE
      O3NameSolver<GUM_SCALAR>::O3NameSolver( O3NameSolver<GUM_SCALAR>&& src )
          : __prm( std::move( src.__prm ) )
          , __o3_prm( std::move( src.__o3_prm ) )
          , __errors( std::move( src.__errors ) )
          , __typeName( std::move( src.__typeName ) )
          , __eltName( std::move( src.__eltName ) )
          , __refName( std::move( src.__refName ) )
          , __interfaceName( std::move( src.__interfaceName ) )
          , __className( std::move( src.__className ) ) {
        GUM_CONS_MOV( O3NameSolver );
      }

      template <typename GUM_SCALAR>
      INLINE O3NameSolver<GUM_SCALAR>::~O3NameSolver() {
        GUM_DESTRUCTOR( O3NameSolver );
      }

      template <typename GUM_SCALAR>
      INLINE O3NameSolver<GUM_SCALAR>& O3NameSolver<GUM_SCALAR>::
      operator=( const O3NameSolver<GUM_SCALAR>& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = src.__prm;
        __o3_prm = src.__o3_prm;
        __errors = src.__errors;
        __typeName = src.__typeName;
        __eltName = src.__eltName;
        __refName = src.__refName;
        __interfaceName = src.__interfaceName;
        __className = src.__className;
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE O3NameSolver<GUM_SCALAR>& O3NameSolver<GUM_SCALAR>::
      operator=( O3NameSolver<GUM_SCALAR>&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = std::move( src.__prm );
        __o3_prm = std::move( src.__o3_prm );
        __errors = std::move( src.__errors );
        __typeName = std::move( src.__typeName );
        __eltName = std::move( src.__eltName );
        __refName = std::move( src.__refName );
        __interfaceName = std::move( src.__interfaceName );
        __className = std::move( src.__className );
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3NameSolver<GUM_SCALAR>::resolveClassElement( O3Label& name ) {
        // If empty string, we return an empty string
        if ( name.label() == "" ) {
          return true;
        }
        // If we've already found the element real name
        if ( __eltName.exists( name.label() ) ) {
          name.label() = __eltName[name.label()];
          return true;
        }
        // If name exists as is
        if ( __prm->isType( name.label() ) ) {
          __eltName.insert( name.label(), name.label() );
          return true;
        }
        // If name exists as is
        if ( __prm->isInterface( name.label() ) ) {
          __eltName.insert( name.label(), name.label() );
          return true;
        }
        // If name exists as is
        if ( __prm->isClass( name.label() ) ) {
          __eltName.insert( name.label(), name.label() );
          return true;
        }
        // If name exists as is in O3PRM types
        for ( auto& t : __o3_prm->types() ) {
          if ( t->name().label() == name.label() ) {
            __eltName.insert( name.label(), name.label() );
            return true;
          }
        }
        // If name exists as is in O3PRM interfaces
        for ( auto& i : __o3_prm->interfaces() ) {
          if ( i->name().label() == name.label() ) {
            __eltName.insert( name.label(), name.label() );
            return true;
          }
        }
        // If name exists as is in O3PRM classes
        for ( auto& c : __o3_prm->classes() ) {
          if ( c->name().label() == name.label() ) {
            __eltName.insert( name.label(), name.label() );
            return true;
          }
        }

        auto lookup = "." + name.label();
        auto found = Set<std::string>();
        auto matches = std::vector<std::string>();

        // Trying with types
        for ( auto t : __prm->types() ) {
          if ( endsWith( t->name(), lookup ) ) {
            if ( !found.exists( t->name() ) ) {
              found.insert( t->name() );
              matches.push_back( t->name() );
            }
          }
        }
        // Trying with O3Types
        for ( auto& t : __o3_prm->types() ) {
          if ( endsWith( t->name().label(), lookup ) ) {
            if ( !found.exists( t->name().label() ) ) {
              found.insert( t->name().label() );
              matches.push_back( t->name().label() );
            }
          }
        }

        // Trying with interfaces
        for ( auto i : __prm->interfaces() ) {
          if ( endsWith( i->name(), lookup ) ) {
            if ( !found.exists( i->name() ) ) {
              found.insert( i->name() );
              matches.push_back( i->name() );
            }
          }
        }
        // Trying with O3Interface
        for ( auto& i : __o3_prm->interfaces() ) {
          if ( endsWith( i->name().label(), lookup ) ) {
            if ( !found.exists( i->name().label() ) ) {
              found.insert( i->name().label() );
              matches.push_back( i->name().label() );
            }
          }
        }

        // Trying with class
        for ( auto c : __prm->classes() ) {
          if ( endsWith( c->name(), lookup ) ) {
            if ( !found.exists( c->name() ) ) {
              found.insert( c->name() );
              matches.push_back( c->name() );
            }
          }
        }
        // Trying with O3Class
        for ( auto& c : __o3_prm->classes() ) {
          if ( endsWith( c->name().label(), lookup ) ) {
            if ( !found.exists( c->name().label() ) ) {
              found.insert( c->name().label() );
              matches.push_back( c->name().label() );
            }
          }
        }

        if ( matches.size() == 1 ) {  // One match is good
          __eltName.insert( name.label(), matches.back() );
          name.label() = matches.back();
          return true;

        } else if ( matches.size() == 0 ) {  // 0 match is not found

          // Unknown name type
          O3PRM_TYPE_NOT_FOUND( name, *__errors );
          return false;

        } else {  // More than one match is ambiguous

          // Ambiguous name
          O3PRM_TYPE_AMBIGUOUS( name, matches, *__errors );
          return false;
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3NameSolver<GUM_SCALAR>::resolveType( O3Label& name ) {
        // If empty string, we return an empty string
        if ( name.label() == "" ) {
          return true;
        }

        // If we've already found the type real name
        if ( __typeName.exists( name.label() ) ) {
          name.label() = __typeName[name.label()];
          return true;
        }

        // If name exists as is in PRM
        if ( __prm->isType( name.label() ) ) {
          __typeName.insert( name.label(), name.label() );
          return true;
        }

        // If name exists as is in O3PRM
        for ( auto& t : __o3_prm->types() ) {
          if ( t->name().label() == name.label() ) {
            __typeName.insert( name.label(), name.label() );
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup = "." + name.label();
        auto found = Set<std::string>();
        auto matches = std::vector<std::string>();

        // Trying with types
        for ( auto t : __prm->types() ) {
          if ( endsWith( t->name(), lookup ) ) {
            if ( !found.exists( t->name() ) ) {
              found.insert( t->name() );
              matches.push_back( t->name() );
            }
          }
        }

        // Trying with O3Types
        for ( auto& t : __o3_prm->types() ) {
          if ( endsWith( t->name().label(), lookup ) ) {
            if ( !found.exists( t->name().label() ) ) {
              found.insert( t->name().label() );
              matches.push_back( t->name().label() );
            }
          }
        }

        if ( matches.size() == 1 ) {  // One match is good
          __typeName.insert( name.label(), matches.back() );
          name.label() = matches.back();
          return true;

        } else if ( matches.size() == 0 ) {  // 0 match is not found

          // Unknown name type
          O3PRM_TYPE_NOT_FOUND( name, *__errors );
          return false;

        } else {  // More than one match is ambiguous

          // Ambiguous name
          O3PRM_TYPE_AMBIGUOUS( name, matches, *__errors );
          return false;
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3NameSolver<GUM_SCALAR>::resolveInterface( O3Label& name ) {
        // If empty string, we return an empty string
        if ( name.label() == "" ) {
          return true;
        }

        // If we've already found the interface real name
        if ( __interfaceName.exists( name.label() ) ) {
          name.label() = __interfaceName[name.label()];
          return true;
        }

        // If name exists as is
        if ( __prm->isInterface( name.label() ) ) {
          __interfaceName.insert( name.label(), name.label() );
          return true;
        }

        for ( auto& i : __o3_prm->interfaces() ) {
          if ( i->name().label() == name.label() ) {
            __interfaceName.insert( name.label(), name.label() );
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup = "." + name.label();
        auto found = Set<std::string>();
        auto matches = std::vector<std::string>();

        // Trying with interfaces
        for ( auto i : __prm->interfaces() ) {
          if ( endsWith( i->name(), lookup ) ) {
            if ( !found.exists( i->name() ) ) {
              found.insert( i->name() );
              matches.push_back( i->name() );
            }
          }
        }

        // Trying with O3Interface
        for ( auto& i : __o3_prm->interfaces() ) {
          if ( endsWith( i->name().label(), lookup ) ) {
            if ( !found.exists( i->name().label() ) ) {
              found.insert( i->name().label() );
              matches.push_back( i->name().label() );
            }
          }
        }

        if ( matches.size() == 1 ) {  // One match is good

          __interfaceName.insert( name.label(), matches.back() );
          name.label() = matches.back();
          return true;

        } else if ( matches.size() == 0 ) {  // 0 match is not found

          // Unknown name type
          O3PRM_INTERFACE_NOT_FOUND( name, *__errors );
          return false;

        } else {  // More than one match is ambiguous

          // Ambiguous name
          O3PRM_INTERFACE_AMBIGUOUS( name, matches, *__errors );
          return false;
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3NameSolver<GUM_SCALAR>::resolveClass( O3Label& name ) {

        // If empty string, we return an empty string
        if ( name.label() == "" ) {
          return true;
        }

        // If we've already found super real name
        if ( __className.exists( name.label() ) ) {
          name.label() = __className[name.label()];
          return true;
        }

        // If class name exists as is
        if ( __prm->isClass( name.label() ) ) {
          __className.insert( name.label(), name.label() );
          return true;
        }

        for ( auto& c : __o3_prm->classes() ) {
          if ( c->name().label() == name.label() ) {
            __className.insert( name.label(), name.label() );
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup = "." + name.label();
        auto matches = std::vector<std::string>();
        auto found = Set<std::string>();

        // Try to complete with Class
        for ( auto c : __prm->classes() ) {
          if ( endsWith( c->name(), lookup ) ) {
            if ( !found.exists( c->name() ) ) {
              found.insert( c->name() );
              matches.push_back( c->name() );
            }
          }
        }

        // Try to complete with O3Class
        for ( auto& c : __o3_prm->classes() ) {
          if ( endsWith( c->name().label(), lookup ) ) {
            if ( !found.exists( c->name().label() ) ) {
              found.insert( c->name().label() );
              matches.push_back( c->name().label() );
            }
          }
        }

        if ( matches.size() == 1 ) {  // One match is good

          __className.insert( name.label(), matches.back() );
          name.label() = matches.back();
          return true;

        } else if ( matches.size() == 0 ) {  // 0 match is not found

          // Unknown super class
          O3PRM_CLASS_NOT_FOUND( name, *__errors );
          return false;

        } else {  // More than one match is ambiguous

          // Ambiguous name
          O3PRM_CLASS_AMBIGUOUS( name, matches, *__errors );
          return false;
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3NameSolver<GUM_SCALAR>::resolveSlotType( O3Label& name ) {
        // If empty string, we return an empty string
        if ( name.label() == "" ) {
          return true;
        }
        // If we've already found the reference real name
        if ( __refName.exists( name.label() ) ) {
          name.label() = __refName[name.label()];
          return true;
        }
        // If name exists as is
        if ( __prm->isInterface( name.label() ) ||
             __prm->isClass( name.label() ) ) {
          __refName.insert( name.label(), name.label() );
          return true;
        }

        // We check if it matches an O3Interface
        for ( auto& i : __o3_prm->interfaces() ) {
          if ( i->name().label() == name.label() ) {
            __interfaceName.insert( name.label(), name.label() );
            return true;
          }
        }

        // We check if it matches an O3Class
        for ( auto& c : __o3_prm->classes() ) {
          if ( c->name().label() == name.label() ) {
            __className.insert( name.label(), name.label() );
            return true;
          }
        }

        // If we didn't find it as is, then we must find a namespace
        // in which it was declared
        auto lookup = "." + name.label();
        auto found = Set<std::string>();
        auto matches = std::vector<std::string>();

        // Trying with interfaces
        for ( auto i : __prm->interfaces() ) {
          if ( endsWith( i->name(), lookup ) ) {
            if ( !found.exists( i->name() ) ) {
              found.insert( i->name() );
              matches.push_back( i->name() );
            }
          }
        }

        // Trying with O3Interface
        for ( auto& i : __o3_prm->interfaces() ) {
          if ( endsWith( i->name().label(), lookup ) ) {
            if ( !found.exists( i->name().label() ) ) {
              found.insert( i->name().label() );
              matches.push_back( i->name().label() );
            }
          }
        }

        // Try to complete with Class
        for ( auto c : __prm->classes() ) {
          if ( endsWith( c->name(), lookup ) ) {
            if ( !found.exists( c->name() ) ) {
              found.insert( c->name() );
              matches.push_back( c->name() );
            }
          }
        }

        // Try to complete with O3Class
        for ( auto& c : __o3_prm->classes() ) {
          if ( endsWith( c->name().label(), lookup ) ) {
            if ( !found.exists( c->name().label() ) ) {
              found.insert( c->name().label() );
              matches.push_back( c->name().label() );
            }
          }
        }

        if ( matches.size() == 1 ) {  // One match is good

          __refName.insert( name.label(), matches.back() );
          name.label() = matches.back();
          return true;

        } else if ( matches.size() == 0 ) {  // 0 match is not found

          // Unknown name type
          O3PRM_REFERENCE_NOT_FOUND( name, *__errors );
          return false;

        } else {  // More than one match is ambiguous

          // Ambiguous name
          O3PRM_REFERENCE_AMBIGUOUS( name, matches, *__errors );
          return false;
        }
      }


    }  // o3prm
  }    // prm
}  // gum
