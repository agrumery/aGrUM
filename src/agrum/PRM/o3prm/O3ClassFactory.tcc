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

#include <agrum/PRM/o3prm/O3ClassFactory.h>

namespace gum {
  namespace prm {
    namespace o3prm {

      template <typename GUM_SCALAR>
      INLINE
      O3ClassFactory<GUM_SCALAR>::O3ClassFactory( PRM<GUM_SCALAR>&          prm,
                                                  O3PRM&                    o3_prm,
                                                  O3NameSolver<GUM_SCALAR>& solver,
                                                  ErrorsContainer& errors )
          : __prm( &prm )
          , __o3_prm( &o3_prm )
          , __solver( &solver )
          , __errors( &errors ) {
        GUM_CONSTRUCTOR( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3ClassFactory<GUM_SCALAR>::O3ClassFactory(
          const O3ClassFactory<GUM_SCALAR>& src )
          : __prm( src.__prm )
          , __o3_prm( src.__o3_prm )
          , __solver( src.__solver )
          , __errors( src.__errors )
          , __nameMap( src.__nameMap )
          , __classMap( src.__classMap )
          , __nodeMap( src.__nodeMap )
          , __dag( src.__dag )
          , __o3Classes( src.__o3Classes ) {
        GUM_CONS_CPY( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3ClassFactory<GUM_SCALAR>::O3ClassFactory(
          O3ClassFactory<GUM_SCALAR>&& src )
          : __prm( std::move( src.__prm ) )
          , __o3_prm( std::move( src.__o3_prm ) )
          , __solver( std::move( src.__solver ) )
          , __errors( std::move( src.__errors ) )
          , __nameMap( std::move( src.__nameMap ) )
          , __classMap( std::move( src.__classMap ) )
          , __nodeMap( std::move( src.__nodeMap ) )
          , __dag( std::move( src.__dag ) )
          , __o3Classes( std::move( src.__o3Classes ) ) {
        GUM_CONS_MOV( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3ClassFactory<GUM_SCALAR>::~O3ClassFactory() {
        GUM_DESTRUCTOR( O3ClassFactory );
      }

      template <typename GUM_SCALAR>
      INLINE O3ClassFactory<GUM_SCALAR>& O3ClassFactory<GUM_SCALAR>::
      operator=( const O3ClassFactory<GUM_SCALAR>& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = src.__prm;
        __o3_prm = src.__o3_prm;
        __solver = src.__solver;
        __errors = src.__errors;
        __nameMap = src.__nameMap;
        __classMap = src.__classMap;
        __nodeMap = src.__nodeMap;
        __dag = src.__dag;
        __o3Classes = src.__o3Classes;
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE O3ClassFactory<GUM_SCALAR>& O3ClassFactory<GUM_SCALAR>::
      operator=( O3ClassFactory<GUM_SCALAR>&& src ) {
        if ( this == &src ) {
          return *this;
        }
        __prm = std::move( src.__prm );
        __o3_prm = std::move( src.__o3_prm );
        __solver = std::move( src.__solver );
        __errors = std::move( src.__errors );
        __nameMap = std::move( src.__nameMap );
        __classMap = std::move( src.__classMap );
        __nodeMap = std::move( src.__nodeMap );
        __dag = std::move( src.__dag );
        __o3Classes = std::move( src.__o3Classes );
        return *this;
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::buildClasses() {

        PRMFactory<GUM_SCALAR> factory( __prm );

        // Class with a super class must be declared after
        if ( __checkO3Classes() ) {

          __setO3ClassCreationOrder();

          for ( auto c : __o3Classes ) {

            // Soving interfaces
            auto implements = Set<std::string>();
            for ( auto& i : c->interfaces() ) {

              if ( __solver->resolveInterface( i ) ) {
                implements.insert( i.label() );
              }
            }

            // Adding the class
            if ( __solver->resolveClass( c->superLabel() ) ) {
              factory.startClass(
                  c->name().label(), c->superLabel().label(), &implements, true );
              factory.endClass( false );
            }
          }
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::__setO3ClassCreationOrder() {

        auto topo_order = __dag.topologicalOrder();

        for ( auto id = topo_order.rbegin(); id != topo_order.rend(); --id ) {
          __o3Classes.push_back( __nodeMap[*id] );
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkO3Classes() {
        return __checkAndAddNodesToDag() && __checkAndAddArcsToDag();
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkAndAddNodesToDag() {

        for ( auto& c : __o3_prm->classes() ) {

          auto id = __dag.addNode();

          try {

            __nameMap.insert( c->name().label(), id );
            __classMap.insert( c->name().label(), c.get() );
            __nodeMap.insert( id, c.get() );

          } catch ( DuplicateElement& ) {
            O3PRM_CLASS_DUPLICATE( c->name(), *__errors );
            return false;
          }
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkAndAddArcsToDag() {
        for ( auto& c : __o3_prm->classes() ) {

          if ( c->superLabel().label() != "" ) {

            if ( !__solver->resolveClass( c->superLabel() ) ) {
              return false;
            }

            auto head = __nameMap[c->superLabel().label()];
            auto tail = __nameMap[c->name().label()];

            try {

              __dag.addArc( tail, head );
            } catch ( InvalidDirectedCycle& ) {
              // Cyclic inheritance
              O3PRM_CLASS_CYLIC_INHERITANCE(
                  c->name(), c->superLabel(), *__errors );
              return false;
            }
          }
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::buildImplementations() {

        for ( auto& c : __o3_prm->classes() ) {
          if ( __checkImplementation( *c ) ) {
            __prm->getClass( c->name().label() ).initializeInheritance();
          }
        }
      }

      using AttrMap = HashTable<std::string, O3Attribute*>;
      using RefMap = HashTable<std::string, O3ReferenceSlot*>;
      using AggMap = HashTable<std::string, O3Aggregate*>;

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkImplementation( O3Class& c ) {

        // Saving attributes names for fast lookup
        auto attr_map = AttrMap();
        for ( auto& a : c.attributes() ) {
          attr_map.insert( a->name().label(), a.get() );
        }

        // Saving aggregates names for fast lookup
        auto agg_map = AggMap();
        for ( auto& agg : c.aggregates() ) {
          agg_map.insert( agg.name().label(), &agg );
        }

        auto ref_map = RefMap();
        for ( auto& ref : c.referenceSlots() ) {
          ref_map.insert( ref.name().label(), &ref );
        }

        // Cheking interface implementation
        for ( auto& i : c.interfaces() ) {

          if ( __solver->resolveInterface( i ) ) {

            if ( !__checkImplementation( c, i, attr_map, agg_map, ref_map ) ) {
              return false;
            }
          }
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool
      O3ClassFactory<GUM_SCALAR>::__checkImplementation( O3Class& c,
                                                         O3Label& i,
                                                         AttrMap& attr_map,
                                                         AggMap&  agg_map,
                                                         RefMap&  ref_map ) {
        const auto& real_i = __prm->getInterface( i.label() );

        auto counter = (Size)0;
        for ( const auto& a : real_i.attributes() ) {

          if ( attr_map.exists( a->name() ) ) {
            ++counter;

            if ( !__checkImplementation( attr_map[a->name()]->type(),
                                         a->type() ) ) {
              O3PRM_CLASS_ATTR_IMPLEMENTATION(
                  c.name(), i, attr_map[a->name()]->name(), *__errors );
              return false;
            }
          }

          if ( agg_map.exists( a->name() ) ) {
            ++counter;

            if ( !__checkImplementation( agg_map[a->name()]->variableType(),
                                         a->type() ) ) {
              O3PRM_CLASS_AGG_IMPLEMENTATION(
                  c.name(), i, agg_map[a->name()]->name(), *__errors );
              return false;
            }
          }
        }

        if ( counter != real_i.attributes().size() ) {
          O3PRM_CLASS_MISSING_ATTRIBUTES( c.name(), i, *__errors );
          return false;
        }

        counter = 0;
        for ( const auto& r : real_i.referenceSlots() ) {

          if ( ref_map.exists( r->name() ) ) {
            ++counter;

            if ( !__checkImplementation( ref_map[r->name()]->type(),
                                         r->slotType() ) ) {
              O3PRM_CLASS_REF_IMPLEMENTATION(
                  c.name(), i, ref_map[r->name()]->name(), *__errors );
              return false;
            }
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkImplementation(
          O3Label& o3_type, const PRMType<GUM_SCALAR>& type ) {
        if ( !__solver->resolveType( o3_type ) ) {
          return false;
        }

        return __prm->type( o3_type.label() ).isSubTypeOf( type );
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkImplementation(
          O3Label& o3_type, const PRMClassElementContainer<GUM_SCALAR>& type ) {

        if ( !__solver->resolveSlotType( o3_type ) ) {
          return false;
        }

        if ( __prm->isInterface( o3_type.label() ) ) {
          return __prm->getInterface( o3_type.label() ).isSubTypeOf( type );
        } else {
          return __prm->getClass( o3_type.label() ).isSubTypeOf( type );
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::buildParameters() {

        PRMFactory<GUM_SCALAR> factory( __prm );
        // Class with a super class must be declared after
        for ( auto c : __o3Classes ) {

          __prm->getClass( c->name().label() ).inheritParameters();

          factory.continueClass( c->name().label() );

          __addParameters( factory, *c );

          factory.endClass( false );
        }
      }

      template <typename GUM_SCALAR>
      INLINE void
      O3ClassFactory<GUM_SCALAR>::__addParameters( PRMFactory<GUM_SCALAR>& factory,
                                                   O3Class&                c ) {

        for ( auto& p : c.parameters() ) {

          switch ( p.type() ) {

            case O3Parameter::PRMType::INT: {
              factory.addParameter( "int", p.name().label(), p.value().value() );
              break;
            }

            case O3Parameter::PRMType::FLOAT: {
              factory.addParameter( "real", p.name().label(), p.value().value() );
              break;
            }

            default: { GUM_ERROR( FatalError, "unknown O3Parameter type" ); }
          }
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::buildReferenceSlots() {

        // Class with a super class must be declared after
        for ( auto c : __o3Classes ) {
          __prm->getClass( c->name().label() ).inheritReferenceSlots();
          __addReferenceSlots( *c );
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::__addReferenceSlots( O3Class& c ) {

        PRMFactory<GUM_SCALAR> factory( __prm );

        factory.continueClass( c.name().label() );

        // References
        for ( auto& ref : c.referenceSlots() ) {

          if ( __checkReferenceSlot( c, ref ) ) {

            factory.addReferenceSlot(
                ref.type().label(), ref.name().label(), ref.isArray() );
          }
        }

        factory.endClass( false );
      }

      template <typename GUM_SCALAR>
      INLINE bool
      O3ClassFactory<GUM_SCALAR>::__checkReferenceSlot( O3Class&         c,
                                                        O3ReferenceSlot& ref ) {

        if ( !__solver->resolveSlotType( ref.type() ) ) {
          return false;
        }

        const auto& real_c = __prm->getClass( c.name().label() );

        // Check for dupplicates
        if ( real_c.exists( ref.name().label() ) ) {

          const auto& elt = real_c.get( ref.name().label() );

          if ( PRMClassElement<GUM_SCALAR>::isReferenceSlot( elt ) ) {

            auto slot_type = (PRMClassElementContainer<GUM_SCALAR>*)nullptr;

            if ( __prm->isInterface( ref.type().label() ) ) {

              slot_type = &( __prm->getInterface( ref.type().label() ) );

            } else {

              slot_type = &( __prm->getClass( ref.type().label() ) );
            }

            auto real_ref =
                static_cast<const PRMReferenceSlot<GUM_SCALAR>*>( &elt );

            if ( slot_type->name() == real_ref->slotType().name() ) {

              O3PRM_CLASS_DUPLICATE_REFERENCE( ref.name(), *__errors );
              return false;

            } else if ( !slot_type->isSubTypeOf( real_ref->slotType() ) ) {

              O3PRM_CLASS_ILLEGAL_OVERLOAD( ref.name(), c.name(), *__errors );
              return false;
            }

          } else {

            O3PRM_CLASS_DUPLICATE_REFERENCE( ref.name(), *__errors );
            return false;
          }
        }

        // If class we need to check for illegal references
        if ( __prm->isClass( ref.type().label() ) ) {

          const auto& ref_type = __prm->getClass( ref.type().label() );

          // No recursive reference
          if ( ( &ref_type ) == ( &real_c ) ) {

            O3PRM_CLASS_SELF_REFERENCE( c.name(), ref.name(), *__errors );
            return false;
          }

          // No reference to subclasses
          if ( ref_type.isSubTypeOf( real_c ) ) {

            O3PRM_CLASS_ILLEGAL_SUB_REFERENCE( c.name(), ref.type(), *__errors );
            return false;
          }
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::declareAttributes() {
        // Class with a super class must be declared after
        for ( auto c : __o3Classes ) {
          __prm->getClass( c->name().label() ).inheritAttributes();
          __declareAttribute( *c );
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::declareAggregates() {
        // Class with a super class must be declared after
        for ( auto c : __o3Classes ) {
          __prm->getClass( c->name().label() ).inheritAggregates();
          __declareAggregates( *c );
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::__declareAttribute( O3Class& c ) {

        PRMFactory<GUM_SCALAR> factory( __prm );
        factory.continueClass( c.name().label() );

        for ( auto& attr : c.attributes() ) {

          if ( __checkAttributeForDeclaration( c, *attr ) ) {

            factory.startAttribute( attr->type().label(), attr->name().label() );
            factory.endAttribute();
          }
        }

        factory.endClass( false );
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkAttributeForDeclaration(
          O3Class& c, O3Attribute& attr ) {

        // Check type
        if ( !__solver->resolveType( attr.type() ) ) {
          return false;
        }

        // Checking type legality if overload
        if ( c.superLabel().label() != "" ) {
          const auto& super = __prm->getClass( c.superLabel().label() );

          if ( !super.exists( attr.name().label() ) ) {
            return true;
          }

          const auto& super_type = super.get( attr.name().label() ).type();
          const auto& type = __prm->type( attr.type().label() );

          if ( !type.isSubTypeOf( super_type ) ) {

            O3PRM_CLASS_ILLEGAL_OVERLOAD( attr.name(), c.superLabel(), *__errors );
            return false;
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::completeAttributes() {

        PRMFactory<GUM_SCALAR> factory( __prm );

        // Class with a super class must be declared in order
        for ( auto c : __o3Classes ) {

          __prm->getClass( c->name().label() ).inheritSlotChains();
          factory.continueClass( c->name().label() );

          __completeAttribute( factory, *c );

          if ( c->superLabel().label() != "" ) {

            auto& super = __prm->getClass( c->superLabel().label() );
            auto  to_complete = Set<std::string>();

            for ( auto a : super.attributes() ) {
              to_complete.insert( a->safeName() );
            }

            for ( auto a : super.aggregates() ) {
              to_complete.insert( a->safeName() );
            }

            for ( auto& a : c->attributes() ) {
              to_complete.erase( __prm->getClass( c->name().label() )
                                     .get( a->name().label() )
                                     .safeName() );
            }

            for ( auto& a : c->aggregates() ) {
              to_complete.erase( __prm->getClass( c->name().label() )
                                     .get( a.name().label() )
                                     .safeName() );
            }

            for ( auto a : to_complete ) {
              __prm->getClass( c->name().label() ).completeInheritance( a );
            }
          }

          factory.endClass( true );
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::completeAggregates() {

        PRMFactory<GUM_SCALAR> factory( __prm );

        // Class with a super class must be declared in order
        for ( auto c : __o3Classes ) {

          factory.continueClass( c->name().label() );

          __completeAggregates( factory, *c );

          factory.endClass( false );
        }
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::__completeAggregates(
          PRMFactory<GUM_SCALAR>& factory, O3Class& c ) {

        // Attributes
        for ( auto& agg : c.aggregates() ) {

          if ( __checkAggregateForCompletion( c, agg ) ) {

            factory.continueAggregator( agg.name().label() );

            for ( const auto& parent : agg.parents() ) {
              factory.addParent( parent.label() );
            }

            factory.endAggregator();
          }
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkAggregateForCompletion(
          O3Class& c, O3Aggregate& agg ) {
        // Checking parents
        auto t = __checkAggParents( c, agg );
        if ( t == nullptr ) {
          return false;
        }

        // Checking parameters numbers
        if ( !__checkAggParameters( c, agg, t ) ) {
          return false;
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::__completeAttribute(
          PRMFactory<GUM_SCALAR>& factory, O3Class& c ) {

        // Attributes
        for ( auto& attr : c.attributes() ) {

          if ( __checkAttributeForCompletion( c, *attr ) ) {

            factory.continueAttribute( attr->name().label() );

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
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkAttributeForCompletion(
          const O3Class& o3_c, O3Attribute& attr ) {

        // Check for parents existence
        const auto& c = __prm->getClass( o3_c.name().label() );
        for ( auto& prnt : attr.parents() ) {
          if ( !__checkParent( c, prnt ) ) {
            return false;
          }
        }

        // Check that CPT sums to 1
        auto raw = dynamic_cast<O3RawCPT*>( &attr );
        if ( raw ) {
          return __checkRawCPT( c, *raw );
        }

        auto rule = dynamic_cast<O3RuleCPT*>( &attr );
        if ( rule ) {
          return __checkRuleCPT( c, *rule );
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool
      O3ClassFactory<GUM_SCALAR>::__checkParent( const PRMClass<GUM_SCALAR>& c,
                                                 const O3Label& prnt ) {
        if ( prnt.label().find( '.' ) == std::string::npos ) {

          return __checkLocalParent( c, prnt );

        } else {

          return __checkRemoteParent( c, prnt );
        }
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkLocalParent(
          const PRMClass<GUM_SCALAR>& c, const O3Label& prnt ) {

        if ( !c.exists( prnt.label() ) ) {
          O3PRM_CLASS_PARENT_NOT_FOUND( prnt, *__errors );
          return false;
        }

        const auto& elt = c.get( prnt.label() );
        if ( !( gum::prm::PRMClassElement<GUM_SCALAR>::isAttribute( elt ) ||
                gum::prm::PRMClassElement<GUM_SCALAR>::isSlotChain( elt ) ||
                gum::prm::PRMClassElement<GUM_SCALAR>::isAggregate( elt ) ) ) {
          O3PRM_CLASS_ILLEGAL_PARENT( prnt, *__errors );
          return false;
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkRemoteParent(
          const PRMClassElementContainer<GUM_SCALAR>& c, const O3Label& prnt ) {

        if ( __resolveSlotChain( c, prnt ) == nullptr ) {
          return false;
        }
        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool
      O3ClassFactory<GUM_SCALAR>::__checkRuleCPT( const PRMClass<GUM_SCALAR>& c,
                                                  O3RuleCPT& attr ) {

        const auto& scope = c.scope();

        for ( auto& rule : attr.rules() ) {

          // Check labels for all parents
          if ( rule.first.size() != attr.parents().size() ) {
            O3PRM_CLASS_ILLEGAL_RULE_SIZE(
                rule, rule.first.size(), attr.parents().size(), *__errors );
            return false;
          }

          // Add parameters to formulas
          for ( auto& f : rule.second ) {
            f.formula().variables().clear();
            for ( const auto& values : scope ) {
              f.formula().variables().insert( values.first,
                                              values.second->value() );
            }
          }

          // Check that formulas are valid and sums to 1
          GUM_SCALAR sum = 0.0;
          for ( const auto& f : rule.second ) {
            try {
              auto value = GUM_SCALAR( f.formula().result() );
              sum += value;

              if ( value < 0.0 || 1.0 < value ) {
                O3PRM_CLASS_ILLEGAL_CPT_VALUE(
                    c.name(), attr.name(), f, *__errors );
                return false;
              }

            } catch ( OperationNotAllowed& ) {

              O3PRM_CLASS_ILLEGAL_CPT_VALUE( c.name(), attr.name(), f, *__errors );
              return false;
            }
          }

          // Check that CPT sums to 1
          if ( std::abs( sum - 1.0 ) > 1e-3 ) {
            O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1(
                c.name(), attr.name(), float( sum ), *__errors );
            return false;
          } else if ( std::abs( sum - 1.0f ) > 1e-6 ) {
            O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1_WARNING(
                c.name(), attr.name(), float( sum ), *__errors );
          }
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool
      O3ClassFactory<GUM_SCALAR>::__checkRawCPT( const PRMClass<GUM_SCALAR>& c,
                                                 O3RawCPT& attr ) {

        const auto& type = __prm->type( attr.type().label() );

        auto domainSize = type->domainSize();
        for ( auto& prnt : attr.parents() ) {
          try {
            domainSize *= c.get( prnt.label() ).type()->domainSize();
          } catch ( NotFound& ) {
            // If we are here, all parents have been check so __resolveSlotChain
            // will not raise an error and not return a nullptr
            domainSize *= __resolveSlotChain( c, prnt )->type()->domainSize();
          }
        }

        // Check for CPT size
        if ( domainSize != attr.values().size() ) {
          O3PRM_CLASS_ILLEGAL_CPT_SIZE( c.name(),
                                        attr.name(),
                                        Size( attr.values().size() ),
                                        domainSize,
                                        *__errors );
          return false;
        }

        // Add parameters to formulas
        const auto& scope = c.scope();
        for ( auto& f : attr.values() ) {

          f.formula().variables().clear();

          for ( const auto& values : scope ) {

            f.formula().variables().insert( values.first, values.second->value() );
          }
        }

        // Check that CPT sums to 1
        Size parent_size = domainSize / type->domainSize();
        auto values = std::vector<GUM_SCALAR>( parent_size, 0.0f );

        for ( std::size_t i = 0; i < attr.values().size(); ++i ) {
          try {

            auto idx = i % parent_size;
            auto val = (GUM_SCALAR)attr.values()[i].formula().result();
            values[idx] += val;

            if ( val < 0.0 || 1.0 < val ) {
              O3PRM_CLASS_ILLEGAL_CPT_VALUE(
                  c.name(), attr.name(), attr.values()[i], *__errors );
              return false;
            }
          } catch ( Exception& ) {
            O3PRM_CLASS_ILLEGAL_CPT_VALUE(
                c.name(), attr.name(), attr.values()[i], *__errors );
            return false;
          }
        }

        for ( auto f : values ) {
          if ( std::abs( f - GUM_SCALAR( 1.0 ) ) > 1.0e-3 ) {
            O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1(
                c.name(), attr.name(), float( f ), *__errors );
            return false;
          } else if ( std::abs( f - GUM_SCALAR( 1.0 ) ) > 1.0e-6 ) {
            O3PRM_CLASS_CPT_DOES_NOT_SUM_TO_1_WARNING(
                c.name(), attr.name(), float( f ), *__errors );
          }
        }
        return true;
      }

      template <typename GUM_SCALAR>
      INLINE const PRMClassElement<GUM_SCALAR>*
                   O3ClassFactory<GUM_SCALAR>::__resolveSlotChain(
          const PRMClassElementContainer<GUM_SCALAR>& c, const O3Label& chain ) {

        auto link_regex = std::regex( R"d((\([\w\.]*\))?\w+)d" );
        auto s = chain.label();
        auto current = &c;
        auto match = std::smatch();

        while ( std::regex_search( s, match, link_regex ) ) {
          auto link = match[0];

          if ( !__checkSlotChainLink( *current, chain, link ) ) {
            return nullptr;
          }

          auto elt = &( current->get( link ) );

          if ( match[0].str().size() == s.size() ) {
            // last link, should be an attribute or aggregate
            return elt;

          } else {
            // should be a reference slot

            auto ref = dynamic_cast<const PRMReferenceSlot<GUM_SCALAR>*>( elt );
            if ( ref ) {
              current = &( ref->slotType() );
              // Links are seperated by dots, so we need to skip the next one
              s = s.substr( match[0].str().size() + 1 );
            } else {
              return nullptr;  // failsafe to prevent infinite loop
            }
          }
        }

        // Encountered only reference slots
        return nullptr;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkSlotChainLink(
          const PRMClassElementContainer<GUM_SCALAR>& c,
          const O3Label&                              chain,
          const std::string&                          s ) {

        if ( !c.exists( s ) ) {

          O3PRM_CLASS_LINK_NOT_FOUND( chain, s, *__errors );
          return false;
        }
        return true;
      }

      template <typename GUM_SCALAR>
      INLINE void O3ClassFactory<GUM_SCALAR>::__declareAggregates( O3Class& c ) {

        PRMFactory<GUM_SCALAR> factory( __prm );
        factory.continueClass( c.name().label() );

        for ( auto& agg : c.aggregates() ) {

          if ( __checkAggregateForDeclaration( c, agg ) ) {
            auto params = std::vector<std::string>();
            for ( auto& p : agg.parameters() ) {
              params.push_back( p.label() );
            }

            factory.startAggregator( agg.name().label(),
                                     agg.aggregateType().label(),
                                     agg.variableType().label(),
                                     params );
            factory.endAggregator();
          }
        }

        factory.endClass( false );
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkAggregateForDeclaration(
          O3Class& o3class, O3Aggregate& agg ) {
        if ( !__solver->resolveType( agg.variableType() ) ) {
          return false;
        }

        // Checking type legality if overload
        if ( !__checkAggTypeLegality( o3class, agg ) ) {
          return false;
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE const PRMType<GUM_SCALAR>*
      O3ClassFactory<GUM_SCALAR>::__checkAggParents( O3Class&     o3class,
                                                     O3Aggregate& agg ) {

        const auto& c = __prm->getClass( o3class.name().label() );
        auto        t = (const PRMType<GUM_SCALAR>*)nullptr;

        for ( const auto& prnt : agg.parents() ) {

          auto elt = __resolveSlotChain( c, prnt );

          if ( elt == nullptr ) {

            O3PRM_CLASS_PARENT_NOT_FOUND( prnt, *__errors );
            return nullptr;

          } else {

            if ( t == nullptr ) {

              try {

                t = &( elt->type() );

              } catch ( OperationNotAllowed& ) {

                O3PRM_CLASS_WRONG_PARENT( prnt, *__errors );
                return nullptr;
              }

            } else if ( ( *t ) != elt->type() ) {

              // Wront type in chain
              O3PRM_CLASS_WRONG_PARENT_TYPE(
                  prnt, t->name(), elt->type().name(), *__errors );
              return nullptr;
            }
          }
        }
        return t;
      }

      template <typename GUM_SCALAR>
      INLINE bool
      O3ClassFactory<GUM_SCALAR>::__checkAggTypeLegality( O3Class&     o3class,
                                                          O3Aggregate& agg ) {

        if ( __prm->isClass( o3class.superLabel().label() ) ) {

          const auto& super = __prm->getClass( o3class.superLabel().label() );
          const auto& agg_type = __prm->type( agg.variableType().label() );

          if ( super.exists( agg.name().label() ) &&
               !agg_type.isSubTypeOf( super.get( agg.name().label() ).type() ) ) {

            O3PRM_CLASS_ILLEGAL_OVERLOAD(
                agg.name(), o3class.superLabel(), *__errors );
            return false;
          }
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkAggParameters(
          O3Class& o3class, O3Aggregate& agg, const PRMType<GUM_SCALAR>* t ) {

        bool ok = false;

        switch ( gum::prm::PRMAggregate<GUM_SCALAR>::str2enum(
            agg.aggregateType().label() ) ) {

          case PRMAggregate<GUM_SCALAR>::AggregateType::MIN:
          case PRMAggregate<GUM_SCALAR>::AggregateType::MAX:
          case PRMAggregate<GUM_SCALAR>::AggregateType::AMPLITUDE:
          case PRMAggregate<GUM_SCALAR>::AggregateType::MEDIAN:
          case PRMAggregate<GUM_SCALAR>::AggregateType::OR:
          case PRMAggregate<GUM_SCALAR>::AggregateType::AND: {

            ok = __checkParametersNumber( agg, 0 );
            break;
          }

          case PRMAggregate<GUM_SCALAR>::AggregateType::FORALL:
          case PRMAggregate<GUM_SCALAR>::AggregateType::EXISTS:
          case PRMAggregate<GUM_SCALAR>::AggregateType::COUNT: {

            ok = __checkParametersNumber( agg, 1 );
            break;
          }

          default: { GUM_ERROR( FatalError, "unknown aggregate type" ); }
        }

        if ( !ok ) {
          return false;
        }

        // Checking parameters type
        switch ( gum::prm::PRMAggregate<GUM_SCALAR>::str2enum(
            agg.aggregateType().label() ) ) {

          case PRMAggregate<GUM_SCALAR>::AggregateType::FORALL:
          case PRMAggregate<GUM_SCALAR>::AggregateType::EXISTS:
          case PRMAggregate<GUM_SCALAR>::AggregateType::COUNT: {

            ok = __checkParameterValue( agg, *t );
            break;
          }

          default: { /* Nothing to do */
          }
        }

        return ok;
      }

      template <typename GUM_SCALAR>
      INLINE bool
      O3ClassFactory<GUM_SCALAR>::__checkParametersNumber( O3Aggregate& agg,
                                                           Size         n ) {

        if ( agg.parameters().size() != n ) {

          O3PRM_CLASS_AGG_PARAMETERS(
              agg.name(), Size( n ), Size( agg.parameters().size() ), *__errors );
          return false;
        }

        return true;
      }

      template <typename GUM_SCALAR>
      INLINE bool O3ClassFactory<GUM_SCALAR>::__checkParameterValue(
          O3Aggregate& agg, const gum::prm::PRMType<GUM_SCALAR>& t ) {

        const auto& param = agg.parameters().front();
        bool        found = false;
        for ( Size idx = 0; idx < t.variable().domainSize(); ++idx ) {
          if ( t.variable().label( idx ) == param.label() ) {
            found = true;
            break;
          }
        }

        if ( !found ) {
          O3PRM_CLASS_AGG_PARAMETER_NOT_FOUND( agg.name(), param, *__errors );
          return false;
        }

        return true;
      }

    }  // o3prm
  }    // prm
}  // gum
