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
        __nameMap = HashTable<std::string, gum::NodeId>();
        __interfaceMap = HashTable<std::string, const O3Interface*>();
        __nodeMap = HashTable<NodeId, const O3Interface*>();
        __dag = DAG();
        __o3Interface = std::vector<const O3Interface*>();
      }

      template <typename GUM_SCALAR>
      bool O3InterfaceFactory<GUM_SCALAR>::__addInterface2Dag(
          PRM<GUM_SCALAR>& prm, const O3PRM& tmp_prm, std::ostream& output ) {
        // Adding nodes to the type inheritance graph
        for ( const auto& i : tmp_prm.interfaces() ) {
          if ( name_used<GUM_SCALAR>( prm, i->name().label() ) ) {
            // Raised if duplicate interface names
            const auto& pos = i->name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Interface error : "
                   << "Interface name " << i->name().label() << " exists already"
                   << std::endl;
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
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Interface error : "
                   << "Interface name " << i->name().label() << " exists already"
                   << std::endl;
            return false;
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool
      O3InterfaceFactory<GUM_SCALAR>::__addArcs2Dag( const O3PRM& prm,
                                                     std::ostream& output ) {
        // Adding arcs to the graph inheritance graph
        for ( const auto& i : prm.interfaces() ) {
          if ( i->super().label() != "" ) {
            try {
              auto head = __nameMap[i->super().label()];
              auto tail = __nameMap[i->name().label()];
              __dag.addArc( tail, head );
            } catch ( NotFound& e ) {
              // Unknown super interface
              const auto& pos = i->super().position();
              output << pos.file() << "|" << pos.line() << " col "
                     << pos.column() << "|"
                     << " Interface error : "
                     << "Unknown interface " << i->super().label() << std::endl;
              return false;
            } catch ( InvalidDirectedCycle& e ) {
              // Cyclic inheritance
              const auto& pos = i->position();
              output << pos.file() << "|" << pos.line() << " col "
                     << pos.column() << "|"
                     << " Interface error : "
                     << "Cyclic inheritance between interface "
                     << i->name().label() << " and interface "
                     << i->super().label() << std::endl;
              return false;
            }
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      void O3InterfaceFactory<GUM_SCALAR>::__setO3InterfaceCreationOrder() {
        auto topo_order = topologicalOrder( __dag );
        for ( auto iter = topo_order.rbegin(); iter != topo_order.rend();
              --iter ) {
          __o3Interface.push_back( __nodeMap[*iter] );
        }
      }

      template <typename GUM_SCALAR>
      bool O3InterfaceFactory<GUM_SCALAR>::__checkInterfaceElement(
          PRM<GUM_SCALAR>& prm,
          const O3InterfaceElement& elt,
          std::ostream& output ) {
        if ( not( prm.isType( elt.type().label() ) or
                  prm.isInterface( elt.type().label() ) or
                  prm.isClass( elt.type().label() ) ) ) {
          // Raised unknown type
          const auto& pos = elt.type().position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Interface error : "
                 << "Unknown identifier " << elt.type().label() << std::endl;
          return false;
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3InterfaceFactory<GUM_SCALAR>::__checkO3Interfaces(
          PRM<GUM_SCALAR>& prm, const O3PRM& tmp_prm, std::ostream& output ) {
        return __addInterface2Dag( prm, tmp_prm, output ) and
               __addArcs2Dag( tmp_prm, output );
      }

      template <typename GUM_SCALAR>
      void O3InterfaceFactory<GUM_SCALAR>::buildInterfaces(
          PRM<GUM_SCALAR>& prm, const O3PRM& tmp_prm, std::ostream& output ) {
        __initialize();
        PRMFactory<GUM_SCALAR> factory( &prm );
        if ( __checkO3Interfaces( prm, tmp_prm, output ) ) {
          __setO3InterfaceCreationOrder();
          for ( auto i : __o3Interface ) {
            factory.startInterface( i->name().label(), i->super().label() );
            factory.endInterface();
          }
        }
      }

      template <typename GUM_SCALAR>
      void O3InterfaceFactory<GUM_SCALAR>::buildElements(
          PRM<GUM_SCALAR>& prm, const O3PRM& tmp_prm, std::ostream& output ) {
        PRMFactory<GUM_SCALAR> factory( &prm );
        for ( auto i : __o3Interface ) {
          factory.continueInterface( i->name().label() );
          for ( const auto& elt : i->elements() ) {
            if ( __checkInterfaceElement( prm, elt, output ) ) {
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
                output << pos.file() << "|" << pos.line() << " col "
                       << pos.column() << "|"
                       << " Interface error : "
                       << "Element " << elt.name().label() << " already exists"
                       << std::endl;
              }
            }
          }
          factory.endInterface();
        }
      }

      template <typename GUM_SCALAR>
      void O3InterfaceFactory<GUM_SCALAR>::build( PRM<GUM_SCALAR>& prm,
                                                  const O3PRM& tmp_prm,
                                                  std::ostream& output ) {
        buildInterfaces( prm, tmp_prm, output );
        buildElements( prm, tmp_prm, output );
      }

      //template <typename GUM_SCALAR>
      //void O3InterfaceFactory<GUM_SCALAR>::build( PRM<GUM_SCALAR>& prm,
      //                                            const O3PRM& tmp_prm,
      //                                            std::ostream& output ) {
      //  __initialize();
      //  PRMFactory<GUM_SCALAR> factory( &prm );
      //  if ( __checkO3Interfaces( prm, tmp_prm, output ) ) {
      //    __setO3InterfaceCreationOrder();

      //    for ( auto i : __o3Interface ) {
      //      factory.startInterface( i->name().label(), i->super().label() );
      //      factory.endInterface();
      //    }

      //    for ( auto i : __o3Interface ) {
      //      factory.continueInterface( i->name().label() );
      //      for ( const auto& elt : i->elements() ) {
      //        if ( __checkInterfaceElement( prm, elt, output ) ) {
      //          try {
      //            if ( prm.isType( elt.type().label() ) ) {
      //              factory.addAttribute( elt.type().label(),
      //                                    elt.name().label() );
      //            } else {
      //              factory.addReferenceSlot(
      //                  elt.type().label(), elt.name().label(), false );
      //            }
      //          } catch ( OperationNotAllowed& e ) {
      //            // Duplicate or Wrong overload
      //            const auto& pos = elt.type().position();
      //            output << pos.file() << "|" << pos.line() << " col "
      //                   << pos.column() << "|"
      //                   << " Interface error : "
      //                   << "Element " << elt.name().label()
      //                   << " already exists" << std::endl;
      //          }
      //        }
      //      }
      //      factory.endInterface();
      //    }
      //  }
      //}
    }
  }
}

