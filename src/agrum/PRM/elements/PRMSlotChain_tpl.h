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
 * @brief Inline implementation of gum::PRMSlotChain<GUM_SCALAR>
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#include <agrum/PRM/elements/PRMAggregate.h>
#include <agrum/PRM/elements/PRMAttribute.h>
#include <agrum/PRM/elements/PRMReferenceSlot.h>
#include <agrum/PRM/elements/PRMSlotChain.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR>
    PRMSlotChain<GUM_SCALAR>::PRMSlotChain(
        const std::string&                            name,
        const Sequence<PRMClassElement<GUM_SCALAR>*>& chain )
        : PRMClassElement<GUM_SCALAR>( name )
        , __chain( new Sequence<PRMClassElement<GUM_SCALAR>*>( chain ) )
        , __isMultiple( false ) {
      GUM_CONSTRUCTOR( PRMSlotChain );

      if ( __chain->size() < 2 ) {
        GUM_ERROR( OperationNotAllowed,
                   "chain must contain at least two ClassElement" );
      }

      for ( Size i = 0; i < __chain->size() - 1; ++i ) {
        if ( __chain->atPos( i )->elt_type() !=
             PRMClassElement<GUM_SCALAR>::prm_refslot ) {
          GUM_ERROR( WrongClassElement, "illegal ClassElement in chain" );
        } else {
          __isMultiple =
              __isMultiple ||
              static_cast<PRMReferenceSlot<GUM_SCALAR>*>( __chain->atPos( i ) )
                  ->isArray();
        }
      }

      __copyLastElt();

      this->_safeName = PRMObject::LEFT_CAST() + lastElt().type().name() +
                        PRMObject::RIGHT_CAST() + name;
      this->lastElt().type().variable().setName( name );
    }

    // Parameters are inverse to prevent unwanted constructors calls (it
    // happened)
    template <typename GUM_SCALAR>
    PRMSlotChain<GUM_SCALAR>::PRMSlotChain(
        Sequence<PRMClassElement<GUM_SCALAR>*>* chain, const std::string& name )
        : PRMSlotChain( name, *chain ) {
      // No need to
      // GUM_CONSTRUCTOR(PRMSlotChain);

      // if (__chain->size() < 2) {
      //  GUM_ERROR(OperationNotAllowed,
      //            "chain must containt at least two PRMClassElement");
      //}

      // for (Size i = 0; i < __chain->size() - 1; ++i) {
      //  if (not(__chain->atPos(i)->elt_type() !=
      //          PRMClassElement<GUM_SCALAR>::prm_refslot)) {
      //    GUM_ERROR(WrongClassElement, "illegal PRMClassElement in chain");
      //  } else {
      //    __isMultiple =
      //        __isMultiple or
      //        static_cast<PRMReferenceSlot<GUM_SCALAR>
      //        *>(__chain->atPos(i))->isArray();
      //  }
      //}

      //__copyLastElt();
      // this->_safeName = PRMObject::LEFT_CAST() + lastElt().type().name() +
      // PRMObject::RIGHT_CAST() + name;
    }

    template <typename GUM_SCALAR>
    void PRMSlotChain<GUM_SCALAR>::__copyLastElt() {
      PRMClassElement<GUM_SCALAR>* new_elt = 0;

      switch ( __chain->back()->elt_type() ) {
        case PRMClassElement<GUM_SCALAR>::prm_attribute: {
          auto old_attr =
              static_cast<const PRMAttribute<GUM_SCALAR>*>( __chain->back() );

          Bijection<const DiscreteVariable*, const DiscreteVariable*> bij;
          for ( auto iter = old_attr->cpf().begin(); iter != old_attr->cpf().end();
                ++iter ) {
            if ( ( *iter ) != &( old_attr->type().variable() ) ) {

              bij.insert( *iter, *iter );
            }
          }

          new_elt = old_attr->copy( bij );
          break;
        }

        case PRMClassElement<GUM_SCALAR>::prm_aggregate: {
          const PRMAggregate<GUM_SCALAR>* c_agg =
              static_cast<const PRMAggregate<GUM_SCALAR>*>( __chain->back() );
          PRMAggregate<GUM_SCALAR>* agg = new PRMAggregate<GUM_SCALAR>(
              c_agg->name(), c_agg->agg_type(), c_agg->type(), c_agg->id() );
          new_elt = agg;
          break;
        }

        default: {
          GUM_ERROR( WrongClassElement, "last element of chain is not valid" );
        }
      }

      new_elt->setId( __chain->back()->id() );
      __chain->setAtPos( __chain->size() - 1, new_elt );
    }

    template <typename GUM_SCALAR>
    PRMSlotChain<GUM_SCALAR>::~PRMSlotChain() {
      GUM_DESTRUCTOR( PRMSlotChain );
      delete __chain->back();
      delete __chain;
    }

    template <typename GUM_SCALAR>
    PRMSlotChain<GUM_SCALAR>::PRMSlotChain(
        const PRMSlotChain<GUM_SCALAR>& source )
        : PRMClassElement<GUM_SCALAR>( source.name() )
        , __chain( new Sequence<PRMClassElement<GUM_SCALAR>*>( source.chain() ) )
        , __isMultiple( source.isMultiple() ) {
      GUM_CONS_CPY( PRMSlotChain );
      __copyLastElt();
    }

    template <typename GUM_SCALAR>
    PRMSlotChain<GUM_SCALAR>& PRMSlotChain<GUM_SCALAR>::
    operator=( const PRMSlotChain<GUM_SCALAR>& source ) {
      GUM_ERROR( FatalError,
                 "Illegal call to gum::SlotChain<GUM_SCALAR> copy constructor." );
    }

    template <typename GUM_SCALAR>
    INLINE typename PRMClassElement<GUM_SCALAR>::ClassElementType
    PRMSlotChain<GUM_SCALAR>::elt_type() const {
      return this->prm_slotchain;
    }

    template <typename GUM_SCALAR>
    INLINE PRMType<GUM_SCALAR>& PRMSlotChain<GUM_SCALAR>::type() {
      return __chain->back()->type();
    }

    template <typename GUM_SCALAR>
    INLINE const PRMType<GUM_SCALAR>& PRMSlotChain<GUM_SCALAR>::type() const {
      return __chain->back()->type();
    }

    template <typename GUM_SCALAR>
    INLINE const Potential<GUM_SCALAR>& PRMSlotChain<GUM_SCALAR>::cpf() const {
      return __chain->back()->cpf();
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElementContainer<GUM_SCALAR>& PRMSlotChain<GUM_SCALAR>::end() {
      return static_cast<PRMReferenceSlot<GUM_SCALAR>*>(
                 __chain->atPos( __chain->size() - 2 ) )
          ->slotType();
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElementContainer<GUM_SCALAR>&
                 PRMSlotChain<GUM_SCALAR>::end() const {
      return static_cast<PRMReferenceSlot<GUM_SCALAR>*>(
                 __chain->atPos( __chain->size() - 2 ) )
          ->slotType();
    }

    template <typename GUM_SCALAR>
    INLINE PRMClassElement<GUM_SCALAR>& PRMSlotChain<GUM_SCALAR>::lastElt() {
      return *( __chain->back() );
    }

    template <typename GUM_SCALAR>
    INLINE const PRMClassElement<GUM_SCALAR>&
                 PRMSlotChain<GUM_SCALAR>::lastElt() const {
      return *( __chain->back() );
    }

    template <typename GUM_SCALAR>
    INLINE Sequence<PRMClassElement<GUM_SCALAR>*>&
           PRMSlotChain<GUM_SCALAR>::chain() {
      return *__chain;
    }

    template <typename GUM_SCALAR>
    INLINE const Sequence<PRMClassElement<GUM_SCALAR>*>&
                 PRMSlotChain<GUM_SCALAR>::chain() const {
      return *__chain;
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMSlotChain<GUM_SCALAR>::addParent( const PRMClassElement<GUM_SCALAR>& elt ) {
    }

    template <typename GUM_SCALAR>
    INLINE void
    PRMSlotChain<GUM_SCALAR>::addChild( const PRMClassElement<GUM_SCALAR>& elt ) {}

    template <typename GUM_SCALAR>
    INLINE bool PRMSlotChain<GUM_SCALAR>::isMultiple() const {
      return __isMultiple;
    }

    template <typename GUM_SCALAR>
    INLINE PRMAttribute<GUM_SCALAR>*
           PRMSlotChain<GUM_SCALAR>::getCastDescendant() const {
      GUM_ERROR( OperationNotAllowed, "This is a slotchain" );
    }

  } /* namespace prm */
} /* namespace gum */
