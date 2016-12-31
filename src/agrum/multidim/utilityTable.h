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
* @brief Headers of the UtilityTable class.
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES
* @author Jean-Philippe DUBUS
*/

#ifndef GUM_UTILITY_TABLE_H
#define GUM_UTILITY_TABLE_H

#include <agrum/config.h>

#include <agrum/multidim/multiDimArray.h>
#include <agrum/multidim/multiDimDecorator.h>

namespace gum {


  /**
   * @class UtilityTable utilityTable.h <agrum/multidim/utilityTable.h>
   * @ingroup multidim_group
   *
   * @brief Representation of a multi-dimensional utility table.
   * @tparam GUM_SCALAR The type of scalars stored in this multidimensional
   * table.
   */
  template <typename GUM_SCALAR>
  class UtilityTable : public MultiDimDecorator<GUM_SCALAR> {
    public:
    /**
     * @brief Default constructor.
     *
     * Use a MultiDimArray as its implementation.
     */
    UtilityTable();

    /**
     * @brief Constructor.
     * @param aContent The implementation of this utility table.
     */
    UtilityTable( MultiDimImplementation<GUM_SCALAR>* aContent );

    /**
     * @brief Class destructor.
     */
    virtual ~UtilityTable();

    /**
     * @brief Copy constructor.
     *
     * @param toCopy The UtilityTable to copy.
     */
    UtilityTable( const UtilityTable<GUM_SCALAR>& toCopy );

    /**
     * @brief Copy operator.
     *
     * @param toCopy The UtilityTable to copy.
     * @return This UtilityTable.
     */
    UtilityTable<GUM_SCALAR>& operator=( const UtilityTable<GUM_SCALAR>& toCopy );

    virtual UtilityTable<GUM_SCALAR>* newFactory() const;

    protected:
    virtual void _swap( const DiscreteVariable* x, const DiscreteVariable* y );
  };


  extern template class UtilityTable<float>;
  extern template class UtilityTable<double>;


} /* namespace gum */

#include <agrum/multidim/utilityTable_tpl.h>

#endif /* GUM_UTILITY_TABLE_H */
