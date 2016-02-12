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
 * @brief Implementation for the O3PRMFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/newo3prm/utils.h>
#include <agrum/PRM/newo3prm/o3prm.h>
#include <agrum/PRM/newo3prm/O3PRMFactory.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdint>

namespace gum {
  namespace prm {
    namespace o3prm {

      bool
      check_system( PRM<double>& prm, O3System& sys, std::ostream& output ) {
        auto name_map = HashTable<std::string, O3Instance*>();
        for ( auto& i : sys.instances() ) {
          if ( not prm.isClass( i.type().label() ) ) {
            const auto& pos = i.type().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Instance error : "
                   << "Instance type is not a class " << i.type().label()
                   << std::endl;
            return false;
          }
          if ( name_map.exists( i.name().label() ) ) {
            const auto& pos = i.type().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Instance error : "
                   << "Instance " << i.name().label() << " already exists"
                   << std::endl;
            return false;
          }
          name_map.insert( i.name().label(), &i );
        }
        for ( auto& ass : sys.assignments() ) {
          if ( ass.leftInstance().label() == ass.leftReference().label() ) {
            const auto& pos = ass.leftInstance().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Assignment error : "
                   << "Invalid left expression " << ass.leftInstance().label()
                   << std::endl;
            return false;
          }
          if ( not name_map.exists( ass.leftInstance().label() ) ) {
            const auto& pos = ass.leftInstance().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Assignment error : "
                   << "Instance " << ass.leftInstance().label() << " not found"
                   << std::endl;
            return false;
          }
          auto i = name_map[ass.leftInstance().label()];
          const auto& type = prm.getClass( i->type().label() );
          const auto& ref = ass.leftReference().label();
          if ( not( type.exists( ass.leftReference().label() ) and
                    ClassElement<double>::isReferenceSlot(
                        type.get( ref ) ) ) ) {
            const auto& pos = ass.leftReference().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Assignment error : "
                   << "Reference " << ass.leftReference().label()
                   << " not found in class " << type.name() << std::endl;
            return false;
          }
        }
        for ( auto& inc : sys.increments() ) {
          if ( inc.leftInstance().label() == inc.leftReference().label() ) {
            const auto& pos = inc.leftInstance().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Increment error : "
                   << "Invalid left expression " << inc.leftInstance().label()
                   << std::endl;
            return false;
          }
          if ( not name_map.exists( inc.leftInstance().label() ) ) {
            const auto& pos = inc.leftInstance().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Increment error : "
                   << "Instance " << inc.leftInstance().label() << " not found"
                   << std::endl;
            return false;
          }
          auto i = name_map[inc.leftInstance().label()];
          const auto& type = prm.getClass( i->type().label() );
          const auto& ref = inc.leftReference().label();
          if ( not( type.exists( inc.leftReference().label() ) and
                    ClassElement<double>::isReferenceSlot(
                        type.get( ref ) ) ) ) {
            const auto& pos = inc.leftReference().position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Increment error : "
                   << "Reference " << inc.leftReference().label()
                   << " not found in class " << type.name() << std::endl;
            return false;
          }
        }
        return true;
      }

      void
      build_systems( PRM<double>& prm, O3PRM& o3_prm, std::ostream& output ) {
        PRMFactory<double> factory( &prm );
        for ( auto& sys : o3_prm.systems() ) {
          if ( check_system( prm, *sys, output ) ) {
            factory.startSystem( sys->name().label() );
            for ( auto& i : sys->instances() ) {
              if ( i.size().value() > 1 ) {
                factory.addArray(
                    i.type().label(), i.name().label(), i.size().value() );
              } else {
                factory.addInstance( i.type().label(), i.name().label() );
              }
            }
            for ( auto& ass : sys->assignments() ) {
              factory.setReferenceSlot( ass.leftInstance().label(),
                                        ass.leftReference().label(),
                                        ass.rightInstance().label() );
            }
            for ( auto& inc : sys->increments() ) {
              factory.setReferenceSlot( inc.leftInstance().label(),
                                        inc.leftReference().label(),
                                        inc.rightInstance().label() );
            }
            factory.endSystem();
          }
        }
      }

      using o3prm_scanner = gum::prm::newo3prm::Scanner;
      using o3prm_parser = gum::prm::newo3prm::Parser;

      std::string read_stream( std::istream& input ) {
        if ( input ) {
          input.seekg( 0, input.end );
          auto length = input.tellg();
          input.seekg( 0, input.beg );

          auto str = std::string();
          str.resize( length, ' ' );
          auto begin = &*str.begin();

          input.read( begin, length );

          return std::move(str);
        }
        GUM_ERROR( OperationNotAllowed, "Could not open file" );
      }

      void parse_stream( gum::prm::PRM<double>& prm,
                         std::istream& input,
                         std::ostream& output ) {
        auto sBuff = read_stream( input );
        auto buffer = std::unique_ptr<unsigned char[]>(
            new unsigned char[sBuff.length() + 1] );
        strcpy( (char*)buffer.get(), sBuff.c_str() );
        auto s = o3prm_scanner( buffer.get(), sBuff.length() + 1, "" );
        auto p = o3prm_parser( &s );
        auto o3_prm = gum::prm::o3prm::O3PRM();
        p.set_prm( &o3_prm );
        p.Parse();
        const auto& errors = p.errors();
        for ( auto i = 0; i < p.errors().count(); ++i ) {
          auto err = p.errors().error( i );
          output << print( err ) << std::endl;
        }

        auto type_factory = O3TypeFactory<double>();

        auto interface_factory = O3InterfaceFactory<double>();
        auto class_factory = O3ClassFactory<double>();

        type_factory.build( prm, o3_prm, output );
        interface_factory.buildInterfaces( prm, o3_prm, output );
        class_factory.buildClasses( prm, o3_prm, output );
        interface_factory.buildElements( prm, o3_prm, output );
        class_factory.buildImplementations( prm, o3_prm, output );
        class_factory.buildParameters( prm, o3_prm, output );
        class_factory.buildReferenceSlots( prm, o3_prm, output );
        class_factory.declareAttributes( prm, o3_prm, output );
        class_factory.buildAggregates( prm, o3_prm, output );
        class_factory.completeAttributes( prm, o3_prm, output );
        build_systems( prm, o3_prm, output );
      }
    }
  }
}
