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
/** @file
 * @brief Class representing a potential. Using the decorator pattern, this
 * representation is independant from the implementation of the multidimension
 * matrix
 *
 * @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#ifndef GUM_POTENTIAL_H
#define GUM_POTENTIAL_H

#include <vector>

#include <agrum/config.h>

#include <agrum/core/set.h>

#include <agrum/multidim/multiDimDecorator.h>
#include <agrum/multidim/multiDimImplementation.h>

namespace gum {

  /* =========================================================================== */
  /* =========================================================================== */
  /* ===                          GUM_PROBA                                  === */
  /* =========================================================================== */
  /* =========================================================================== */
  /** @class Potential
   * @brief Class representing a potential. Using the decorator pattern, this
   * representation is independant from the implementation of the multidimension
   * matrix
   * @ingroup multidim_group */
  /* =========================================================================== */
  template <typename GUM_SCALAR>

  class Potential : public MultiDimDecorator<GUM_SCALAR> {

    public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /**
     * Default constructor: creates an empty null dimensional matrix
     * choose a MultiDimArray<> as decorated implementation.
     */
    Potential();

    /**
     * Creates an potential around aContent.
     * @param aContent decorated implementation
     */
    explicit Potential(MultiDimImplementation<GUM_SCALAR> *aContent);

    /**
     * Copy constructor.
     */
    explicit Potential(const Potential<GUM_SCALAR> &src);

    /**
     * Copy constructor.
     *
     * The newly created matrix contains the same variables and the same values as
     * from, but no instantiation is associated to it.
     * @param aContent decorated implementation
     * @param src the multidimensional matrix we copy into this
     */
    explicit Potential(MultiDimImplementation<GUM_SCALAR> *aContent,
                       const MultiDimContainer<GUM_SCALAR> &src);

    /**
     * Destructor.
     *
     * Note that, when the multidimensional array is removed from memory, its
     * variables are not removed as well.
     */
    ~Potential();

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
    virtual Potential<GUM_SCALAR> *newFactory() const;

    /// @name classic operations on probability in Shafer-Shenoy
    ///@{

    /// normalisation of this do nothing if sum is 0
    Potential<GUM_SCALAR> &normalize() const;

    /// marginalizing p over the vars on *this.
    /// @throw OperationNotAllowed if there is var in *this not in p.
    Potential<GUM_SCALAR> &marginalize(const Potential<GUM_SCALAR> &p) const;

    /// Multiplication of args.
    /// @throw OperationNotAllowed if *this is not empty
    void multiplicate(const Potential<GUM_SCALAR> &p1,
                      const Potential<GUM_SCALAR> &p2);

    /// Multiplication of this and arg (in this).
    Potential<GUM_SCALAR> &multiplicateBy(const Potential<GUM_SCALAR> &p1);

    ///@}

    Potential<GUM_SCALAR> &operator=(const Potential<GUM_SCALAR> &src);

    /// sum of all elements in this
    const GUM_SCALAR sum() const;

    /**
     * string representation of this.
     */
    virtual const std::string toString() const;

    protected:
    /// perform the marginalization p over the vars on *this.
    void _marginalize(const Potential &p,
                      const Set<const DiscreteVariable *> &del_vars) const;

    /// perform the multiplication of args.
    void _multiplicate(const Potential &p1, const Potential &p2);

    virtual void _swap(const DiscreteVariable *x, const DiscreteVariable *y);
  };

  extern template class Potential<float>;
  extern template class Potential<double>;
} /* namespace gum */

#include <agrum/multidim/potential.tcc>

#endif /* GUM_POTENTIAL_H */

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
