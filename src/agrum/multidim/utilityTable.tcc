/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr   *
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
#ifndef DOXYGEN_SHOULD_SKIP_THIS


namespace gum {

  template <typename T_DATA> INLINE
  MultiDimContainer<T_DATA>* UtilityTable<T_DATA>::newFactory() const {
    return new UtilityTable<T_DATA>(static_cast<MultiDimImplementation<T_DATA>*>(this->getContent()->newFactory()));
  }


//   template<typename T_DATA> INLINE
//   T_DATA UtilityTable<T_DATA>::add( const T_DATA& a,const T_DATA& b ) const {
//     return ( a>b )?a:b;
//   }

//   template<typename T_DATA> INLINE
//   T_DATA UtilityTable<T_DATA>::multiply( const T_DATA& a,const T_DATA& b ) const {
//     return a+b;
//   }


} /* namespace gum */


#endif  // DOXYGEN_SHOULD_SKIP_THIS
