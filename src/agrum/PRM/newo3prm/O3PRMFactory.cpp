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

#include <agrum/PRM/newo3prm/O3PRMFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      using o3prm_scanner = gum::prm::newo3prm::Scanner;
      using o3prm_parser = gum::prm::newo3prm::Parser;

      void build_class( PRM<double>& prm,
                        O3PRM& o3_prm,
                        std::ostream& output ) {
        PRMFactory<double> factory(&prm);
        for ( const auto& c: o3_prm.classes() ) {
          Set<std::string> implements;
          for (const auto& i: c->interfaces()) {
            implements.insert(i.label());
          }
          factory.startClass(
              c->name().label(), c->super().label(), &implements );
          for (const auto& attr: c->elements()) {
            factory.startAttribute( attr.type().label(), attr.name().label() );
            std::vector<double> values;
            for (const auto& val: attr.values()) {
              values.push_back( val.value() );
            }
            factory.setRawCPFByColumns( values );
            factory.endAttribute();
          }
          factory.endClass();
        }
      }

      void parse_stream( gum::prm::PRM<double>& prm,
                         std::stringstream& input,
                         std::stringstream& output ) {
        auto buffer = std::unique_ptr<unsigned char[]>(
            new unsigned char[input.str().length() + 1] );
        strcpy( (char*)buffer.get(), input.str().c_str() );
        auto s = o3prm_scanner( buffer.get(), input.str().length() + 1, "" );
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
        type_factory.build( prm, o3_prm, output );

        auto interface_factory = O3InterfaceFactory<double>();
        interface_factory.build( prm, o3_prm, output );

        build_class( prm, o3_prm, output );
      }
    }
  }
}
