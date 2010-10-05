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
namespace gum {

template<typename T_DATA>
MultiDimBijArray<T_DATA>::MultiDimBijArray(const MultiDimBijArray<T_DATA>& from):
  MultiDimWithOffset<T_DATA>(), __array(from.__array), __name(from.__name)
{
  GUM_CONS_CPY( MultiDimBijArray );
  for (MultiDimInterface::iterator iter = from.begin(); iter != from.end(); ++iter) {
    MultiDimWithOffset<T_DATA>::add(**iter);
  }
}

template<typename T_DATA>
MultiDimBijArray<T_DATA>::MultiDimBijArray(const VarBijection& bijection,
                                           const MultiDimArray<T_DATA>& array):
  MultiDimWithOffset<T_DATA>(), __array(array), __name("MultiDimBijArray")
{
  GUM_CONSTRUCTOR( MultiDimBijArray );
  for (MultiDimInterface::iterator iter = array.begin();
      iter != array.end(); ++iter)
  {
    try {
      MultiDimWithOffset<T_DATA>::add(*(bijection.first(*iter)));
    } catch (NotFound&) {
      MultiDimWithOffset<T_DATA>::add(**iter);
    }
  }
}

template<typename T_DATA>
MultiDimBijArray<T_DATA>::MultiDimBijArray(const VarBijection& bijection,
                                           const MultiDimBijArray<T_DATA>& array):
  MultiDimWithOffset<T_DATA>(), __array(array.__array), __name("MultiDimBijArray")
{
  GUM_CONSTRUCTOR( MultiDimBijArray );
  for (MultiDimInterface::iterator iter = array.begin(); iter != array.end(); ++iter) {
    try {
      MultiDimWithOffset<T_DATA>::add(*(bijection.first(*iter)));
    } catch (NotFound&) {
      MultiDimWithOffset<T_DATA>::add(**iter);
    }
  }
}

template<typename T_DATA> INLINE
MultiDimBijArray<T_DATA>::~MultiDimBijArray() {
  GUM_DESTRUCTOR( MultiDimBijArray );
}

template<typename T_DATA>
MultiDimBijArray<T_DATA>&
MultiDimBijArray<T_DATA>::operator=(const MultiDimBijArray<T_DATA>& from) {
  GUM_ERROR(OperationNotAllowed, "MultiDimBijArray are readonly.");
}

template<typename T_DATA> INLINE
MultiDimBijArray<T_DATA>*
MultiDimBijArray<T_DATA>::newFactory() const {
  return new MultiDimBijArray<T_DATA>(*this);
}

template<typename T_DATA> INLINE
const std::string&
MultiDimBijArray<T_DATA>::name() const { return __name; }

template<typename T_DATA> INLINE
void
MultiDimBijArray<T_DATA>::add( const DiscreteVariable& v ) {
  GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<T_DATA> are read only.");
}

template<typename T_DATA> INLINE
void
MultiDimBijArray<T_DATA>::erase( const DiscreteVariable& v ) {
  GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<T_DATA> are read only.");
}

template<typename T_DATA> INLINE
Size
MultiDimBijArray<T_DATA>::realSize() const {
  return (Size) 0;
}

template<typename T_DATA> INLINE
void
MultiDimBijArray<T_DATA>::fill( const T_DATA& d ) const {
  GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<T_DATA> are read only.");
}

template<typename T_DATA> INLINE
void
MultiDimBijArray<T_DATA>::_commitMultipleChanges( void ) {
  // Do nothing
}

template<typename T_DATA> INLINE
T_DATA
MultiDimBijArray<T_DATA>::get (const Instantiation &i) const
{
  if ( i.isMaster( this ) ) {
    return __array._values[this->_offsets[&i]];
  } else {
    return __array._values[this->_getOffs( i )];
  }
}

template<typename T_DATA> INLINE
void
MultiDimBijArray<T_DATA>::set(const Instantiation &i, const T_DATA &value) const
{
  GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<T_DATA> are read only.");
}

template<typename T_DATA> INLINE
void
MultiDimBijArray<T_DATA>::fillWith (const std::vector< T_DATA > &v) const
{
  GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<T_DATA> are read only.");
}

template<typename T_DATA> INLINE
T_DATA&
MultiDimBijArray<T_DATA>::_get (const Instantiation &i) const {
  GUM_ERROR(OperationNotAllowed, "MultiDimBijArray<T_DATA> are read only.");
}

template<typename T_DATA> INLINE
void
MultiDimBijArray<T_DATA>::_swap(const DiscreteVariable* x,
                                const DiscreteVariable* y)
{
  MultiDimImplementation<T_DATA>::_swap(x,y);
}

} // namespace gum
// ============================================================================
