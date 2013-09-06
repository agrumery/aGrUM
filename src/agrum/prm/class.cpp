/***************************************************************************
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
 * @brief Headers of gum::prm::Class
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 *
 */

#include <agrum/prm/class.h>

#ifdef GUM_NO_INLINE
#include <agrum/prm/class.inl>
#endif //GUM_NO_INLINE

namespace gum {
  namespace prm {


    Class::Class( const std::string& name ):
      ClassElementContainer( name ), __super( 0 ), __implements( 0 ) {
      GUM_CONSTRUCTOR( Class );
    }

    Class::Class( const std::string& name, Class& super ):
      ClassElementContainer( name ), __dag( super.dag() ),
      __super( &super ), __implements( 0 ), __instantiations( super.__instantiations ) {
      GUM_CONSTRUCTOR( Class );
      super.__addExtension( this );
      __inheritClass( super );
    }

    Class::Class( const std::string& name, const Set<Interface*>& set ):
      ClassElementContainer( name ), __super( 0 ),
      __implements( new Set<Interface*>( set ) ) {
      GUM_CONSTRUCTOR( Class );
      typedef Set<Interface*>::iterator Iterator;

      for ( Iterator iter = __implements->begin(); iter != __implements->end(); ++iter ) {
        ( **iter ).__addImplementation( this );
      }
    }

    Class::Class( const std::string& name, Class& super, const Set<Interface*>& set ):
      ClassElementContainer( name ), __dag( super.dag() ),
      __super( &super ), __implements( 0 ), __instantiations( super.__instantiations ) {
      GUM_CONSTRUCTOR( Class );
      super.__addExtension( this );
      __inheritClass( super );

      // Adding other implementation
      if ( not __implements ) {
        __implements = new Set<Interface*>( set );
      } else {
        typedef Set<Interface*>::iterator Iterator;

        for ( Iterator iter = set.begin(); iter != set.end(); ++iter ) {
          __implements->insert( *iter );
        }
      }

      typedef Set<Interface*>::iterator Iterator;

      for ( Iterator iter = __implements->begin(); iter != __implements->end(); ++iter ) {
        ( **iter ).__addImplementation( this );
      }
    }

    Class::Class( const Class& source ):
      ClassElementContainer( source.name() ), __dag( source.dag() ),
      __super( source.__super ), __implements( 0 ), __instantiations( source.__instantiations ) {
      GUM_CONS_CPY( Class );
      GUM_ERROR( FatalError, "don't copy classes" );
    }

    Class::~Class() {
      GUM_DESTRUCTOR( Class );
      typedef Property<ClassElement*>::onNodes::iterator Iterator;

      for ( Iterator iter = __nodeIdMap.begin(); iter != __nodeIdMap.end(); ++iter )
        delete *iter;

      if ( __implements )
        delete __implements;
    }

