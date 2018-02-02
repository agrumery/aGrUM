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
 * @brief ContinuousVariable.
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#include <utility>

#include <agrum/agrum.h>
#include <agrum/variables/continuousVariable.h>

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  
  /// Default constructor
  template <typename T_VAL>
  INLINE
  ContinuousVariable<T_VAL>::ContinuousVariable ( const std::string& aName,
                                                  const std::string& aDesc,
                                                  T_VAL lower_bound,
                                                  T_VAL upper_bound )
    : Variable(aName, aDesc)
    , __lower_bound ( lower_bound )
    , __upper_bound ( upper_bound ) {
    if ( __lower_bound > __upper_bound ) {
      std::swap ( __upper_bound, __lower_bound );
    }
    GUM_CONSTRUCTOR( ContinuousVariable );
  }

  
  /// Copy Constructor.
  template <typename T_VAL>
  INLINE ContinuousVariable<T_VAL>::ContinuousVariable (
       const ContinuousVariable<T_VAL>& from )
    : Variable ( from )
    , __lower_bound ( from.__lower_bound )
    , __upper_bound ( from.__upper_bound ) {
    GUM_CONS_CPY( ContinuousVariable );
  }


  /// generalized copy constructor
  template <typename T_VAL>
  template <typename TX_VAL>
  INLINE ContinuousVariable<T_VAL>::ContinuousVariable (
       const ContinuousVariable<TX_VAL>& from )
    : Variable ( from )
    , __lower_bound ( from.__lower_bound )
    , __upper_bound ( from.__upper_bound ) {
    GUM_CONS_CPY( ContinuousVariable );
  }

    
  /// move constructor
  template <typename T_VAL>
  INLINE ContinuousVariable<T_VAL>::ContinuousVariable (
       ContinuousVariable<T_VAL>&& from )
    : Variable ( std::move ( from ) )
    , __lower_bound ( from.__lower_bound )
    , __upper_bound ( from.__upper_bound ) {
    GUM_CONS_MOV( ContinuousVariable );
  }
    

  /// destructor
  template <typename T_VAL>
  INLINE ContinuousVariable<T_VAL>::~ContinuousVariable () {
    GUM_DESTRUCTOR( ContinuousVariable );
  }
  

  /// Copy Factory.
  template <typename T_VAL>
  INLINE ContinuousVariable<T_VAL>* ContinuousVariable<T_VAL>::clone() const {
    return new ContinuousVariable<T_VAL> ( *this );
  }

   
  /// copy operator
  template <typename T_VAL>
  INLINE ContinuousVariable<T_VAL>&
  ContinuousVariable<T_VAL>::operator= (const ContinuousVariable<T_VAL>& from ) {
    Variable::operator= ( from );
    __lower_bound = from.__lower_bound;
    __upper_bound = from.__upper_bound;
    return *this;
  }


  /// generalized copy operator
  template <typename T_VAL>
  template <typename TX_VAL>
  INLINE ContinuousVariable<T_VAL>&
  ContinuousVariable<T_VAL>::operator= (const ContinuousVariable<TX_VAL>& from ) {
    Variable::operator= ( from );
    __lower_bound = from.__lower_bound;
    __upper_bound = from.__upper_bound;
    return *this;
  }

    
  /// move operator
  template <typename T_VAL>
  INLINE ContinuousVariable<T_VAL>&
  ContinuousVariable<T_VAL>::operator= (ContinuousVariable<T_VAL>&& from ) {
    Variable::operator= ( std::move ( from ) );
    __lower_bound = from.__lower_bound;
    __upper_bound = from.__upper_bound;
    return *this;
  }

    
  /// returns the T_VAL corresponding to a string (unspecialized version)
  template <typename T_VAL>
  INLINE T_VAL
  ContinuousVariable<T_VAL>::operator[] ( const std::string& str ) const {
    std::istringstream stream ( str );
    T_VAL value;
    stream >> value;

    if ( belongs ( value ) )
      return value;
    else 
      GUM_ERROR ( OutOfBounds,
                  "the value does not delong to the domain of the variable" );
  }
  
  
  /// returns the lower bound of the domain of the variable
  template <typename T_VAL>
  INLINE T_VAL ContinuousVariable<T_VAL>::lowerBound () const {
    return __lower_bound;
  }
      

  /// returns the upper bound of the domain of the variable
  template <typename T_VAL>
  INLINE T_VAL ContinuousVariable<T_VAL>::upperBound () const {
    return __upper_bound;
  }
      

  /// updates the lower bound of the domain of the variable
  template <typename T_VAL>
  INLINE void
  ContinuousVariable<T_VAL>::setLowerBound ( const T_VAL& new_bound ) {
    if ( new_bound <= __upper_bound )
      __lower_bound = new_bound;
    else
      GUM_ERROR ( OutOfBounds,
                  "the new lower bound would be higher than the upper bound" ); 
  }

  
  /// updates the lower bound of the domain of the variable
  template <typename T_VAL>
  INLINE void
  ContinuousVariable<T_VAL>::setUpperBound ( const T_VAL& new_bound ) {
    if ( new_bound >= __lower_bound )
      __upper_bound = new_bound;
    else
      GUM_ERROR ( OutOfBounds,
                  "the new upper bound would be lower than the lower bound" ); 
  }


  /// returns the type of the variable
  template <typename T_VAL>
  INLINE VarType ContinuousVariable<T_VAL>::varType(void) const {
    return VarType::Continuous;
  }


  /// returns a string containing the value of the variable passed in argument
  template <typename T_VAL>
  INLINE std::string
  ContinuousVariable<T_VAL>::label ( const T_VAL& value ) const {
    if ( belongs ( value ) )
      return std::to_string ( value );
    GUM_ERROR ( OutOfBounds,
                "the value does not belong to the domain of the variable" );
  }
  

  /// Returns true if the param belongs to the domain of the variable
  template <typename T_VAL>
  INLINE bool ContinuousVariable<T_VAL>::belongs ( const T_VAL& value ) const {
    return ( value >= __lower_bound ) && ( value <= __upper_bound );
  }
    

  /// returns the domain of the variable as a string
  template <typename T_VAL>
  INLINE std::string ContinuousVariable<T_VAL>::domain() const {
    std::ostringstream stream;
    stream << '[' << __lower_bound << ';' << __upper_bound << ']';
    return stream.str ();
  }
  

  /// string version of *this
  template <typename T_VAL>
  INLINE const std::string
  ContinuousVariable<T_VAL>::toString() const {
    std::string str ( this->name() );
    str += domain();
    return str;
  }

  
  /// string version of *this using description attribute instead of name.
  template <typename T_VAL>
  INLINE const std::string
  ContinuousVariable<T_VAL>::toStringWithDescription() const {
    std::string str ( this->description() );
    str += domain();
    return str;
  }


  /// for friendly displaying the content of the variable
  template <typename T_VAL>
  std::ostream&
  operator<<(std::ostream& stream, const ContinuousVariable<T_VAL>& var ) {
    return stream << var.toString ();
  }
  

} /* namespace gum */


#endif /* DOXYGEN_SHOULD_SKIP_THIS */
