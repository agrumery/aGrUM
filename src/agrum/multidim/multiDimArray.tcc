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

#include <agrum/multidim/multiDimWithOffset.h>
#include <agrum/multidim/multiDimArray.h>

namespace gum {


/// Default constructor: creates an empty null dimensional matrix
template<typename T_DATA>
MultiDimArray<T_DATA>::MultiDimArray() : MultiDimWithOffset<T_DATA>() {
    // for debugging purposes
    GUM_CONSTRUCTOR( MultiDimArray );
}


/// copy constructor
template<typename T_DATA>
MultiDimArray<T_DATA>::MultiDimArray( const MultiDimArray<T_DATA>& src ) :
        MultiDimWithOffset<T_DATA> ( src ), _values( src._values ) {
    // for debugging purposes
    GUM_CONS_CPY( MultiDimArray );
}


/// destructor
template<typename T_DATA>
MultiDimArray<T_DATA>::~MultiDimArray() {
    // for debugging purposes
    GUM_DESTRUCTOR( MultiDimArray );
    // no need to unregister all slaves as it will be done by MultiDimWithOffset
}


/// data access operator
template<typename T_DATA>
INLINE
T_DATA& MultiDimArray<T_DATA>::_get( const Instantiation& i ) const {
    if ( i.isMaster( this ) ) {
        return _values[this->_offsets[&i]];
    } else {
        return _values[this->_getOffs( i )];
    }
}


/// add a new dimension, needed for updating the _offsets & _gaps
template<typename T_DATA>
INLINE
void MultiDimArray<T_DATA>::add( const DiscreteVariable& v ) {
    Size lg=MultiDimWithOffset<T_DATA>::domainSize();
    MultiDimWithOffset<T_DATA>::add( v );

    if ( ! this->_isInMultipleChangeMethod() ) {
        _values.resize( lg*v.domainSize() );
    }
}


/// removes a dimension, needed for updating the _offsets & _gaps
template<typename T_DATA>
INLINE
void MultiDimArray<T_DATA>::erase( const DiscreteVariable& v ) {
    Sequence<const DiscreteVariable *> variables=this->variablesSequence();
    Idx pos=variables[&v]; // throw a NotFound if necessary

    if ( variables.size() ==1 ) {
        if ( ! this->_isInMultipleChangeMethod() )
            _values.clear();
    } else {
        Size v_size = v.domainSize();
        Size size  = this->domainSize();
        // here, the variable does belong to the array.
        // => if pos = variables.size() - 1 then we just have to extract the
        // beginning of the array (actually the first gap of variable v)
        // if pos = 0, then copy every element whose index is a multiple of |v|
        // Assume now that pos != 0 and pos != variables.size() - 1, then
        // let w be the next variable in the set of variables of the array.
        // Then we must copy |gap(v)| elements every |gap(w)| elements

        if ( ! this->_isInMultipleChangeMethod() ) {
            if ( pos != variables.size() - 1 ) {
                Size gap_v =this->_gaps[variables[pos]];
                Size gap_w = this->_gaps[variables[pos+1]];

                for ( Idx i=0, j=0; i < size; i+=gap_w ) {
                    Idx last = i + gap_v;

                    for ( Idx k=i; k < last; ++k, ++j )
                        _values[j] = _values[k];
                }
            }

            // shrink _values
            _values.resize( size / v_size );
        }
    }

    MultiDimWithOffset<T_DATA>::erase( v );
}


template<typename T_DATA> INLINE
Size MultiDimArray<T_DATA>::realSize( void ) const {
    return this->domainSize();
}


/// synchronise content after MultipleChanges
template<typename T_DATA>
INLINE
void MultiDimArray<T_DATA>::_commitMultipleChanges( void ) {
    if ( MultiDimWithOffset<T_DATA>::domainSize()!=_values.size() ) {
        _values.resize( MultiDimWithOffset<T_DATA>::domainSize() );
    }
}


// fill the array with the arg
template<typename T_DATA>
INLINE
void MultiDimArray<T_DATA>::fill( const T_DATA& d )  const {
    if ( ! this->empty() ) std::fill( _values.begin(),_values.end(),d );
}


// virtual constructor
template<typename T_DATA> INLINE
MultiDimContainer<T_DATA>* MultiDimArray<T_DATA>::newFactory() const {
    return new MultiDimArray<T_DATA>;
}


/// returns the element stored in the multidimArray at a given offset
template<typename T_DATA> INLINE
const T_DATA& MultiDimArray<T_DATA>::unsafeGet ( const Idx offset ) const {
    return _values[offset];
}


template<typename T_DATA> INLINE
void MultiDimArray<T_DATA>::unsafeSet ( const Idx offset, const T_DATA& data ) {
    _values[offset] = data;
}


/// returns the element stored in the multidimArray at a given offset
template<typename T_DATA> INLINE
const T_DATA& MultiDimArray<T_DATA>::getByOffset ( const Idx offset ) const {
    if ( offset >= _values.size() )
        GUM_ERROR ( OutOfBounds, "offset too large" );
    return _values[offset];
}


template<typename T_DATA> INLINE
void
MultiDimArray<T_DATA>::setByOffset ( const Idx offset, const T_DATA& data ) {
    if ( offset >= _values.size() )
        GUM_ERROR ( OutOfBounds, "offset too large" );
    _values[offset] = data;
}


// returns the name of the implementation
template<typename T_DATA>
const std::string& MultiDimArray<T_DATA>::name () const {
    static const std::string str = "MultiDimArray";
    return str;
}


} /* namespace gum */

// kate: indent-mode cstyle; space-indent on; indent-width 0; 
