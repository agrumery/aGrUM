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

      bool check_o3type( gum::prm::PRM<double>& prm,
                         const gum::prm::o3prm::O3Type& type,
                         std::stringstream& output ) {
        if ( prm.isType( type.name() ) ) {
          const auto& pos = type.position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Type error : "
                 << "Type name " << type.name() << " already used" << std::endl;
          return false;
        }
        if ( type.super() != "" ) {
          if ( not prm.isType( type.super() ) ) {
            const auto& pos = type.position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Type error : "
                   << "Unknown type " << type.super() << std::endl;
            return false;
          } else {
            for ( auto pair : type.labels() ) {
              const auto& super = prm.type( type.super() ).variable();
              auto super_labels = gum::Set<std::string>();
              for ( gum::Size i = 0; i < super.domainSize(); ++i ) {
                super_labels.insert( super.label( i ) );
              }
              if ( not super_labels.contains( pair.second ) ) {
                const auto& pos = type.position();
                output << pos.file() << "|" << pos.line() << " col "
                       << pos.column() << "|"
                       << " Type error : "
                       << "Label " << pair.second << " does not belong to type "
                       << type.super() << std::endl;
                return false;
              }
            }
          }
        }
        return true;
      }

      void build_prm( gum::prm::PRM<double>& prm,
                      gum::prm::o3prm::O3PRM& tmp_prm,
                      std::stringstream& output ) {
        gum::prm::PRMFactory<double> factory( &prm );
        // building types
        for ( auto type : tmp_prm.types() ) {
          if ( check_o3type( prm, type, output ) ) {
            factory.startDiscreteType( type.name(), type.super() );
            for ( auto label : type.labels() ) {
              factory.addLabel( label.first, label.second );
            }
            factory.endDiscreteType();
          }
        }
      }

      std::string clean( const std::string& text ) {
        std::regex regex( "TYPE_BODY" );
        std::stringstream output;
        output << std::regex_replace( text, regex, "type declaration" );
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
