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
        __typeMap = HashTable<std::string, const O3Type*>();
        __nodeMap = HashTable<NodeId, const O3Type*>();
        __dag = DAG();
      }

      template <typename GUM_SCALAR>
      bool O3TypeFactory<GUM_SCALAR>::__isPrimitiveType( const O3Type& type ) {
        return type.name().label() == "boolean";
      }

      template <typename GUM_SCALAR>
      void O3TypeFactory<GUM_SCALAR>::build( PRM<GUM_SCALAR>& prm,
                                             const O3PRM& my_o3prm,
                                             ErrorsContainer& errors ) {
        __initialize();
        PRMFactory<GUM_SCALAR> factory( &prm );
        // building types
        if ( __checkO3Types( prm, my_o3prm, errors ) ) {
          __setO3TypeCreationOrder();
          for ( auto type : __o3Types ) {
            if ( not __isPrimitiveType( *type ) ) {
              factory.startDiscreteType( type->name().label(),
                                         type->super().label() );
              for ( const auto& label : type->labels() ) {
                factory.addLabel( label.first.label(), label.second.label() );
              }
              factory.endDiscreteType();
            }
          }
        }
        // building int types
        if ( __checkO3IntTypes( my_o3prm, errors ) ) {
          for ( auto type : __o3IntTypes ) {
            factory.startDiscreteType( type->name().label() );
            auto n = type->end().value() - type->start().value();
            for ( auto i = 0; i <= n; ++i ) {
              auto s = std::stringstream();
              s << type->start().value() + i;
              factory.addLabel( std::string( s.str() ) );
            }
            factory.endDiscreteType();
          }
        }
      }

      template <typename GUM_SCALAR>
      bool
      O3TypeFactory<GUM_SCALAR>::__addTypes2Dag( PRM<GUM_SCALAR>& prm,
                                                 const O3PRM& tmp_prm,
                                                 ErrorsContainer& errors ) {
        // Adding nodes to the type inheritance graph
        for ( const auto& type : tmp_prm.types() ) {
          if ( not __isPrimitiveType( *type ) and
               name_used<GUM_SCALAR>( prm, type->name().label() ) ) {
            // Raised if duplicate type names
            const auto& pos = type->name().position();
            auto msg = std::stringstream();
            msg << "Type error : "
                << "Type name " << type->name().label() << " exists already";
            errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
            return false;
          }
          auto id = __dag.addNode();
          try {
            __nameMap.insert( type->name().label(), id );
            __typeMap.insert( type->name().label(), type.get() );
            __nodeMap.insert( id, type.get() );
          } catch ( DuplicateElement& e ) {
            // Raised if duplicate type names
            const auto& pos = type->name().position();
            auto msg = std::stringstream();
            msg << "Type error : "
                << "Type " << type->name().label() << " exists already";
            errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
            return false;
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3TypeFactory<GUM_SCALAR>::__addArcs2Dag( const O3PRM& prm,
                                                     ErrorsContainer& errors ) {
        // Adding arcs to the graph inheritance graph
        for ( const auto& type : prm.types() ) {
          if ( type->super().label() != "" ) {
            try {
              auto head = __nameMap[type->super().label()];
              auto tail = __nameMap[type->name().label()];
              __dag.addArc( tail, head );
            } catch ( NotFound& e ) {
              // Unknown super type
              const auto& pos = type->super().position();
              auto msg = std::stringstream();
              msg << "Type error : "
                  << "Unknown type " << type->super().label();
              errors.addError(
                  msg.str(), pos.file(), pos.line(), pos.column() );
              return false;
            } catch ( InvalidDirectedCycle& e ) {
              // Cyclic inheritance
              const auto& pos = type->position();
              auto msg = std::stringstream();
              msg << "Type error : "
                  << "Cyclic inheritance between type " << type->name().label()
                  << " and type " << type->super().label();
              errors.addError(
                  msg.str(), pos.file(), pos.line(), pos.column() );
              return false;
            }
            // Check labels inheritance
            for ( const auto& pair : type->labels() ) {
              const auto& super = __typeMap[type->super().label()];
              auto super_labels = Set<std::string>();
              for ( const auto& label : super->labels() ) {
                super_labels.insert( label.first.label() );
              }
              if ( not super_labels.contains( pair.second.label() ) ) {
                const auto& pos = pair.second.position();
                auto msg = std::stringstream();
                msg << "Type error : "
                    << "Unknown label " << pair.second.label() << " in "
                    << type->super().label();
                errors.addError(
                    msg.str(), pos.file(), pos.line(), pos.column() );
                return false;
              }
            }
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      void O3TypeFactory<GUM_SCALAR>::__setO3TypeCreationOrder() {
        auto topo_order = topological_order( __dag );
        for ( auto iter = topo_order.rbegin(); iter != topo_order.rend();
              --iter ) {
          if ( __nodeMap[*iter]->name().label() != "boolean" ) {
            __o3Types.push_back( __nodeMap[*iter] );
          }
        }
      }

      template <typename GUM_SCALAR>
      bool
      O3TypeFactory<GUM_SCALAR>::__checkO3Types( PRM<GUM_SCALAR>& prm,
                                                 const O3PRM& tmp_prm,
                                                 ErrorsContainer& errors ) {
        return __addTypes2Dag( prm, tmp_prm, errors ) and
               __addArcs2Dag( tmp_prm, errors );
      }

      template <typename GUM_SCALAR>
      bool
      O3TypeFactory<GUM_SCALAR>::__checkO3IntTypes( const O3PRM& prm,
                                                    ErrorsContainer& errors ) {
        auto names = gum::Set<std::string>();
        for ( const auto& type : prm.types() ) {
          try {
            names.insert( type->name().label() );
          } catch ( DuplicateElement& e ) {
            // Ignore, they are checked for in check_o3type
          }
        }
        for ( const auto& type : prm.int_types() ) {
          if ( names.contains( type->name().label() ) ) {
            // Raised if duplicate type names
            const auto& pos = type->name().position();
            auto msg = std::stringstream();
            msg << "Type error : "
                << "Type name " << type->name().label() << " already used";
            errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
            return false;
          } else if ( type->end().value() - type->start().value() < 1 ) {
            // Invalid range
            const auto& pos = type->name().position();
            auto msg = std::stringstream();
            msg << "Type error : "
                << "Invalid range " << type->start().value() << " -> "
                << type->end().value();
            errors.addError( msg.str(), pos.file(), pos.line(), pos.column() );
            return false;
          } else {
            __o3IntTypes.push_back( type.get() );
          }
        }
        return true;
      }

    }  // o3prm
  }    // prm
}  // gum
