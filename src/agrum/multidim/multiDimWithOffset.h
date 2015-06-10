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
 * @brief Headers of the gum::MultiDimWithOffset class.
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */

#ifndef GUM_MULTI_DIM_WITH_OFFSET_H
#define GUM_MULTI_DIM_WITH_OFFSET_H

#include <vector>

#include <agrum/config.h>

#include <agrum/multidim/multiDimImplementation.h>

namespace gum {
  /* ============================================================================ */
  /* ============================================================================ */
  /* ===                    GUM_MULTI_DIM_WITH_OFFSET                         === */
  /* ============================================================================ */
  /* ============================================================================ */
  /**
   * @class MultiDimWithOffset
   * @brief Abstract class for Multidimensional matrix stored as an array in memory
   *        and with an offset associated with each slave instantiation.
   *
   * @ingroup multidim_group
   */
  template <typename GUM_SCALAR>

  class MultiDimWithOffset : public MultiDimImplementation<GUM_SCALAR> {
    public:
    MultiDimWithOffset();

    /**
     * @brief Copy constructor.
     *
     * The newly created matrix contains the same variables but no instantiation
     * is associated to it.
     * @param from The multidimensional matrix we copy into this.
     */
    MultiDimWithOffset(const MultiDimWithOffset<GUM_SCALAR> &from);

    /**
     * @brief Destrucor.
     *
     * Note that, when the multidimensional array is removed from memory, its
     * variables are not removed as well.
     */
    virtual ~MultiDimWithOffset();

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################

    /// @{
    /**
     * Copy operator.
     *
     * @param from The multidimensional matrix we copy into this.
     */
    MultiDimWithOffset<GUM_SCALAR> &
    operator=(const MultiDimWithOffset<GUM_SCALAR> &from);

    /// @}

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
    virtual MultiDimContainer<GUM_SCALAR> *newFactory() const = 0;

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################

    /// @{

    /**
     * Add a new dimension, needed for updating the offsets & gaps.
     *
     * See also gum::MultiDimInterface::add().
     */
    virtual void add(const DiscreteVariable &v);

    /**
     * Removes a dimension, needed for updating the offsets & gaps
     *
     * See also gum::MultiDimInterface::erase().
     */
    virtual void erase(const DiscreteVariable &v);

    /**
     * Fill the table with the scalar value d.
     *
     * @param d A scalar which is used to fill the multidimensional table.
     */
    virtual void fill(const GUM_SCALAR &d) const = 0;

    /// Returns string representating of internal data about i in this.
    virtual const std::string toString(const Instantiation *i) const;

    /// @}

    // ############################################################################
    /// @name Slave management and extension due to slave management
    // ############################################################################

    /// @{

    /// See gum::MultiDimAdressable::changeNotification().
    virtual void changeNotification(Instantiation &i,
                                    const DiscreteVariable *const var,
                                    const Idx &oldval, const Idx &newval);

    /// See gum::MultiDimAdressable::setChangeNotification().
    virtual void setChangeNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::setFirstNotification().
    virtual void setFirstNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::setLastNotification().
    virtual void setLastNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::setChangeNotification().
    void setIncNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::setDecNotification().
    void setDecNotification(Instantiation &i);

    /// See gum::MultiDimAdressable::registerSlave().
    virtual bool registerSlave(Instantiation &i);

    /// See gum::MultiDimAdressable::unregisterSlave().
    virtual bool unregisterSlave(Instantiation &i);

    /// @}

    // ############################################################################
    /// @name access to offset in MultiDimWithOffset
    // ############################################################################
    ///@{

    ///
    /**
     * @brief Compute offset from an Instantiation (in this array).
     *
     * If the instantiation is not fully compatible with the MultiDimWithOffset,
     * no exception will be thrown
     * but 0 is assumed for dimensions not present in the instantiation.
     * for instance : M<<a<<b<<c; with i=b:1|c:2|d:1 then M.toOffset(i) give the
     * offset of a:0|b:1|c:2.
     */
    Size toOffset(const Instantiation &i) const;

    /**
     * @brief Set the Instantiation to the values corresponding to the offset
     *        (in this array).
     *
     * We assume that the given instantiation already contains all the variables
     * contained in the multidimarray (if V is the set of variables of this tab,
     * V must be a subset of variables in result or the exact set).
     *
     * @param i The instantiation i modified giving the offset.
     * @param offset The offset used to compute the value of i.
     * @return Returns a reference over i.
     */
    Instantiation &fromOffset(Instantiation &i, Size offset) const;

    ///@}

    protected:
    /**
     * @brief Compute the offset of a Instantiation.
     *
     * If the instantiation is not fully compatible with the MultiDimWithOffset,
     * no exception thrown
     * but 0 is assumed for dimensions not present in the instantiation.
     *
     * for instance : M<<a<<b<<c; with i=b:1|c:2|d:1 then M._getOffs(i) give the
     * offset of a:0|b:1|c:2.
     *
     * @param i An instantiation for which the offset is computed.
     * @return The offset of i.
     */
    Size _getOffs(const Instantiation &i) const;

    /**
     * @brief The gaps between consecutive values of a given variable.
     *
     * For each variable, we keep track of the interval between two consecutive
     * values of the variable in vector values. Thus, shifting from one value to
     * the next one is equivalent to incrementing/decrementing by gaps the current
     * offset w.r.t. vector values.
     */
    HashTable<const DiscreteVariable *, Size> _gaps;

    /// The position in the array of each slave Instantiation.
    HashTable<const Instantiation *, Size> _offsets;

    /** @brief For a given index of a value in the vector values, this method
     *         computes the corresponding instantiation.
     *
     * @param result the result of this methods, we assume that the given
     *               instantiation already contains all the variables
     *               contained in the gum::MultiDimArray (if V is the set of
     *variables
     *               of this tab, V must be a subset of variables in
     *               result or the exact set)
     * @param indice The index in the vector values
     */
    void _computeInstantiationValue(Instantiation &result, Size indice) const;

    /// data access operator
    virtual GUM_SCALAR &_get(const Instantiation &i) const = 0;
  };
} /* namespace gum */

#include <agrum/multidim/multiDimWithOffset.tcc>

#endif /* GUM_MULTI_DIM_WITH_OFFSET_H */