    void
    Class::__inheritClass( const Class& c ) {
      // Adding implemented interfaces of c, if any
      __implements = ( c.__implements )?new Set<Interface*>( *( c.__implements ) ):0;
      // Copying attributes, the bijection's firsts are attributes in this and its seconds are attributes
      // in c.
      Bijection<const DiscreteVariable*, const DiscreteVariable*> bij;

      for ( Set<Attribute*>::iterator iter = c.__attributes.begin(); iter != c.__attributes.end(); ++iter ) {
        // using multiDimSparse to prevent unecessary memory allocation for large arrays
        Attribute* attr = new Attribute( ( *iter )->name(), ( *iter )->type(), new MultiDimSparse<prm_float>( 0.0 ) );
        bij.insert( &( ( *iter )->type().variable() ), &( attr->type().variable() ) );
        attr->setId( ( *iter )->id() );
        __nodeIdMap.insert( attr->id(), attr );
        __attributes.insert( attr );

        if ( c.__parameters.exists( const_cast<Attribute*>( *iter ) ) ) {
          __parameters.insert( attr );
          __paramValueFlags.insert( attr, c.__paramValueFlags[*iter] );
        }

        if ( c.__nameMap[( *iter )->name()] == c.__nameMap[( *iter )->safeName()] )
          __nameMap.insert( attr->name(), attr );

        __nameMap.insert( attr->safeName(), attr );
      }

      // Copying aggregates
      for ( Set<Aggregate*>::iterator iter = c.__aggregates.begin(); iter != c.__aggregates.end(); ++iter ) {
        Aggregate* agg = 0;

        try {
          agg = new Aggregate( ( *iter )->name(), ( *iter )->agg_type(), ( *iter )->type(), ( *iter )->label() );
        } catch ( OperationNotAllowed& ) {
          agg = new Aggregate( ( *iter )->name(), ( *iter )->agg_type(), ( *iter )->type() );
        }

        bij.insert( &( ( *iter )->type().variable() ), &( agg->type().variable() ) );
        agg->setId( ( *iter )->id() );
        __nodeIdMap.insert( agg->id(), agg );
        __aggregates.insert( agg );

        if ( c.__nameMap[( *iter )->name()] == c.__nameMap[( *iter )->safeName()] )
          __nameMap.insert( agg->name(), agg );

        __nameMap.insert( agg->safeName(), agg );
      }

      // Copying reference slots
      for ( Set<ReferenceSlot*>::iterator iter = c.__referenceSlots.begin(); iter != c.__referenceSlots.end(); ++iter ) {
        ReferenceSlot* ref = new ReferenceSlot( ( *iter )->name(), const_cast<ClassElementContainer&>( ( *iter )->slotType() ), ( *iter )->isArray() );
        ref->setId( ( *iter )->id() );
        __nodeIdMap.insert( ref->id(), ref );
        __referenceSlots.insert( ref );

        if ( c.__nameMap[( *iter )->name()] == c.__nameMap[( *iter )->safeName()] )
          __nameMap.insert( ref->name(), ref );

        __nameMap.insert( ref->safeName(), ref );
      }

      // Copying slot chains
      for ( Set<SlotChain*>::iterator iter = c.__slotChains.begin(); iter != c.__slotChains.end(); ++iter ) {
        // We just need to change the first ReferenceSlot in the chain
        Sequence<ClassElement*> chain( ( *iter )->chain() );
        chain.setAtPos( 0, __nameMap[( *iter )->chain().front()->name()] );
        SlotChain* sc = new SlotChain( ( *iter )->name(), chain );
        bij.insert( &( ( *iter )->type().variable() ), &( sc->type().variable() ) );
        sc->setId( ( *iter )->id() );
        __nodeIdMap.insert( sc->id(), sc );
        __slotChains.insert( sc );
        // Slot chains do not have safe names
        __nameMap.insert( sc->name(), sc );
      }

      // Copying dependencies yield by arcs
      for ( DAG::ArcIterator arc = c.dag().beginArcs(); arc != c.dag().endArcs(); ++arc ) {
        __nodeIdMap[arc->tail()]->addChild( *( __nodeIdMap[arc->head()] ) );
        __nodeIdMap[arc->head()]->addParent( *( __nodeIdMap[arc->tail()] ) );
      }

      // Copying the IO flag
      _copyIOFlags( c );
      // Copying content of CPF
      Attribute* a = 0;

      for ( Set<Attribute*>::iterator iter = c.__attributes.begin(); iter != c.__attributes.end(); ++iter ) {
        a = static_cast<Attribute*>( __nameMap[( **iter ).safeName()] );
        delete a->__cpf;
        a->__cpf = copyPotential( bij, ( **iter ).cpf() );
      }
    }

    bool
    Class::isSubTypeOf( const ClassElementContainer& cec ) const {
      switch ( cec.obj_type() ) {
        case PRMObject::PRMType::CLASS: {
          const Class* current = this;

          while ( current != 0 ) {
            if ( current == &( cec ) ) return true;

            current = current->__super;
          }

          return false;
        }

        case PRMObject::PRMType::INTERFACE: {
          if ( __implements ) {
            const Interface& i = static_cast<const Interface&>( cec );

            if ( __implements->exists( const_cast<Interface*>( &i ) ) )
              return true;

            for ( Set<Interface*>::iterator iter = __implements->begin(); iter != __implements->end(); ++iter )
              if ( ( *iter )->isSubTypeOf( i ) )
                return true;
          }

          return false;
        }

        default:
        { GUM_ERROR( FatalError, "unknown ClassElementContainer" ); }
      }
    }

