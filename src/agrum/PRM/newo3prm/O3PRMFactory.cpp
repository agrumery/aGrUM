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

      bool check_aggregate( const PRM<double>& prm,
                            O3Class& c,
                            O3Aggregate& agg,
                            std::ostream& output ) {
        return true;
      }

      bool check_ref( const PRM<double>& prm, O3ReferenceSlot& ref ) {
        return true;
      }

      bool check_slotchain_link( const ClassElementContainer<double>* c,
                                 const O3Label& chain,
                                 const std::string& s,
                                 std::ostream& output ) {
        if ( not c->exists( s ) ) {
          const auto& pos = chain.position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Slot chain error : "
                 << "Link " << s << " in chain " << chain.label()
                 << " not found" << std::endl;
          return false;
        }
        return true;
      }

      const ClassElement<double>*
      resolve_slotchain( const ClassElementContainer<double>& c,
                         const O3Label& chain,
                         std::ostream& output ) {
        auto s = chain.label();
        auto idx = (std::size_t)0;
        auto current = &c;
        for ( idx = s.find( '.', idx ); idx != std::string::npos;
              idx = s.find( '.', idx ) ) {
          auto value = s.substr( 0, idx );
          if ( not check_slotchain_link( current, chain, value, output ) ) {
            return nullptr;
          }
          auto ref = dynamic_cast<const ReferenceSlot<double>*>(
              &( current->get( value ) ) );
          if ( ref ) {
            current = &( ref->slotType() );
            s = s.substr( idx + 1 );
          }
        }
        if ( not check_slotchain_link( current, chain, s, output ) ) {
          return nullptr;
        }
        return &( current->get( s ) );
      }

      bool check_raw_cpt( const PRM<double>& prm,
                          const Class<double>& c,
                          O3RawCPT& attr,
                          std::ostream& output ) {
        const auto& type = prm.type( attr.type().label() );

        auto domainSize = type->domainSize();
        for ( auto& prnt : attr.parents() ) {
          try {
            domainSize *= c.get( prnt.label() ).type()->domainSize();
          } catch ( NotFound& ) {
            // If we are here, all parents have been check so resolve_slotchain
            // will not raise an error and not return a null_ptr
            domainSize *=
                resolve_slotchain( c, prnt, output )->type()->domainSize();
          }
        }
        // Check for CPT size
        if ( domainSize != attr.values().size() ) {
          const auto& pos = attr.name().position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Attribute error : "
                 << "Illegal CPT size, expected " << domainSize << " found "
                 << attr.values().size() << std::endl;
          return false;
        }
        // Add parameters to formulas
        for ( auto& f : attr.values() ) {
          for ( const auto& values : c.scope() ) {
            f.formula().variables().insert( values.first,
                                            values.second->value() );
          }
        }
        // Check that CPT sums to 1
        Size parent_size = domainSize / type->domainSize();
        auto values = std::vector<float>( parent_size );
        for ( auto i = 0; i < attr.values().size(); ++i ) {
          auto idx = i % parent_size;
          values[idx] += attr.values()[i].formula().result();
        }
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

      bool check_local_parent( const Class<double>& c,
                               const O3Label& prnt,
                               std::ostream& output ) {
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
        return true;
      }

      bool check_remote_parent( const ClassElementContainer<double>& c,
                                const O3Label& prnt,
                                std::ostream& output ) {
        return resolve_slotchain( c, prnt, output ) != nullptr;
      }

      bool check_parent( const Class<double>& c,
                         const O3Label& prnt,
                         std::ostream& output ) {
        if ( prnt.label().find( '.' ) == std::string::npos ) {
          return check_local_parent( c, prnt, output );
        } else {
          return check_remote_parent( c, prnt, output );
        }
      }

      bool check_rule_cpt( const Class<double>& c,
                           O3RuleCPT& attr,
                           std::ostream& output ) {
        for ( auto& rule : attr.rules() ) {
          // Check labels for all parents
          if ( rule.first.size() != attr.parents().size() ) {
            const auto& pos = rule.first.front().position();
            output << pos.file() << "|" << pos.line() << "|"
                   << " Attribute error : "
                   << "Expected " << attr.parents().size()
                   << " value(s), found " << rule.first.size() << std::endl;
            return false;
          }
          // Add parameters to formulas
          for ( auto& f : rule.second ) {
            for ( const auto& values : c.scope() ) {
              f.formula().variables().insert( values.first,
                                              values.second->value() );
            }
          }
          // Check that formulas are valid and sums to 1
          auto sum = 0.0f;
          for ( const auto& f : rule.second ) {
            try {
              sum += f.formula().result();
            } catch ( OperationNotAllowed& ) {
              const auto& pos = f.position();
              output << pos.file() << "|" << pos.line() << "|"
                     << " Attribute error : "
                     << "Unknown value in CPT: \"" << f.formula().formula()
                     << "\"";
              return false;
            }
          }
          // Check that CPT sums to 1
          if ( std::abs( sum - 1.0f ) > 1e-6 ) {
            const auto& pos = rule.first.front().position();
            output << pos.file() << "|" << pos.line() << "|"
                   << " Attribute error : "
                   << "CPT does not sum to 1";
            return false;
          }
        }
        return true;
      }

      bool check_attribute( PRM<double>& prm,
                            const O3Class& o3_c,
                            O3Attribute& attr,
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
        // Check for parents existence
        const auto& c = prm.getClass( o3_c.name().label() );
        for ( auto& prnt : attr.parents() ) {
          if ( not check_parent( c, prnt, output ) ) {
            return false;
          }
        }
        // Check that CPT sums to 1
        auto raw = dynamic_cast<O3RawCPT*>( &attr );
        if ( raw ) {
          return check_raw_cpt( prm, c, *raw, output );
        }
        auto rule = dynamic_cast<O3RuleCPT*>( &attr );
        if ( rule ) {
          return check_rule_cpt( c, *rule, output );
        }
        return true;
      }

      void
      build_class( PRM<double>& prm, O3PRM& o3_prm, std::ostream& output ) {
        PRMFactory<double> factory( &prm );
        for ( const auto& c : o3_prm.classes() ) {
          auto implements = Set<std::string>();
          for ( const auto& i : c->interfaces() ) {
            implements.insert( i.label() );
          }
          factory.startClass(
              c->name().label(), c->super().label(), &implements );
          // Parameters
          for ( const auto& p : c->parameters() ) {
            switch ( p.type() ) {
              case O3Parameter::Type::INT: {
                factory.addParameter(
                    "int", p.name().label(), p.value().value() );
                break;
              }
              case O3Parameter::Type::FLOAT: {
                factory.addParameter(
                    "real", p.name().label(), p.value().value() );
                break;
              }
              default: { GUM_ERROR( FatalError, "unknown O3Paramater type" ); }
            }
          }
          // References
          for ( auto& ref : c->referenceSlots() ) {
            if ( check_ref( prm, ref ) ) {
              factory.addReferenceSlot(
                  ref.type().label(), ref.name().label(), ref.isArray() );
            }
          }
          // Attributes
          for ( auto& attr : c->attributes() ) {
            if ( check_attribute( prm, *c, *attr, output ) ) {
              factory.startAttribute( attr->type().label(),
                                      attr->name().label() );
              for ( const auto& parent : attr->parents() ) {
                factory.addParent( parent.label() );
              }
              auto raw = dynamic_cast<const O3RawCPT*>( attr.get() );
              if ( raw ) {
                auto values = std::vector<std::string>();
                for ( const auto& val : raw->values() ) {
                  values.push_back( val.formula().formula() );
                }
                factory.setRawCPFByColumns( values );
              }
              auto rule_cpt = dynamic_cast<const O3RuleCPT*>( attr.get() );
              if ( rule_cpt ) {
                for ( const auto& rule : rule_cpt->rules() ) {
                  auto labels = std::vector<std::string>();
                  auto values = std::vector<std::string>();
                  for ( const auto& lbl : rule.first ) {
                    labels.push_back( lbl.label() );
                  }
                  for ( const auto& form : rule.second ) {
                    values.push_back( form.formula().formula() );
                  }
                  factory.setCPFByRule( labels, values );
                }
              }
              factory.endAttribute();
            }
          }
          // Aggregates
          for ( auto& agg : c->aggregates() ) {
            if ( check_aggregate( prm, *c, agg, output ) ) {
              auto parents = std::vector<std::string>();
              for ( auto& prnt : agg.parents() ) {
                parents.push_back( prnt.label() );
              }
              auto params = std::vector<std::string>();
              for ( auto& param : agg.parameters() ) {
                params.push_back( param.label() );
              }
              factory.addAggregator( agg.name().label(),
                                     agg.aggregateType().label(),
                                     parents,
                                     params,
                                     agg.variableType().label() );
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
