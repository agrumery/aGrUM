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
 * @brief Implementation for the O3TypeFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/prm/newo3prm/O3TypeFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      O3TypeFactory<GUM_SCALAR>::O3TypeFactory() {
        GUM_CONSTRUCTOR( O3TypeFactory );
      }

      template <typename GUM_SCALAR>
      O3TypeFactory<GUM_SCALAR>::O3TypeFactory(
          const O3TypeFactory<GUM_SCALAR>& src ) {
        GUM_CONS_CPY( O3TypeFactory );
      }

      template <typename GUM_SCALAR>
      O3TypeFactory<GUM_SCALAR>::O3TypeFactory(
          O3TypeFactory<GUM_SCALAR>&& src ) {
        GUM_CONS_MOV( O3TypeFactory );
      }

      template <typename GUM_SCALAR>
      O3TypeFactory<GUM_SCALAR>::~O3TypeFactory() {
        GUM_DESTRUCTOR( O3TypeFactory );
      }

      template <typename GUM_SCALAR>
      O3TypeFactory<GUM_SCALAR>& O3TypeFactory<GUM_SCALAR>::
      operator=( const O3TypeFactory<GUM_SCALAR>& src ) {
        return *this;
      }

      template <typename GUM_SCALAR>
      O3TypeFactory<GUM_SCALAR>& O3TypeFactory<GUM_SCALAR>::
      operator=( O3TypeFactory<GUM_SCALAR>&& src ) {
        return *this;
      }

      template <typename GUM_SCALAR>
      void O3TypeFactory<GUM_SCALAR>::__initialize() {
        __nameMap = HashTable<std::string, NodeId>();
        __typeMap = HashTable<std::string, O3Type>();
        __nodeMap = HashTable<NodeId, O3Type>();
        __dag = DAG();
        __boolean();
      }

      template <typename GUM_SCALAR>
      void O3TypeFactory<GUM_SCALAR>::build( PRM<GUM_SCALAR>& prm,
                                             O3PRM& my_o3prm,
                                             std::ostream& output ) {
        __initialize();
        PRMFactory<GUM_SCALAR> factory( &prm );
        // building types
        if ( __checkO3Types( prm, my_o3prm, output ) ) {
          __setO3TypeCreationOrder();
          for ( auto type : __o3Types ) {
            factory.startDiscreteType( type.name().label(),
                                       type.super().label() );
            for ( auto label : type.labels() ) {
              factory.addLabel( label.first.label(), label.second.label() );
            }
            factory.endDiscreteType();
          }
        }
        // building int types
        if ( __checkO3IntTypes( my_o3prm, output ) ) {
          for ( auto type : __o3IntTypes ) {
            factory.startDiscreteType( type.name().label() );
            auto n = type.end().value() - type.start().value();
            for ( auto i = 0; i <= n; ++i ) {
              auto s = std::stringstream();
              s << type.start().value() + i;
              factory.addLabel( std::string( s.str() ) );
            }
            factory.endDiscreteType();
          }
        }
      }

      template <typename GUM_SCALAR>
      void O3TypeFactory<GUM_SCALAR>::__boolean() {
        // Creating the boolean type
        auto name = O3Label( Position(), "boolean" );
        auto f = O3Label( Position(), "false" );
        auto t = O3Label( Position(), "true" );
        auto labels = O3Type::LabelMap();
        labels.push_back( O3Type::LabelPair( f, O3Label() ) );
        labels.push_back( O3Type::LabelPair( t, O3Label() ) );
        auto boolean = O3Type( Position(), name, O3Label(), labels );

        // Adding the boolean type
        auto id = __dag.addNode();
        __nameMap.insert( "boolean", id );
        __typeMap.insert( "boolean", boolean );
        __nodeMap.insert( id, boolean );
      }

      template <typename GUM_SCALAR>
      bool O3TypeFactory<GUM_SCALAR>::__addTypes2Dag( PRM<GUM_SCALAR>& prm,
                                                      O3PRM& tmp_prm,
                                                      std::ostream& output ) {
        // Adding nodes to the type inheritance graph
        for ( auto& type : tmp_prm.types() ) {
          if ( name_used<GUM_SCALAR>( prm, type.name().label() ) ) {
            // Raised if duplicate type names
            const auto& pos = type.name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Type error : "
                   << "Type name " << type.name().label() << " exists already"
                   << std::endl;
            return false;
          }
          auto id = __dag.addNode();
          try {
            __nameMap.insert( type.name().label(), id );
            __typeMap.insert( type.name().label(), type );
            __nodeMap.insert( id, type );
          } catch ( DuplicateElement& e ) {
            // Raised if duplicate type names
            const auto& pos = type.name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Type error : "
                   << "Type " << type.name().label() << " exists already"
                   << std::endl;
            return false;
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3TypeFactory<GUM_SCALAR>::__addArcs2Dag( O3PRM& prm,
                                                     std::ostream& output ) {
        // Adding arcs to the graph inheritance graph
        for ( const auto& type : prm.types() ) {
          if ( type.super().label() != "" ) {
            try {
              auto head = __nameMap[type.super().label()];
              auto tail = __nameMap[type.name().label()];
              __dag.addArc( tail, head );
            } catch ( NotFound& e ) {
              // Unknown super type
              const auto& pos = type.super().position();
              output << pos.file() << "|" << pos.line() << " col "
                     << pos.column() << "|"
                     << " Type error : "
                     << "Unknown type " << type.super().label() << std::endl;
              return false;
            } catch ( InvalidDirectedCycle& e ) {
              // Cyclic inheritance
              const auto& pos = type.position();
              output << pos.file() << "|" << pos.line() << " col "
                     << pos.column() << "|"
                     << " Type error : "
                     << "Cyclic inheritance between type "
                     << type.name().label() << " and type "
                     << type.super().label() << std::endl;
              return false;
            }
            // Check labels inheritance
            for ( auto pair : type.labels() ) {
              auto super = __typeMap[type.super().label()];
              auto super_labels = Set<std::string>();
              for ( auto label : super.labels() ) {
                super_labels.insert( label.first.label() );
              }
              if ( not super_labels.contains( pair.second.label() ) ) {
                const auto& pos = pair.second.position();
                output << pos.file() << "|" << pos.line() << " col "
                       << pos.column() << "|"
                       << " Type error : "
                       << "Unknown label " << pair.second.label() << " in "
                       << type.super().label() << std::endl;
                return false;
              }
            }
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      void O3TypeFactory<GUM_SCALAR>::__setO3TypeCreationOrder() {
        auto topo_order = topologicalOrder( __dag );
        for ( auto iter = topo_order.rbegin(); iter != topo_order.rend();
              --iter ) {
          if ( __nodeMap[*iter].name().label() != "boolean" ) {
            __o3Types.push_back( __nodeMap[*iter] );
          }
        }
      }

      template <typename GUM_SCALAR>
      bool O3TypeFactory<GUM_SCALAR>::__checkO3Types( PRM<GUM_SCALAR>& prm,
                                                      O3PRM& tmp_prm,
                                                      std::ostream& output ) {
        return __addTypes2Dag( prm, tmp_prm, output ) and
               __addArcs2Dag( tmp_prm, output );
      }

      template <typename GUM_SCALAR>
      bool
      O3TypeFactory<GUM_SCALAR>::__checkO3IntTypes( O3PRM& prm,
                                                    std::ostream& output ) {
        auto names = gum::Set<std::string>();
        for ( auto type : prm.types() ) {
          try {
            names.insert( type.name().label() );
          } catch ( DuplicateElement& e ) {
            // Ignore, they are checked for in check_o3type
          }
        }
        for ( auto type : prm.int_types() ) {
          if ( names.contains( type.name().label() ) ) {
            // Raised if duplicate type names
            const auto& pos = type.name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Type error : "
                   << "Type name " << type.name().label() << " already used"
                   << std::endl;
            return false;
          } else if ( type.end().value() - type.start().value() < 1 ) {
            // Invalid range
            const auto& pos = type.name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Type error : "
                   << "Invalid range " << type.start().value() << " -> "
                   << type.end().value() << std::endl;
            return false;
          } else {
            __o3IntTypes.push_back( type );
          }
        }
        return true;
      }

    }  // o3prm
  }    // prm
}  // gum
