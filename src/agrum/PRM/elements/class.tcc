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

#include <agrum/PRM/elements/class.h>

#include <agrum/PRM/elements/interface.h>
#include <agrum/PRM/elements/aggregate.h>
#include <agrum/PRM/elements/slotChain.h>

namespace gum {
  namespace prm {
    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class( const std::string& name )
        : ClassElementContainer<GUM_SCALAR>( name )
        , __super( 0 )
        , __implements( 0 ) {
      GUM_CONSTRUCTOR( Class );
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class( const std::string& name,
                              Class<GUM_SCALAR>& super )
        : ClassElementContainer<GUM_SCALAR>( name )
        , __dag( super.dag() )
        , __super( &super )
        , __implements( 0 ) {
      GUM_CONSTRUCTOR( Class );
      super.__addExtension( this );
      __inheritClass( super );
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class( const std::string& name,
                              const Set<Interface<GUM_SCALAR>*>& set )
        : ClassElementContainer<GUM_SCALAR>( name )
        , __super( 0 )
        , __implements( new Set<Interface<GUM_SCALAR>*>( set ) ) {
      GUM_CONSTRUCTOR( Class );

      for ( const auto impl : *__implements )
        impl->__addImplementation( this );
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class( const std::string& name,
                              Class<GUM_SCALAR>& super,
                              const Set<Interface<GUM_SCALAR>*>& set )
        : ClassElementContainer<GUM_SCALAR>( name )
        , __dag( super.dag() )
        , __super( &super )
        , __implements( nullptr ) {
      GUM_CONSTRUCTOR( Class );
      super.__addExtension( this );
      __inheritClass( super );

      // Adding other implementation
      if ( __implements == nullptr ) {  // super has not created __implements
        __implements = new Set<Interface<GUM_SCALAR>*>( set );
      } else {  // we just add the new implementations
        for ( const auto elt : set ) {
          __implements->insert( elt );
        }
      }

      for ( const auto impl : *__implements )
        impl->__addImplementation( this );
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::Class( const Class<GUM_SCALAR>& source )
        : ClassElementContainer<GUM_SCALAR>( source.name() )
        , __dag( source.dag() )
        , __super( source.__super )
        , __implements( 0 ) {
      GUM_CONS_CPY( Class );
      GUM_ERROR( FatalError, "don't copy classes" );
    }

    template <typename GUM_SCALAR>
    Class<GUM_SCALAR>::~Class() {
      GUM_DESTRUCTOR( Class );

      for ( const auto& elt : __nodeIdMap ) {
        delete elt.second;
      }

      if ( __implements ) {
        delete __implements;
      }
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::__inheritClass( const Class<GUM_SCALAR>& c ) {
      // Adding implemented interfaces of c, if any
      __implements =
          ( c.__implements )
              ? new Set<Interface<GUM_SCALAR>*>( *( c.__implements ) )
              : 0;
      // Copying attributes, the bijection's firsts are attributes in this and
      // its
      // seconds are attributes
      // in c.
      Bijection<const DiscreteVariable*, const DiscreteVariable*> bij;

      // Copying parameters
      for ( const auto c_param : c.__parameters ) {
        auto param = new Parameter<GUM_SCALAR>(
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
        Aggregate<GUM_SCALAR>* agg = nullptr;

        try {
          agg = new Aggregate<GUM_SCALAR>(
              c_agg->name(), c_agg->agg_type(), c_agg->type(), c_agg->label() );
        } catch ( OperationNotAllowed& ) {
          agg = new Aggregate<GUM_SCALAR>(
              c_agg->name(), c_agg->agg_type(), c_agg->type() );
        }

        bij.insert( &( c_agg->type().variable() ),
                    &( agg->type().variable() ) );
        agg->setId( c_agg->id() );
        __nodeIdMap.insert( agg->id(), agg );
        __aggregates.insert( agg );

        if ( c.__nameMap[c_agg->name()] == c.__nameMap[c_agg->safeName()] )
          __nameMap.insert( agg->name(), agg );

        __nameMap.insert( agg->safeName(), agg );
      }

      // Copying reference slots
      for ( const auto c_refslot : c.__referenceSlots ) {
        ReferenceSlot<GUM_SCALAR>* ref = new ReferenceSlot<GUM_SCALAR>(
            c_refslot->name(),
            const_cast<ClassElementContainer<GUM_SCALAR>&>(
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
        // We just need to change the first ReferenceSlot<GUM_SCALAR> in the
        // chain
        Sequence<ClassElement<GUM_SCALAR>*> chain( c_slotchain->chain() );

        chain.setAtPos( 0, __nameMap[c_slotchain->chain().front()->name()] );

        SlotChain<GUM_SCALAR>* sc =
            new SlotChain<GUM_SCALAR>( c_slotchain->name(), chain );
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
      Attribute<GUM_SCALAR>* a = 0;

      for ( const auto attr : c.__attributes ) {
        a = static_cast<Attribute<GUM_SCALAR>*>( __nameMap[attr->safeName()] );
        a->copyCpf( bij, *attr );
      }
    }

    template <typename GUM_SCALAR>
    bool Class<GUM_SCALAR>::isSubTypeOf(
        const ClassElementContainer<GUM_SCALAR>& cec ) const {
      switch ( cec.obj_type() ) {
        case PRMObject::PRMType::CLASS: {
          const Class<GUM_SCALAR>* current = this;

          while ( current != 0 ) {
            if ( current == &( cec ) ) return true;

            current = current->__super;
          }

          return false;
        }

        case PRMObject::PRMType::INTERFACE: {
          if ( __implements != nullptr ) {
            const Interface<GUM_SCALAR>& i =
                static_cast<const Interface<GUM_SCALAR>&>( cec );

            if ( __implements->exists(
                     const_cast<Interface<GUM_SCALAR>*>( &i ) ) )
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
    void Class<GUM_SCALAR>::addArc( const std::string& tail_name,
                                    const std::string& head_name ) {
      ClassElement<GUM_SCALAR>* tail = 0;
      ClassElement<GUM_SCALAR>* head = 0;

      try {
        tail = __nameMap[tail_name];
        head = __nameMap[head_name];
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "tail and/or head of arc does not exists in this Class" );
      }

      if ( ( tail->elt_type() == ClassElement<GUM_SCALAR>::prm_refslot ) or
           ( head->elt_type() == ClassElement<GUM_SCALAR>::prm_refslot ) ) {
        GUM_ERROR(
            OperationNotAllowed,
            "a ReferenceSlot<GUM_SCALAR> can not on neither side of an arc" );
      }

      if ( ( tail->elt_type() == ClassElement<GUM_SCALAR>::prm_slotchain ) and
           ( head->elt_type() == ClassElement<GUM_SCALAR>::prm_slotchain ) ) {
        GUM_ERROR(
            OperationNotAllowed,
            "illegal insertion of an arc between two SlotChain<GUM_SCALAR>" );
      }

      if ( not __dag.existsArc( Arc( tail->id(), head->id() ) ) ) {
        __dag.addArc( tail->id(), head->id() );
      } else {
        GUM_ERROR( DuplicateElement, "duplicate arc" );
      }

      get( tail->id() ).addChild( get( head->id() ) );
      get( head->id() ).addParent( get( tail->id() ) );

      // Defining input / output nodes
      if ( tail->elt_type() == ClassElement<GUM_SCALAR>::prm_slotchain ) {
        SlotChain<GUM_SCALAR>* sc = static_cast<SlotChain<GUM_SCALAR>*>( tail );
        this->setInputNode( *head, true );
        sc->end().setOutputNode( sc->end().get( sc->lastElt().safeName() ),
                                 true );
      }
    }

    template <typename GUM_SCALAR>
    NodeId Class<GUM_SCALAR>::add( ClassElement<GUM_SCALAR>* elt ) {
      if ( __nameMap.exists( elt->name() ) ) {
        GUM_ERROR( DuplicateElement,
                   "name already used by another ClassElement<GUM_SCALAR>" );
      }

      elt->setId( __dag.addNode() );
      __nodeIdMap.insert( elt->id(), elt );
      __nameMap.insert( elt->name(), elt );

      try {
        __nameMap.insert( elt->safeName(), elt );
      } catch ( DuplicateElement& ) {
        GUM_ASSERT(
            elt->elt_type() == ClassElement<GUM_SCALAR>::prm_slotchain or
            elt->elt_type() == ClassElement<GUM_SCALAR>::prm_parameter );
      }

      switch ( elt->elt_type() ) {
        case ClassElement<GUM_SCALAR>::prm_attribute: {
          __attributes.insert( static_cast<Attribute<GUM_SCALAR>*>( elt ) );
          __addCastDescendants( static_cast<Attribute<GUM_SCALAR>*>( elt ) );
          __addIOInterfaceFlags( elt );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_aggregate: {
          __aggregates.insert( static_cast<Aggregate<GUM_SCALAR>*>( elt ) );
          __addIOInterfaceFlags( elt );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_refslot: {
          ReferenceSlot<GUM_SCALAR>* ref =
              static_cast<ReferenceSlot<GUM_SCALAR>*>( elt );
          __referenceSlots.insert( ref );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_slotchain: {
          __slotChains.insert( static_cast<SlotChain<GUM_SCALAR>*>( elt ) );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_parameter: {
          __parameters.insert( static_cast<Parameter<GUM_SCALAR>*>( elt ) );
          break;
        }

        default: {
          GUM_ERROR( FatalError, "unknown ClassElement<GUM_SCALAR> type" );
        }
      }

      return elt->id();
    }

    template <typename GUM_SCALAR>
    void
    Class<GUM_SCALAR>::__addCastDescendants( Attribute<GUM_SCALAR>* attr ) {
      Attribute<GUM_SCALAR>* parent = attr;
      Attribute<GUM_SCALAR>* child = 0;

      while ( parent->type().isSubType() ) {
        child = parent->getCastDescendant();
        child->setId( __dag.addNode() );
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
    NodeId Class<GUM_SCALAR>::overload( ClassElement<GUM_SCALAR>* overloader ) {
      try {
        if ( not super().exists( overloader->name() ) ) {
          GUM_ERROR( OperationNotAllowed,
                     "found no ClassElement<GUM_SCALAR> to overload" );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed,
                   "overload is possible only with subclasses" );
      }

      ClassElement<GUM_SCALAR>* overloaded = __nameMap[overloader->name()];
      if ( overloaded == overloader ) {
        GUM_ERROR( DuplicateElement, "dupplicate ClassElement" );
      }
      // Checking overload legality
      if ( not __checkOverloadLegality( overloaded, overloader ) ) {
        GUM_ERROR( OperationNotAllowed, "illegal overload" );
      }

      switch ( overloader->elt_type() ) {
        case ClassElement<GUM_SCALAR>::prm_attribute: {
          auto overloader_attr =
              static_cast<Attribute<GUM_SCALAR>*>( overloader );
          auto overloaded_attr =
              static_cast<Attribute<GUM_SCALAR>*>( overloaded );
          __overloadAttribute( overloader_attr, overloaded_attr );
          __addIOInterfaceFlags( overloader );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_aggregate: {
          __overloadAggregate(
              static_cast<Aggregate<GUM_SCALAR>*>( overloader ), overloaded );
          __addIOInterfaceFlags( overloader );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_refslot: {
          // __checkOverloadLegality guaranties that overloaded is a
          // ReferenceSlot<GUM_SCALAR>
          auto overloader_ref =
              static_cast<ReferenceSlot<GUM_SCALAR>*>( overloader );
          auto overloaded_ref =
              static_cast<ReferenceSlot<GUM_SCALAR>*>( overloaded );
          __overloadReference( overloader_ref, overloaded_ref );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_slotchain: {
          GUM_ERROR( OperationNotAllowed,
                     "SlotChain<GUM_SCALAR> can not be overloaded" );
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_parameter: {
          auto overloaded_param =
              static_cast<Parameter<GUM_SCALAR>*>( overloaded );
          auto overloader_param =
              static_cast<Parameter<GUM_SCALAR>*>( overloader );
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
    void Class<GUM_SCALAR>::__overloadAttribute(
        Attribute<GUM_SCALAR>* overloader, Attribute<GUM_SCALAR>* overloaded ) {
      __dag.eraseParents( overloaded->id() );

      // Checking if we have to add cast descendant
      if ( overloader->type() != overloaded->type() ) {
        overloader->setId( __dag.addNode() );
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
    void Class<GUM_SCALAR>::__overloadReference(
        ReferenceSlot<GUM_SCALAR>* overloader,
        ReferenceSlot<GUM_SCALAR>* overloaded ) {
      // Adding overloading reference
      overloader->setId( overloaded->id() );
      __nodeIdMap[overloader->id()] = overloader;
      __nameMap[overloader->name()] = overloader;
      __nameMap.insert( overloader->safeName(), overloader );
      __referenceSlots.insert( overloader );
      SlotChain<GUM_SCALAR>* sc = 0;
      ReferenceSlot<GUM_SCALAR>* ref = 0;
      ClassElement<GUM_SCALAR>* next = 0;
      std::vector<SlotChain<GUM_SCALAR> *> toRemove, toAdd;

      // Updating SlotChain<GUM_SCALAR> which started with overloaded
      for ( const auto slotchain : __slotChains ) {
        // If the attribute pointed by this slotchain is overloaded, we need to
        // change the slotchain
        // names to it's safename version: ref.attr is replaced by
        // ref.(type)attr.
        if ( ( slotchain->chain().atPos( 0 ) == overloaded ) ) {
          Sequence<ClassElement<GUM_SCALAR>*> seq;
          seq.insert( overloader );

          auto elt = ++( slotchain->chain().begin() );

          while ( elt != slotchain->chain().end() ) {
            ref = static_cast<ReferenceSlot<GUM_SCALAR>*>( seq.back() );
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
            seq.insert( &( static_cast<ReferenceSlot<GUM_SCALAR>*>( seq.back() )
                               ->slotType()
                               .get( slotchain->lastElt().safeName() ) ) );
            std::string sc_name;
            std::string dot = ".";

            for ( Size i = 0; i < seq.size() - 1; ++i ) {
              sc_name += seq.atPos( i )->name() + dot;
            }

            sc_name += seq.back()->safeName();
            sc = new SlotChain<GUM_SCALAR>( sc_name, seq );
            sc->setId( slotchain->id() );

            for ( const auto child : this->dag().children( sc->id() ) ) {
              auto& elt = get( child );
              if ( ClassElement<GUM_SCALAR>::isAttribute( elt ) ) {
                auto& attr = static_cast<Attribute<GUM_SCALAR>&>( elt );
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

      // Removing overloaded ReferenceSlot<GUM_SCALAR>
      __referenceSlots.erase( overloaded );
      __nameMap.erase( overloaded->safeName() );
      delete overloaded;
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::__overloadParameter(
        Parameter<GUM_SCALAR>* overloader, Parameter<GUM_SCALAR>* overloaded ) {
      overloader->setId( overloaded->id() );
      __nodeIdMap[overloader->id()] = overloader;
      __nameMap[overloader->name()] = overloader;
      __nameMap[overloader->safeName()] = overloader;
      __parameters.erase( overloaded );
      __parameters.insert( overloader );
      delete overloaded;
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::__addCastDescendants( Attribute<GUM_SCALAR>* start,
                                                  Attribute<GUM_SCALAR>* end ) {
      Attribute<GUM_SCALAR>* parent = start;
      Attribute<GUM_SCALAR>* child = 0;

      while ( parent->type().super() != end->type() ) {
        child = parent->getCastDescendant();
        child->setId( __dag.addNode() );
        __nodeIdMap.insert( child->id(), child );
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
    void Class<GUM_SCALAR>::_findAllSubtypes(
        Set<ClassElementContainer<GUM_SCALAR>*>& set ) {
      for ( const auto ext : __extensions ) {
        set.insert( ext );
        ext->_findAllSubtypes( set );
      }
    }

    template <typename GUM_SCALAR>
    void
    Class<GUM_SCALAR>::__addIOInterfaceFlags( ClassElement<GUM_SCALAR>* elt ) {
      // We only add IO Flags if elt matches is required by and interface
      if ( __implements != nullptr ) {
        for ( const auto impl : *__implements ) {
          Interface<GUM_SCALAR>* super = impl;
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
            } catch ( NotFound& e ) {
              super = nullptr;
            }
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void Class<GUM_SCALAR>::_updateDescendants(
        const ClassElement<GUM_SCALAR>& elt ) {
      //for ( const auto ext : __extensions ) {
      //  // We test to prevent unnecessary recursive call from iter
      //  if ( !ext->isOutputNode( elt ) ) {
      //    ext->setOutputNode( elt, true );
      //  }
      //}
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMObject::PRMType Class<GUM_SCALAR>::obj_type() const {
      return PRMObject::PRMType::CLASS;
    }

    template <typename GUM_SCALAR>
    INLINE const DAG& Class<GUM_SCALAR>::_dag() const {
      return __dag;
    }

    template <typename GUM_SCALAR>
    INLINE DAG& Class<GUM_SCALAR>::_dag() {
      return __dag;
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR>& Class<GUM_SCALAR>::get( NodeId id ) {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR>&
    Class<GUM_SCALAR>::get( NodeId id ) const {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR>&
    Class<GUM_SCALAR>::get( const std::string& name ) {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR>&
    Class<GUM_SCALAR>::get( const std::string& name ) const {
      try {
        return *( __nameMap[name] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ClassElement<GUM_SCALAR> with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Attribute<GUM_SCALAR>*>&
    Class<GUM_SCALAR>::attributes() const {
      return __attributes;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Parameter<GUM_SCALAR>*>&
    Class<GUM_SCALAR>::parameters() const {
      return __parameters;
    }

    // Private struct for retrieving all params in scope
    template <typename GUM_SCALAR>
    struct ParamScopeData {
      std::string prefix;
      const Class<GUM_SCALAR>* c;

      ParamScopeData( std::string s, const ReferenceSlot<GUM_SCALAR>& ref ) {
        prefix = s + ref.name() + ".";
        c = static_cast<const Class<GUM_SCALAR>*>( &( ref.slotType() ) );
      }
    };

    template <typename GUM_SCALAR>
    INLINE HashTable<std::string, const Parameter<GUM_SCALAR>*>
    Class<GUM_SCALAR>::scope() const {
      HashTable<std::string, const Parameter<GUM_SCALAR>*> params;
      Set<const Class<GUM_SCALAR>*> visited;
      visited.insert( this );

      for ( const auto p : parameters() ) {
        params.insert( p->name(), p );
      }

      std::queue<ParamScopeData<GUM_SCALAR>> queue;

      for ( const auto ref : referenceSlots() ) {

        if ( PRMObject::isClass( ref->slotType() ) ) {
          queue.push( ParamScopeData<GUM_SCALAR>( "", *ref ) );
        }
      }

      while ( not queue.empty() ) {
        auto data = queue.front();
        queue.pop();

        if ( not visited.contains( data.c ) ) {
          visited.insert( data.c );

          for ( const auto p : data.c->parameters() ) {
            params.insert( data.prefix + p->name(), p );
          }

          for ( const auto ref : data.c->referenceSlots() ) {

            if ( PRMObject::isClass( ref->slotType() ) ) {
              queue.push( ParamScopeData<GUM_SCALAR>( data.prefix, *ref ) );
            }
          }
        }
      }

      return params;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Aggregate<GUM_SCALAR>*>&
    Class<GUM_SCALAR>::aggregates() const {
      return __aggregates;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<ReferenceSlot<GUM_SCALAR>*>&
    Class<GUM_SCALAR>::referenceSlots() const {
      return __referenceSlots;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<SlotChain<GUM_SCALAR>*>&
    Class<GUM_SCALAR>::slotChains() const {
      return __slotChains;
    }

    template <typename GUM_SCALAR>
    INLINE const Class<GUM_SCALAR>& Class<GUM_SCALAR>::super() const {
      if ( __super ) {
        return *__super;
      } else {
        GUM_ERROR( NotFound, "this Class is not a subclass" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Interface<GUM_SCALAR>*>&
    Class<GUM_SCALAR>::implements() const {
      if ( __implements ) {
        return *__implements;
      } else {
        GUM_ERROR( NotFound,
                   "this Class does not implement any Interface<GUM_SCALAR>" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR>& Class<GUM_SCALAR>::
    operator[]( NodeId id ) {
      return get( id );
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR>& Class<GUM_SCALAR>::
    operator[]( NodeId id ) const {
      return get( id );
    }

    template <typename GUM_SCALAR>
    INLINE ClassElement<GUM_SCALAR>& Class<GUM_SCALAR>::
    operator[]( const std::string& name ) {
      return get( name );
    }

    template <typename GUM_SCALAR>
    INLINE const ClassElement<GUM_SCALAR>& Class<GUM_SCALAR>::
    operator[]( const std::string& name ) const {
      return get( name );
    }

    template <typename GUM_SCALAR>
    INLINE void Class<GUM_SCALAR>::__overloadAggregate(
        Aggregate<GUM_SCALAR>* overloader,
        ClassElement<GUM_SCALAR>* overloaded ) {
      __nameMap.insert( overloader->safeName(), overloader );
      __aggregates.insert( overloader );
    }

    template <typename GUM_SCALAR>
    INLINE bool Class<GUM_SCALAR>::__checkOverloadLegality(
        const ClassElement<GUM_SCALAR>* overloaded,
        const ClassElement<GUM_SCALAR>* overloader ) {

      if ( overloaded->elt_type() != overloader->elt_type() ) {
        return false;
      }

      switch ( overloaded->elt_type() ) {

        case ClassElement<GUM_SCALAR>::prm_attribute: {
          if ( not overloader->type().isSubTypeOf( overloaded->type() ) ) {
            return false;
          }
          break;
        }

        case ClassElement<GUM_SCALAR>::prm_refslot: {

          const auto& new_slot_type =
              static_cast<const ReferenceSlot<GUM_SCALAR>*>( overloader )
                  ->slotType();
          const auto& old_slot_type =
              static_cast<const ReferenceSlot<GUM_SCALAR>*>( overloaded )
                  ->slotType();

          if ( not new_slot_type.isSubTypeOf( old_slot_type ) ) {
            return false;
          }

          break;
        }

        case ClassElement<GUM_SCALAR>::prm_parameter: {
          auto overloaded_param =
              static_cast<const Parameter<GUM_SCALAR>*>( overloaded );
          auto overloader_param =
              static_cast<const Parameter<GUM_SCALAR>*>( overloader );

          return overloaded_param->valueType() == overloader_param->valueType();
          break;
        }

        default: { return false; }
      }
      return true;
    }

    template <typename GUM_SCALAR>
    INLINE const Set<Class<GUM_SCALAR>*>&
    Class<GUM_SCALAR>::extensions() const {
      return __extensions;
    }

    template <typename GUM_SCALAR>
    INLINE void Class<GUM_SCALAR>::__addExtension( Class<GUM_SCALAR>* c ) {
      __extensions.insert( c );
    }

    template <typename GUM_SCALAR>
    INLINE bool
    Class<GUM_SCALAR>::isCastDescendant( const std::string& safe_name ) const {
      const ClassElement<GUM_SCALAR>& elt = get( safe_name );

      try {
        return elt.type().name() == get( elt.name() ).type().name();
      } catch ( OperationNotAllowed& ) {
        GUM_ERROR( NotFound, "no attribute with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE bool Class<GUM_SCALAR>::isOutputNode(
        const ClassElement<GUM_SCALAR>& elt ) const {
      try {
        if ( not this->_getIOFlag( elt ).second ) {

          if ( __implements ) {
            for ( auto i : *__implements ) {
              if ( i->isOutputNode( elt ) ) {
                return true;
              }
            }
          }

          if ( __super and __super->isOutputNode( elt ) ) {
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
