/**
 *
 *   Copyright (c) 2005-2023  by Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 *   info_at_agrum_dot_org
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/**
 * @file
 * @brief Headers of the MultiDimWithOffset class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */

#ifndef GUM_MULTI_DIM_WITH_OFFSET_H
#define GUM_MULTI_DIM_WITH_OFFSET_H

#include <agrum/agrum.h>
#include <agrum/tools/multidim/implementations/multiDimImplementation.h>
#include <vector>

namespace gum {
  // ==========================================================================
  // ===                    GUM_MULTI_DIM_WITH_OFFSET === */
  // ==========================================================================
  /**
   * @class MultiDimWithOffset
   * @headerfile multiDimWithOffset.h <agrum/tools/multidim/multiDimWithOffset.h>
   * @brief Abstract class for Multidimensional matrix stored as an array in
   * memory and with an offset associated with each slave instantiation.
   *
   * @ingroup multidim_group
   * @tparam GUM_SCALAR The type of scalar stored in the multidimensional
   * table.
   */
  template < typename GUM_SCALAR >
  class MultiDimWithOffset: public MultiDimImplementation< GUM_SCALAR > {
    public:
    // =========================================================================
    /// @name Constructors, destructor and copy.
    // =========================================================================
    /// @{

    /**
     * @brief Class constructor.
     */
    MultiDimWithOffset();

    /**
     * @brief Copy constructor.
     *
     * The newly created matrix contains the same variables but no
     * instantiation is associated to it.
     *
     * @param from The MultiDimWithOffset to copy.
     */
    MultiDimWithOffset(const MultiDimWithOffset< GUM_SCALAR >& from);

    /**
     * @brief Class destrucor.
     */
    virtual ~MultiDimWithOffset();

    /**
     * Copy operator.
     *
     * @param from The multidimensional matrix we copy into this.
     */
    MultiDimWithOffset< GUM_SCALAR >& operator=(const MultiDimWithOffset< GUM_SCALAR >& from);

    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const = 0;

    /// @}
    // =========================================================================
    /// @name Accessors and modifiers
    // =========================================================================
    /// @{

    virtual void add(const DiscreteVariable& v);

    virtual void erase(const DiscreteVariable& v);

    virtual void fill(const GUM_SCALAR& d) const = 0;

    virtual std::string toString(const Instantiation* i) const;

    virtual void changeNotification(const Instantiation&          i,
                                    const DiscreteVariable* const var,
                                    Idx                           oldval,
                                    Idx                           newval);

    virtual void setChangeNotification(const Instantiation& i);

    virtual void setFirstNotification(const Instantiation& i);

    virtual void setLastNotification(const Instantiation& i);

    void setIncNotification(const Instantiation& i);

    void setDecNotification(const Instantiation& i);

    virtual bool registerSlave(Instantiation& i);

    virtual bool unregisterSlave(Instantiation& i);

    /// @}
    // =========================================================================
    /// @name Access to offset in MultiDimWithOffset
    // =========================================================================
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
    Size toOffset(const Instantiation& i) const;

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
    Instantiation& fromOffset(Instantiation& i, Size offset) const;

    ///@}

    protected:
    /**
     * @brief Compute the offset of a Instantiation.
     *
     * If the instantiation is not fully compatible with the MultiDimWithOffset,
     * no exception thrown
     * but 0 is assumed for dimensions not present in the instantiation.
     *
     * for instance : M<<a<<b<<c; with i=b:1|c:2|d:1 then M.getOffs_(i) give the
     * offset of a:0|b:1|c:2.
     *
     * @param i An instantiation for which the offset is computed.
     * @return The offset of i.
     */
    Size getOffs_(const Instantiation& i) const;

    /**
     * @brief The gaps between consecutive values of a given variable.
     *
     * For each variable, we keep track of the interval between two consecutive
     * values of the variable in vector values. Thus, shifting from one value
     * to the next one is equivalent to incrementing/decrementing by gaps the
     * current offset w.r.t. vector values.
     */
    HashTable< const DiscreteVariable*, Size > gaps_;

    /// The position in the array of each slave Instantiation.
    HashTable< const Instantiation*, Size > offsets_;

    /**
     * @brief For a given index of a value in the vector values, this method
     * computes the corresponding instantiation.
     *
     * @param result the result of this methods, we assume that the given
     * instantiation already contains all the variables contained in the
     * MultiDimArray (if V is the set of variables of this tab, V must be a
     * subset of variables in result or the exact set)
     * @param indice The index in the vector values
     */
    void computeInstantiationValue_(Instantiation& result, Size indice) const;

    virtual GUM_SCALAR& get_(const Instantiation& i) const = 0;
  };
} /* namespace gum */

#include <agrum/tools/multidim/implementations/multiDimWithOffset_tpl.h>

#endif /* GUM_MULTI_DIM_WITH_OFFSET_H */
