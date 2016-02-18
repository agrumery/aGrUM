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
 * @brief Implementation for the O3InterfaceFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/newo3prm/O3InterfaceFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      O3InterfaceFactory<GUM_SCALAR>::O3InterfaceFactory() {
        GUM_CONSTRUCTOR( O3InterfaceFactory );
      }

      template <typename GUM_SCALAR>
      O3InterfaceFactory<GUM_SCALAR>::O3InterfaceFactory(
          const O3InterfaceFactory<GUM_SCALAR>& src ) {
        GUM_CONS_CPY( O3InterfaceFactory );
      }

      template <typename GUM_SCALAR>
      O3InterfaceFactory<GUM_SCALAR>::O3InterfaceFactory(
          O3InterfaceFactory<GUM_SCALAR>&& src ) {
        GUM_CONS_MOV( O3InterfaceFactory );
      }

      template <typename GUM_SCALAR>
      O3InterfaceFactory<GUM_SCALAR>::~O3InterfaceFactory() {
        GUM_DESTRUCTOR( O3InterfaceFactory );
      }

      template <typename GUM_SCALAR>
      O3InterfaceFactory<GUM_SCALAR>& O3InterfaceFactory<GUM_SCALAR>::
      operator=( const O3InterfaceFactory<GUM_SCALAR>& src ) {
        return *this;
      }

      template <typename GUM_SCALAR>
      O3InterfaceFactory<GUM_SCALAR>& O3InterfaceFactory<GUM_SCALAR>::
      operator=( O3InterfaceFactory<GUM_SCALAR>&& src ) {
        return *this;
      }

      template <typename GUM_SCALAR>
      void O3InterfaceFactory<GUM_SCALAR>::__initialize() {
        __superMap = HashTable<std::string, const O3Interface*>();
        __eltName = HashTable<std::string, std::string>();
        __nameMap = HashTable<std::string, gum::NodeId>();
        __interfaceMap = HashTable<std::string, const O3Interface*>();
        __nodeMap = HashTable<NodeId, const O3Interface*>();
        __dag = DAG();
        __o3Interface = std::vector<const O3Interface*>();
        __build = true;
      }

      template <typename GUM_SCALAR>
      bool O3InterfaceFactory<GUM_SCALAR>::__addInterface2Dag(
          PRM<GUM_SCALAR>& prm,
          const O3PRM& tmp_prm,
          ErrorsContainer& errors ) {
        // Adding nodes to the type inheritance graph
        for ( const auto& i : tmp_prm.interfaces() ) {
          if ( name_used<GUM_SCALAR>( prm, i->name().label() ) ) {
            // Raised if duplicate interface names
            const auto& pos = i->name().position();
            auto msg = std::stringstream();
            msg << "Interface error : "
                << "Interface name " << i->name().label() << " exists already";
            errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
            __build = false;
            return false;
          }
          auto id = __dag.addNode();
          try {
            __nameMap.insert( i->name().label(), id );
            __interfaceMap.insert( i->name().label(), i.get() );
            __nodeMap.insert( id, i.get() );
          } catch ( DuplicateElement& e ) {
            // Raised if duplicate type names
            const auto& pos = i->name().position();
            auto msg = std::stringstream();
            msg << "Interface error : "
                << "Interface name " << i->name().label() << " exists already";
            errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
            __build = false;
            return false;
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool
      O3InterfaceFactory<GUM_SCALAR>::__addArcs2Dag( const O3PRM& prm,
                                                     ErrorsContainer& errors ) {
        // Adding arcs to the graph inheritance graph
        for ( const auto& i : prm.interfaces() ) {
          if ( i->super().label() != "" ) {
            if ( not __resolveInterface( i->super(), errors ) ) {
              return false;
            }
            auto super = __superMap[i->super().label()];
            auto head = __nameMap[super->name().label()];
            auto tail = __nameMap[i->name().label()];
            try {
              __dag.addArc( tail, head );
            } catch ( InvalidDirectedCycle& e ) {
              // Cyclic inheritance
              const auto& pos = i->position();
              auto msg = std::stringstream();
              msg << "Interface error : "
                  << "Cyclic inheritance between interface "
                  << i->name().label() << " and interface "
                  << super->name().label();
              errors.addError(
                  msg.str(), pos.file(), pos.line(), pos.column() );
              __build = false;
              return false;
            }
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      void O3InterfaceFactory<GUM_SCALAR>::__setO3InterfaceCreationOrder() {
        auto topo_order = topological_order( __dag );
        for ( auto iter = topo_order.rbegin(); iter != topo_order.rend();
              --iter ) {
          __o3Interface.push_back( __nodeMap[*iter] );
        }
      }

      template <typename GUM_SCALAR>
      bool O3InterfaceFactory<GUM_SCALAR>::__checkInterfaceElement(
          PRM<GUM_SCALAR>& prm,
          const O3Interface& i,
          const O3InterfaceElement& elt,
          ErrorsContainer& errors ) {
        if ( not __resolveElementName( prm, elt.name(), errors ) ) {
          // Raised unknown type
          const auto& pos = elt.type().position();
          auto msg = std::stringstream();
          msg << "Interface error : "
              << "Unknown identifier " << elt.type().label();
          errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
          __build = false;
          return false;
        }
        if ( prm.isInterface( __eltName[elt.type().label()] ) ) {
          const auto& real_i = prm.interface( i.name().label() );
          const auto& ref_type = prm.interface( __eltName[elt.type().label()] );
          if ( ( &real_i ) == ( &ref_type ) ) {
            const auto& pos = elt.type().position();
            auto msg = std::stringstream();
            msg << "Reference Slot error : "
                << "Interface " << i.name().label()
                << " cannot reference itself";
            errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
            __build = false;
            return false;
          }
          if ( ref_type.isSubTypeOf( real_i ) ) {
            const auto& pos = elt.type().position();
            auto msg = std::stringstream();
            msg << "Reference Slot error : "
                << "Interface " << i.name().label()
                << " cannot reference subinterface "
                << __eltName[elt.type().label()];
            errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
            __build = false;
            return false;
          }
          if ( real_i.isSubTypeOf( ref_type ) ) {
            const auto& pos = elt.type().position();
            auto msg = std::stringstream();
            msg << "Reference Slot error : "
                << "Interface " << i.name().label()
                << " cannot reference super interface "
                << __eltName[elt.type().label()];
            errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
            __build = false;
            return false;
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3InterfaceFactory<GUM_SCALAR>::__checkO3Interfaces(
          PRM<GUM_SCALAR>& prm,
          const O3PRM& tmp_prm,
          ErrorsContainer& errors ) {
        return __addInterface2Dag( prm, tmp_prm, errors ) and
               __addArcs2Dag( tmp_prm, errors );
      }

      template <typename GUM_SCALAR>
      void O3InterfaceFactory<GUM_SCALAR>::buildInterfaces(
          PRM<GUM_SCALAR>& prm,
          const O3PRM& tmp_prm,
          ErrorsContainer& errors ) {
        __initialize();
        PRMFactory<GUM_SCALAR> factory( &prm );
        if ( __checkO3Interfaces( prm, tmp_prm, errors ) ) {
          __setO3InterfaceCreationOrder();
          for ( auto i : __o3Interface ) {
            auto super = std::string();
            if ( i->super().label() != "" ) {
              super = __superMap[i->super().label()]->name().label();
            }
            factory.startInterface( i->name().label(), super );
            factory.endInterface();
          }
        }
      }

      template <typename GUM_SCALAR>
      void
      O3InterfaceFactory<GUM_SCALAR>::buildElements( PRM<GUM_SCALAR>& prm,
                                                     const O3PRM& tmp_prm,
                                                     ErrorsContainer& errors ) {
        if ( __build ) {
          PRMFactory<GUM_SCALAR> factory( &prm );
          for ( auto i : __o3Interface ) {
            factory.continueInterface( i->name().label() );
            for ( const auto& elt : i->elements() ) {
              if ( __checkInterfaceElement( prm, *i, elt, errors ) ) {
                try {
                  if ( prm.isType( elt.type().label() ) ) {
                    factory.addAttribute( elt.type().label(),
                                          elt.name().label() );
                  } else {
                    factory.addReferenceSlot(
                        elt.type().label(), elt.name().label(), false );
                  }
                } catch ( OperationNotAllowed& e ) {
                  // Duplicate or Wrong overload
                  const auto& pos = elt.type().position();
                  auto msg = std::stringstream();
                  msg << "Interface error : "
                      << "Element " << elt.name().label() << " already exists";
                  errors.addError(
                      msg.str(), pos.file(), pos.line(), pos.column() );
                  __build = false;
                }
              }
            }
            factory.endInterface();
          }
        }
      }

      template <typename GUM_SCALAR>
      bool O3InterfaceFactory<GUM_SCALAR>::__resolveElementName(
          const PRM<GUM_SCALAR>& prm,
          const O3Label& name,
          ErrorsContainer& errors ) {
        // If we've already found the interface real name
        if ( __eltName.exists( name.label() ) ) {
          return true;
        }
        // If name exists as is
        if ( prm.isType( name.label() ) or prm.isInterface( name.label() ) or
             prm.isClass( name.label() ) ) {
          __eltName.insert( name.label(), name.label() );
          return true;
        }
        // Trying with types
        auto matches = std::vector<const PRMObject*>();
        std::copy_if( prm.types().begin(),
                      prm.types().end(),
                      std::back_inserter( matches ),
                      [&name]( const Type<GUM_SCALAR>* type ) {
                        return ends_with( type->name(), name.label() );
                      } );
        // Trying with classes
        std::copy_if( prm.classes().begin(),
                      prm.classes().end(),
                      std::back_inserter( matches ),
                      [&name]( const Class<GUM_SCALAR>* c ) {
                        return ends_with( c->name(), name.label() );
                      } );
        // Trying with interfaces
        std::copy_if( prm.interfaces().begin(),
                      prm.interfaces().end(),
                      std::back_inserter( matches ),
                      [&name]( const Interface<GUM_SCALAR>* i ) {
                        return ends_with( i->name(), name.label() );
                      } );
        if ( matches.size() == 1 ) {  // One match is good
          __eltName.insert( name.label(), matches.back()->name() );
          return true;
        } else if ( matches.size() == 0 ) {  // 0 match is not found
          // Unknown name type
          const auto& pos = name.position();
          auto msg = std::stringstream();
          msg << "Interface error : "
              << "Unknown type, class or interface " << name.label();
          errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
          return false;
        } else {  // More than one match is ambiguous
          // Ambiguous name
          const auto& pos = name.position();
          auto msg = std::stringstream();
          msg << "Interface error : "
              << "Ambiguous name " << name.label()
              << ", found more than one elligible types, classes or interfaces: ";
          for ( auto i = 0; i < matches.size() - 1; ++i ) {
            msg << matches[i]->name() << ", ";
          }
          msg << matches.back()->name();
          errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
          return false;
        }
      }

      template <typename GUM_SCALAR>
      bool O3InterfaceFactory<GUM_SCALAR>::__resolveInterface(
          const O3Label& super, ErrorsContainer& errors ) {
        // If we've already found super real name
        if ( __superMap.exists( super.label() ) ) {
          return true;
        }
        // If super's name exists as is
        if ( __interfaceMap.exists( super.label() ) ) {
          __superMap.insert( super.label(), __interfaceMap[super.label()] );
          return true;
        }
        // Try to complete super type's module
        auto matches = std::vector<std::pair<std::string, NodeId>>();
        std::copy_if( __nameMap.begin(),
                      __nameMap.end(),
                      std::back_inserter( matches ),
                      [&super]( const std::pair<std::string, NodeId>& pair ) {
                        return ends_with( pair.first, super.label() );
                      } );
        if ( matches.size() == 1 ) {  // One match is good
          __superMap.insert( super.label(), __interfaceMap[matches.back().first] );
          return true;
        } else if ( matches.size() == 0 ) {  // 0 match is not found
          // Unknown super interface
          const auto& pos = super.position();
          auto msg = std::stringstream();
          msg << "Interface error : "
              << "Unknown interface " << super.label();
          errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
          __build = false;
          return false;
        } else {  // More than one match is ambiguous
          // Ambiguous name
          const auto& pos = super.position();
          auto msg = std::stringstream();
          msg << "Interface error : "
              << "Ambiguous interface " << super.label()
              << ", found more than one elligible interface: ";
          for ( auto i = 0; i < matches.size() - 1; ++i ) {
            msg << matches[i].first << ", ";
          }
          msg << matches.back().first;
          errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
          return false;
        }
      }

    } // o3prm
  } // prm
} // gum

