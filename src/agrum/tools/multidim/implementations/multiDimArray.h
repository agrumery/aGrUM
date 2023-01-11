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
 * @brief Header of the MultiDimArray class.
 *
 * @author Pierre-Henri WUILLEMIN(_at_LIP6) & Christophe GONZALES(_at_AMU)
 */
#ifndef GUM_MULTI_DIM_ARRAY_H
#define GUM_MULTI_DIM_ARRAY_H

#include <agrum/agrum.h>
#include <agrum/tools/multidim/implementations/multiDimWithOffset.h>
#include <vector>

namespace gum {

  template < typename GUM_SCALAR >
  class MultiDimBijArray;

  // ===========================================================================
  // ===                          GUM_MULTI_DIM_ARRAY                        ===
  // ===========================================================================
  /**
   * @class MultiDimArray
   * @headerfile multiDimArray.h <agrum/tools/multidim/multiDimArray.h>
   * @ingroup multidim_group
   *
   * @brief Multidimensional matrix stored as an array in memory.
   * @tparam GUM_SCALAR The type of the scalars stroed in the multidimensional
   * table.
   */
  template < typename GUM_SCALAR >
  class MultiDimArray: public MultiDimWithOffset< GUM_SCALAR > {
    public:
    template < typename GUM_SCALARBIS >
    friend class MultiDimBijArray;
    // =========================================================================
    /// @name Constructors, destructors and copy
    // =========================================================================
    /// @{

    /**
     * @brief Default constructor.
     *
     * Creates an empty null dimensional matrix
     */
    MultiDimArray();

    /**
     * @brief Copy constructor.
     *
     * The newly created matrix contains the same variables and the same values
     * as src, but no instantiation is associated to it.
     *
     * @param src The MultiDimArray to copy.
     */
    MultiDimArray(const MultiDimArray< GUM_SCALAR >& src);

    /**
     * @todo : operator= for MultiDimArray
     *
     * @brief Copy operator.
     * @param src The MultiDimArray to copy.
     * @return Returns this MultiDimArray.
    MultiDimArray<GUM_SCALAR>&
    operator=( const MultiDimArray<GUM_SCALAR>& src );
     */

    /**
     * @brief Class destructor.
     *
     * Note that, when the multidimensional array is removed src memory, its
     * variables are not removed as well.
     */
    virtual ~MultiDimArray();

    virtual MultiDimContainer< GUM_SCALAR >* newFactory() const;

    /// }
    // =========================================================================
    /// @name Inherited methods
    // =========================================================================
    /// @{

    /**
     * @brief Copy from a other MultiDimContainer.
     */
    virtual void copyFrom(const MultiDimContainer< GUM_SCALAR >& src) const;

    /**
     * @brief Apply a function on every element of the container
     * @param f the function to apply
     */
    virtual void apply(std::function< GUM_SCALAR(GUM_SCALAR) > f) const;

    /**
     * @brief compute lfold for this container
     * @param f the function to apply
     * @param base the initial value
     */
    virtual GUM_SCALAR reduce(std::function< GUM_SCALAR(GUM_SCALAR, GUM_SCALAR) > f,
                              GUM_SCALAR                                          base) const;

    /**
     * @brief Adds a variable.
     */
    virtual void add(const DiscreteVariable& v);

    /**
     * @brief Removes a variable.
     */
    virtual void erase(const DiscreteVariable& v);

    /**
     * @brief Returns the real size of this MultiDimArray.
     */
    virtual Size realSize() const;

    /**
     * @brief Fills the MultiDimArray with the given value.
     */
    virtual void fill(const GUM_SCALAR& d) const;

    /**
     * @brief Returns the MultiDimArray name.
     */
    virtual const std::string& name() const;

    /// @}
    // =========================================================================
    /// @name Accessors / Modifiers
    // =========================================================================
    /// @{

