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
   @author Pierre-Henri WUILLEMIN et Christophe GONZALES et Jean-Philippe DUBUS
*/

#ifndef GUM_UTILITY_TABLE_H
#define GUM_UTILITY_TABLE_H

#include <agrum/config.h>

#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/multiDimArray.h>

namespace gum {

  template <typename GUM_SCALAR>

  /** @class UtilityTable
   * representation of multi-dimensional utility (experimental)
   * @ingroup multidim_group
   */

  class UtilityTable : public MultiDimDecorator<GUM_SCALAR> {
    public:
    /**
     * Default constructor (with a multiDimArray<> for implementation)
     */
    UtilityTable();

    /**
     * Constructor
     * @param aContent implementation of utility table
     */
    UtilityTable(MultiDimImplementation<GUM_SCALAR> *aContent);

    /**
     * Destructor
     */
    virtual ~UtilityTable();

    /**
     * Copy constructor
     *
     * @param toCopy utility to copy (with the same variables and content)
     */
    UtilityTable(const UtilityTable<GUM_SCALAR> &toCopy);

    /**
     * Copy operator
     *
     * @param toCopy utility to copy (with the same variables and content)
     * @return the copy
     */
    UtilityTable<GUM_SCALAR> &operator=(const UtilityTable<GUM_SCALAR> &toCopy);

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
    virtual UtilityTable<GUM_SCALAR> *newFactory() const;

    /**
     * This method modifies the current table in order to store the
     * result of the addition p1 + p2)
     *
     * @param p1 an utility
     * @param p2 an utility
     */
    void sum(const UtilityTable<GUM_SCALAR> &p1, const UtilityTable<GUM_SCALAR> &p2);

    /**
     * This method modifies the current table A in order to compute the
     * result of the addition with B (A <- A + B)
     *
     * @param toAdd table to add in this table (B in the description)
     */
    void sumBy(const UtilityTable<GUM_SCALAR> &toAdd);

    /**
     * This method modifies the current table A in order to compute the
     * result of the addition with all tables in B (\f$ A \leftarrow A + \sum_{u \in
     *B} u \f$)
     *
     * @param utilitiesList tables to add in this table (B in the description)
     * @warning we assume that every table in B contains a subset of variables in A
     */
    void sumBy(const List<UtilityTable<GUM_SCALAR> *> &utilitiesList);

    /**
     * This method reduces the current table by maximization
     *
     * @param varList list of variables in the result
     * @return an utility table
     */
    UtilityTable<GUM_SCALAR>
    reduceBy(const List<const DiscreteVariable *> &varList) const;

    protected:
    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);

    private:
  };

} /* namespace gum */

#include <agrum/multidim/utilityTable.tcc>

#endif /* GUM_UTILITY_TABLE_H */
