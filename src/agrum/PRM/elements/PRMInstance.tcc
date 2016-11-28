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
 * @brief Inline implementation of gum::PRMInstance
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#include <agrum/PRM/elements/PRMInstance.h>

#include <agrum/multidim/multiDimSparse.h>

namespace gum {
  namespace prm {
    template <typename GUM_SCALAR>
    PRMInstance<GUM_SCALAR>::PRMInstance( const std::string&    name,
                                          PRMClass<GUM_SCALAR>& type )
        : PRMObject( name )
        , __instantiated( false )
        , __type( &type ) {
      GUM_CONSTRUCTOR( PRMInstance );

      // First we create attributes for each aggregate in type
      for ( const auto agg : __type->aggregates() )
        __copyAggregates( agg );

      // We add attributes in type by reference for inner ones and by copy for
      // output ones
      for ( const auto attr : __type->attributes() )
        __copyAttribute( attr );
    }

    template <typename GUM_SCALAR>
    PRMInstance<GUM_SCALAR>::~PRMInstance() {
      GUM_DESTRUCTOR( PRMInstance );

      for ( const auto& elt : __nodeIdMap )
        delete elt.second;

      for ( const auto& elt : __referenceMap )
        delete elt.second;

      for ( const auto& elt : __referingAttr )
        delete elt.second;
    }

    template <typename GUM_SCALAR>
    void PRMInstance<GUM_SCALAR>::instantiate() {
      if ( !__instantiated ) {
        __instantiated = true;
        __doInstantiate();
      }
    }

