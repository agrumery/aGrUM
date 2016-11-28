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
 * @brief Inline implementation of gum::Class
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

// to ease parsing
#include <queue>

#include <agrum/PRM/elements/PRMClass.h>

#include <agrum/PRM/elements/PRMAggregate.h>
#include <agrum/PRM/elements/PRMInterface.h>
#include <agrum/PRM/elements/PRMSlotChain.h>

namespace gum {
  namespace prm {
    template <typename GUM_SCALAR>
    PRMClass<GUM_SCALAR>::PRMClass( const std::string& name )
        : PRMClassElementContainer<GUM_SCALAR>( name )
        , __superClass( nullptr )
        , __implements( nullptr )
        , __bijection( nullptr ) {
      GUM_CONSTRUCTOR( PRMClass );
    }

    template <typename GUM_SCALAR>
    PRMClass<GUM_SCALAR>::PRMClass( const std::string&    name,
                                    PRMClass<GUM_SCALAR>& super,
                                    bool                  delayInheritance )
        : PRMClassElementContainer<GUM_SCALAR>( name )
        , __superClass( &super )
        , __implements( nullptr )
        , __bijection(
              new Bijection<const DiscreteVariable*, const DiscreteVariable*>() ) {
      GUM_CONSTRUCTOR( PRMClass );
      if ( !delayInheritance ) {
        __dag = super.dag();
        __inheritClass( super );
      }
    }

    template <typename GUM_SCALAR>
    PRMClass<GUM_SCALAR>::PRMClass( const std::string&                    name,
                                    const Set<PRMInterface<GUM_SCALAR>*>& set,
                                    bool delayInheritance )
        : PRMClassElementContainer<GUM_SCALAR>( name )
        , __superClass( nullptr )
        , __implements( new Set<PRMInterface<GUM_SCALAR>*>( set ) )
        , __bijection( nullptr ) {
      GUM_CONSTRUCTOR( PRMClass );

      if ( !delayInheritance ) {
        __implementInterfaces( false );
      }
    }

