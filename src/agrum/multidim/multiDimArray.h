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
/** @file
 * @brief Multidimensional matrix stored as an array in memory
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#ifndef GUM_MULTI_DIM_ARRAY_H
#define GUM_MULTI_DIM_ARRAY_H

#include <vector>

#include <agrum/config.h>

#include <agrum/multidim/multiDimWithOffset.h>

namespace gum {

  template <typename GUM_SCALAR>

  class MultiDimBijArray;

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                          GUM_MULTI_DIM_ARRAY                        === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class MultiDimArray
   * @brief Multidimensional matrix stored as an array in memory
   * @ingroup multidim_group
   */
  /* =========================================================================== */
  template <typename GUM_SCALAR>

  class MultiDimArray : public MultiDimWithOffset<GUM_SCALAR> {
    public:
    template <typename GUM_SCALARBIS>

    friend class MultiDimBijArray;
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor: creates an empty null dimensional matrix
    MultiDimArray();

    /// copy constructor
    /** The newly created matrix contains the same variables and the same values as
     * src, but no instantiation is associated to it.
     * @param src the multidimensional matrix we copy into this */
    MultiDimArray(const MultiDimArray<GUM_SCALAR> &src);

    /// destructor
    /** Note that, when the multidimensional array is removed src memory, its
     * variables are not removed as well. */
    virtual ~MultiDimArray();

    /// virtual copy constructor
    /** This method creates a clone of this object, withouth its content
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
    virtual MultiDimContainer<GUM_SCALAR> *newFactory() const;

    /// @}

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param src the multidimensional matrix we copy into this */
    MultiDimArray<GUM_SCALAR> &operator=(const MultiDimArray<GUM_SCALAR> &src);

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// add a new dimension, needed for updating the offsets & gaps
    /** @param v
     * @throw DuplicateElement
     */
    virtual void add(const DiscreteVariable &v);

    /// removes a dimension, needed for updating the offsets & gaps
    /** If the variable does not belong to the MultiDimBase, then this method does
     * nothing.
     * @param v
     * @throw NotFound
     * @throw OperationNotAllowed
     */
    virtual void erase(const DiscreteVariable &v);

    /// returns the real number of parameters used for this table.
    /** This function is used for compute @see compressionRatio() */
    virtual Size realSize() const;

    /// fill the table with d
    virtual void fill(const GUM_SCALAR &d) const;

    /// returns the element stored in the multidimArray at a given offset
    /** The offset is computed as follows: let S={X1,...,Xk} be the sequence of
     * variables in the multidim, and let x1,...,xk be their values respectively.
     * then the corresponding offset is x1 + |X1| * x2 + [X1| * |X2| * x3 + ...
     * This thus corresponds to the internal MultiDimArray's offset we would get
     * by using function get with instantiation {x1,...,xk}.
     * @warning for efficiency reasons, this function does not check whether
     * the element referenced by the offset actually belongs to the MultiDimArray.
     * If you want this additional feature, use instead function getByOffset */
    const GUM_SCALAR &unsafeGet(Idx offset) const;

    /// returns the element stored in the multidimArray at a given offset
    /** The offset is computed as follows: let S={X1,...,Xk} be the sequence of
     * variables in the multidim, and let x1,...,xk be their values respectively.
     * then the corresponding offset is x1 + |X1| * x2 + [X1| * |X2| * x3 + ...
     * This thus corresponds to the internal MultiDimArray's offset we would get
     * by using function get with instantiation {x1,...,xk}.
     * @throws OutOfBound exception is thrown if the element referenced does not
     * belong to the MultiDimArray */
    const GUM_SCALAR &getByOffset(Idx offset) const;

    /// modifies the the element stored in the multidimArray at a given offset
    /** The offset is computed as follows: let S={X1,...,Xk} be the sequence of
     * variables in the multidim, and let x1,...,xk be their values respectively.
     * then the corresponding offset is x1 + |X1| * x2 + [X1| * |X2| * x3 + ...
     * This thus corresponds to the internal MultiDimArray's offset we would get
     * by using function get with instantiation {x1,...,xk}.
     * @warning for efficiency reasons, this function does not check whether
     * the element referenced by the offset actually belongs to the MultiDimArray.
     * If you want this additional feature, use instead function setByOffset */
    void unsafeSet(Idx offset, const GUM_SCALAR &);

    /// modifies the the element stored in the multidimArray at a given offset
    /** The offset is computed as follows: let S={X1,...,Xk} be the sequence of
     * variables in the multidim, and let x1,...,xk be their values respectively.
     * then the corresponding offset is x1 + |X1| * x2 + [X1| * |X2| * x3 + ...
     * This thus corresponds to the internal MultiDimArray's offset we would get
     * by using function get with instantiation {x1,...,xk}.
     * @throws OutOfBound exception is thrown if the element referenced does not
     * belong to the MultiDimArray */
    void setByOffset(Idx offset, const GUM_SCALAR &);

    /// returns the real name of the multiDimArray
    /** In aGrUM, all the types of multi-dimensional arrays/functionals have a
     * name that describes what they are in reality. For instance, a table stored
     * in extension is a "MultiDimArray", one that stores only non zero elements
     * is a "MultiDimSparseArray", and so on. These names are unique for each type
     * of implementation and is used by the system to determine which is the best
     * functions to use, say, when we wish to use operators such as operator+ on
     * two MultiDimImplementations */
    virtual const std::string &name() const;

    /// @}

    protected:
    /// the true data : the values is mutable since we can change the value
    /// in a const multiDimArray
    mutable std::vector<GUM_SCALAR> _values;

    /// synchronise content after MultipleChanges
    virtual void _commitMultipleChanges(void);
    virtual void _commitMultipleChanges(const GUM_SCALAR &);

    /// data access operator
    /** @param i an Instantiation
     * @throw NotFound
     * @throw NullElement
     */
    virtual GUM_SCALAR &_get(const Instantiation &i) const;

    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);
  };

  extern template class MultiDimArray<float>;
  extern template class MultiDimArray<double>;
} /* namespace gum */

#include <agrum/multidim/multiDimArray.tcc>

#endif /* GUM_MULTI_DIM_ARRAY_H */