    /**
     * @brief Returns the element stored in the multidimArray at a given
     * offset.
     *
     * The offset is computed as follows: let S={X1,...,Xk} be the sequence of
     * variables in the multidim, and let x1,...,xk be their values
     * respectively.  then the corresponding offset is x1 + |X1| * x2 + [X1| *
     * |X2| * x3 + ...  This thus corresponds to the internal MultiDimArray's
     * offset we would get by using function get with instantiation
     * {x1,...,xk}.
     *
     * @warning for efficiency reasons, this function does not check whether
     * the element referenced by the offset actually belongs to the
     * MultiDimArray.  If you want this additional feature, use instead
     * MultiDimArray::getByOffset(Idx).
     *
     * @param offset The offset of the returned value.
     * @brief Returns the element stored in the multidimArray at a given
     * offset.
     */
    const GUM_SCALAR& unsafeGet(Idx offset) const;

    /**
     * @brief Returns the element stored in the multidimArray at a given
     * offset.
     *
     * The offset is computed as follows: let S={X1,...,Xk} be the sequence of
     * variables in the multidim, and let x1,...,xk be their values
     * respectively.  then the corresponding offset is x1 + |X1| * x2 + [X1| *
     * |X2| * x3 + ...  This thus corresponds to the internal MultiDimArray's
     * offset we would get by using function get with instantiation
     * {x1,...,xk}.
     *
     * @param offset The offset of the returned value.
     * @brief Returns the element stored in the multidimArray at a given
     * offset.
     *
     * @throws OutOfBounds Raised if the referenced element does not belong to
     * the MultiDimArray.
     */
    const GUM_SCALAR& getByOffset(Idx offset) const;

    /**
     * @brief Modifies the element stored in the multidimArray at a given
     * offset.
     *
     * The offset is computed as follows: let S={X1,...,Xk} be the sequence of
     * variables in the multidim, and let x1,...,xk be their values
     * respectively.  then the corresponding offset is x1 + |X1| * x2 + [X1| *
     * |X2| * x3 + ...  This thus corresponds to the internal MultiDimArray's
     * offset we would get by using function get with instantiation
     * {x1,...,xk}.
     *
     * @warning for efficiency reasons, this function does not check whether
     * the element referenced by the offset actually belongs to the
     * MultiDimArray.  If you want this additional feature, use instead
     * function MultiDimArray::setByOffset(Idx, const GUM_SCALAR&).
     *
     * @param offset The offset where to store the value.
     * @param val The value to store.
     */
    void unsafeSet(Idx offset, const GUM_SCALAR& val);

    /**
     * @brief Modifies the element stored in the multidimArray at a given
     * offset.
     *
     * The offset is computed as follows: let S={X1,...,Xk} be the sequence of
     * variables in the multidim, and let x1,...,xk be their values
     * respectively.  then the corresponding offset is x1 + |X1| * x2 + [X1| *
     * |X2| * x3 + ...  This thus corresponds to the internal MultiDimArray's
     * offset we would get by using function get with instantiation
     * {x1,...,xk}.
     *
     * @param offset The offset where to store the value.
     * @param val The value to store.
     *
     * @throws OutOfBounds Raised if the referenced element does not belong to
     * the MultiDimArray.
     */
    void setByOffset(Idx offset, const GUM_SCALAR& val);

    /// @}

    protected:
    /// The true data : the values is mutable since we can change the value /
    /// in a const multiDimArray
    mutable std::vector< GUM_SCALAR > values_;

    void commitMultipleChanges_() final;

    void commitMultipleChanges_(const GUM_SCALAR&) final;

    GUM_SCALAR& get_(const Instantiation& i) const final;

    void replace_(const DiscreteVariable* x, const DiscreteVariable* y) final;
  };


#ifndef GUM_NO_EXTERN_TEMPLATE_CLASS
  extern template class MultiDimArray< double >;
#endif


} /* namespace gum */

#include <agrum/tools/multidim/implementations/multiDimArray_tpl.h>

#endif /* GUM_MULTI_DIM_ARRAY_H */
