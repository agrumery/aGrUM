/**************************************************************************
*   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Inline implementation of PRMSystem.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */
#include <agrum/PRM/elements/PRMSystem.h>

#include <agrum/PRM/elements/PRMInstance.h>

#include <agrum/multidim/aggregators/exists.h>
#include <agrum/multidim/aggregators/forall.h>
#include <agrum/multidim/aggregators/max.h>
#include <agrum/multidim/aggregators/min.h>

namespace gum {
  namespace prm {
    template <typename GUM_SCALAR>
    PRMSystem<GUM_SCALAR>::PRMSystem( const std::string& name )
        : PRMObject( name ) {
      GUM_CONSTRUCTOR( PRMSystem );
    }

    template <typename GUM_SCALAR>
    PRMSystem<GUM_SCALAR>::~PRMSystem() {
      GUM_DESTRUCTOR( PRMSystem );

      for ( const auto& elt : *this )
        delete elt.second;

      for ( const auto& elt : __instanceMap )
        delete elt.second;

      for ( const auto& elt : __arrayMap )
        delete elt.second.second;
    }

    template <typename GUM_SCALAR>
    NodeId PRMSystem<GUM_SCALAR>::add( PRMInstance<GUM_SCALAR>* i ) {
      if ( __nameMap.exists( i->name() ) ) {
        GUM_ERROR(
            DuplicateElement,
            "an Instance<GUM_SCALAR> with the same is already in this System" );
      }

      NodeId id = __skeleton.addNode();
      __nodeIdMap.insert( id, i );
      __nameMap.insert( i->name(), i );

      try {
        __instanceMap[&( i->type() )]->insert( i );
      } catch ( NotFound& ) {
        __instanceMap.insert( &( i->type() ),
                              new Set<PRMInstance<GUM_SCALAR>*>() );
        __instanceMap[&( i->type() )]->insert( i );
      }

      return id;
    }

    template <typename GUM_SCALAR>
    void PRMSystem<GUM_SCALAR>::groundedBN(
        BayesNetFactory<GUM_SCALAR>& factory ) const {
      factory.startNetworkDeclaration();
      factory.addNetworkProperty( "name", name() );
      factory.endNetworkDeclaration();

      // Adding nodes
      for ( PRMSystem<GUM_SCALAR>::const_iterator iter = begin(); iter != end();
            ++iter ) {
        __groundAttr( *( iter.val() ), factory );
      }

      // Adding arcs and filling CPTs
      for ( PRMSystem<GUM_SCALAR>::const_iterator iter = begin(); iter != end();
            ++iter ) {
        __groundRef( *( iter.val() ), factory );
      }
    }