    template <typename GUM_SCALAR>
    PRMClass<GUM_SCALAR>::PRMClass( const std::string&                    name,
                                    PRMClass<GUM_SCALAR>&                 super,
                                    const Set<PRMInterface<GUM_SCALAR>*>& set,
                                    bool delayInheritance )
        : PRMClassElementContainer<GUM_SCALAR>( name )
        , __superClass( &super )
        , __implements( nullptr )
        , __bijection(
              new Bijection<const DiscreteVariable*, const DiscreteVariable*>() ) {
      GUM_CONSTRUCTOR( PRMClass );
      if ( !delayInheritance ) {
        __dag = super.dag();
        __inheritClass( super );
      }

      // Adding other implementation
      if ( __implements == nullptr ) {  // super has not created __implements
        __implements = new Set<PRMInterface<GUM_SCALAR>*>( set );
      } else {  // we just add the new implementations
        for ( const auto elt : set ) {
          __implements->insert( elt );
        }
      }

      if ( !delayInheritance ) {
        __implementInterfaces( false );
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::__implementInterfaces( bool delayedInheritance ) {
      for ( const auto impl : *__implements ) {
        impl->__addImplementation( this );
        if ( ( !__superClass ) ||
             ( __superClass && !super().isSubTypeOf( *impl ) ) ||
             ( __superClass && delayedInheritance ) ) {
          // Reserve reference id in DAG
          for ( auto ref : impl->referenceSlots() ) {
            __dag.addNode( ref->id() );
          }
          // Reserve attribute id in DAG
          for ( auto attr : impl->attributes() ) {
            __dag.addNode( attr->id() );
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    PRMClass<GUM_SCALAR>::~PRMClass() {
      GUM_DESTRUCTOR( PRMClass );

      for ( const auto& elt : __nodeIdMap ) {
        delete elt.second;
      }

      if ( __implements ) {
        delete __implements;
      }

      if ( __bijection ) {
        delete __bijection;
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::initializeInheritance() {
      if ( __superClass ) {
        __superClass->__addExtension( this );
        // Adding implemented interfaces, if any
        if ( __superClass->__implements ) {
          if ( !__implements ) {
            __implements = new Set<PRMInterface<GUM_SCALAR>*>(
                *( __superClass->__implements ) );
          } else {
            for ( auto i : *( __superClass->__implements ) ) {
              __implements->insert( i );
            }
          }
        }
      }
      if ( __implements ) {
        __implementInterfaces( true );
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::inheritReferenceSlots() {
      if ( __superClass ) {

        // Copying reference slots
        for ( const auto c_refslot : __superClass->__referenceSlots ) {
          auto ref = new PRMReferenceSlot<GUM_SCALAR>(
              c_refslot->name(),
              const_cast<PRMClassElementContainer<GUM_SCALAR>&>(
                  c_refslot->slotType() ),
              c_refslot->isArray() );

          ref->setId( c_refslot->id() );
          // Not reserved by an interface
          if ( !__dag.existsNode( ref->id() ) ) {
            __dag.addNode( ref->id() );
          }
          __nodeIdMap.insert( ref->id(), ref );
          __referenceSlots.insert( ref );

          if ( __superClass->__nameMap[c_refslot->name()] ==
               __superClass->__nameMap[c_refslot->safeName()] ) {
            __nameMap.insert( ref->name(), ref );
          }

          __nameMap.insert( ref->safeName(), ref );
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::inheritParameters() {
      if ( __superClass ) {
        // Copying parameters
        for ( const auto c_param : __superClass->__parameters ) {
          auto param = new PRMParameter<GUM_SCALAR>(
              c_param->name(), c_param->valueType(), c_param->value() );

          __parameters.insert( param );

          param->setId( c_param->id() );
          __dag.addNode( param->id() );
          __nodeIdMap.insert( param->id(), param );
          __nameMap.insert( param->name(), param );
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::inheritAttributes() {
      if ( __superClass ) {
        for ( const auto c_attr : __superClass->__attributes ) {
          // using multiDimSparse to prevent unecessary memory allocation for
          // large arrays (the potentials are copied latter)
          auto attr = c_attr->newFactory( *this );

          __bijection->insert( &( c_attr->type().variable() ),
                               &( attr->type().variable() ) );
          attr->setId( c_attr->id() );
          try {
            __dag.addNode( attr->id() );
          } catch ( gum::Exception& ) {
            // Node reserved by an interface
          }
          __nodeIdMap.insert( attr->id(), attr );
          __attributes.insert( attr );

          if ( __superClass->__nameMap[c_attr->name()] ==
               __superClass->__nameMap[c_attr->safeName()] ) {
            __nameMap.insert( attr->name(), attr );
          }

          __nameMap.insert( attr->safeName(), attr );
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::inheritAggregates() {
      if ( __superClass ) {
        for ( const auto c_agg : __superClass->__aggregates ) {

          PRMAggregate<GUM_SCALAR>* agg = nullptr;

          try {
            agg = new PRMAggregate<GUM_SCALAR>(
                c_agg->name(), c_agg->agg_type(), c_agg->type(), c_agg->label() );
          } catch ( OperationNotAllowed& ) {
            agg = new PRMAggregate<GUM_SCALAR>(
                c_agg->name(), c_agg->agg_type(), c_agg->type() );
            agg->sharedLabel( c_agg->sharedLabel() );
            agg->setLabel( c_agg->labelValue() );
          }

          __bijection->insert( &( c_agg->type().variable() ),
                               &( agg->type().variable() ) );
          agg->setId( c_agg->id() );
          __dag.addNode( agg->id() );
          __nodeIdMap.insert( agg->id(), agg );
          __aggregates.insert( agg );

          if ( __superClass->__nameMap[c_agg->name()] ==
               __superClass->__nameMap[c_agg->safeName()] )
            __nameMap.insert( agg->name(), agg );

          __nameMap.insert( agg->safeName(), agg );
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::inheritSlotChains() {
      if ( __superClass ) {
        // Copying slot chains
        for ( const auto c_sc : __superClass->__slotChains ) {
          // Because of aggregators, some slotchains may exists already
          if ( !( __nameMap.exists( c_sc->name() ) &&
                  __nameMap.exists( c_sc->safeName() ) ) ) {
            // We just need to change the first PRMReferenceSlot<GUM_SCALAR> in
            // the
            // chain
            auto chain = c_sc->chain();

            chain.setAtPos( 0, __nameMap[c_sc->chain().front()->name()] );

            auto sc = new PRMSlotChain<GUM_SCALAR>( c_sc->name(), chain );
            __bijection->insert( &( c_sc->type().variable() ),
                                 &( sc->type().variable() ) );
            sc->setId( c_sc->id() );
            __dag.addNode( sc->id() );
            __nodeIdMap.insert( sc->id(), sc );
            __slotChains.insert( sc );

            if ( !__nameMap.exists( sc->name() ) ) {
              __nameMap.insert( sc->name(), sc );
            }
            if ( !__nameMap.exists( sc->safeName() ) ) {
              __nameMap.insert( sc->safeName(), sc );
            }
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::completeInheritance( const std::string& name ) {
      if ( __superClass ) {
        auto& elt = this->get( name );
        if ( !( PRMClassElement<GUM_SCALAR>::isAttribute( elt ) ||
                PRMClassElement<GUM_SCALAR>::isAggregate( elt ) ) ) {
          GUM_ERROR( OperationNotAllowed,
                     "you can only complete inheritance for attributes" );
        }

        for ( const auto& prnt : super().dag().parents( elt.id() ) ) {
          this->addArc( super().get( prnt ).safeName(), elt.safeName() );
        }

        if ( PRMClassElement<GUM_SCALAR>::isAttribute( elt ) ) {
          auto& attr = static_cast<PRMAttribute<GUM_SCALAR>&>( elt );
          auto& super_attr =
              static_cast<const PRMAttribute<GUM_SCALAR>&>( super().get( name ) );
          attr.copyCpf( *__bijection, super_attr );
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::__inheritClass( const PRMClass<GUM_SCALAR>& c ) {
      if ( __superClass ) {
        __superClass->__addExtension( this );
        // Adding implemented interfaces of c, if any
        if ( c.__implements ) {
          if ( !__implements ) {
            __implements =
                new Set<PRMInterface<GUM_SCALAR>*>( *( c.__implements ) );
          } else {
            for ( auto i : *( c.__implements ) ) {
              __implements->insert( i );
            }
          }
        }

        // Copying attributes, the bijection's firsts are attributes in this and
        // its
        // seconds are attributes
        // in c.
        Bijection<const DiscreteVariable*, const DiscreteVariable*> bij;

        // Copying parameters
        for ( const auto c_param : c.__parameters ) {
          auto param = new PRMParameter<GUM_SCALAR>(
              c_param->name(), c_param->valueType(), c_param->value() );

          __parameters.insert( param );

          param->setId( c_param->id() );
          __nodeIdMap.insert( param->id(), param );
          __nameMap.insert( param->name(), param );
        }

        // Copying attributes
        for ( const auto c_attr : c.__attributes ) {
          // using multiDimSparse to prevent unecessary memory allocation for
          // large arrays (the potentials are copied latter)
          auto attr = c_attr->newFactory( *this );

          bij.insert( &( c_attr->type().variable() ),
                      &( attr->type().variable() ) );
          attr->setId( c_attr->id() );
          __nodeIdMap.insert( attr->id(), attr );
          __attributes.insert( attr );

          if ( c.__nameMap[c_attr->name()] == c.__nameMap[c_attr->safeName()] ) {
            __nameMap.insert( attr->name(), attr );
          }

          __nameMap.insert( attr->safeName(), attr );
        }

        // Copying aggregates
        for ( const auto c_agg : c.__aggregates ) {
          PRMAggregate<GUM_SCALAR>* agg = nullptr;

          try {
            agg = new PRMAggregate<GUM_SCALAR>(
                c_agg->name(), c_agg->agg_type(), c_agg->type(), c_agg->label() );
          } catch ( OperationNotAllowed& ) {
            agg = new PRMAggregate<GUM_SCALAR>(
                c_agg->name(), c_agg->agg_type(), c_agg->type() );
          }

          bij.insert( &( c_agg->type().variable() ), &( agg->type().variable() ) );
          agg->setId( c_agg->id() );
          __nodeIdMap.insert( agg->id(), agg );
          __aggregates.insert( agg );

          if ( c.__nameMap[c_agg->name()] == c.__nameMap[c_agg->safeName()] )
            __nameMap.insert( agg->name(), agg );

          __nameMap.insert( agg->safeName(), agg );
        }

        // Copying reference slots
        for ( const auto c_refslot : c.__referenceSlots ) {
          PRMReferenceSlot<GUM_SCALAR>* ref = new PRMReferenceSlot<GUM_SCALAR>(
              c_refslot->name(),
              const_cast<PRMClassElementContainer<GUM_SCALAR>&>(
                  c_refslot->slotType() ),
              c_refslot->isArray() );

          ref->setId( c_refslot->id() );
          __nodeIdMap.insert( ref->id(), ref );
          __referenceSlots.insert( ref );

          if ( c.__nameMap[c_refslot->name()] ==
               c.__nameMap[c_refslot->safeName()] )
            __nameMap.insert( ref->name(), ref );

          __nameMap.insert( ref->safeName(), ref );
        }

        // Copying slot chains
        for ( const auto c_slotchain : c.__slotChains ) {
          // We just need to change the first PRMReferenceSlot<GUM_SCALAR> in
          // the
          // chain
          Sequence<PRMClassElement<GUM_SCALAR>*> chain( c_slotchain->chain() );

          chain.setAtPos( 0, __nameMap[c_slotchain->chain().front()->name()] );

          PRMSlotChain<GUM_SCALAR>* sc =
              new PRMSlotChain<GUM_SCALAR>( c_slotchain->name(), chain );
          bij.insert( &( c_slotchain->type().variable() ),
                      &( sc->type().variable() ) );
          sc->setId( c_slotchain->id() );
          __nodeIdMap.insert( sc->id(), sc );
          __slotChains.insert( sc );

          __nameMap.insert( sc->name(), sc );
          __nameMap.insert( sc->safeName(), sc );
        }

        // Copying dependencies yield by arcs
        for ( const auto& arc : c.dag().arcs() ) {
          __nodeIdMap[arc.tail()]->addChild( *( __nodeIdMap[arc.head()] ) );
          __nodeIdMap[arc.head()]->addParent( *( __nodeIdMap[arc.tail()] ) );
        }

        // Copying the IO flag
        this->_copyIOFlags( c );
        // Copying content of CPF
        PRMAttribute<GUM_SCALAR>* a = 0;

        for ( const auto attr : c.__attributes ) {
          a = static_cast<PRMAttribute<GUM_SCALAR>*>(
              __nameMap[attr->safeName()] );
          a->copyCpf( bij, *attr );
        }
      }
    }

    template <typename GUM_SCALAR>
    bool PRMClass<GUM_SCALAR>::isSubTypeOf(
        const PRMClassElementContainer<GUM_SCALAR>& cec ) const {
      switch ( cec.obj_type() ) {
        case PRMObject::prm_type::CLASS: {
          const PRMClass<GUM_SCALAR>* current = this;

          while ( current != 0 ) {
            if ( current == &( cec ) ) return true;

            current = current->__superClass;
          }

          return false;
        }

        case PRMObject::prm_type::PRM_INTERFACE: {
          if ( __implements != nullptr ) {
            const PRMInterface<GUM_SCALAR>& i =
                static_cast<const PRMInterface<GUM_SCALAR>&>( cec );

            if ( __implements->exists(
                     const_cast<PRMInterface<GUM_SCALAR>*>( &i ) ) )
              return true;

            for ( const auto impl : *__implements )
              if ( impl->isSubTypeOf( i ) ) return true;
          }

          return false;
        }

        default: {
          GUM_ERROR( FatalError, "unknown ClassElementContainer<GUM_SCALAR>" );
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::addArc( const std::string& tail_name,
                                       const std::string& head_name ) {
      PRMClassElement<GUM_SCALAR>* tail = 0;
      PRMClassElement<GUM_SCALAR>* head = 0;

      try {
        tail = __nameMap[tail_name];
        head = __nameMap[head_name];
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "tail and/or head of arc does not exists in this Class" );
      }

      if ( ( tail->elt_type() == PRMClassElement<GUM_SCALAR>::prm_refslot ) ||
           ( head->elt_type() == PRMClassElement<GUM_SCALAR>::prm_refslot ) ) {
        GUM_ERROR( OperationNotAllowed,
                   "a PRMReferenceSlot<GUM_SCALAR> can "
                   "not on neither side of an arc" );
      }

      if ( ( tail->elt_type() == PRMClassElement<GUM_SCALAR>::prm_slotchain ) &&
           ( head->elt_type() == PRMClassElement<GUM_SCALAR>::prm_slotchain ) ) {
        GUM_ERROR(
            OperationNotAllowed,
            "illegal insertion of an arc between two SlotChain<GUM_SCALAR>" );
      }

      if ( !__dag.existsArc( Arc( tail->id(), head->id() ) ) ) {
        __dag.addArc( tail->id(), head->id() );
      } else {
        GUM_ERROR( DuplicateElement, "duplicate arc" );
      }

      get( tail->id() ).addChild( get( head->id() ) );
      get( head->id() ).addParent( get( tail->id() ) );

      // Defining input / output nodes
      if ( tail->elt_type() == PRMClassElement<GUM_SCALAR>::prm_slotchain ) {
        PRMSlotChain<GUM_SCALAR>* sc =
            static_cast<PRMSlotChain<GUM_SCALAR>*>( tail );
        this->setInputNode( *head, true );
        sc->end().setOutputNode( sc->end().get( sc->lastElt().safeName() ), true );
      }
    }

    template <typename GUM_SCALAR>
    void
    PRMClass<GUM_SCALAR>::__checkInterfaces( PRMClassElement<GUM_SCALAR>* elt ) {
      try {
        for ( auto i : implements() ) {
          if ( i->exists( elt->name() ) ) {
            __checkInterface( elt, i );
          }
        }
      } catch ( NotFound& ) {
        // No interface
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::__checkInterface( PRMClassElement<GUM_SCALAR>* elt,
                                                 PRMInterface<GUM_SCALAR>*    i ) {
      const auto& i_elt = i->get( elt->name() );
      bool        is_attr = PRMClassElement<GUM_SCALAR>::isAttribute( i_elt );
      bool        is_agg = PRMClassElement<GUM_SCALAR>::isAggregate( i_elt );

      if ( !( is_attr || is_agg ) ) {
        GUM_ERROR( OperationNotAllowed, "Class does not respect it's interface" );
      }

      if ( !elt->type().isSubTypeOf( i_elt.type() ) ) {
        GUM_ERROR( OperationNotAllowed,
                   "Attribute type does not respect class interface" );
      }

      if ( elt->type() != i_elt.type() ) {
        if ( !this->exists( i_elt.safeName() ) ) {
          GUM_ERROR( OperationNotAllowed,
                     "Attribute type does not respect class interface" );
        }
        elt = &( this->get( i_elt.safeName() ) );
      }

      // Node must be reserved by constructor
      if ( !__dag.existsNode( i_elt.id() ) ) {
        GUM_ERROR( FatalError, "Class does not reserved implemented nodes" );
      }

      // Removing unused node and changing to proper node
      if ( elt->id() != i_elt.id() ) {
        // Update cast descendants
        for ( auto child : __dag.children( elt->id() ) ) {
          __dag.addArc( i_elt.id(), child );
        }
        __dag.eraseNode( elt->id() );
      }
      __nodeIdMap.erase( elt->id() );
      elt->setId( i_elt.id() );
      __nodeIdMap.insert( elt->id(), elt );
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::__checkRefInterfaces(
        PRMReferenceSlot<GUM_SCALAR>* ref ) {
      try {
        for ( auto i : implements() ) {
          if ( i->exists( ref->name() ) ) {
            __checkRefInterface( ref, i );
          }
        }
      } catch ( NotFound& ) {
        // No interface to check
      }
    }

    template <typename GUM_SCALAR>
    void
    PRMClass<GUM_SCALAR>::__checkRefInterface( PRMReferenceSlot<GUM_SCALAR>* ref,
                                               PRMInterface<GUM_SCALAR>*     i ) {
      auto& i_elt = i->get( ref->name() );
      if ( i_elt.elt_type() != ref->elt_type() ) {
        GUM_ERROR( OperationNotAllowed, "Class does not respect it's interface" );
      }
      auto& i_ref = static_cast<PRMReferenceSlot<GUM_SCALAR>&>( i_elt );
      if ( !ref->slotType().isSubTypeOf( i_ref.slotType() ) ) {
        GUM_ERROR( OperationNotAllowed,
                   "ReferenceSlot type does not respect class interface" );
      }
      // Node must be reserved by constructor
      if ( !__dag.exists( i_ref.id() ) ) {
        GUM_ERROR( FatalError,
                   "class " << this->name() << " does not respect interface "
                            << i->name()
                            << " implementation" );
      }
      // Removing unused node and changin to propoer node
      if ( ref->id() != i_ref.id() ) {
        __dag.eraseNode( ref->id() );
      }
      __nodeIdMap.erase( ref->id() );
      ref->setId( i_ref.id() );
      __nodeIdMap.insert( ref->id(), ref );
    }

    template <typename GUM_SCALAR>
    NodeId PRMClass<GUM_SCALAR>::add( PRMClassElement<GUM_SCALAR>* elt ) {
      if ( __nameMap.exists( elt->name() ) ) {
        GUM_ERROR( DuplicateElement,
                   "name already used by another ClassElement<GUM_SCALAR>" );
      }

      elt->setId( nextNodeId() );
      __dag.addNode( elt->id() );
      __nodeIdMap.insert( elt->id(), elt );
      __nameMap.insert( elt->name(), elt );

      try {
        __nameMap.insert( elt->safeName(), elt );
      } catch ( DuplicateElement& e ) {
        if ( !( PRMClassElement<GUM_SCALAR>::isSlotChain( *elt ) ||
                PRMClassElement<GUM_SCALAR>::isParameter( *elt ) ) ) {
          throw e;
        }
      }

      switch ( elt->elt_type() ) {
        case PRMClassElement<GUM_SCALAR>::prm_attribute: {
          __attributes.insert( static_cast<PRMAttribute<GUM_SCALAR>*>( elt ) );
          __addCastDescendants( static_cast<PRMAttribute<GUM_SCALAR>*>( elt ) );

          // Update attribute or cast descendant id to respect implemented
          // interface
          __checkInterfaces( elt );

          __addIOInterfaceFlags( elt );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_aggregate: {
          __aggregates.insert( static_cast<PRMAggregate<GUM_SCALAR>*>( elt ) );
          __addCastDescendants( static_cast<PRMAttribute<GUM_SCALAR>*>( elt ) );

          // Update attribute or cast descendant id to respect implemented
          // interface
          __checkInterfaces( elt );

          __addIOInterfaceFlags( elt );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_refslot: {
          auto ref = static_cast<PRMReferenceSlot<GUM_SCALAR>*>( elt );
          __referenceSlots.insert( ref );

          // Updating ref's id if ref implements an interface
          __checkRefInterfaces( ref );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_slotchain: {
          __slotChains.insert( static_cast<PRMSlotChain<GUM_SCALAR>*>( elt ) );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_parameter: {
          __parameters.insert( static_cast<PRMParameter<GUM_SCALAR>*>( elt ) );
          break;
        }

        default: {
          GUM_ERROR( FatalError, "unknown ClassElement<GUM_SCALAR> type" );
        }
      }

      return elt->id();
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::__addCastDescendants(
        PRMClassElement<GUM_SCALAR>* attr ) {

      auto                      parent = attr;
      PRMAttribute<GUM_SCALAR>* child = 0;

      while ( parent->type().isSubType() ) {
        child = parent->getCastDescendant();

        // Check if id was reserved by one of the class interfaces
        bool found = false;
        try {
          for ( auto i : implements() ) {
            if ( i->exists( child->safeName() ) ) {
              child->setId( i->get( child->safeName() ).id() );
              found = true;
              break;
            }
          }
        } catch ( NotFound& ) {
          // No interface
        }
        if ( !found ) {
          child->setId( nextNodeId() );
          __dag.addNode( child->id() );
        }
        __nodeIdMap.insert( child->id(), child );
        // Only use child's safe name when adding to the name map!
        __nameMap.insert( child->safeName(), child );
        __attributes.insert( child );
        // Do not use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        __dag.addArc( parent->id(), child->id() );

        parent = child;
      }
    }

    template <typename GUM_SCALAR>
    NodeId
    PRMClass<GUM_SCALAR>::overload( PRMClassElement<GUM_SCALAR>* overloader ) {
      try {
        if ( !super().exists( overloader->name() ) ) {
          GUM_ERROR( OperationNotAllowed,
                     "found no ClassElement<GUM_SCALAR> to overload" );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed,
                   "overload is possible only with subclasses" );
      }

      PRMClassElement<GUM_SCALAR>* overloaded = __nameMap[overloader->name()];
      if ( overloaded == overloader ) {
        GUM_ERROR( DuplicateElement, "dupplicate ClassElement" );
      }
      // Checking overload legality
      if ( !__checkOverloadLegality( overloaded, overloader ) ) {
        GUM_ERROR( OperationNotAllowed, "illegal overload" );
      }

      switch ( overloader->elt_type() ) {
        case PRMClassElement<GUM_SCALAR>::prm_attribute: {
          auto overloader_attr =
              static_cast<PRMAttribute<GUM_SCALAR>*>( overloader );
          auto overloaded_attr =
              static_cast<PRMAttribute<GUM_SCALAR>*>( overloaded );
          __overloadAttribute( overloader_attr, overloaded_attr );
          __addIOInterfaceFlags( overloader );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_aggregate: {
          __overloadAggregate(
              static_cast<PRMAggregate<GUM_SCALAR>*>( overloader ), overloaded );
          __addIOInterfaceFlags( overloader );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_refslot: {
          // __checkOverloadLegality guaranties that overloaded is a
          // PRMReferenceSlot<GUM_SCALAR>
          auto overloader_ref =
              static_cast<PRMReferenceSlot<GUM_SCALAR>*>( overloader );
          auto overloaded_ref =
              static_cast<PRMReferenceSlot<GUM_SCALAR>*>( overloaded );
          __overloadReference( overloader_ref, overloaded_ref );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_slotchain: {
          GUM_ERROR( OperationNotAllowed,
                     "SlotChain<GUM_SCALAR> can not be overloaded" );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_parameter: {
          auto overloaded_param =
              static_cast<PRMParameter<GUM_SCALAR>*>( overloaded );
          auto overloader_param =
              static_cast<PRMParameter<GUM_SCALAR>*>( overloader );
          __overloadParameter( overloader_param, overloaded_param );
          break;
        }
        default: {
          GUM_ERROR( OperationNotAllowed,
                     "unknown ClassElement<GUM_SCALAR> type" );
        }
      }

      return overloader->id();
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::__overloadAttribute(
        PRMAttribute<GUM_SCALAR>* overloader,
        PRMAttribute<GUM_SCALAR>* overloaded ) {
      __dag.eraseParents( overloaded->id() );

      // Checking if we have to add cast descendant
      if ( overloader->type() != overloaded->type() ) {
        overloader->setId( nextNodeId() );
        __dag.addNode( overloader->id() );
        __nodeIdMap.insert( overloader->id(), overloader );
        __nameMap[overloader->name()] = overloader;
        __nameMap.insert( overloader->safeName(), overloader );
        __attributes.insert( overloader );
        __addCastDescendants( overloader, overloaded );
      } else {
        overloader->setId( overloaded->id() );
        __nodeIdMap[overloader->id()] = overloader;
        __nameMap[overloader->name()] = overloader;
        __nameMap[overloader->safeName()] = overloader;
        __attributes.erase( overloaded );
        __attributes.insert( overloader );
        overloader->overload( overloaded );
        delete overloaded;
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::__overloadReference(
        PRMReferenceSlot<GUM_SCALAR>* overloader,
        PRMReferenceSlot<GUM_SCALAR>* overloaded ) {
      // Adding overloading reference
      overloader->setId( overloaded->id() );
      __nodeIdMap[overloader->id()] = overloader;
      __nameMap[overloader->name()] = overloader;
      __nameMap.insert( overloader->safeName(), overloader );
      __referenceSlots.insert( overloader );
      PRMSlotChain<GUM_SCALAR>*               sc = 0;
      PRMReferenceSlot<GUM_SCALAR>*           ref = 0;
      PRMClassElement<GUM_SCALAR>*            next = 0;
      std::vector<PRMSlotChain<GUM_SCALAR> *> toRemove, toAdd;

      // Updating PRMSlotChain<GUM_SCALAR> which started with overloaded
      for ( const auto slotchain : __slotChains ) {
        // If the attribute pointed by this slotchain is overloaded, we need to
        // change the slotchain
        // names to it's safename version: ref.attr is replaced by
        // ref.(type)attr.
        if ( ( slotchain->chain().atPos( 0 ) == overloaded ) ) {
          Sequence<PRMClassElement<GUM_SCALAR>*> seq;
          seq.insert( overloader );

          auto elt = ++( slotchain->chain().begin() );

          while ( elt != slotchain->chain().end() ) {
            ref = static_cast<PRMReferenceSlot<GUM_SCALAR>*>( seq.back() );
            next = &( ref->slotType().get( ( *elt )->name() ) );
            seq.insert( next );
            ++elt;
          }

          // If the slotchain last element type changes, we change the slotchain
          // to
          // point towards the cast decendant
          // with the correct type
          if ( seq.back()->type() != slotchain->lastElt().type() ) {
            seq.erase( seq.back() );
            seq.insert( &( static_cast<PRMReferenceSlot<GUM_SCALAR>*>( seq.back() )
                               ->slotType()
                               .get( slotchain->lastElt().safeName() ) ) );
            std::string sc_name;
            std::string dot = ".";

            for ( Size i = 0; i < seq.size() - 1; ++i ) {
              sc_name += seq.atPos( i )->name() + dot;
            }

            sc_name += seq.back()->safeName();
            sc = new PRMSlotChain<GUM_SCALAR>( sc_name, seq );
            sc->setId( slotchain->id() );

            for ( const auto child : this->dag().children( sc->id() ) ) {
              auto& elt = get( child );
              if ( PRMClassElement<GUM_SCALAR>::isAttribute( elt ) ) {
                auto& attr = static_cast<PRMAttribute<GUM_SCALAR>&>( elt );
                auto& old_type = slotchain->lastElt().type().variable();
                auto& new_type = sc->lastElt().type().variable();
                attr.swap( old_type, new_type );
              } else {
                GUM_ERROR( OperationNotAllowed, "unexpected ClassElement" );
                // get( child ).cpf().swap(
                // slotchain->lastElt().type().variable(),
                // sc->lastElt().type().variable() );
              }
            }

            toAdd.push_back( sc );
            toRemove.push_back( slotchain );
          } else {
            // Types are identical, we just need to change the first reference
            slotchain->chain().setAtPos( 0, overloader );
          }
        }
      }

      for ( const auto torem : toRemove ) {
        __nameMap.erase( torem->name() );
        __slotChains.erase( torem );
        delete torem;
      }

      for ( const auto toadd : toAdd ) {
        __nameMap.insert( toadd->name(), toadd );
        __nodeIdMap[toadd->id()] = toadd;
        __slotChains.insert( sc );
      }

      // Removing overloaded PRMReferenceSlot<GUM_SCALAR>
      __referenceSlots.erase( overloaded );
      __nameMap.erase( overloaded->safeName() );
      delete overloaded;
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::__overloadParameter(
        PRMParameter<GUM_SCALAR>* overloader,
        PRMParameter<GUM_SCALAR>* overloaded ) {
      overloader->setId( overloaded->id() );
      __nodeIdMap[overloader->id()] = overloader;
      __nameMap[overloader->name()] = overloader;
      __nameMap[overloader->safeName()] = overloader;
      __parameters.erase( overloaded );
      __parameters.insert( overloader );
      delete overloaded;
    }

    template <typename GUM_SCALAR>
    void
    PRMClass<GUM_SCALAR>::__addCastDescendants( PRMAttribute<GUM_SCALAR>* start,
                                                PRMAttribute<GUM_SCALAR>* end ) {

      PRMAttribute<GUM_SCALAR>* parent = start;
      PRMAttribute<GUM_SCALAR>* child = 0;

      while ( parent->type().superType() != end->type() ) {
        child = parent->getCastDescendant();
        child->setId( nextNodeId() );
        __nodeIdMap.insert( child->id(), child );
        __dag.addNode( child->id() );
        // Only use child's safe name when adding to the name map!
        __nameMap.insert( child->safeName(), child );
        __attributes.insert( child );
        __addIOInterfaceFlags( child );
        // Do not use Class<GUM_SCALAR>::insertArc(), child's CPF is already
        // initialized properly
        __dag.addArc( parent->id(), child->id() );
        parent = child;
      }

      parent->setAsCastDescendant( end );
      __dag.addArc( parent->id(), end->id() );
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::_findAllSubtypes(
        Set<PRMClassElementContainer<GUM_SCALAR>*>& set ) {
      for ( const auto ext : __extensions ) {
        set.insert( ext );
        ext->_findAllSubtypes( set );
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::__addIOInterfaceFlags(
        PRMClassElement<GUM_SCALAR>* elt ) {
      // We only add IO Flags if elt matches is required by and interface
      if ( __implements != nullptr ) {
        for ( const auto impl : *__implements ) {
          PRMInterface<GUM_SCALAR>* super = impl;
          while ( super ) {
            // If the attribute is defined in an interface, we set it as an
            // OutputNode
            if ( impl->exists( elt->name() ) ) {
              try {
                this->_getIOFlag( *elt ).second = true;
              } catch ( NotFound& ) {
                this->_setIOFlag( *elt, std::make_pair( false, true ) );
              }
            }
            try {
              super = &( super->super() );
            } catch ( NotFound& ) {
              super = nullptr;
            }
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMClass<GUM_SCALAR>::_updateDescendants(
        const PRMClassElement<GUM_SCALAR>& elt ) {
      // for ( const auto ext : __extensions ) {
      //  // We test to prevent unnecessary recursive call from iter
      //  if ( !ext->isOutputNode( elt ) ) {
      //    ext->setOutputNode( elt, true );
      //  }
      //}
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMObject::prm_type PRMClass<GUM_SCALAR>::obj_type() const {
      return PRMObject::prm_type::CLASS;
    }

    template <typename GUM_SCALAR>
    INLINE const DAG& PRMClass<GUM_SCALAR>::_dag() const {
      return __dag;
    }

    template <typename GUM_SCALAR>
    INLINE DAG& PRMClass<GUM_SCALAR>::_dag() {
      return __dag;
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>& PRMClass<GUM_SCALAR>::get( NodeId id ) {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement<GUM_SCALAR> with the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElement<GUM_SCALAR>&
    PRMClass<GUM_SCALAR>::get( NodeId id ) const {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given NodeId (" << id
                                                                         << ")" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>&
    PRMClass<GUM_SCALAR>::get( const std::string& name ) {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given name (" << name
                                                                       << ")" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElement<GUM_SCALAR>&
    PRMClass<GUM_SCALAR>::get( const std::string& name ) const {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given name (" << name
                                                                       << ")" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMAttribute<GUM_SCALAR>*>&
                 PRMClass<GUM_SCALAR>::attributes() const {
      return __attributes;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMParameter<GUM_SCALAR>*>&
                 PRMClass<GUM_SCALAR>::parameters() const {
      return __parameters;
    }

    // Private struct for retrieving all params in scope
    template <typename GUM_SCALAR>
    struct ParamScopeData {
      std::string                 prefix;
      const PRMClass<GUM_SCALAR>* c;
      Idx                         depth;

      ParamScopeData( std::string                         s,
                      const PRMReferenceSlot<GUM_SCALAR>& ref,
                      Idx                                 d ) {
        prefix = s + ref.name() + ".";
        c = static_cast<const PRMClass<GUM_SCALAR>*>( &( ref.slotType() ) );
        depth = d;
      }
    };

    template <typename GUM_SCALAR>
    INLINE HashTable<std::string, const PRMParameter<GUM_SCALAR>*>
           PRMClass<GUM_SCALAR>::scope() const {
      HashTable<std::string, const PRMParameter<GUM_SCALAR>*> params;

      for ( const auto p : parameters() ) {
        params.insert( p->name(), p );
      }

      std::queue<ParamScopeData<GUM_SCALAR>> queue;

      for ( const auto ref : referenceSlots() ) {

        if ( PRMObject::isClass( ref->slotType() ) ) {
          queue.push( ParamScopeData<GUM_SCALAR>( "", *ref, 1 ) );
        }
      }

      while ( !queue.empty() ) {
        auto data = queue.front();
        queue.pop();

        if ( data.depth < 5 ) {

          for ( const auto p : data.c->parameters() ) {
            params.insert( data.prefix + p->name(), p );
          }

          for ( const auto ref : data.c->referenceSlots() ) {

            if ( PRMObject::isClass( ref->slotType() ) ) {
              queue.push( ParamScopeData<GUM_SCALAR>(
                  data.prefix, *ref, data.depth + 1 ) );
            }
          }
        } else {
          // @todo depth>5 is a workaround. Cycle detection is needed here !
          GUM_TRACE( "Depth limit reached when looking up parameters" );
        }
      }

      return params;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMAggregate<GUM_SCALAR>*>&
                 PRMClass<GUM_SCALAR>::aggregates() const {
      return __aggregates;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMReferenceSlot<GUM_SCALAR>*>&
                 PRMClass<GUM_SCALAR>::referenceSlots() const {
      return __referenceSlots;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMSlotChain<GUM_SCALAR>*>&
                 PRMClass<GUM_SCALAR>::slotChains() const {
      return __slotChains;
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClass<GUM_SCALAR>& PRMClass<GUM_SCALAR>::super() const {
      if ( __superClass ) {
        return *__superClass;
      } else {
        GUM_ERROR( NotFound, "this Class is not a subclass" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMInterface<GUM_SCALAR>*>&
                 PRMClass<GUM_SCALAR>::implements() const {
      if ( __implements ) {
        return *__implements;
      } else {
        GUM_ERROR( NotFound,
                   "this Class does not implement any Interface<GUM_SCALAR>" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>& PRMClass<GUM_SCALAR>::
    operator[]( NodeId id ) {
      return get( id );
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElement<GUM_SCALAR>& PRMClass<GUM_SCALAR>::
    operator[]( NodeId id ) const {
      return get( id );
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>& PRMClass<GUM_SCALAR>::
    operator[]( const std::string& name ) {
      return get( name );
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElement<GUM_SCALAR>& PRMClass<GUM_SCALAR>::
    operator[]( const std::string& name ) const {
      return get( name );
    }

    template <typename GUM_SCALAR>
    INLINE void PRMClass<GUM_SCALAR>::__overloadAggregate(
        PRMAggregate<GUM_SCALAR>*    overloader,
        PRMClassElement<GUM_SCALAR>* overloaded ) {
      __nameMap.insert( overloader->safeName(), overloader );
      __aggregates.insert( overloader );
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMClass<GUM_SCALAR>::__checkOverloadLegality(
        const PRMClassElement<GUM_SCALAR>* overloaded,
        const PRMClassElement<GUM_SCALAR>* overloader ) {

      if ( overloaded->elt_type() != overloader->elt_type() ) {
        return false;
      }

      switch ( overloaded->elt_type() ) {

        case PRMClassElement<GUM_SCALAR>::prm_attribute: {
          if ( !overloader->type().isSubTypeOf( overloaded->type() ) ) {
            return false;
          }
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_refslot: {

          const auto& new_slot_type =
              static_cast<const PRMReferenceSlot<GUM_SCALAR>*>( overloader )
                  ->slotType();
          const auto& old_slot_type =
              static_cast<const PRMReferenceSlot<GUM_SCALAR>*>( overloaded )
                  ->slotType();

          if ( !new_slot_type.isSubTypeOf( old_slot_type ) ) {
            return false;
          }

          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_parameter: {
          auto overloaded_param =
              static_cast<const PRMParameter<GUM_SCALAR>*>( overloaded );
          auto overloader_param =
              static_cast<const PRMParameter<GUM_SCALAR>*>( overloader );

          return overloaded_param->valueType() == overloader_param->valueType();
          break;
        }

        default: { return false; }
      }
      return true;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMClass<GUM_SCALAR>*>&
                 PRMClass<GUM_SCALAR>::extensions() const {
      return __extensions;
    }

    template <typename GUM_SCALAR>
    INLINE void PRMClass<GUM_SCALAR>::__addExtension( PRMClass<GUM_SCALAR>* c ) {
      __extensions.insert( c );
    }

    template <typename GUM_SCALAR>
    INLINE bool
    PRMClass<GUM_SCALAR>::isCastDescendant( const std::string& safe_name ) const {
      const PRMClassElement<GUM_SCALAR>& elt = get( safe_name );

      try {
        return elt.type().name() == get( elt.name() ).type().name();
      } catch ( OperationNotAllowed& ) {
        GUM_ERROR( NotFound, "no attribute with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMClass<GUM_SCALAR>::isOutputNode(
        const PRMClassElement<GUM_SCALAR>& elt ) const {
      try {
        if ( !this->_getIOFlag( elt ).second ) {

          if ( __implements ) {
            for ( auto i : *__implements ) {
              if ( i->isOutputNode( elt ) ) {
                return true;
              }
            }
          }

          if ( __superClass && ( __superClass->isOutputNode( elt ) ) ) {
            return true;
          }

        } else {
          return true;
        }
      } catch ( NotFound& ) {
      }
      return false;
    }


  } /* namespace prm */
} /* namespace gum */
