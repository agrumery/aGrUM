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


      bool check_attribute( PRM<double>& prm,
                            const O3Class& o3_c,
                            const O3Attribute& attr,
                            std::ostream& output ) {
        // Check type
        if ( not prm.isType( attr.type().label() ) ) {
          const auto& pos = attr.type().position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Attribute error : "
                 << "Type " << attr.type().label() << " not found" << std::endl;
          return false;
        }
        const auto& type = prm.type( attr.type().label() );
        Size domain_size = type->domainSize();
        // Check for parents existence
        const auto& c = prm.getClass( o3_c.name().label() );
        for ( const auto& prnt : attr.parents() ) {
          if ( not c.exists( prnt.label() ) ) {
            const auto& pos = prnt.position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Attribute error : "
                   << "Parent " << prnt.label() << " not found" << std::endl;
            return false;
          }
          const auto& elt = c.get( prnt.label() );
          if ( not( gum::prm::ClassElement<double>::isAttribute( elt ) or
                    gum::prm::ClassElement<double>::isSlotChain( elt ) ) ) {
            const auto& pos = prnt.position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Attribute error : "
                   << "Illegal parent " << prnt.label() << std::endl;
            return false;
          }
          domain_size *= elt.type()->domainSize();
        }
        // Check for CPT size
        if ( domain_size != attr.values().size() ) {
          const auto& pos = attr.name().position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Attribute error : "
                 << "Illegal CPT size, expected " << domain_size << " found "
                 << attr.values().size() << std::endl;
          return false;
        }
        // Check that CPT sums to 1
        Size parent_size = domain_size / type->domainSize();
        auto values = std::vector<float>( parent_size );
        for ( auto i = 0; i < attr.values().size(); ++i ) {
          auto idx = i % parent_size;
          values[idx] += attr.values()[i].value();
        }
        //{
        //  std::stringstream plop;
        //  for ( auto f : values ) {
        //    plop << f << " (" << (std::abs( f - 1.0f ) < 1.0e-6) << ") ";
        //  }
        //  GUM_TRACE( plop.str() )
        //}
        if ( not std::all_of(
                 values.cbegin(),
                 values.cend(),
                 []( float f ) { return std::abs( f - 1.0f ) < 1.0e-6; } ) ) {
          const auto& pos = attr.name().position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Attribute error : "
                 << "CPT does not sum to 1" << std::endl;
          return false;
        }
        return true;
      }

      void
      build_class( PRM<double>& prm, O3PRM& o3_prm, std::ostream& output ) {
        PRMFactory<double> factory( &prm );
        for ( const auto& c : o3_prm.classes() ) {
          Set<std::string> implements;
          for ( const auto& i : c->interfaces() ) {
            implements.insert( i.label() );
          }
          factory.startClass(
              c->name().label(), c->super().label(), &implements );
          for ( const auto& attr : c->elements() ) {
            if ( check_attribute( prm, *c, attr, output ) ) {
              factory.startAttribute( attr.type().label(),
                                      attr.name().label() );
              for ( const auto& parent : attr.parents() ) {
                factory.addParent( parent.label() );
              }
              std::vector<double> values;
              for ( const auto& val : attr.values() ) {
                values.push_back( val.value() );
              }
              factory.setRawCPFByColumns( values );
              factory.endAttribute();
            }
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