    template <typename GUM_SCALAR>
    void PRMSystem<GUM_SCALAR>::__groundAttr(
        const PRMInstance<GUM_SCALAR>& instance,
        BayesNetFactory<GUM_SCALAR>&   factory ) const {
      for ( const auto node : instance.type().dag() ) {
        // Working a Class<GUM_SCALAR> level because PRMAggregate<GUM_SCALAR>
        // are
        // instantiated as PRMAttribute<GUM_SCALAR> in an
        // PRMInstance<GUM_SCALAR>
        switch ( instance.type().get( node ).elt_type() ) {
          case PRMClassElement<GUM_SCALAR>::prm_attribute: {
            // TODO: make a special case for noisy-or
            std::stringstream elt_name;
            elt_name << instance.name() << "."
                     << instance.type().get( node ).safeName();
            DiscreteVariable* var = instance.get( node ).type().variable().clone();
            var->setName( elt_name.str() );
            factory.setVariable( *var );  // var is copied by the factory
            delete var;
            break;
          }

          case PRMClassElement<GUM_SCALAR>::prm_aggregate: {
            std::stringstream elt_name;
            elt_name << instance.name() << "."
                     << instance.type().get( node ).safeName();
            __groundAgg( instance.type().get( node ), elt_name.str(), factory );
            break;
          }

          default:
            break;
            /* Do nothing */;
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMSystem<GUM_SCALAR>::__groundAgg(
        const PRMClassElement<GUM_SCALAR>& elt,
        const std::string&                 name,
        BayesNetFactory<GUM_SCALAR>&       factory ) const {
      factory.startVariableDeclaration();
      factory.variableName( name );
      const DiscreteVariable& agg_var = elt.type().variable();

      for ( Idx i = 0; i < agg_var.domainSize(); ++i ) {
        factory.addModality( agg_var.label( i ) );
      }

      const PRMAggregate<GUM_SCALAR>& agg =
          static_cast<const PRMAggregate<GUM_SCALAR>&>( elt );

      switch ( agg.agg_type() ) {
        case PRMAggregate<GUM_SCALAR>::AggregateType::MIN: {
          factory.setVariableCPTImplementation(
              new aggregator::Min<GUM_SCALAR>() );
          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::MAX: {
          factory.setVariableCPTImplementation(
              new aggregator::Max<GUM_SCALAR>() );
          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::EXISTS: {
          factory.setVariableCPTImplementation(
              new aggregator::Exists<GUM_SCALAR>( agg.label() ) );
          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::FORALL: {
          factory.setVariableCPTImplementation(
              new aggregator::Forall<GUM_SCALAR>( agg.label() ) );
          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::COUNT: {
          factory.setVariableCPTImplementation(
              new aggregator::Count<GUM_SCALAR>( agg.label() ) );
          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::MEDIAN: {
          factory.setVariableCPTImplementation(
              new aggregator::Median<GUM_SCALAR>() );
          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::AMPLITUDE: {
          factory.setVariableCPTImplementation(
              new aggregator::Amplitude<GUM_SCALAR>() );
          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::OR: {
          factory.setVariableCPTImplementation( new aggregator::Or<GUM_SCALAR>() );
          break;
        }

        case PRMAggregate<GUM_SCALAR>::AggregateType::AND: {
          factory.setVariableCPTImplementation(
              new aggregator::And<GUM_SCALAR>() );
          break;
        }

        default: {
          GUM_ERROR( OperationNotAllowed, "Aggregator not handled yet" );
          break;
        }
      }

      factory.endVariableDeclaration();
    }

    template <typename GUM_SCALAR>
    void PRMSystem<GUM_SCALAR>::__groundRef(
        const PRMInstance<GUM_SCALAR>& instance,
        BayesNetFactory<GUM_SCALAR>&   factory ) const {
      for ( const auto& elt : instance ) {
        std::stringstream elt_name;
        elt_name << instance.name() << "." << elt.second->safeName();
        factory.startParentsDeclaration( elt_name.str() );

        for ( const auto par :
              instance.type().dag().parents( elt.second->id() ) ) {
          switch ( instance.type().get( par ).elt_type() ) {
            case PRMClassElement<GUM_SCALAR>::prm_aggregate:
            case PRMClassElement<GUM_SCALAR>::prm_attribute: {
              std::stringstream parent_name;
              parent_name << instance.name() << "."
                          << instance.get( par ).safeName();
              factory.addParent( parent_name.str() );
              break;
            }

            case PRMClassElement<GUM_SCALAR>::prm_slotchain: {
              std::string parent_name =
                  static_cast<const PRMSlotChain<GUM_SCALAR>&>(
                      instance.type().get( par ) )
                      .lastElt()
                      .safeName();

              for ( const auto ref : instance.getInstances( par ) ) {
                std::stringstream sBuff;
                sBuff << ref->name() << "." << parent_name;
                factory.addParent( sBuff.str() );
              }

              break;
            }

            default:
              break;
              /* nothing to do by default */
          }
        }

        factory.endParentsDeclaration();

        // Checking if we need to ground the Potential (only for class level
        // attributes since
        // aggregates Potentials are generated)
        if ( PRMClassElement<GUM_SCALAR>::isAttribute(
                 instance.type().get( elt.second->safeName() ) ) )
          __groundPotential( instance, *elt.second, factory );
      }
    }

    template <typename GUM_SCALAR>
    void PRMSystem<GUM_SCALAR>::__groundPotential(
        const PRMInstance<GUM_SCALAR>&  instance,
        const PRMAttribute<GUM_SCALAR>& attr,
        BayesNetFactory<GUM_SCALAR>&    factory ) const {
      Bijection<const DiscreteVariable*, const DiscreteVariable*> bijection;
      std::stringstream var_name;
      var_name << instance.name() << "." << attr.safeName();
      bijection.insert( &( attr.type().variable() ),
                        &( factory.variable( var_name.str() ) ) );

      for ( const auto parent : instance.type().dag().parents( attr.id() ) ) {
        switch ( instance.type().get( parent ).elt_type() ) {
          case PRMClassElement<GUM_SCALAR>::prm_aggregate:
          case PRMClassElement<GUM_SCALAR>::prm_attribute: {
            std::stringstream parent_name;
            parent_name << instance.name() << "."
                        << instance.get( parent ).safeName();
            bijection.insert( &( instance.get( parent ).type().variable() ),
                              &( factory.variable( parent_name.str() ) ) );
            break;
          }

          case PRMClassElement<GUM_SCALAR>::prm_slotchain: {
            std::stringstream               parent_name;
            const PRMSlotChain<GUM_SCALAR>& sc =
                static_cast<const PRMSlotChain<GUM_SCALAR>&>(
                    instance.type().get( parent ) );
            parent_name << instance.getInstance( sc.id() ).name() << "."
                        << sc.lastElt().safeName();
            bijection.insert( &( instance.getInstance( sc.id() )
                                     .get( sc.lastElt().safeName() )
                                     .type()
                                     .variable() ),
                              &( factory.variable( parent_name.str() ) ) );
            break;
          }

          default: {
            GUM_ERROR( FatalError,
                       "invalid ClassElement<GUM_SCALAR> type as parent." );
            break;
          }
        }
      }

      // Copy Potential
      // DO NOT USE MultiDimBijArray as they will wreck havok if you delete
      // the prm befor its grounded BN (happens a lot in pyAgrum)
      Potential<GUM_SCALAR>* p = new Potential<GUM_SCALAR>();
      for ( auto var : attr.cpf().variablesSequence() ) {
        p->add( *( bijection.second( var ) ) );
      }
      Instantiation inst( attr.cpf() ), jnst( *p );
      for ( inst.begin(), jnst.begin(); !( inst.end() || jnst.end() );
            inst.inc(), jnst.inc() ) {
        p->set( jnst, attr.cpf().get( inst ) );
      }
      GUM_ASSERT( inst.end() && jnst.end() );
      factory.setVariableCPT( var_name.str(), p, false );
    }

    template <typename GUM_SCALAR>
    INLINE NodeId PRMSystem<GUM_SCALAR>::add( const std::string&       array,
                                              PRMInstance<GUM_SCALAR>& i ) {
      return add( array, &i );
    }

    template <typename GUM_SCALAR>
    INLINE const DiGraph& PRMSystem<GUM_SCALAR>::skeleton() const {
      return __skeleton;
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>& PRMSystem<GUM_SCALAR>::get( NodeId id ) {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "found no Instance<GUM_SCALAR> matching the given id" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const PRMInstance<GUM_SCALAR>&
    PRMSystem<GUM_SCALAR>::get( NodeId id ) const {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "found no Instance<GUM_SCALAR> matching the given id" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE NodeId
    PRMSystem<GUM_SCALAR>::get( const PRMInstance<GUM_SCALAR>& i ) const {
      try {
        return __nodeIdMap.keyByVal( const_cast<PRMInstance<GUM_SCALAR>*>( &i ) );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "found no Instance<GUM_SCALAR> matching the given id" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject::prm_type PRMSystem<GUM_SCALAR>::obj_type() const {
      return PRMObject::prm_type::SYSTEM;
    }

    template <typename GUM_SCALAR>
    INLINE Size PRMSystem<GUM_SCALAR>::size() const {
      return __nodeIdMap.size();
    }

    template <typename GUM_SCALAR>
    INLINE bool
    PRMSystem<GUM_SCALAR>::isInstantiated( const PRMClass<GUM_SCALAR>& c ) const {
      return __instanceMap.exists( const_cast<PRMClass<GUM_SCALAR>*>( &c ) );
    }

    template <typename GUM_SCALAR>
    INLINE bool
    PRMSystem<GUM_SCALAR>::isInstance( const std::string& name ) const {
      return __nameMap.exists( name );
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMSystem<GUM_SCALAR>::isArray( const std::string& name ) const {
      return __arrayMap.exists( name );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMSystem<GUM_SCALAR>::instantiate() {
      for ( auto iter = begin(); iter != end(); ++iter ) {
        ( *( iter.val() ) ).instantiate();
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>&
    PRMSystem<GUM_SCALAR>::get( const std::string& name ) {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "found no Instance<GUM_SCALAR> matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const PRMInstance<GUM_SCALAR>&
    PRMSystem<GUM_SCALAR>::get( const std::string& name ) const {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "found no Instance<GUM_SCALAR> matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMInstance<GUM_SCALAR>*>&
    PRMSystem<GUM_SCALAR>::get( const PRMClass<GUM_SCALAR>& type ) const {
      try {
        return *( __instanceMap[const_cast<PRMClass<GUM_SCALAR>*>( &type )] );
      } catch ( NotFound& ) {
        GUM_ERROR(
            NotFound,
            "the given Class<GUM_SCALAR> has no instantiation in this System" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Sequence<PRMInstance<GUM_SCALAR>*>&
    PRMSystem<GUM_SCALAR>::getArray( const std::string& name ) const {
      try {
        return *( __arrayMap[name].second );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "found no array matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElementContainer<GUM_SCALAR>&
    PRMSystem<GUM_SCALAR>::getArrayType( const std::string& name ) {
      try {
        return *( __arrayMap[name].first );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "found no array matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElementContainer<GUM_SCALAR>&
    PRMSystem<GUM_SCALAR>::getArrayType( const std::string& name ) const {
      try {
        return *( __arrayMap[name].first );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "found no array matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE NodeId PRMSystem<GUM_SCALAR>::add( const std::string&       array,
                                              PRMInstance<GUM_SCALAR>* i ) {
      try {
        if ( i->type().isSubTypeOf( *( __arrayMap[array].first ) ) ) {
          NodeId id = add( i );
          __arrayMap[array].second->insert( i );
          return id;
        } else {
          GUM_ERROR( TypeError,
                     "the given Instance<GUM_SCALAR> is of an incorrect "
                     "Class<GUM_SCALAR> type" );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "found no array matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMSystem<GUM_SCALAR>::addArray( const std::string&                    array,
                                     PRMClassElementContainer<GUM_SCALAR>& type ) {
      if ( __arrayMap.exists( array ) ) {
        GUM_ERROR( DuplicateElement,
                   "an array with the same is already in this System" );
      }

      __arrayMap.insert( array,
                         PRMSystem<GUM_SCALAR>::model_pair(
                             &type, new Sequence<PRMInstance<GUM_SCALAR>*>() ) );
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMSystem<GUM_SCALAR>::iterator
    PRMSystem<GUM_SCALAR>::begin() {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMSystem<GUM_SCALAR>::iterator&
    PRMSystem<GUM_SCALAR>::end() {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMSystem<GUM_SCALAR>::const_iterator
    PRMSystem<GUM_SCALAR>::begin() const {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMSystem<GUM_SCALAR>::const_iterator&
    PRMSystem<GUM_SCALAR>::end() const {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMSystem<GUM_SCALAR>::array_iterator
    PRMSystem<GUM_SCALAR>::begin( const std::string& a ) {
      try {
        return __arrayMap[a].second->begin();
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "found no array matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMSystem<GUM_SCALAR>::array_iterator&
    PRMSystem<GUM_SCALAR>::end( const std::string& a ) {
      try {
        return __arrayMap[a].second->end();
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "found no array matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMSystem<GUM_SCALAR>::const_array_iterator
    PRMSystem<GUM_SCALAR>::begin( const std::string& a ) const {
      try {
        return __arrayMap[a].second->begin();
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "found no array matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMSystem<GUM_SCALAR>::const_array_iterator&
    PRMSystem<GUM_SCALAR>::end( const std::string& a ) const {
      try {
        return __arrayMap[a].second->end();
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "found no array matching the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMSystem<GUM_SCALAR>::exists( const std::string& name ) const {
      return __nameMap.exists( name ) || __arrayMap.exists( name );
    }

  } /* namespace prm */
} /* namespace gum */
