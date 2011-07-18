/***************************************************************************
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
 * @brief Implementation of the MultiDimBucket class.
 *
 * @author Lionel Torti
 */
// ============================================================================
#include <agrum/multidim/multiDimBucket.h>
// ============================================================================
namespace gum {

  template<typename T_DATA>
  MultiDimBucket<T_DATA>::MultiDimBucket( Size bufferSize ):
      MultiDimReadOnly<T_DATA>(), __bufferSize( bufferSize ),
      __bucket( 0 ), __changed( false ), __name( "MultiDimBucket" ) {
    GUM_CONSTRUCTOR( MultiDimBucket );
  }

  template<typename T_DATA>
  MultiDimBucket<T_DATA>::MultiDimBucket( const MultiDimBucket<T_DATA>& source ):
      MultiDimReadOnly<T_DATA>( source ), __bufferSize( source.__bufferSize ),
      __bucket( 0 ), __multiDims( source.__multiDims ),
      __allVariables( source.__allVariables ), __allVarsInst( source.__allVarsInst ),
      __changed( source.__changed ), __name( "MultiDimBucket" ) {
    GUM_CONS_CPY( MultiDimBucket );
  }

  template<typename T_DATA>
  MultiDimBucket<T_DATA>::~MultiDimBucket() {
    GUM_DESTRUCTOR( MultiDimBucket );
    typedef Bijection<Instantiation*, Instantiation*>::iterator BiIter;
    for ( BiIter iter = __instantiations.begin(); iter != __instantiations.end(); ++iter ) {
      delete iter.second();
    }
    if ( __bucket ) {
      delete __bucket;
    }
    for ( HashTableIterator<const MultiDimContainer<T_DATA>*, Instantiation* > iter =
            __multiDims.begin(); iter != __multiDims.end(); ++iter ) {
      delete *iter;
    }
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::add( const MultiDimContainer<T_DATA>& impl ) {
    this->add( &impl );
  }

  template<typename T_DATA>
  void
  MultiDimBucket<T_DATA>::add( const MultiDimContainer<T_DATA>* impl ) {
    __multiDims.insert( impl, new Instantiation( *impl ) );
    if ( ! MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod() ) {
      for ( MultiDimInterface::iterator iter = impl->begin(); iter != impl->end(); ++iter ) {
        __addVariable( *iter );
      }
    }
    __changed = true;
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::erase( const MultiDimContainer<T_DATA>& impl ) {
    this->erase( &impl );
  }

  template<typename T_DATA>
  void
  MultiDimBucket<T_DATA>::erase( const MultiDimContainer<T_DATA>* impl ) {
    try {
      delete __multiDims[impl];
      __multiDims.erase( impl );
      if ( ! MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod() ) {
        for ( MultiDimInterface::iterator iter = impl->begin(); iter != impl->end(); ++iter ) {
          __eraseVariable( *iter );
        }
      }
      __changed = true;
    } catch ( NotFound& ) {
      // Do nothing
    }
  }

  template<typename T_DATA> INLINE
  bool
  MultiDimBucket<T_DATA>::contains( const MultiDimContainer<T_DATA>& impl ) const {
    return __multiDims.exists( &impl );
  }

  template<typename T_DATA> INLINE
  const Set<const DiscreteVariable*>&
  MultiDimBucket<T_DATA>::allVariables() const {
    return __allVariables;
  }

  template<typename T_DATA> INLINE
  Size
  MultiDimBucket<T_DATA>::bucketSize() const {
    return __multiDims.size();
  }


  template<typename T_DATA> INLINE
  bool
  MultiDimBucket<T_DATA>::isBucketEmpty() const {
    return __multiDims.empty();
  }

  template<typename T_DATA> INLINE
  bool
  MultiDimBucket<T_DATA>::bucketChanged() const {
    return __changed;
  }

  template<typename T_DATA> INLINE
  Size
  MultiDimBucket<T_DATA>::bufferSize() const {
    return __bufferSize;
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::setBufferSize( Size ammount ) {
    __bufferSize = ammount;
    if (( this->domainSize() > __bufferSize ) and( __bucket != 0 ) ) {
      __eraseBuffer();
    } else if ( __bucket == 0 ) {
      __initializeBuffer();
    }
  }

  template<typename T_DATA>
  void
  MultiDimBucket<T_DATA>::compute( bool force ) const {
    if (( __bucket ) and( __changed or force ) ) {
      Instantiation values( *__bucket );
      for ( values.setFirst(); ! values.end(); values.inc() ) {
        __bucket->set( values, __computeValue( values ) );
      }
    } else if (( __bucket == 0 ) and __changed ) {
      __slavesValue.clear();
      __changed = false;
    }
    __changed = false;
  }


  template<typename T_DATA>
  const std::string&
  MultiDimBucket<T_DATA>::name() const { return __name; }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::add( const DiscreteVariable &v ) {
    MultiDimImplementation<T_DATA>::add( v );
    if ( not MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod() ) {
      if ( this->domainSize() <= __bufferSize ) {
        if ( __bucket )
          __bucket->add( v );
        else
          __initializeBuffer();
      } else if ( __bucket ) {
        __eraseBuffer();
      }
    }
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::erase( const DiscreteVariable &v ) {
    MultiDimImplementation<T_DATA>::erase( v );
    if (( not MultiDimImplementation<T_DATA>::_isInMultipleChangeMethod() ) and
        ( this->domainSize() <= __bufferSize ) ) {
      if ( __bucket ) {
        __bucket->erase( v );
      } else {
        __initializeBuffer();
      }
    }
  }

  template<typename T_DATA> INLINE
  Size
  MultiDimBucket<T_DATA>::realSize() const {
    return ( __bucket )?__bucket->realSize():( Size ) 0;
  }

  template<typename T_DATA> INLINE
  bool
  MultiDimBucket<T_DATA>::contains( const DiscreteVariable &v ) const {
    return MultiDimImplementation<T_DATA>::contains( v );
  }

  template<typename T_DATA> INLINE
  T_DATA
  MultiDimBucket<T_DATA>::get( const Instantiation &i ) const {
    compute();
    if ( __bucket ) {
      try {
        return __bucket->get( *( __instantiations.second( const_cast<Instantiation*>( &i ) ) ) );
      } catch ( NotFound& ) {
        return __bucket->get( i );
      }
    } else if ( i.isMaster( this ) ) {
      if ( ! __slavesValue.exists( &i ) ) {
        __slavesValue.insert( &i, __computeValue( i ) );
      }
      return __slavesValue[&i];
    } else {
      return __computeValue( i );
    }
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::changeNotification( Instantiation &i,
      const DiscreteVariable *const var,
      const Idx &oldval, const Idx &newval ) {
    if ( __bucket ) {
      try {
        __bucket->changeNotification( *( __instantiations ).second( &i ), var, oldval, newval );
      } catch ( NotFound& ) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase( &i );
    }
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::setFirstNotification( Instantiation &i ) {
    if ( __bucket ) {
      try {
        __bucket->setFirstNotification( *( __instantiations ).second( &i ) );
      } catch ( NotFound& ) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase( &i );
    }
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::setLastNotification( Instantiation &i ) {
    if ( __bucket ) {
      try {
        __bucket->setLastNotification( *( __instantiations ).second( &i ) );
      } catch ( NotFound& ) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase( &i );
    }
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::setIncNotification( Instantiation &i ) {
    if ( __bucket ) {
      try {
        __bucket->setIncNotification( *( __instantiations.second( &i ) ) );
      } catch ( NotFound& ) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase( &i );
    }
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::setDecNotification( Instantiation &i ) {
    if ( __bucket ) {
      try {
        __bucket->setDecNotification( *( __instantiations.second( &i ) ) );
      } catch ( NotFound& ) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase( &i );
    }
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::setChangeNotification( Instantiation &i ) {
    if ( __bucket ) {
      try {
        __bucket->setChangeNotification( *( __instantiations.second( &i ) ) );
      } catch ( NotFound& ) {
        // Then i is not a slave of this
      }
    } else {
      __slavesValue.erase( &i );
    }
  }

  template<typename T_DATA> INLINE
  bool
  MultiDimBucket<T_DATA>::registerSlave( Instantiation &i ) {
    if ( __bucket ) {
      try {
        __instantiations.insert( &i, new Instantiation( *__bucket ) );
      } catch ( DuplicateElement& ) {
        return false;
      }
    }
    return MultiDimImplementation<T_DATA>::registerSlave( i );
  }

  template<typename T_DATA> INLINE
  bool
  MultiDimBucket<T_DATA>::unregisterSlave( Instantiation &i ) {
    MultiDimReadOnly<T_DATA>::unregisterSlave( i );
    if ( __bucket ) {
      try {
        delete __instantiations.second( &i );
        __instantiations.eraseFirst( &i );
        return true;
      } catch ( NotFound& ) {
        return false;
      }
    } else {
      if ( __slavesValue.exists( &i ) ) {
        __slavesValue.erase( &i );
        return true;
      } else {
        return false;
      }
    }
  }

  template<typename T_DATA> INLINE
  MultiDimAdressable&
  MultiDimBucket<T_DATA>::getMasterRef( void ) {
    if ( __bucket ) {
      return *__bucket;
    } else {
      return *this;
    }
  }

  template<typename T_DATA> INLINE
  const MultiDimAdressable&
  MultiDimBucket<T_DATA>::getMasterRef( void ) const {
    if ( __bucket ) {
      return *__bucket;
    } else {
      return *this;
    }
  }

  template<typename T_DATA> INLINE
  const std::string
  MultiDimBucket<T_DATA>::toString( const Instantiation *i ) const {
    std::stringstream sBuff;
    sBuff << ( *i ) << " = " << get( *i );
    return sBuff.str();
  }

  template<typename T_DATA>
  void
  MultiDimBucket<T_DATA>::_commitMultipleChanges() {
    MultiDimImplementation<T_DATA>::_commitMultipleChanges();
    if ( this->domainSize() <= __bufferSize ) {
      __initializeBuffer();
    } else {
      __eraseBuffer();
    }
    __allVariables.clear();
    while ( not __allVarsInst.empty() ) {
      __allVarsInst.erase( **( __allVarsInst.variablesSequence().begin() ) );
    }
    for ( HashTableIterator<const MultiDimContainer<T_DATA>*, Instantiation*> iter =
            __multiDims.begin(); iter != __multiDims.end(); ++iter ) {
      for ( MultiDimInterface::iterator jter = iter.key()->begin();
            jter != iter.key()->end(); ++jter ) {
        __addVariable( *jter );
      }
    }
    __changed = true;
  }

  template<typename T_DATA> INLINE
  T_DATA&
  MultiDimBucket<T_DATA>::_get( const Instantiation &i ) const {
    GUM_ERROR( OperationNotAllowed, "a MultiDimBucket is a read only MultiDim" );
  }

  template<typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::__addVariable( const DiscreteVariable* var ) {
    try {
      __allVariables.insert( var );
      __allVarsInst.add( *var );
    } catch ( DuplicateElement& e ) {
      // Nothing to do then!
    }
  }

  template<typename T_DATA>
  void
  MultiDimBucket<T_DATA>::__eraseVariable( const DiscreteVariable* var ) {
    bool found = false;
    for ( HashTableIterator<const MultiDimContainer<T_DATA>*, Instantiation*> iter =
            __multiDims.begin(); iter != __multiDims.end(); ++iter ) {
      if ( iter.key()->contains( *var ) ) {
        found = true;
        break;
      }
    }
    // No one use it, we can safely remove it
    if ( ! found ) {
      __allVariables.erase( var );
      __allVarsInst.erase( *var );
    }
  }

  template<typename T_DATA>
  void
  MultiDimBucket<T_DATA>::__initializeBuffer() {
    if ( __bucket ) {
      typedef Bijection<Instantiation*, Instantiation*>::iterator BiIter;
      for ( BiIter iter = __instantiations.begin(); iter != __instantiations.end(); ++iter ) {
        delete iter.second();
      }
      __instantiations.clear();
      delete __bucket;
      __bucket = 0;
    }
    // Creating the table.
    __bucket = new MultiDimArray<T_DATA>();
    for ( MultiDimInterface::iterator iter = this->begin(); iter != this->end(); ++iter ) {
      __bucket->add( **iter );
    }
    if ( not this->_slaves().empty() ) {
      for ( List<Instantiation*>::const_iterator iter = this->_slaves().begin(); iter != this->_slaves().end(); ++iter ) {
        __instantiations.insert( *iter, new Instantiation( *__bucket ) );
      }
    }
    __changed = true;
  }

  template<typename T_DATA>
  void
  MultiDimBucket<T_DATA>::__eraseBuffer() {
    if ( __bucket ) {
      typedef Bijection<Instantiation*, Instantiation*>::iterator BiIter;
      for ( BiIter iter = __instantiations.begin(); iter != __instantiations.end(); ++iter ) {
        delete iter.second();
      }
      __instantiations.clear();
      delete __bucket;
      __bucket = 0;
    }
  }

  template<typename T_DATA>
  T_DATA
  MultiDimBucket<T_DATA>::__computeValue( const Instantiation& value ) const {
    try {
      T_DATA sum = ( T_DATA ) 0;
      T_DATA current;
      __allVarsInst.chgValIn( value );
      for ( __allVarsInst.setFirstOut( value ); not __allVarsInst.end();
            __allVarsInst.incOut( value ) ) {
        current = ( T_DATA ) 1;
        for ( HashTableIterator<const MultiDimContainer<T_DATA>*, Instantiation*> iter =
                __multiDims.begin(); iter != __multiDims.end(); ++iter ) {
          ( *iter )->chgValIn( __allVarsInst );
          current *= iter.key()->get( **iter );
        }
        sum += current;
      }
      return sum;
    } catch ( NotFound& e ) {
      std::cerr << std::endl << e.getContent() << std::endl;
      // This happens if the bucket is empty.
      GUM_ERROR( SizeError, "This MultiDimBucket is empty." );
    }
  }

  template <typename T_DATA> INLINE
  MultiDimContainer<T_DATA>*
  MultiDimBucket<T_DATA>::newFactory() const {
    return new MultiDimBucket<T_DATA>;
  }

  template <typename T_DATA> INLINE
  const MultiDimArray<T_DATA>&
  MultiDimBucket<T_DATA>::bucket() const {
    if ( __bucket ) {
      return *__bucket;
    } else {
      GUM_ERROR( OperationNotAllowed, "bucket not used." );
    }
  }

  template <typename T_DATA> INLINE
  void
  MultiDimBucket<T_DATA>::_swap( const DiscreteVariable* x, const DiscreteVariable* y ) {
    MultiDimImplementation<T_DATA>::_swap( x, y );
    typedef Bijection<Instantiation*, Instantiation*>::iterator Iter;
    for ( Iter iter = __instantiations.begin(); iter != __instantiations.end(); ++iter ) {
      iter.first()->swap( *x, *y );
      iter.second()->swap( *x, *y );
    }
    if ( __bucket ) __bucket->swap( *x, *y );
    __allVariables.erase( x );
    __allVariables.insert( y );
    __allVarsInst.swap( *x, *y );
  }

  template <typename T_DATA> INLINE
  const HashTable<const MultiDimContainer<T_DATA>*, Instantiation*>&
  MultiDimBucket<T_DATA>::multidims() const {
    return __multiDims;
  }

} /* namespace gum */
// ============================================================================