    void
    Class::insertArc( const std::string& tail_name, const std::string& head_name ) {
      ClassElement* tail = 0;
      ClassElement* head = 0;

      try {
        tail = __nameMap[tail_name];
        head = __nameMap[head_name];
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "tail and/or head of arc does not exists in this Class" );
      }

      if ( ( tail->elt_type() == ClassElement::prm_refslot ) or
           ( head->elt_type() == ClassElement::prm_refslot ) ) {
        GUM_ERROR( OperationNotAllowed, "a ReferenceSlot can not on neither side of an arc" );
      }

      if ( ( tail->elt_type() == ClassElement::prm_slotchain ) and
           ( head->elt_type() == ClassElement::prm_slotchain ) ) {
        GUM_ERROR( OperationNotAllowed, "illegal insertion of an arc between two SlotChain" );
      }

      if ( not __dag.existsArc( Arc( tail->id(), head->id() ) ) ) {
        __dag.insertArc( tail->id(), head->id() );
      } else {
        GUM_ERROR( DuplicateElement, "duplicate arc" );
      }

      get( tail->id() ).addChild( get( head->id() ) );
      get( head->id() ).addParent( get( tail->id() ) );

      // Defining input / output nodes
      if ( tail->elt_type() == ClassElement::prm_slotchain ) {
        SlotChain* sc = static_cast<SlotChain*>( tail );
        setInputNode( *head, true );
        sc->end().setOutputNode( sc->end().get( sc->lastElt().safeName() ), true );
      }
    }

    NodeId
    Class::add( ClassElement* elt ) {
      if ( __nameMap.exists( elt->name() ) ) {
        GUM_ERROR( DuplicateElement, "name already used by another ClassElement" );
      }

      elt->setId( __dag.insertNode() );
      __nodeIdMap.insert( elt->id(), elt );
      __nameMap.insert( elt->name(), elt );

      try {
        __nameMap.insert( elt->safeName(), elt );
      } catch ( DuplicateElement& ) {
        // happens when elt is a slot chain
        GUM_ASSERT( elt->elt_type() == ClassElement::prm_slotchain );
      }

      switch ( elt->elt_type() ) {
        case ClassElement::prm_attribute: {
          __attributes.insert( static_cast<Attribute*>( elt ) );
          __addCastDescendants( static_cast<Attribute*>( elt ) );
          __addIOInterfaceFlags( elt );
          break;
        }

        case ClassElement::prm_aggregate: {
          __aggregates.insert( static_cast<Aggregate*>( elt ) );
          __addIOInterfaceFlags( elt );
          break;
        }

        case ClassElement::prm_refslot: {
          ReferenceSlot* ref = static_cast<ReferenceSlot*>( elt );
          __referenceSlots.insert( ref );
          break;
        }

        case ClassElement::prm_slotchain: {
          __slotChains.insert( static_cast<SlotChain*>( elt ) );
          break;
        }

        default: {
          GUM_ERROR( FatalError, "unknown ClassElement type" );
        }
      }

      return elt->id();
    }

    void
    Class::__addCastDescendants( Attribute* attr ) {
      Attribute* parent = attr;
      Attribute* child = 0;

      while ( parent->type().isSubType() ) {
        child = parent->getCastDescendant();
        child->setId( __dag.insertNode() );
        __nodeIdMap.insert( child->id(), child );
        // Only use child's safe name when adding to the name map!
        __nameMap.insert( child->safeName(), child );
        __attributes.insert( child );
        // Do not use Class::insertArc(), child's CPF is already initialized properly
        __dag.insertArc( parent->id(), child->id() );
        parent = child;
      }
    }

