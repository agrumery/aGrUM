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
 * @brief Implementation for the O3ClassFactory class.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 * @author Lionel TORTI
 */

#include <agrum/PRM/newo3prm/O3ClassFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>::O3ClassFactory() {
        GUM_CONSTRUCTOR( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>::O3ClassFactory(
          const O3ClassFactory<GUM_SCALAR>& src ) {
        GUM_CONS_CPY( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>::O3ClassFactory(
          O3ClassFactory<GUM_SCALAR>&& src ) {
        GUM_CONS_MOV( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>::~O3ClassFactory() {
        GUM_DESTRUCTOR( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>& O3ClassFactory<GUM_SCALAR>::
      operator=( const O3ClassFactory<GUM_SCALAR>& src ) {
        return *this;
      }

      template <typename GUM_SCALAR>
      O3ClassFactory<GUM_SCALAR>& O3ClassFactory<GUM_SCALAR>::
      operator=( O3ClassFactory<GUM_SCALAR>&& src ) {
        return *this;
      }

      template <typename GUM_SCALAR>
      void O3ClassFactory<GUM_SCALAR>::__addParameters(
          PRMFactory<GUM_SCALAR>& factory, O3Class& c, std::ostream& output ) {
        for ( const auto& p : c.parameters() ) {
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
      }

      template <typename GUM_SCALAR>
      bool O3ClassFactory<GUM_SCALAR>::__checkReferenceSlot(
          const PRM<GUM_SCALAR>& prm, O3Class& c, O3ReferenceSlot& ref, std::ostream& output ) {
        if ( not( prm.isInterface( ref.type().label() ) or
                  prm.isClass( ref.type().label() ) ) ) {
          const auto& pos = ref.name().position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Reference Slot error : "
                 << " Reference Slot type " << ref.type().label()
                 << " not found" << std::endl;
          return false;
        }
        const auto& real_c = prm.getClass( c.name().label() );
        if ( real_c.exists( ref.name().label() ) ) {
          const auto& pos = ref.name().position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Reference Slot error : "
                 << " Reference Slot name " << ref.name().label()
                 << " exists already" << std::endl;
          return false;
        }
        return true;
      }

      template <typename GUM_SCALAR>
      void O3ClassFactory<GUM_SCALAR>::__addReferenceSlots(
          PRMFactory<GUM_SCALAR>& factory, O3Class& c, std::ostream& output ) {
        // References
        for ( auto& ref : c.referenceSlots() ) {
          if ( __checkReferenceSlot( *( factory.prm() ), c, ref, output ) ) {
            factory.addReferenceSlot(
                ref.type().label(), ref.name().label(), ref.isArray() );
          }
        }
      }

      template <typename GUM_SCALAR>
      bool O3ClassFactory<GUM_SCALAR>::__checkLocalParent(
          const Class<GUM_SCALAR>& c,
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
        if ( not( gum::prm::ClassElement<GUM_SCALAR>::isAttribute( elt ) or
                  gum::prm::ClassElement<GUM_SCALAR>::isSlotChain( elt ) ) ) {
          const auto& pos = prnt.position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Attribute error : "
                 << "Illegal parent " << prnt.label() << std::endl;
          return false;
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3ClassFactory<GUM_SCALAR>::__checkRemoteParent(
          const ClassElementContainer<GUM_SCALAR>& c,
          const O3Label& prnt,
          std::ostream& output ) {
        return __resolveSlotChain( c, prnt, output ) != nullptr;
      }

      template <typename GUM_SCALAR>
      bool
      O3ClassFactory<GUM_SCALAR>::__checkParent( const Class<GUM_SCALAR>& c,
                                                 const O3Label& prnt,
                                                 std::ostream& output ) {
        if ( prnt.label().find( '.' ) == std::string::npos ) {
          return __checkLocalParent( c, prnt, output );
        } else {
          return __checkRemoteParent( c, prnt, output );
        }
      }

      template <typename GUM_SCALAR>
      const ClassElement<GUM_SCALAR>*
      O3ClassFactory<GUM_SCALAR>::__resolveSlotChain(
          const ClassElementContainer<GUM_SCALAR>& c,
          const O3Label& chain,
          std::ostream& output ) {
        auto s = chain.label();
        auto idx = (std::size_t)0;
        auto current = &c;
        for ( idx = s.find( '.', idx ); idx != std::string::npos;
              idx = s.find( '.', idx ) ) {
          auto value = s.substr( 0, idx );
          if ( not __checkSlotChainLink( *current, chain, value, output ) ) {
            return nullptr;
          }
          auto ref = dynamic_cast<const ReferenceSlot<GUM_SCALAR>*>(
              &( current->get( value ) ) );
          if ( ref ) {
            current = &( ref->slotType() );
            s = s.substr( idx + 1 );
          }
        }
        if ( not __checkSlotChainLink( *current, chain, s, output ) ) {
          return nullptr;
        }
        return &( current->get( s ) );
      }

      template <typename GUM_SCALAR>
      bool O3ClassFactory<GUM_SCALAR>::__checkSlotChainLink(
          const ClassElementContainer<GUM_SCALAR>& c,
          const O3Label& chain,
          const std::string& s,
          std::ostream& output ) {
        if ( not c.exists( s ) ) {
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

      template <typename GUM_SCALAR>
      bool
      O3ClassFactory<GUM_SCALAR>::__checkRawCPT( const PRM<GUM_SCALAR>& prm,
                                                 const Class<GUM_SCALAR>& c,
                                                 O3RawCPT& attr,
                                                 std::ostream& output ) {
        const auto& type = prm.type( attr.type().label() );

        auto domainSize = type->domainSize();
        for ( auto& prnt : attr.parents() ) {
          try {
            domainSize *= c.get( prnt.label() ).type()->domainSize();
          } catch ( NotFound& ) {
            // If we are here, all parents have been check so __resolveSlotChain
            // will not raise an error and not return a null_ptr
            domainSize *=
                __resolveSlotChain( c, prnt, output )->type()->domainSize();
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

      template <typename GUM_SCALAR>
      bool O3ClassFactory<GUM_SCALAR>::__checkRuleCPT(
          const Class<GUM_SCALAR>& c, O3RuleCPT& attr, std::ostream& output ) {
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

      template <typename GUM_SCALAR>
      bool
      O3ClassFactory<GUM_SCALAR>::__checkAttribute( PRM<GUM_SCALAR>& prm,
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
          if ( not __checkParent( c, prnt, output ) ) {
            return false;
          }
        }
        // Check that CPT sums to 1
        auto raw = dynamic_cast<O3RawCPT*>( &attr );
        if ( raw ) {
          return __checkRawCPT( prm, c, *raw, output );
        }
        auto rule = dynamic_cast<O3RuleCPT*>( &attr );
        if ( rule ) {
          return __checkRuleCPT( c, *rule, output );
        }
        return true;
      }

      template <typename GUM_SCALAR>
      void O3ClassFactory<GUM_SCALAR>::__addAttributes(
          PRMFactory<GUM_SCALAR>& factory, O3Class& c, std::ostream& output ) {
        // Attributes
        for ( auto& attr : c.attributes() ) {
          if ( __checkAttribute( *(factory.prm()), c, *attr, output ) ) {
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
      }

      template <typename GUM_SCALAR>
      bool O3ClassFactory<GUM_SCALAR>::__checkParametersNumber(
          O3Aggregate& agg, size_t n, std::ostream& output ) {
        if ( agg.parameters().size() != n ) {
          const auto& pos = agg.name().position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Aggregate error : "
                 << "Expected " << n << " parameters "
                 << ", found " << agg.parameters().size() << std::endl;
          return false;
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool O3ClassFactory<GUM_SCALAR>::__checkParameterValue(
          O3Aggregate& agg,
          const gum::prm::Type<GUM_SCALAR>& t,
          std::ostream& output ) {
        const auto& param = agg.parameters().front();
        bool found = false;
        for ( Size idx = 0; idx < t.variable().domainSize(); ++idx ) {
          if ( t.variable().label( idx ) == param.label() ) {
            found = true;
            break;
          }
        }
        if ( not found ) {
          const auto& pos = param.position();
          output << pos.file() << "|" << pos.line() << " col " << pos.column()
                 << "|"
                 << " Aggregate error : "
                 << "Parameter " << param.label() << " in aggregate "
                 << agg.name().label() << " does not match any expected values"
                 << std::endl;
          return false;
        }
        return true;
      }

      template <typename GUM_SCALAR>
      bool
      O3ClassFactory<GUM_SCALAR>::__checkAggregate( const PRM<GUM_SCALAR>& prm,
                                                    O3Class& o3class,
                                                    O3Aggregate& agg,
                                                    std::ostream& output ) {
        const auto& c = prm.getClass( o3class.name().label() );
        auto t = (const gum::prm::Type<GUM_SCALAR>*)nullptr;
        // Checking parents
        for ( const auto& prnt : agg.parents() ) {
          auto elt = __resolveSlotChain( c, prnt, output );
          if ( elt == nullptr ) {
            const auto& pos = prnt.position();
            output << pos.file() << "|" << pos.line() << " col " << pos.column()
                   << "|"
                   << " Aggregate error : "
                   << "Parent " << prnt.label() << " not found" << std::endl;
            return false;
          } else {
            if ( t == nullptr or ( *t ) == elt->type() ) {
              t = &( elt->type() );
            } else {
              // Wront type in chain
              const auto& pos = prnt.position();
              output << pos.file() << "|" << pos.line() << " col "
                     << pos.column() << "|"
                     << " Aggregate error : "
                     << "Expected type " << t->name() << " for parent "
                     << prnt.label() << ", found " << elt->type().name()
                     << std::endl;
              return false;
            }
          }
        }
        // Checking parameters numbers
        bool ok = false;
        switch ( gum::prm::Aggregate<GUM_SCALAR>::str2enum(
            agg.aggregateType().label() ) ) {
          case Aggregate<GUM_SCALAR>::AggregateType::MIN:
          case Aggregate<GUM_SCALAR>::AggregateType::MAX:
          case Aggregate<GUM_SCALAR>::AggregateType::AMPLITUDE:
          case Aggregate<GUM_SCALAR>::AggregateType::MEDIAN:
          case Aggregate<GUM_SCALAR>::AggregateType::OR:
          case Aggregate<GUM_SCALAR>::AggregateType::AND: {
            ok = __checkParametersNumber( agg, 0, output );
            break;
          }
          case Aggregate<GUM_SCALAR>::AggregateType::FORALL:
          case Aggregate<GUM_SCALAR>::AggregateType::EXISTS:
          case Aggregate<GUM_SCALAR>::AggregateType::COUNT: {
            ok = __checkParametersNumber( agg, 1, output );
            break;
          }
          default: { GUM_ERROR( FatalError, "unknown aggregate type" ); }
        }
        if ( not ok ) {
          return false;
        }
        // Checking parameters type
        switch ( gum::prm::Aggregate<GUM_SCALAR>::str2enum(
            agg.aggregateType().label() ) ) {
          case Aggregate<GUM_SCALAR>::AggregateType::FORALL:
          case Aggregate<GUM_SCALAR>::AggregateType::EXISTS:
          case Aggregate<GUM_SCALAR>::AggregateType::COUNT: {
            ok = __checkParameterValue( agg, *t, output );
            break;
          }
          default: {  // Nothing to do
          }
        }
        return ok;
      }

      template <typename GUM_SCALAR>
      void O3ClassFactory<GUM_SCALAR>::__addAggregates(
          PRMFactory<GUM_SCALAR>& factory, O3Class& c, std::ostream& output ) {
        // Aggregates
        for ( auto& agg : c.aggregates() ) {
          if ( __checkAggregate( *(factory.prm()), c, agg, output ) ) {
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
      }

      template <typename GUM_SCALAR>
      void O3ClassFactory<GUM_SCALAR>::build( PRM<GUM_SCALAR>& prm,
                                              const O3PRM& o3_prm,
                                              std::ostream& output ) {
        PRMFactory<GUM_SCALAR> factory( &prm );
        for ( const auto& c : o3_prm.classes() ) {
          // Interfaces
          auto implements = Set<std::string>();
          for ( const auto& i : c->interfaces() ) {
            implements.insert( i.label() );
          }
          factory.startClass(
              c->name().label(), c->super().label(), &implements );

          __addParameters( factory, *c, output );
          __addReferenceSlots( factory, *c, output );
          __addAttributes( factory, *c, output );
          __addAggregates( factory, *c, output );
          factory.endClass();
        }
      }

    }  // o3prm
  }    // prm
}  // gum

