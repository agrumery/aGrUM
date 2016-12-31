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

#include <agrum/PRM/o3prm/O3InterfaceFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      INLINE O3InterfaceFactory<GUM_SCALAR>::O3InterfaceFactory(
          PRM<GUM_SCALAR>&          prm,
          O3PRM&                    o3_prm,
          O3NameSolver<GUM_SCALAR>& solver,
          ErrorsContainer&          errors )
          : __prm( &prm )
          , __o3_prm( &o3_prm )
          , __solver( &solver )
          , __errors( &errors ) {
        GUM_CONSTRUCTOR( O3InterfaceFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3InterfaceFactory<GUM_SCALAR>::O3InterfaceFactory(
          const O3InterfaceFactory<GUM_SCALAR>& src )
          : __prm( src.__prm )
          , __o3_prm( src.__o3_prm )
          , __solver( src.__solver )
          , __errors( src.__errors ) {
        GUM_CONS_CPY( O3InterfaceFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3InterfaceFactory<GUM_SCALAR>::O3InterfaceFactory(
          O3InterfaceFactory<GUM_SCALAR>&& src )
          : __prm( std::move( src.__prm ) )
          , __o3_prm( std::move( src.__o3_prm ) )
          , __solver( std::move( src.__solver ) )
          , __errors( std::move( src.__errors ) ) {
        GUM_CONS_MOV( O3InterfaceFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3InterfaceFactory<GUM_SCALAR>::~O3InterfaceFactory() {
        GUM_DESTRUCTOR( O3InterfaceFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3InterfaceFactory<GUM_SCALAR>& O3InterfaceFactory<GUM_SCALAR>::
      operator=( const O3InterfaceFactory<GUM_SCALAR>& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = src.__prm;
        __o3_prm = src.__o3_prm;
        __solver = src.__solver;
        __errors = src.__errors;
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE O3InterfaceFactory<GUM_SCALAR>& O3InterfaceFactory<GUM_SCALAR>::
      operator=( O3InterfaceFactory<GUM_SCALAR>&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = std::move( src.__prm );
        __o3_prm = std::move( src.__o3_prm );
        __solver = std::move( src.__solver );
        __errors = std::move( src.__errors );
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE void O3InterfaceFactory<GUM_SCALAR>::buildInterfaces() {

        PRMFactory<GUM_SCALAR> factory( __prm );
        if ( __checkO3Interfaces() ) {

          __setO3InterfaceCreationOrder();

          for ( auto i : __o3Interface ) {

            if ( __solver->resolveInterface( i->superLabel() ) ) {
              factory.startInterface(
                  i->name().label(), i->superLabel().label(), true );
              factory.endInterface();
            }
          }
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3InterfaceFactory<GUM_SCALAR>::__checkO3Interfaces() {
        return __addInterface2Dag() && __addArcs2Dag();
      }

      template <typename GUM_SCALAR>
      INLINE bool O3InterfaceFactory<GUM_SCALAR>::__addInterface2Dag() {

        // Adding nodes to the type inheritance graph
        for ( auto& i : __o3_prm->interfaces() ) {

          auto id = __dag.addNode();
          try {

            __nameMap.insert( i->name().label(), id );
            __interfaceMap.insert( i->name().label(), i.get() );
            __nodeMap.insert( id, i.get() );

          } catch ( DuplicateElement& ) {
            // Raised if duplicate type names
            O3PRM_INTERFACE_DUPLICATE( i->name(), *__errors );
            return false;
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3InterfaceFactory<GUM_SCALAR>::__addArcs2Dag() {

        // Adding arcs to the graph inheritance graph
        for ( auto& i : __o3_prm->interfaces() ) {

          if ( i->superLabel().label() != "" ) {

            if ( !__solver->resolveInterface( i->superLabel() ) ) {
              return false;
            }

            auto head = __nameMap[i->superLabel().label()];
            auto tail = __nameMap[i->name().label()];

            try {

              __dag.addArc( tail, head );

            } catch ( InvalidDirectedCycle& ) {
              // Cyclic inheritance
              O3PRM_INTERFACE_CYCLIC_INHERITANCE(
                  i->name(), i->superLabel(), *__errors );
              return false;
            }
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      INLINE void O3InterfaceFactory<GUM_SCALAR>::__setO3InterfaceCreationOrder() {

        auto topo_order = __dag.topologicalOrder();
        for ( auto id = topo_order.rbegin(); id != topo_order.rend(); --id ) {
          __o3Interface.push_back( __nodeMap[*id] );
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3InterfaceFactory<GUM_SCALAR>::buildElements() {

        PRMFactory<GUM_SCALAR> factory( __prm );

        for ( auto i : __o3Interface ) {

          __prm->getInterface( i->name().label() ).inheritInterface();

          factory.continueInterface( i->name().label() );

          for ( auto& elt : i->elements() ) {

            if ( __checkInterfaceElement( *i, elt ) ) {

              try {

                if ( __prm->isType( elt.type().label() ) ) {

                  factory.addAttribute( elt.type().label(), elt.name().label() );
                } else {

                  factory.addReferenceSlot(
                      elt.type().label(), elt.name().label(), elt.isArray() );
                }

              } catch ( OperationNotAllowed& ) {
                // Duplicate or Wrong overload
                O3PRM_INTERFACE_DUPLICATE_ELEMENT( elt, *__errors );
              }
            }
          }
          factory.endInterface();
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3InterfaceFactory<GUM_SCALAR>::__checkInterfaceElement(
          O3Interface& i, O3InterfaceElement& elt ) {

        if ( !__solver->resolveClassElement( elt.type() ) ) {
          return false;
        }

        if ( __prm->isType( elt.type().label() ) && elt.isArray() ) {
          O3PRM_INTERFACE_ILLEGAL_ARRAY( elt.name(), *__errors );
          return false;
        }

        const auto& real_i = __prm->getInterface( i.name().label() );

        if ( real_i.exists( elt.name().label() ) ) {
          if ( !__checkOverloadLegality( i, elt ) ) {
            return false;
          }
        }

        if ( !__checkCyclicReference( i, elt ) ) {
          return false;
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3InterfaceFactory<GUM_SCALAR>::__checkOverloadLegality(
          O3Interface& i, O3InterfaceElement& elt ) {

        const auto& real_i = __prm->getInterface( i.name().label() );
        const auto& real_elt = real_i.get( elt.name().label() );

        if ( PRMClassElement<GUM_SCALAR>::isAttribute( real_elt ) ) {
          return __checkAttributeOverloadLegality( i, elt );
        }

        if ( PRMClassElement<GUM_SCALAR>::isReferenceSlot( real_elt ) ) {
          return __checkReferenceOverloadLegality( i, elt );
        }

        return false;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3InterfaceFactory<GUM_SCALAR>::__checkAttributeOverloadLegality(
          O3Interface& i, O3InterfaceElement& elt ) {

        const auto& real_i = __prm->getInterface( i.name().label() );
        const auto& real_elt = real_i.get( elt.name().label() );

        const auto& sub_type = __prm->type( elt.type().label() );
        const auto& super_type = real_elt.type();

        if ( !sub_type.isSubTypeOf( super_type ) ) {
          O3PRM_INTERFACE_ILLEGAL_OVERLOAD( elt, *__errors );
          return false;
        }

        if ( sub_type.name() == super_type.name() ) {
          O3PRM_INTERFACE_DUPLICATE_ELEMENT( elt, *__errors );
          return false;
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3InterfaceFactory<GUM_SCALAR>::__checkReferenceOverloadLegality(
          O3Interface& i, O3InterfaceElement& elt ) {
        const auto& real_i = __prm->getInterface( i.name().label() );
        const auto& real_elt = static_cast<const PRMReferenceSlot<GUM_SCALAR>&>(
            real_i.get( elt.name().label() ) );

        auto sub_type = (const PRMClassElementContainer<GUM_SCALAR>*)nullptr;

        if ( __prm->isClass( elt.type().label() ) ) {
          sub_type = &( __prm->getClass( elt.type().label() ) );
        } else {
          sub_type = &( __prm->getInterface( elt.type().label() ) );
        }

        auto super_type = &( real_elt.slotType() );

        if ( !sub_type->isSubTypeOf( *super_type ) ) {
          O3PRM_INTERFACE_ILLEGAL_OVERLOAD( elt, *__errors );
          return false;
        }

        if ( sub_type->name() == super_type->name() ) {
          O3PRM_INTERFACE_DUPLICATE_ELEMENT( elt, *__errors );
          return false;
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3InterfaceFactory<GUM_SCALAR>::__checkCyclicReference(
          O3Interface& i, O3InterfaceElement& elt ) {

        if ( __prm->isInterface( elt.type().label() ) ||
             __prm->isClass( elt.type().label() ) ) {

          auto ref_type = (const PRMClassElementContainer<GUM_SCALAR>*)nullptr;

          if ( __prm->isInterface( elt.type().label() ) ) {
            ref_type = &( __prm->getInterface( elt.type().label() ) );
          } else {
            ref_type = &( __prm->getClass( elt.type().label() ) );
          }

          const auto& real_i = __prm->getInterface( i.name().label() );

          if ( &real_i == ref_type ) {
            O3PRM_INTERFACE_SELF_REFERENCE( i, elt, *__errors );
            return false;
          }

          if ( ref_type->isSubTypeOf( real_i ) ) {
            O3PRM_INTERFACE_ILLEGAL_SUB_REFERENCE( i, elt, *__errors );
            return false;
          }
        }

        return true;
      }

    }  // o3prm
  }    // prm
}  // gum