    template <typename GUM_SCALAR>
    void PRMInstance<GUM_SCALAR>::__doInstantiate() {

      // First retrieving any referenced instance
      for ( const auto chain : type().slotChains() ) {
        __instantiateSlotChain( chain );
      }

      // Now we need to add referred instance to each input node
      // For Attributes we first add parents, then we initialize CPF
      for ( const auto attr : type().attributes() ) {
        __copyAttributeCPF( __nodeIdMap[( *attr ).id()] );
      }

      // For PRMAggregate<GUM_SCALAR> we add parents
      for ( const auto agg : type().aggregates() ) {
        PRMAttribute<GUM_SCALAR>& attr = get( agg->safeName() );

        for ( const auto node : type().dag().parents( agg->id() ) ) {
          try {
            attr.addParent( get( node ) );
          } catch ( NotFound& ) {

            auto        elt = &( type().get( node ) );
            auto        sc = static_cast<PRMSlotChain<GUM_SCALAR>*>( elt );
            const auto& instances = getInstances( sc->id() );

            for ( const auto inst : instances ) {
              attr.addParent( inst->get( sc->lastElt().safeName() ) );
            }
          }
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMInstance<GUM_SCALAR>::__instantiateSlotChain(
        PRMSlotChain<GUM_SCALAR>* sc ) {
      auto first_id = sc->chain()[0]->id();
      auto set =
          new Set<PRMInstance<GUM_SCALAR>*>( *( __referenceMap[first_id] ) );
      // We proceed with a width-first run of the slot chain
      for ( Size idx = 1; idx < sc->chain().size() - 1; ++idx ) {
        auto temp = new Set<PRMInstance<GUM_SCALAR>*>();
        for ( auto current : *set ) {
          auto& ref = current->type().get( sc->chain()[idx]->name() );
          for ( auto next : *( current->__referenceMap[ref.id()] ) ) {
            temp->insert( next );
          }
        }
        delete set;
        set = temp;
      }
      GUM_ASSERT( set->size() > 0 );
      // set contains all the instances references by sc
      if ( __referenceMap.exists( sc->id() ) ) {
        delete __referenceMap[sc->id()];
        __referenceMap[sc->id()] = set;
      } else {
        __referenceMap.insert( sc->id(), set );
      }

      // Add refering instances
      for ( auto i : *set ) {
        __addReferingInstance( sc, i );
      }

      // If sc is not multiple so it can be added as a parent of an attribute
      if ( !sc->isMultiple() ) {
        // We should have only one instance
        // Less ugly way to get the single instance in set
        for ( auto instance : *set ) {
          auto& attr = instance->get( sc->lastElt().safeName() );
          __bijection.insert( &( sc->type().variable() ),
                              &( attr.type().variable() ) );
        }
      }
    }

    template <typename GUM_SCALAR>
    void PRMInstance<GUM_SCALAR>::add( NodeId                   id,
                                       PRMInstance<GUM_SCALAR>& instance ) {
      PRMClassElement<GUM_SCALAR>* elt = 0;

      try {
        elt = &( type().get( id ) );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no ClassElement<GUM_SCALAR> matches the given id" );
      }

      switch ( elt->elt_type() ) {
        case PRMClassElement<GUM_SCALAR>::prm_refslot: {
          PRMReferenceSlot<GUM_SCALAR>* ref =
              static_cast<PRMReferenceSlot<GUM_SCALAR>*>( elt );

          // Checking if instance's type is legal
          if ( !instance.type().isSubTypeOf( ref->slotType() ) ) {
            GUM_ERROR( TypeError,
                       "given Instance type is not a proper "
                       "subclass of the ReferenceSlot<GUM_SCALAR> slot type" );
          }

          // Checking the reference's size limit
          if ( __referenceMap.exists( id ) &&
               ( !static_cast<PRMReferenceSlot<GUM_SCALAR>&>( type().get( id ) )
                      .isArray() ) &&
               ( __referenceMap[id]->size() == 1 ) ) {
            GUM_ERROR( OutOfUpperBound,
                       "ReferenceSlot<GUM_SCALAR> size limit reached" );
          }

          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_slotchain: {
          PRMSlotChain<GUM_SCALAR>& sc =
              static_cast<PRMSlotChain<GUM_SCALAR>&>( type().get( id ) );

          // Checking if instance's type is legal
          if ( !instance.type().isSubTypeOf( sc.end() ) ) {
            GUM_ERROR( TypeError,
                       "given Instance type is not a proper "
                       "subclass of the ClassElementContainer pointed"
                       " by the SlotChain<GUM_SCALAR>" );
          }

          // Checking the reference's size limit
          if ( __referenceMap.exists( id ) &&
               ( !static_cast<PRMSlotChain<GUM_SCALAR>&>( type().get( id ) )
                      .isMultiple() ) &&
               ( __referenceMap[id]->size() == 1 ) ) {
            GUM_ERROR( OutOfUpperBound,
                       "SlotChain<GUM_SCALAR> size limit reached" );
          }

          break;
        }

        default: {
          if ( !type().isOutputNode( *elt ) ) {
            GUM_ERROR( WrongClassElement,
                       "given ClassElement<GUM_SCALAR> is not an output node" );
          }
        }
      }

      if ( !__referenceMap.exists( id ) ) {
        __referenceMap.insert( id, new Set<PRMInstance<GUM_SCALAR>*>() );
      }

      __referenceMap[id]->insert( &instance );
    }

    template <typename GUM_SCALAR>
    INLINE Size PRMInstance<GUM_SCALAR>::size() const {
      return __nodeIdMap.size();
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMInstance<GUM_SCALAR>::__copyAggregates( PRMAggregate<GUM_SCALAR>* source ) {
      auto attr = new PRMScalarAttribute<GUM_SCALAR>(
          source->name(), source->type(), source->buildImpl() );
      GUM_ASSERT( &( attr->type().variable() ) != &( source->type().variable() ) );
      attr->setId( source->id() );
      __nodeIdMap.insert( attr->id(), attr );
      __bijection.insert( &( source->type().variable() ),
                          &( attr->type().variable() ) );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMInstance<GUM_SCALAR>::__copyAttribute( PRMAttribute<GUM_SCALAR>* source ) {
      auto attr =
          new PRMScalarAttribute<GUM_SCALAR>( source->name(), source->type() );
      GUM_ASSERT( &( attr->type().variable() ) != &( source->type().variable() ) );
      // The potential is copied when instantiate() is called
      attr->cpf().fill( (GUM_SCALAR)0 );
      attr->setId( source->id() );
      __bijection.insert( &( source->type().variable() ),
                          &( attr->type().variable() ) );
      __nodeIdMap.insert( attr->id(), attr );
    }

    template <typename GUM_SCALAR>
    INLINE
    PRMInstance<GUM_SCALAR>::PRMInstance( const PRMInstance<GUM_SCALAR>& source )
        : PRMObject( source )
        , __type( source.__type ) {
      GUM_CONS_CPY( PRMInstance );
      GUM_ERROR( FatalError, "do not copy Instance" );
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>& /**/
        PRMInstance<GUM_SCALAR>::operator=( const PRMClass<GUM_SCALAR>& from ) {
      GUM_ERROR( FatalError, "do not copy Instance" );
    }

    template <typename GUM_SCALAR>
    INLINE PRMObject::prm_type PRMInstance<GUM_SCALAR>::obj_type() const {
      return PRMObject::prm_type::INSTANCE;
    }

    template <typename GUM_SCALAR>
    INLINE PRMClass<GUM_SCALAR>& PRMInstance<GUM_SCALAR>::type() {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClass<GUM_SCALAR>& PRMInstance<GUM_SCALAR>::type() const {
      return *__type;
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInstance<GUM_SCALAR>::exists( NodeId id ) const {
      return __nodeIdMap.exists( id );
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInstance<GUM_SCALAR>::exists( const std::string& name ) const {
      return __type->exists( name ) && exists( __type->get( name ).id() );
    }

    template <typename GUM_SCALAR>
    INLINE PRMAttribute<GUM_SCALAR>& PRMInstance<GUM_SCALAR>::get( NodeId id ) {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no PRMAttribute<GUM_SCALAR> with the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const PRMAttribute<GUM_SCALAR>&
    PRMInstance<GUM_SCALAR>::get( NodeId id ) const {
      try {
        return *( __nodeIdMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no PRMAttribute<GUM_SCALAR> with the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMAttribute<GUM_SCALAR>&
    PRMInstance<GUM_SCALAR>::get( const std::string& name ) {
      try {
        return *( __nodeIdMap[type().get( name ).id()] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no PRMAttribute<GUM_SCALAR> with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const PRMAttribute<GUM_SCALAR>&
    PRMInstance<GUM_SCALAR>::get( const std::string& name ) const {
      try {
        return *( __nodeIdMap[type().get( name ).id()] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no PRMAttribute<GUM_SCALAR> with the given name" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMInstance<GUM_SCALAR>::__addReferingInstance( PRMSlotChain<GUM_SCALAR>* sc,
                                                    PRMInstance<GUM_SCALAR>*  i ) {
      NodeId      id = i->get( sc->lastElt().safeName() ).id();
      std::string name = sc->lastElt().safeName();

      try {
        i->__referenceMap[id]->insert( this );
        i->__referingAttr[id]->push_back(
            std::make_pair( this, sc->lastElt().safeName() ) );
      } catch ( NotFound& ) {
        i->__referenceMap.insert( id, new Set<PRMInstance<GUM_SCALAR>*>() );
        i->__referenceMap[id]->insert( this );
        i->__referingAttr.insert( id, new std::vector<pair>() );
        i->__referingAttr[id]->push_back(
            std::make_pair( this, sc->lastElt().safeName() ) );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Bijection<const DiscreteVariable*, const DiscreteVariable*>&
                 PRMInstance<GUM_SCALAR>::bijection() const {
      return __bijection;
    }

    template <typename GUM_SCALAR>
    INLINE const PRMInstance<GUM_SCALAR>&
    PRMInstance<GUM_SCALAR>::getInstance( NodeId id ) const {
      try {
        if ( __referenceMap[id]->size() > 0 ) {
          return **( __referenceMap[id]->begin() );
        } else {
          GUM_ERROR( UndefinedElement,
                     "no Instance associated with the given NodeId" );
        }
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ReferenceSlot<GUM_SCALAR> or SlotChain<GUM_SCALAR> "
                   "matches the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE const Set<PRMInstance<GUM_SCALAR>*>&
    PRMInstance<GUM_SCALAR>::getInstances( NodeId id ) const {
      try {
        return *( __referenceMap[id] );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound,
                   "no ReferenceSlot<GUM_SCALAR> or SlotChain<GUM_SCALAR> "
                   "matches the given NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::iterator
    PRMInstance<GUM_SCALAR>::begin() {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMInstance<GUM_SCALAR>::iterator&
    PRMInstance<GUM_SCALAR>::end() {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::const_iterator
    PRMInstance<GUM_SCALAR>::begin() const {
      return __nodeIdMap.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMInstance<GUM_SCALAR>::const_iterator&
    PRMInstance<GUM_SCALAR>::end() const {
      return __nodeIdMap.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::RefIterator
    PRMInstance<GUM_SCALAR>::begin( NodeId id ) {
      try {
        return PRMInstance<GUM_SCALAR>::RefIterator( *( __referenceMap[id] ) );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no referred instances from this NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::RefConstIterator
    PRMInstance<GUM_SCALAR>::begin( NodeId id ) const {
      try {
        return PRMInstance<GUM_SCALAR>::RefConstIterator(
            *( __referenceMap[id] ) );
      } catch ( NotFound& ) {
        GUM_ERROR( NotFound, "no referred instances from this NodeId" );
      }
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>::RefIterator::RefIterator(
        Set<PRMInstance<GUM_SCALAR>*>& set )
        : __set( set )
        , __iter( set.begin() ) {
      GUM_CONSTRUCTOR( PRMInstance<GUM_SCALAR>::RefIterator );
    }

    template <typename GUM_SCALAR>
    INLINE
    PRMInstance<GUM_SCALAR>::RefIterator::RefIterator( const RefIterator& from )
        : __set( const_cast<Set<PRMInstance<GUM_SCALAR>*>&>( from.__set ) )
        , __iter( from.__iter ) {
      GUM_CONS_CPY( PRMInstance<GUM_SCALAR>::RefIterator );
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>::RefIterator::~RefIterator() {
      GUM_DESTRUCTOR( PRMInstance<GUM_SCALAR>::RefIterator );
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::RefIterator&
    PRMInstance<GUM_SCALAR>::RefIterator::operator=( const RefIterator& from ) {
      __iter = from.__iter;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::RefIterator&
        PRMInstance<GUM_SCALAR>::RefIterator::operator++() {
      ++__iter;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInstance<GUM_SCALAR>::RefIterator::isEnd() const {
      return __iter == __set.end();
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInstance<GUM_SCALAR>::RefIterator::
    operator!=( const RefIterator& from ) const {
      return __iter != from.__iter;
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInstance<GUM_SCALAR>::RefIterator::
    operator==( const RefIterator& from ) const {
      return __iter == from.__iter;
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>& PRMInstance<GUM_SCALAR>::RefIterator::
    operator*() const {
      return **__iter;
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>* PRMInstance<GUM_SCALAR>::RefIterator::
    operator->() const {
      return *__iter;
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>::RefConstIterator::RefConstIterator(
        const Set<PRMInstance<GUM_SCALAR>*>& set )
        : __set( set )
        , __iter( set.begin() ) {
      GUM_CONSTRUCTOR( PRMInstance<GUM_SCALAR>::RefConstIterator );
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>::RefConstIterator::RefConstIterator(
        const RefConstIterator& from )
        : __set( from.__set )
        , __iter( from.__iter ) {
      GUM_CONS_CPY( PRMInstance<GUM_SCALAR>::RefConstIterator );
    }

    template <typename GUM_SCALAR>
    INLINE PRMInstance<GUM_SCALAR>::RefConstIterator::~RefConstIterator() {
      GUM_DESTRUCTOR( PRMInstance<GUM_SCALAR>::RefConstIterator );
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::RefConstIterator&
    PRMInstance<GUM_SCALAR>::RefConstIterator::
    operator=( const RefConstIterator& from ) {
      __iter = from.__iter;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::RefConstIterator&
        PRMInstance<GUM_SCALAR>::RefConstIterator::operator++() {
      ++__iter;
      return *this;
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInstance<GUM_SCALAR>::RefConstIterator::isEnd() const {
      return __iter == __set.end();
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInstance<GUM_SCALAR>::RefConstIterator::
    operator!=( const RefConstIterator& from ) const {
      return __iter != from.__iter;
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInstance<GUM_SCALAR>::RefConstIterator::
    operator==( const RefConstIterator& from ) const {
      return __iter == from.__iter;
    }

    template <typename GUM_SCALAR>
    INLINE const PRMInstance<GUM_SCALAR>&
                 PRMInstance<GUM_SCALAR>::RefConstIterator::operator*() const {
      return **__iter;
    }

    template <typename GUM_SCALAR>
    INLINE const PRMInstance<GUM_SCALAR>*
                 PRMInstance<GUM_SCALAR>::RefConstIterator::operator->() const {
      return *__iter;
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::InvRefIterator
    PRMInstance<GUM_SCALAR>::beginInvRef() {
      return __referingAttr.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMInstance<GUM_SCALAR>::InvRefIterator&
    PRMInstance<GUM_SCALAR>::endInvRef() {
      return __referingAttr.end();
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMInstance<GUM_SCALAR>::InvRefConstIterator
    PRMInstance<GUM_SCALAR>::beginInvRef() const {
      return __referingAttr.begin();
    }

    template <typename GUM_SCALAR>
    INLINE const typename PRMInstance<GUM_SCALAR>::InvRefConstIterator&
    PRMInstance<GUM_SCALAR>::endInvRef() const {
      return __referingAttr.end();
    }

    template <typename GUM_SCALAR>
    INLINE std::vector<std::pair<PRMInstance<GUM_SCALAR>*, std::string>>&
    PRMInstance<GUM_SCALAR>::getRefAttr( NodeId id ) {
      return *( __referingAttr[id] );
    }

    template <typename GUM_SCALAR>
    INLINE const std::vector<std::pair<PRMInstance<GUM_SCALAR>*, std::string>>&
    PRMInstance<GUM_SCALAR>::getRefAttr( NodeId id ) const {
      return *( __referingAttr[id] );
    }

    template <typename GUM_SCALAR>
    INLINE bool PRMInstance<GUM_SCALAR>::hasRefAttr( NodeId id ) const {
      return __referingAttr.exists( id ) && ( !__referingAttr[id]->empty() );
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMInstance<GUM_SCALAR>::__copyAttributeCPF( PRMAttribute<GUM_SCALAR>* attr ) {
      const auto& type_attr = static_cast<const PRMAttribute<GUM_SCALAR>&>(
          type().get( attr->safeName() ) );
      attr->copyCpf( bijection(), type_attr );
      GUM_ASSERT( attr->cpf().contains( attr->type().variable() ) );
    }

  } /* namespace prm */
} /* namespace gum */
