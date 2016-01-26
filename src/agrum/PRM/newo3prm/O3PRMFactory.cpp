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

      void build_interface( PRM<double>& prm,
                            O3PRM& tmp_prm,
                            std::ostream& output ) {
        PRMFactory<double> factory( &prm );
        for ( auto i : tmp_prm.interfaces() ) {
          factory.startInterface( i.name().label(), i.super().label() );
          for ( auto elt : i.elements() ) {
            try {
              if ( prm.isType( elt.type().label() ) ) {
                factory.addAttribute( elt.type().label(), elt.name().label() );
              } else if ( prm.isInterface( elt.type().label() ) or
                          prm.isClass( elt.type().label() ) ) {
                factory.addReferenceSlot(
                    elt.type().label(), elt.name().label(), false );
              } else {
                // Raised unknown type
                const auto& pos = elt.type().position();
                output << pos.file() << "|" << pos.line() << " col "
                       << pos.column() << "|"
                       << " Interface error : "
                       << "Unknown identifier " << elt.type().label()
                       << std::endl;
              }
            } catch ( OperationNotAllowed& e ) {
              // Duplicate
              const auto& pos = elt.type().position();
              output << pos.file() << "|" << pos.line() << " col "
                     << pos.column() << "|"
                     << " Interface error : "
                     << " Element " << elt.type().label() << " already exists"
                     << std::endl;
            }
          }
          factory.endInterface();
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
        auto tmp_prm = gum::prm::o3prm::O3PRM();
        p.set_prm( &tmp_prm );
        p.Parse();
        const auto& errors = p.errors();
        for ( auto i = 0; i < p.errors().count(); ++i ) {
          auto err = p.errors().error( i );
          output << print( err ) << std::endl;
        }

        auto type_factory = O3TypeFactory<double>();
        type_factory.build( prm, tmp_prm, output );

        build_interface( prm, tmp_prm, output );
      }
    }
  }
}
