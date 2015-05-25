/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Headers of gum::Instance
 *
 * @author Lionel TORTI
 */

#ifndef GUM_MULTIDIMBIJARRAY_H
#define GUM_MULTIDIMBIJARRAY_H

#include <agrum/core/bijection.h>

#include <agrum/multidim/multiDimWithOffset.h>
#include <agrum/multidim/multiDimArray.h>

namespace gum {

  /**
   * @class MultiDimBijArray "multiDimBijArray.h" <agrum/multidim/multiDimBijArray.h>
   * @brief Decorator of a MultiDimArray, using a bijection over the variables.
   * @ingroup multidim_group
   */
  template <typename GUM_SCALAR>
  class MultiDimBijArray : public MultiDimWithOffset<GUM_SCALAR> {
    public:
    typedef Bijection<const DiscreteVariable *, const DiscreteVariable *>
        VarBijection;

    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// Gets the content of decorator and uses its implementation.
    /// @throw OperationNotAllowed If decorator's content is not a MultiDimArray
    ///                            raise an OperationNotAllowed

    MultiDimBijArray(const VarBijection &bijection,
                     const MultiDimArray<GUM_SCALAR> &array);

    /// Gets the content of decorator and uses its implementation.
    /// @throw OperationNotAllowed If decorator's content is not a MultiDimArray
    ///                            raise an OperationNotAllowed

    MultiDimBijArray(const VarBijection &bijection,
                     const MultiDimBijArray<GUM_SCALAR> &array);

    /// copy constructor
    /** The newly created matrix contains the same variables and the same values as
     * from, but no instantiation is associated to it.
     * @param from the multidimensional matrix we copy into this */

    MultiDimBijArray(const MultiDimBijArray<GUM_SCALAR> &from);

    /// destructor
    /** Note that, when the multidimensional array is removed from memory, its
     * variables are not removed as well. */

    virtual ~MultiDimBijArray();

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
     * This will return a MultiDimArray since MultiDimBijArray are a decoration
     * of that class.
     *
     * @warning you must desallocate by yourself the memory
     * @return an empty clone of this object with the same type
     */
    virtual MultiDimBijArray<GUM_SCALAR> *newFactory() const;

    // ############################################################################
    /// @name Operators
    // ############################################################################
    /// @{

    /**
     * This will raise an exception: read only structure
     * @param from
     * @throw OperationNotAllowed You can't change a readonly structure
     */
    MultiDimBijArray<GUM_SCALAR> &
    operator=(const MultiDimBijArray<GUM_SCALAR> &from);

    /// @}

    /// Returns the real name of this implementation
    virtual const std::string &name() const;

    /// Returns the value pointed by i.
    virtual GUM_SCALAR get(const Instantiation &i) const;

    /**
     * This will raise an exception:  read only structure
     * MultiDimBijArray.
     * @param i
     * @param value
     * @throw OperationNotAllowed You can't change a readonly structure
     */
    virtual void set(const Instantiation &i, const GUM_SCALAR &value) const;

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param v The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void add(const DiscreteVariable &v);

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param v The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void erase(const DiscreteVariable &v);

    /// This function is used for compute @see compressionRatio().
    /// @return the real number of parameters used for this table.
    virtual Size realSize() const;

    /**
     * This will raise an exception: you can't change the data
     * @param d the value changed
     * @throw OperationNotAllowed You can't change data.
     */
    virtual void fill(const GUM_SCALAR &d) const;

    /**
     * This will raise an exception: you can't change the variables in a
     * MultiDimBijArray.
     * @param v The variable not added.
     * @throw OperationNotAllowed You can't add variable in a MultiDimBijArray.
     */
    virtual void fillWith(const std::vector<GUM_SCALAR> &v) const;

    protected:
    virtual GUM_SCALAR &_get(const Instantiation &i) const;

    /// synchronise content after MultipleChanges
    virtual void _commitMultipleChanges(void);

    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);

    private:
    /// the true data.
    const MultiDimArray<GUM_SCALAR> &__array;

    std::string __name;
  };
} // namespace gum

#include <agrum/multidim/multiDimBijArray.tcc>

#endif // GUM_MULTIDIMBIJARRAY_H