    NodeId
    Class::overload( ClassElement* overloader ) {
      try {
        if ( not super().exists( overloader->name() ) ) {
          GUM_ERROR( OperationNotAllowed, "found no ClassElement to overload" );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( OperationNotAllowed, "overload is possible only with subclasses" );
      }

      ClassElement* overloaded = __nameMap[overloader->name()];
      // Checking overload legality
      __checkOverloadLegality( overloaded, overloader );

      switch ( overloader->elt_type() ) {
        case ClassElement::prm_attribute: {
          __overloadAttribute( static_cast<Attribute*>( overloader ), static_cast<Attribute*>( overloaded ) );
          __addIOInterfaceFlags( overloader );
          break;
        }

        case ClassElement::prm_aggregate: {
          __overloadAggregate( static_cast<Aggregate*>( overloader ), overloaded );
          __addIOInterfaceFlags( overloader );
          break;
        }

        case ClassElement::prm_refslot: {
          // __checkOverloadLegality guaranties that overloaded is a ReferenceSlot
          __overloadReference( static_cast<ReferenceSlot*>( overloader ),
                               static_cast<ReferenceSlot*>( overloaded ) );
          break;
        }

        case ClassElement::prm_slotchain: {
          GUM_ERROR( WrongClassElement, "SlotChain can not be overloaded" );
          break;
        }

        default: {
          GUM_ERROR( FatalError, "unknown ClassElement type" );
        }
      }

      return overloader->id();
    }

    void
    Class::__overloadAttribute( Attribute* overloader, Attribute* overloaded ) {
      __dag.eraseParents( overloaded->id() );

      // Checking if we have to add cast descendant
      if ( overloader->type() != overloaded->type() ) {
        overloader->setId( __dag.insertNode() );
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
        // Swapping types, ugly but necessary to preserve the Type pointer of overloaded
        __swap_types( overloader, overloaded );
      }
    }

    void
    Class::__overloadReference( ReferenceSlot* overloader, ReferenceSlot* overloaded ) {
      // Adding overloading reference
      overloader->setId( overloaded->id() );
      __nodeIdMap[overloader->id()] = overloader;
      __nameMap[overloader->name()] = overloader;
      __referenceSlots.insert( overloader );
      SlotChain* sc = 0;
      ReferenceSlot* ref = 0;
      ClassElement* next = 0;
      std::vector<SlotChain*> toRemove, toAdd;

      // Updating SlotChain which started with overloaded
      for ( Set<SlotChain*>::iterator iter = __slotChains.begin(); iter != __slotChains.end(); ++iter ) {
        // If the attribute pointed by this slotchain is overloaded, we need to change the slotchain
        // names to it's safename version: ref.attr is replaced by ref.(type)attr.
        if ( ( ( **iter ).chain().atPos( 0 ) == overloaded ) ) {
          Sequence<ClassElement*> seq;
          Sequence<ClassElement*>::iterator elt = ++( ( **iter ).chain().begin() );
          seq.insert( overloader );

          while ( elt != ( **iter ).chain().end() ) {
            ref = static_cast<ReferenceSlot*>( seq.back() );
            next = &( ref->slotType().get( ( **elt ).name() ) );
            seq.insert( next );
            ++elt;
          }

          // If the slotchain last element type changes, we change the slotchain to point towards the cast decendant
          // with the correct type
          if ( seq.back()->type() != ( **iter ).lastElt().type() ) {
            seq.erase( seq.back() );
            seq.insert( &( static_cast<ReferenceSlot*>( seq.back() )->slotType().get( ( **iter ).lastElt().safeName() ) ) );
            std::string sc_name; std::string dot = ".";

            for ( Size i = 0; i < seq.size() - 1; ++i ) { sc_name += seq.atPos( i )->name() + dot; }

            sc_name += seq.back()->safeName();
            sc = new SlotChain( sc_name, seq );
            sc->setId( ( **iter ).id() );

            for ( NodeSet::const_iterator child = dag().children( sc->id() ).begin(); child != dag().children( sc->id() ).end(); ++child )
              get( *child ).cpf().swap( ( **iter ).lastElt().type().variable(), sc->lastElt().type().variable() );

            toAdd.push_back( sc );
            toRemove.push_back( *iter );
          } else {
            // Types are identical, we just need to change the first reference
            ( **iter ).chain().setAtPos( 0, overloader );
          }
        }
      }

      for ( std::vector<SlotChain*>::iterator iter = toRemove.begin(); iter != toRemove.end(); ++iter ) {
        __nameMap.erase( ( **iter ).name() );
        __slotChains.erase( *iter );
        delete *iter;
      }

      for ( std::vector<SlotChain*>::iterator iter = toAdd.begin(); iter != toAdd.end(); ++iter ) {
        __nameMap.insert( sc->name(), sc );
        __nodeIdMap[sc->id()] = sc;
        __slotChains.insert( sc );
      }

      // Removing overloaded ReferenceSlot
      __referenceSlots.erase( overloaded );
      delete overloaded;
    }

    void
    Class::__swap_types( ClassElement* overloader, ClassElement* overloaded ) {
      if ( overloader->elt_type() == ClassElement::prm_attribute ) {
        Attribute* loader = static_cast<Attribute*>( overloader );

        if ( overloaded->elt_type() == ClassElement::prm_attribute ) {
          Attribute* loaded = static_cast<Attribute*>( overloaded );
          Type* tmp = loader->__type;
          loader->__type = loaded->__type;
          loaded->__type = tmp;
          loader->__cpf->erase( tmp->variable() );
          loader->__cpf->add( loader->__type->variable() );
        } else if ( overloaded->elt_type() == ClassElement::prm_aggregate ) {
          Aggregate* loaded = static_cast<Aggregate*>( overloaded );
          Type* tmp = loader->__type;
          loader->__type = loaded->__type;
          loaded->__type = tmp;
        } else {
          GUM_ERROR( FatalError, "swapping types impossible" );
        }
      } else if ( overloader->elt_type() == ClassElement::prm_aggregate ) {
        Aggregate* loader = static_cast<Aggregate*>( overloader );

        if ( overloaded->elt_type() == ClassElement::prm_attribute ) {
          Attribute* loaded = static_cast<Attribute*>( overloaded );
          Type* tmp = loader->__type;
          loader->__type = loaded->__type;
          loaded->__type = tmp;
        } else if ( overloaded->elt_type() == ClassElement::prm_aggregate ) {
          Aggregate* loaded = static_cast<Aggregate*>( overloaded );
          Type* tmp = loader->__type;
          loader->__type = loaded->__type;
          loaded->__type = tmp;
        } else {
          GUM_ERROR( FatalError, "swapping types impossible" );
        }
      }

      delete overloaded;
    }

    void
    Class::__addCastDescendants( Attribute* start, Attribute* end ) {
      Attribute* parent = start;
      Attribute* child = 0;

      while ( parent->type().super() != end->type() ) {
        child = parent->getCastDescendant();
        child->setId( __dag.insertNode() );
        __nodeIdMap.insert( child->id(), child );
        // Only use child's safe name when adding to the name map!
        __nameMap.insert( child->safeName(), child );
        __attributes.insert( child );
        __addIOInterfaceFlags( child );
        // Do not use Class::insertArc(), child's CPF is already initialized properly
        __dag.insertArc( parent->id(), child->id() );
        parent = child;
      }

      parent->setAsCastDescendant( end );
      __dag.insertArc( parent->id(), end->id() );
    }

    void
    Class::_findAllSubtypes( Set<ClassElementContainer*>& set ) {
      for ( Set<Class*>::iterator iter = __extensions.begin();
            iter != __extensions.end(); ++iter ) {
        set.insert( *iter );
        ( **iter )._findAllSubtypes( set );
      }
    }

    void
    Interface::_findAllSubtypes( Set<ClassElementContainer*>& set ) {
      for ( Set<Class*>::iterator iter = __implementations.begin();
            iter != __implementations.end(); ++iter ) {
        set.insert( *iter );
        ( **iter )._findAllSubtypes( set );
      }

      for ( Set<Interface*>::iterator iter = __extensions.begin();
            iter != __extensions.end(); ++iter ) {
        set.insert( *iter );
        ( **iter )._findAllSubtypes( set );
      }
    }

    void
    Class::__addIOInterfaceFlags( ClassElement* elt ) {
      if ( __implements != 0 ) {
        for ( Set<Interface*>::iterator iter = __implements->begin(); iter != __implements->end(); ++iter ) {
          if ( ( **iter ).isOutputNode( *elt ) ) {
            try {
              _getIOFlag( *elt ).second = true;
            } catch ( NotFound& ) {
              _setIOFlag( *elt, std::make_pair( false, true ) );
            }
          }
        }
      }
    }

    void
    Class::_updateDescendants( const ClassElement& elt ) {
      typedef Set<Class*>::iterator Iter;

      for ( Iter iter = __extensions.begin(); iter != __extensions.end(); ++iter ) {
        // We test to prevent unnecessary recursive call from iter
        if ( not( **iter ).isOutputNode( elt ) ) {
          ( *iter )->setOutputNode( elt, true );
        }
      }
    }


  } /* namespace prm */
} /* namespace gum */

