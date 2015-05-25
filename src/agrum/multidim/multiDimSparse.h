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
#ifndef GUM_MULTI_DIM_SPARSE_H
#define GUM_MULTI_DIM_SPARSE_H

#include <vector>

#include <agrum/config.h>

#include <agrum/multidim/multiDimWithOffset.h>

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                          GUM_MULTI_DIM_ARRAY                        === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class MultiDimSparse
   * @brief Multidimensional matrix stored as an array in memory
   * @ingroup multidim_group
   */
  /* =========================================================================== */
  template <typename GUM_SCALAR>

  class MultiDimSparse : public MultiDimWithOffset<GUM_SCALAR> {
    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Default constructor: creates an empty null dimensional matrix

    MultiDimSparse(const GUM_SCALAR &default_value);

    /// copy constructor
    /** The newly created matrix contains the same variables and the same values as
     * from, but no instantiation is associated to it.
     * @param from the multidimensional matrix we copy into this */

    MultiDimSparse(const MultiDimSparse<GUM_SCALAR> &from);

    /// destructor
    /** Note that, when the multidimensional array is removed from memory, its
     * variables are not removed as well. */

    virtual ~MultiDimSparse();

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
    virtual MultiDimContainer<GUM_SCALAR> *newFactory() const;

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /// copy operator
    /** @param from the multidimensional matrix we copy into this */

    MultiDimSparse<GUM_SCALAR> &operator=(const MultiDimSparse<GUM_SCALAR> &from);
    /// @}

    /// add a new dimension
    /** @param v
     * @throw DuplicateElement
     */

    void add(const DiscreteVariable &v);

    /// removes a dimension
    /** If the variable does not belong to the MultiDimBase, then this method does
     * nothing.
     * @param v
     * @throw NotFound
     * @throw OperationNotAllowed
     */

    void erase(const DiscreteVariable &v);

    /// fill the table with d
    virtual void fill(const GUM_SCALAR &d) const;

    virtual GUM_SCALAR get(const Instantiation &i) const;
    virtual void set(const Instantiation &i, const GUM_SCALAR &value) const;

    /// returns the real name of the multiDimArray
    /** In aGrUM, all the types of multi-dimensional arrays/functionals have a
     * name that describes what they are in reality. For instance, a table stored
     * in extension is a "MultiDimArray", one that stores only non zero elements
     * is a "MultiDimSparseArray", and so on. These names are unique for each type
     * of implementation and is used by the system to determine which is the best
     * functions to use, say, when we wish to use operators such as operator+ on
     * two MultiDimImplementations */
    virtual const std::string &name() const;

    /// @return the real number of parameters used for this table. This function is
    /// used for compute @see compressionRatio()
    virtual Size realSize() const;

    protected:
    /// the true data : the values is mutable since we can change the value
    /// in a const multiDimArray
    mutable HashTable<Size, GUM_SCALAR> _params;
    mutable GUM_SCALAR _default;

    /// synchronise content after MultipleChanges
    virtual void _commitMultipleChanges(void);

    /// forbiden r/w access to values
    /** @param i an Instantiation
     * @throw OperationNotAllowed
     */
    virtual GUM_SCALAR &_get(const Instantiation &i) const {
      GUM_ERROR(OperationNotAllowed, "No (unconst) access to an aggregator");
    };

    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);
  };

} /* namespace gum */

#include <agrum/multidim/multiDimSparse.tcc>

#endif /* GUM_MULTI_DIM_SPARSE_H */
