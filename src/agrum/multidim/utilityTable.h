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
/**
   @author Pierre-Henri WUILLEMIN et Christophe GONZALES
*/

#ifndef GUM_UTILITY_TABLE_H
#define GUM_UTILITY_TABLE_H

#include <agrum/core/utils.h>
#include <agrum/multidim/multiDimDecorator.h>


namespace gum {


  template<typename T_DATA>

  /** @class UtilityTable
   * representation of multi-dimensional utility (experimental)
   * @ingroup multidim_group 
   */
  class UtilityTable : public MultiDimDecorator<T_DATA> {
  public:
    UtilityTable( MultiDimImplementation<T_DATA>* aContent ) :
      MultiDimDecorator<T_DATA>( aContent ) {
      GUM_CONSTRUCTOR( UtilityTable );
    };

    ~UtilityTable() {
      GUM_DESTRUCTOR( UtilityTable );
    };

    /**
     * This method creates a clone of this object, withouth its content
     * (including variable), you must use this method if you want to ensure
     * that the generated object has the same type than the object containing
     * the called newFactory()
     * For example :
     *   MultiDimArray<double> y;
     *   MultiDimContainer<double>* x = y.newFactory();
     * Then x is a MultiDimArray<double>*
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
    virtual MultiDimContainer<T_DATA>* newFactory() const;


  };


} /* namespace gum */


#include <agrum/multidim/utilityTable.tcc>


#endif /* GUM_UTILITY_TABLE_H */

