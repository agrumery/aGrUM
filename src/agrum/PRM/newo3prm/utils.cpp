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
 * @brief Implementation for utilities functions for the O3PRM language.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/newO3prm/utils.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      using o3prm_scanner = gum::prm::newo3prm::Scanner;
      using o3prm_parser = gum::prm::newo3prm::Parser;

      O3Type boolean_type() {
        auto name = O3Label( Position(), "boolean" );
        auto f = O3Label( Position(), "false" );
        auto t = O3Label( Position(), "true" );
        auto labels = O3Type::LabelMap();
        labels.push_back( O3Type::LabelPair( f, O3Label() ) );
        labels.push_back( O3Type::LabelPair( t, O3Label() ) );
        return std::move( O3Type( Position(), name, O3Label(), labels ) );
      }

      struct CheckO3Type {
        gum::HashTable<std::string, gum::NodeId> nameMap;
        gum::HashTable<std::string, O3Type> typeMap;
        gum::HashTable<NodeId, O3Type> nodeMap;
        gum::DAG dag;
      };

      bool add_types2dag( CheckO3Type& data,
                          gum::prm::o3prm::O3PRM& prm,
                          std::stringstream& output ) {
        // Adding nodes to the type inheritance graph
        for ( auto& type : prm.types() ) {
          auto id = data.dag.addNode();
          try {
            data.nameMap.insert( type.name().label(), id );
            data.typeMap.insert( type.name().label(), type );
            data.nodeMap.insert( id, type );
          } catch ( gum::DuplicateElement& e ) {
            // Raised if duplicate type names
            const auto& pos = type.name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Type error : "
                   << "Type name " << type.name().label() << " already used"
                   << std::endl;
            return false;
          }
        }
        return true;
      }

      bool add_arcs2dag( CheckO3Type& data,
                         gum::prm::o3prm::O3PRM& prm,
                         std::stringstream& output ) {
        // Adding arcs to the graph inheritance graph
        for ( const auto& type : prm.types() ) {
          if ( type.super().label() != "" ) {
            try {
              auto head = data.nameMap[type.super().label()];
              auto tail = data.nameMap[type.name().label()];
              data.dag.addArc( tail, head );
            } catch ( gum::NotFound& e ) {
              // Unknown super type
              const auto& pos = type.super().position();
              output << pos.file() << "|" << pos.line() << " col "
                     << pos.column() << "|"
                     << " Type error : "
                     << "Unknown type " << type.super().label() << std::endl;
              return false;
            } catch ( gum::InvalidDirectedCycle& e ) {
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
              auto super = data.typeMap[type.super().label()];
              auto super_labels = gum::Set<std::string>();
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

      gum::Sequence<NodeId> topologicalOrder( const gum::DAG& src ) {
        auto dag = src;
        auto roots = std::vector<NodeId>();
        auto order = gum::Sequence<NodeId>();
        for ( const auto node : dag.nodes() ) {
          if ( dag.parents( node ).empty() ) {
            roots.push_back( node );
          }
        }
        while ( roots.size() ) {
          order.insert( roots.back() );
          roots.pop_back();
          while ( dag.children( order.back() ).size() ) {
            auto child = *( dag.children( order.back() ).begin() );
            dag.eraseArc( Arc( order.back(), child ) );
            if ( dag.parents( child ).empty() ) {
              roots.push_back( child );
            }
          }
        }
        return std::move( order );
      }

      void add_type_order( CheckO3Type& data, std::vector<O3Type>& types ) {
        auto topo_order = topologicalOrder( data.dag );
        for ( auto iter = topo_order.rbegin(); iter != topo_order.rend();
              --iter ) {
          if ( data.nodeMap[*iter].name().label() != "boolean" ) {
            types.push_back( data.nodeMap[*iter] );
          }
        }
      }

      bool check_o3type( gum::prm::o3prm::O3PRM& prm,
                         std::vector<O3Type>& types,
                         std::stringstream& output ) {
        auto data = CheckO3Type();
        // Adding boolean as a primitive type
        auto boolean = boolean_type();
        auto id = data.dag.addNode();
        data.nameMap.insert( "boolean", id );
        data.typeMap.insert( "boolean", boolean );
        data.nodeMap.insert( id, boolean );

        if ( not add_types2dag( data, prm, output ) ) {
          return false;
        }
        if ( not add_arcs2dag( data, prm, output ) ) {
          return false;
        }
        add_type_order( data, types );

        return true;
      }

      bool check_o3type( gum::prm::PRM<double>& prm,
                         const gum::prm::o3prm::O3Type& type,
                         std::stringstream& output ) {
        if ( prm.isType( type.name().label() ) ) {
          const auto& pos = type.position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Type error : "
                 << "Type name " << type.name().label() << " already used" << std::endl;
          return false;
        }
        if ( type.super().label() != "" ) {
          if ( not prm.isType( type.super().label() ) ) {
            const auto& pos = type.super().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Type error : "
                   << "Unknown type " << type.super().label() << std::endl;
            return false;
          } else {
            for ( auto pair : type.labels() ) {
              const auto& super = prm.type( type.super().label() ).variable();
              auto super_labels = gum::Set<std::string>();
              for ( gum::Size i = 0; i < super.domainSize(); ++i ) {
                super_labels.insert( super.label( i ) );
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

      bool check_o3inttype( const gum::prm::o3prm::O3PRM& tmp_prm,
                            std::vector<O3IntType>& int_types,
                            std::stringstream& output ) {
        auto names = gum::Set<std::string>();
        for ( auto type: tmp_prm.types() ) {
          try {
            names.insert( type.name().label() );
          } catch (DuplicateElement& e) {
            // Ignore, they are checked for in check_o3type
          }
        }
        for ( auto type: tmp_prm.int_types() ) {
          if ( names.contains( type.name().label() ) ) {
            // Raised if duplicate type names
            const auto& pos = type.name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Type error : "
                   << "Type name " << type.name().label() << " already used"
                   << std::endl;
            return false;
          } else if ( type.end().value() - type.start().value() < 2 ) {
            // Invalid range
            const auto& pos = type.name().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Type error : "
                   << "Invalid range " << type.start().value() << " -> "
                   << type.end().value() << std::endl;
            return false;
          } else {
            int_types.push_back( type );
          }
        }
        return true;
      }

      void build_prm( gum::prm::PRM<double>& prm,
                      gum::prm::o3prm::O3PRM& tmp_prm,
                      std::stringstream& output ) {
        gum::prm::PRMFactory<double> factory( &prm );
        // building types
        auto types = std::vector<O3Type>();
        if ( check_o3type( tmp_prm, types, output ) ) {
          for ( auto type : types ) {
            factory.startDiscreteType( type.name().label(),
                                       type.super().label() );
            for ( auto label : type.labels() ) {
              factory.addLabel( label.first.label(), label.second.label() );
            }
            factory.endDiscreteType();
          }
        }
        // building int types
        auto int_types = std::vector<O3IntType>();
        if ( check_o3inttype( tmp_prm, int_types, output ) ) {
          for ( auto type : int_types ) {
            factory.startDiscreteType( type.name().label() );
            for ( auto i = type.start().value(); i <= type.end().value();
                  ++i ) {
              auto s = std::stringstream();
              s << i;
              factory.addLabel( std::string( s.str() ) );
            }
            factory.endDiscreteType();
          }
        }
      }

      std::string clean( const std::string& text ) {
        auto regex = std::regex( "TYPE_(BODY|UNIT)" );
        auto output = std::stringstream();
        output << std::regex_replace( text, regex, "declaration" );
        return output.str();
      }

      std::string print( const ParseError& err ) {
        std::stringstream s;
        s << err.filename << "|" << err.line << " col " << err.column << "| "
          << clean(err.msg);
        return s.str();
      }

      void parse_stream( gum::prm::PRM<double>& prm,
                         std::stringstream& input,
                         std::stringstream& output ) {
        auto buffer = std::unique_ptr<unsigned char[]>(
            new unsigned char[input.str().length() + 1] );
        strcpy( (char*)buffer.get(), input.str().c_str() );
        auto s = o3prm_scanner(
            buffer.get(), input.str().length() + 1, "" );
        auto p = o3prm_parser( &s );
        auto tmp_prm = gum::prm::o3prm::O3PRM();
        p.set_prm( &tmp_prm );
        p.Parse();
        const auto& errors = p.errors();
        for ( auto i = 0; i < p.errors().count(); ++i ) {
          auto err = p.errors().error( i );
          output << print( err ) << std::endl;
        }
        build_prm( prm, tmp_prm, output );
      }
    }
  }
}
